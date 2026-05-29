/*
 * Title       GS Relay switch controller
 * by          Kiehyun Kevin Park
 *
 * Copyright (C) 2016 to 2018 Kiehyun Kevin Park.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Description:
 *   Full featured relay switch controller.
 *
 * Author: Kiehyun Kevin Park
 * 
 *   Kiehyun.Park@gmail.com
 */
#include <SoftwareSerial.h>

#define serialno 003
#define firmver "1.0b"
#define boardtype 1   // select boardtype

// -------------------------------------------------------
// Relay pin assignments
// -------------------------------------------------------
//  핀 번호   역할          명령키   설명
//  -------   ----------   ------   --------------------
//  relayPin1  일반 채널 1   q / a    ON / OFF
//  relayPin2  CLOSE        c / C    500ms 펄스 (닫기)
//  relayPin3  OPEN         o / O    500ms 펄스 (열기)
//  relayPin4  일반 채널 4   r / f    ON / OFF
// -------------------------------------------------------

#if boardtype==1  // standard 4ch (Arduino Nano)
#define relayPin1  10  // 일반 채널 1
#define relayPin2  8   // CLOSE (닫기) 릴레이 - c/C
#define relayPin3  6   // OPEN  (열기) 릴레이 - o/O
#define relayPin4  4   // 일반 채널 4

#elif boardtype==2  // njp 4ch
#define relayPin1  9   // 일반 채널 1
#define relayPin2  10  // CLOSE (닫기) 릴레이 - c/C
#define relayPin3  11  // OPEN  (열기) 릴레이 - o/O
#define relayPin4  12  // 일반 채널 4
#endif

// -------------------------------------------------------
// Bluetooth module (HC-05 / HC-06) wiring
// -------------------------------------------------------
//  BT 모듈 핀   Arduino Nano 핀   설명
//  ----------   ---------------   --------------------
//  VCC          3.3V or 5V        전원 (HC-06: 3.6~6V)
//  GND          GND               공통 접지
//  TX           D2  (BT_RX_PIN)   BT 송신 → Nano 수신
//  RX           D3  (BT_TX_PIN)   BT 수신 ← Nano 송신
//                                 (RX에 전압분배 권장: 5V→3.3V)
// -------------------------------------------------------
#define BT_RX_PIN  2   // Arduino Nano D2 <- BT TX
#define BT_TX_PIN  3   // Arduino Nano D3 -> BT RX

SoftwareSerial btSerial(BT_RX_PIN, BT_TX_PIN);

int statusPin1 = 0;
int statusPin2 = 0;
int statusPin3 = 0;
int statusPin4 = 0;

char lastCommand = '-';
unsigned long lastCommandTime = 0;
const unsigned long relayOffIntervalMs = 60000UL;
const unsigned long statusReportIntervalMs = 10000UL;
unsigned long lastRelayOffTime = 0;
unsigned long lastStatusReportTime = 0;

// ---- 출력 헬퍼: USB 시리얼 + 블루투스 양쪽에 전송 ----
void serialPrintBoth(const String &msg) {
  Serial.println(msg);
  btSerial.println(msg);
}

// ---- 현재 상태 출력 ----
void printStatus() {
  String msg = "Status(" +
               String(statusPin1) + "," +
               String(statusPin2) + "," +
               String(statusPin3) + "," +
               String(statusPin4) +
               ") last:" + lastCommand +
               " elapsed:" + String(millis() - lastCommandTime) + "ms";
  serialPrintBoth(msg);
}

// ---- 안전 OFF: 1분마다 모든 릴레이 출력을 LOW로 정리 ----
void turnAllRelaysOff(const String &reason) {
  digitalWrite(relayPin1, LOW);
  digitalWrite(relayPin2, LOW);
  digitalWrite(relayPin3, LOW);
  digitalWrite(relayPin4, LOW);

  statusPin1 = 0;
  statusPin2 = 0;
  statusPin3 = 0;
  statusPin4 = 0;

  serialPrintBoth(reason);
}

void periodicRelayOff() {
  unsigned long now = millis();
  if (now - lastRelayOffTime < relayOffIntervalMs) {
    return;
  }

  lastRelayOffTime = now;
  turnAllRelaysOff("AUTO: all relay pins OFF (1 minute interval)");
  printStatus();
  lastStatusReportTime = now;
}

void periodicStatusReport() {
  unsigned long now = millis();
  if (now - lastStatusReportTime < statusReportIntervalMs) {
    return;
  }

  lastStatusReportTime = now;
  printStatus();
}

// ---- OPEN: relayPin3 (o/O) ----
void doOpen() {
  serialPrintBoth("CMD: OPEN  -> relayPin3 ON");
  digitalWrite(relayPin3, HIGH);
  statusPin3 = 1;
  delay(500);
  digitalWrite(relayPin3, LOW);
  statusPin3 = 0;
  lastCommand = 'O';
  lastCommandTime = millis();
  serialPrintBoth("CMD: OPEN  -> relayPin3 OFF (500ms elapsed)");
}

// ---- CLOSE: relayPin2 (c/C) ----
void doClose() {
  serialPrintBoth("CMD: CLOSE -> relayPin2 ON");
  digitalWrite(relayPin2, HIGH);
  statusPin2 = 1;
  delay(500);
  digitalWrite(relayPin2, LOW);
  statusPin2 = 0;
  lastCommand = 'C';
  lastCommandTime = millis();
  serialPrintBoth("CMD: CLOSE -> relayPin2 OFF (500ms elapsed)");
}

// ---- 명령 처리 (USB & BT 공용) ----
void processCommand(char com) {
  switch (com) {
    case 'o': case 'O': doOpen();  break;
    case 'c': case 'C': doClose(); break;
    default: break;
  }
  printStatus();
}

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);

  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);
  turnAllRelaysOff("INIT: all relay pins OFF");
  lastRelayOffTime = millis();
  lastStatusReportTime = millis();

  serialPrintBoth("GS Relay Switch Controller ready.");
  serialPrintBoth("  o/O : OPEN  (relayPin3, 500ms pulse)");
  serialPrintBoth("  c/C : CLOSE (relayPin2, 500ms pulse)");
  serialPrintBoth("  AUTO: all relay pins OFF every 1 minute");
  serialPrintBoth("  STATUS: report every 10 seconds");
}

void loop() {
  // USB 시리얼 수신
  if (Serial.available()) {
    processCommand((char)Serial.read());
  }
  // 블루투스 수신
  if (btSerial.available()) {
    processCommand((char)btSerial.read());
  }
  periodicRelayOff();
  periodicStatusReport();
}
