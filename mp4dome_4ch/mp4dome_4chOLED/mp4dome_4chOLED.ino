
// ===========================================================================================================
// mp4dome_4chOLED : DHT22 온습도 모니터링 + OLED 디스플레이 + 버튼 제어 시스템
// 대상 보드 : Arduino Nano (ATmega328P)
// 업로드 대상 : Arduino Nano
// ===========================================================================================================
// 개요:
//   - DHT22 온습도 센서로 현재 온도/습도를 읽어 시리얼 및 OLED에 표시합니다.
//   - 128x64 SSD1306 OLED 디스플레이(I2C)로 현재 상태를 시각적으로 표시합니다.
//   - 4방향 버튼(UP/DOWN/LEFT/RIGHT)으로 메뉴 탐색이 가능합니다.
//   - PC에서 시리얼 명령어('k': 온습도 조회, 'Z'/'z': 장치 식별)로 제어가 가능합니다.
//
// ===========================================================================================================
// Arduino Nano 핀 사용 현황
// ===========================================================================================================
//
//  [ 사용 중인 핀 ]
//  ┌─────────┬──────────┬──────────────────────────────────────────────────────────────┐
//  │  핀     │  용도    │  설명                                                        │
//  ├─────────┼──────────┼──────────────────────────────────────────────────────────────┤
//  │  D0     │  RX      │  시리얼 수신 — USB(PC) 및 HC-05 블루투스 TXD 공유           │
//  │         │          │  ※ 업로드 시 블루투스 모듈을 반드시 분리할 것               │
//  │  D1     │  TX      │  시리얼 송신 — USB(PC) 및 HC-05 블루투스 RXD 공유           │
//  │         │          │  ※ 업로드 시 블루투스 모듈을 반드시 분리할 것               │
//  │  D2     │  IN      │  DHT22 온습도 센서 데이터                                    │
//  │  D7     │  IN      │  버튼 LEFT  (INPUT_PULLUP)                                   │
//  │  D8     │  IN      │  버튼 UP    (INPUT_PULLUP)                                   │
//  │  D9     │  IN      │  버튼 DOWN  (INPUT_PULLUP)                                   │
//  │  D10    │  IN      │  버튼 RIGHT (INPUT_PULLUP)                                   │
//  │  A4     │  I2C SDA │  OLED SSD1306 데이터 (I2C)                                  │
//  │  A5     │  I2C SCL │  OLED SSD1306 클록  (I2C)                                   │
//  │  D3     │  OUT     │  릴레이 채널 1 — 미사용 (고장)                              │
//  │  D4     │  OUT     │  릴레이 채널 2 — c/C (500ms 펄스, Dome Close)               │
//  │  D5     │  OUT     │  릴레이 채널 3 — o/O (500ms 펄스, Dome Open)                │
//  │  D6     │  OUT     │  릴레이 채널 4 — r=ON / f=OFF                               │
//  └─────────┴──────────┴──────────────────────────────────────────────────────────────┘
//
//  [ HC-05 블루투스 모듈 연결 ]
//  ┌──────────────┬──────────────────────────────────────────────────────────┐
//  │  HC-05 핀   │  연결                                                    │
//  ├──────────────┼──────────────────────────────────────────────────────────┤
//  │  VCC        │  5V (Nano 5V 핀)                                         │
//  │  GND        │  GND                                                     │
//  │  TXD        │  Nano D0 (RX) ← HC-05가 보내는 데이터                   │
//  │  RXD        │  Nano D1 (TX) ← HC-05가 받는 데이터 (분압 권장: 1k/2k)  │
//  │  STATE      │  미연결 (연결 상태 표시용, 필요 시 디지털 핀으로 읽기)   │
//  │  EN/KEY     │  미연결 (AT 명령 모드 진입용, 평소 사용 불필요)          │
//  └──────────────┴──────────────────────────────────────────────────────────┘
//  ※ D0/D1은 USB 시리얼과 동일 포트 공유 → 블루투스로도 동일한 시리얼 명령 사용 가능
//  ※ 펌웨어 업로드 시에는 HC-05의 TXD/RXD 선을 반드시 분리해야 업로드 성공
//
//  [ 미사용 핀 (자유롭게 활용 가능) ]
//  D11(PWM/MOSI), D12(MISO), D13(SCK/내장LED),
//  A0, A1, A2, A3,
//  A6(아날로그 입력 전용), A7(아날로그 입력 전용)
//
// ===========================================================================================================
//
// LEDS, TEMPERATURE PROBE, OLED
// ----------------------------------------------------------------------------------------------------------
// FIRMWARE CHANGE LOG
// (2019. 10. 1) Ver. 1.0 기초 동작 시작
// (2019. 10. 1.) Ver. 1.1 박기현 편집 시작 
// (2019. 11. 14.) Ver. 2.0 프로토콜 변경 

const char firmwareName[] = "mp4dome_4chOLED";
const char firmwareDate[] = __DATE__;
const char firmwareVer[] = "1.2";

// ----------------------------------------------------------------------------------------------------------
// 시리얼 통신 프로토콜 (PC <-> mp4dome_4chOLED)
//
//  'c' / 'C' : CH2 펄스 - CH2(CLOSE) 릴레이 500ms 펄스
//  'o' / 'O' : CH3 펄스 - CH3(OPEN) 릴레이 500ms 펄스
//  'r' / 'R' : CH4 ON   - 채널 4 릴레이 ON
//  'f' / 'F' : CH4 OFF  - 채널 4 릴레이 OFF
//  'T' + HHMMSS : PC 시각 설정 (예: T142530 -> 14:25:30)
//  'k'       : GET TEMPERATURE & HUMIDITY  - 온습도 값을 시리얼로 응답
//  'Z' / 'z' : IDENTIFY  - "mp4dome_4chOLED" 문자열 응답
// ----------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------
// 전역 상태 변수
// ----------------------------------------------------------------------------------------------------------
short subm = 0; // 화면 단계: 0=상태화면, 1=메뉴선택, 2=릴레이 제어
short menu = 1; // 현재 선택된 메뉴 항목 (1=DOME OPEN, 2=DOME CLOSE, 3=CH4)

// ----------------------------------------------------------------------------------------------------------
// 릴레이 핀 정의 및 상태 변수
// ----------------------------------------------------------------------------------------------------------
#define RELAY_CLOSE 4  // D4 — CH2 (c/C, 시리얼/메뉴에서 500ms 펄스)
#define RELAY_OPEN  5  // D5 — CH3 (o/O, 시리얼/메뉴에서 500ms 펄스)
#define RELAY_CH4   6  // D6 — CH4 (r=ON / f=OFF)

int relayStatus4 = 0; // 채널 4 현재 상태 (0=OFF, 1=ON)
int relayStatus2 = 0; // 채널 2 (CLOSE) 현재 상태 (0=OFF, 1=ON)
int relayStatus3 = 0; // 채널 3 (OPEN) 현재 상태 (0=OFF, 1=ON)

// ----------------------------------------------------------------------------------------------------------
// DHT22 온습도 센서 설정
// ----------------------------------------------------------------------------------------------------------
#include <DHT.h>  // DHT11/DHT22 센서 라이브러리 (Adafruit DHT sensor library)
#define DHT22_PIN 2   // DHT22 센서의 데이터 핀 번호 (디지털 2번)
#define DHTTYPE DHT22 // 센서 타입: DHT22 (AM2302)
DHT dht(DHT22_PIN, DHTTYPE); // DHT 객체 생성
int chkSensor;        // DHT22 데이터 핀 디지털 읽기 값 (센서 연결 확인용)
char Temperature[12] = "--.-";
char Humidity[12] = "--.-";

// ----------------------------------------------------------------------------------------------------------
// U8glib OLED 디스플레이 설정 (128x64 SSD1306, I2C 통신)
// 참고: https://github.com/olikraus/u8glib
// ----------------------------------------------------------------------------------------------------------
#include <U8glib.h>
// 일부 1.3" OLED 모듈은 SH1106 컨트롤러를 사용하므로 화면이 깨지면 1로 변경하세요.
#define OLED_USE_SH1106 0

#if OLED_USE_SH1106
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE |
                         U8G_I2C_OPT_DEV_0);
#else
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE |
                          U8G_I2C_OPT_DEV_0);  // I2C / TWI 방식으로 OLED 초기화
#endif
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|
//                          U8G_I2C_OPT_NO_ACK|
//                          U8G_I2C_OPT_FAST);  // 빠른 I2C 모드 (필요 시 사용)


// 릴레이 동작 메시지 표시용 전역 변수
const char *lastRelayMessage = 0;
unsigned long lastRelayTime = 0;  // 메시지 표시 시간
#define RELAY_MSG_DURATION 2000  // 메시지 표시 시간: 2초

// DHT22 읽기 간격 제어: 2초 이상 지났을 때만 읽기
unsigned long lastDHTReadTime = 0;
#define DHT_READ_INTERVAL 2000

// 시각 표시용 상태: PC 시각을 받으면 그 시각 기준으로 진행, 없으면 부팅 후 경과시간 표시
unsigned long bootMillis = 0;
bool hostTimeValid = false;
unsigned long hostTimeSetMillis = 0;
unsigned long hostTimeBaseSeconds = 0;

// 시리얼로 수신하는 PC 시각 패킷(T + HHMMSS) 파싱 상태
bool timePacketActive = false;
char timePacketDigits[7] = {0};
byte timePacketPos = 0;

// JS's pcb
//#define PCB_1

// Kevin's PCB
#define PCB_1

// 스위치 핀 번호
#ifdef PCB_1
  #define UPpin 8
  #define DOWNpin 9
  #define RIGHTpin 10
  #define LEFTpin 7
#endif
#ifdef PCB_2
  #define UPpin 8
  #define DOWNpin 9
  #define RIGHTpin 7
  #define LEFTpin 10
#endif

// 버튼 현재/이전 상태 배열: [0]=UP, [1]=DOWN, [2]=RIGHT, [3]=LEFT
short S[4] = {0};
short now[4] = {1};
short was[4] = {0};

// ----------------------------------------------------------------------------------------------------------
// setup(): 아두이노 초기화 함수 - 전원 인가 또는 리셋 시 1회 실행
// ----------------------------------------------------------------------------------------------------------
void setup() 
  {
    bootMillis = millis();

    Serial.begin(9600);           // 시리얼 통신 시작 (9600 baud)
    Serial.println(firmwareName); // 부팅 시 펌웨어 이름 출력
    Serial.print(F("BOOT:START:"));
    Serial.print(firmwareName);
    Serial.print(F(":VER:"));
    Serial.print(firmwareVer);
    Serial.print(F(":DATE:"));
    Serial.print(firmwareDate);
    Serial.println(F("#"));
      
    U8G_start(); // OLED 디스플레이 초기화 (폰트, 색상 설정)
    dht.begin(); // DHT22 센서 초기화
    
    pinset();    // 버튼 핀 INPUT_PULLUP 설정 (ButtonControl.ino)

    // 릴레이 핀 출력 설정 및 초기 OFF 상태
    pinMode(RELAY_CLOSE, OUTPUT); digitalWrite(RELAY_CLOSE, LOW);
    pinMode(RELAY_OPEN,  OUTPUT); digitalWrite(RELAY_OPEN,  LOW);
    pinMode(RELAY_CH4,   OUTPUT); digitalWrite(RELAY_CH4,   LOW);
    
    U8G_test();  // OLED에 시작 화면 2초간 표시
  }

// ----------------------------------------------------------------------------------------------------------
// loop(): 메인 루프 - 반복 실행
//   1. DHT22에서 온도/습도 읽어 시리얼 전송
//   2. 버튼 입력 처리 (메뉴 탐색)
//   3. OLED 디스플레이 갱신
// ----------------------------------------------------------------------------------------------------------
void loop() 
  {
    // humidityTemperatureReport() 내부에서 DHT22를 읽고 Temperature/Humidity를 갱신한 뒤 시리얼로 전송
    humidityTemperatureReport();

    buttonRead(); // 버튼 입력 읽기 및 메뉴 탐색 처리
    
    U8G_draw(); // OLED 화면 갱신
  }

// ----------------------------------------------------------------------------------------------------------
// doClose(): RELAY_CLOSE(D4)를 500ms HIGH 펄스로 구동 — CH2 (0.5초)
// doOpen() : RELAY_OPEN(D5)를 500ms HIGH 펄스로 구동 — CH3 (0.5초)
// ----------------------------------------------------------------------------------------------------------
void doClose() {
  Serial.println(F("CH2:Pulse:START#"));
  lastRelayMessage = "Closing Dome"; //"CH2 Pulse";
  relayStatus2 = 1;
  digitalWrite(RELAY_CLOSE, HIGH);
  delay(500);
  digitalWrite(RELAY_CLOSE, LOW);
  relayStatus2 = 0;
  // 펄스 완료 후부터 2초간 메시지를 표시하도록 타이머를 시작
  lastRelayTime = millis();
  Serial.println(F("CH2:Pulse:DONE#"));
}

void doOpen() {
  Serial.println(F("CH3:Pulse:START#"));
  lastRelayMessage = "Opening Dome"; //"CH3 Pulse";
  relayStatus3 = 1;
  digitalWrite(RELAY_OPEN, HIGH);  // RELAY_OPEN 사용
  delay(500);
  digitalWrite(RELAY_OPEN, LOW);
  relayStatus3 = 0;
  // 펄스 완료 후부터 2초간 메시지를 표시하도록 타이머를 시작
  lastRelayTime = millis();
  Serial.println(F("CH3:Pulse:DONE#"));
}

unsigned long parseHHMMSS(const char *digits)
  {
    int hh = (digits[0] - '0') * 10 + (digits[1] - '0');
    int mm = (digits[2] - '0') * 10 + (digits[3] - '0');
    int ss = (digits[4] - '0') * 10 + (digits[5] - '0');

    if (hh < 0 || hh > 23 || mm < 0 || mm > 59 || ss < 0 || ss > 59)
      {
        return 0xFFFFFFFFUL;
      }

    return (unsigned long)hh * 3600UL + (unsigned long)mm * 60UL + (unsigned long)ss;
  }

void updateHostTimeFromPacket(const char *digits)
  {
    unsigned long seconds = parseHHMMSS(digits);
    if (seconds == 0xFFFFFFFFUL)
      {
        Serial.println(F("TIME:INVALID#"));
        return;
      }

    hostTimeBaseSeconds = seconds;
    hostTimeSetMillis = millis();
    hostTimeValid = true;
    Serial.println(F("TIME:SYNCED#"));
  }

void buildDisplayTimeText(char *out)
  {
    unsigned long elapsedSec;
    unsigned long totalSec;
    bool usePcTime = hostTimeValid;

    if (usePcTime)
      {
        elapsedSec = (millis() - hostTimeSetMillis) / 1000UL;
        totalSec = (hostTimeBaseSeconds + elapsedSec) % 86400UL;
      }
    else
      {
        totalSec = (millis() - bootMillis) / 1000UL;
      }

    unsigned int hh = (unsigned int)(totalSec / 3600UL);
    unsigned int mm = (unsigned int)((totalSec % 3600UL) / 60UL);
    unsigned int ss = (unsigned int)(totalSec % 60UL);

    if (usePcTime)
      {
        // 형식: PC 12:34:56
        out[0] = 'P';
        out[1] = 'C';
        out[2] = ' ';
      }
    else
      {
        // 형식: UP 00:12:34
        out[0] = 'U';
        out[1] = 'P';
        out[2] = ' ';
      }

    out[3] = (char)('0' + (hh / 10) % 10);
    out[4] = (char)('0' + (hh % 10));
    out[5] = ':';
    out[6] = (char)('0' + (mm / 10) % 10);
    out[7] = (char)('0' + (mm % 10));
    out[8] = ':';
    out[9] = (char)('0' + (ss / 10) % 10);
    out[10] = (char)('0' + (ss % 10));
    out[11] = '\0';
  }

// ----------------------------------------------------------------------------------------------------------
// serialCommand(): PC로부터 수신한 명령 문자열을 파싱하여 동작을 수행
// 명령 형식: <알파벳 1글자>\n
//   'c' / 'C' : CLOSE    - CLOSE 릴레이 500ms 펄스
//   'o' / 'O' : OPEN     - OPEN  릴레이 500ms 펄스
//   'r' / 'R' : CH4 ON   - 채널 4 릴레이 ON
//   'f' / 'F' : CH4 OFF  - 채널 4 릴레이 OFF
//   'k'       : GET TEMPERATURE & HUMIDITY - 온습도 값을 시리얼로 응답
//   'Z' / 'z' : IDENTIFY - "mp4dome_4chOLED" 문자열 응답
// ----------------------------------------------------------------------------------------------------------
void serialCommand(char _command) 
  {
    switch (_command) {
    case 'c': case 'C': // CLOSE 릴레이 500ms 펄스
      doClose();
      break;
    case 'o': case 'O': // OPEN 릴레이 500ms 펄스
      doOpen();
      break;
    case 'r': case 'R': // 채널 4 ON
      digitalWrite(RELAY_CH4, HIGH); relayStatus4 = 1;
      Serial.println(F("CH4:ON#"));
      break;
    case 'f': case 'F': // 채널 4 OFF
      digitalWrite(RELAY_CH4, LOW);  relayStatus4 = 0;
      Serial.println(F("CH4:OFF#"));
      break;
    case 'k': // GET TEMPERATURE & HUMIDITY: 온습도 값 시리얼 출력
      humidityTemperatureReport();
      break;
    case 'Z':  // IDENTIFY: 장치 식별 문자열 응답
    case 'z':
      Serial.println(F("mp4dome_4chOLED#"));
      break;
    default:   // 알 수 없는 명령: 장치 이름으로 응답
      Serial.println(F("mp4dome_4chOLED#"));
      break;
    }
  }

// ----------------------------------------------------------------------------------------------------------
// serialEvent(): 시리얼 수신 인터럽트 핸들러
//   - 단일 문자 명령(c/o/r/f/k/Z 등)은 즉시 처리
//   - 'T' 다음 6자리 숫자(HHMMSS)를 받으면 PC 시각으로 동기화
//   - 개행문자('\n', '\r')는 무시
// ----------------------------------------------------------------------------------------------------------
void serialEvent() 
  {
    while (Serial.available()) 
      {
        char inChar = (char)Serial.read(); // 1바이트 읽기

        if (timePacketActive)
          {
            if (inChar >= '0' && inChar <= '9')
              {
                if (timePacketPos < 6)
                  {
                    timePacketDigits[timePacketPos++] = inChar;
                  }

                if (timePacketPos >= 6)
                  {
                    timePacketDigits[6] = '\0';
                    updateHostTimeFromPacket(timePacketDigits);
                    timePacketActive = false;
                    timePacketPos = 0;
                  }
              }
            else
              {
                timePacketActive = false;
                timePacketPos = 0;
              }
            continue;
          }

        if (inChar == '\r' || inChar == '\n')
          {
            continue;
          }

        if (inChar == 'T')
          {
            timePacketActive = true;
            timePacketPos = 0;
            continue;
          }

        serialCommand(inChar);
      }
  }

// ----------------------------------------------------------------------------------------------------------
// DHT22 온습도 읽기
// ----------------------------------------------------------------------------------------------------------
void humidityTemperatureReport()
  {
    unsigned long currentTime = millis();
    if (currentTime - lastDHTReadTime < DHT_READ_INTERVAL)
      {
        return;
      }
    lastDHTReadTime = currentTime;

    float t = dht.readTemperature();
    float h = dht.readHumidity();
    bool valid = !isnan(t) && !isnan(h) && h >= 0.0 && h <= 100.0;

    if (valid)
      {
        dtostrf(t, 4, 1, Temperature);
        dtostrf(h, 4, 1, Humidity);

        Serial.print(F("TEMPERATURE:"));
        Serial.print(Temperature);
        Serial.println(F("#"));
        delay(50);
        Serial.print(F("HUMIDITY:"));
        Serial.print(Humidity);
        Serial.println(F("#"));
        delay(50);
      }
    else
      {
        strcpy(Temperature, "ERR");
        strcpy(Humidity, "ERR");

        Serial.println(F("TEMPERATURE:SENSORERROR#"));
        Serial.println(F("HUMIDITY:SENSORERROR#"));
      }
  }

// ----------------------------------------------------------------------------------------------------------
// 버튼 입력 및 메뉴 제어
// ----------------------------------------------------------------------------------------------------------
void controljudge()
  {
    now[0] = digitalRead(UPpin);
    now[1] = digitalRead(DOWNpin);
    now[2] = digitalRead(RIGHTpin);
    now[3] = digitalRead(LEFTpin);
    for (int i = 0; i < 4; i++)
      {
        if (now[i] != was[i] && now[i] == 0)
          {
            S[i] = 1;
          }
        else
          {
            S[i] = 0;
          }
        was[i] = now[i];
      }
  }

void pinset()
  {
    pinMode(UPpin, INPUT_PULLUP);
    pinMode(DOWNpin, INPUT_PULLUP);
    pinMode(RIGHTpin, INPUT_PULLUP);
    pinMode(LEFTpin, INPUT_PULLUP);
  }

void relayMenuControl()
  {
    switch (menu)
      {
        case 1:
          if (S[0] || S[2])
            {
              doOpen();
            }
          break;

        case 2:
          if (S[0] || S[2])
            {
              doClose();
            }
          break;

        case 3:
          if (S[0])
            {
              digitalWrite(RELAY_CH4, HIGH);
              relayStatus4 = 1;
              lastRelayMessage = "CH4 ON";
              lastRelayTime = millis();
              Serial.println(F("CH4:ON#"));
            }
          else if (S[1])
            {
              digitalWrite(RELAY_CH4, LOW);
              relayStatus4 = 0;
              lastRelayMessage = "CH4 OFF";
              lastRelayTime = millis();
              Serial.println(F("CH4:OFF#"));
            }
          break;
      }

    if (S[3])
      {
        subm = 1;
      }
  }

void buttonRead()
  {
    controljudge();
    switch (subm)
      {
        case 0:
          if (S[2])
            {
              subm = 1;
            }
          break;

        case 1:
          if (S[3])
            {
              subm = 0;
              break;
            }
          if (S[2])
            {
              subm = 2;
              break;
            }
          if (S[0] && menu > 1)
            {
              menu--;
            }
          if (S[1] && menu < 3)
            {
              menu++;
            }
          break;

        case 2:
          relayMenuControl();
          break;
      }
  }

// ----------------------------------------------------------------------------------------------------------
// OLED 디스플레이
// ----------------------------------------------------------------------------------------------------------
void U8G_start()
  {
    u8g.setFont(u8g_font_6x10);
    u8g.setColorIndex(1);
    u8g.setFontPosTop();
  }

int centerXForText(const char *text)
  {
    int w = u8g.getStrWidth(text);
    int x = (128 - w) / 2;
    if (x < 0)
      {
        x = 0;
      }
    return x;
  }

void U8G_test()
  {
    const char *title = "PROGRAM";
    const char *name = firmwareName;

    u8g.firstPage();
    do
      {
        u8g.drawFrame(0, 0, 128, 64);
        u8g.drawLine(8, 30, 119, 30);
        u8g.setFont(u8g_font_6x12);
        u8g.setFontPosTop();
        u8g.drawStr(centerXForText(title), 10, title);

        u8g.setFontPosTop();
        u8g.drawStr(centerXForText(name), 38, name);
      }
    while (u8g.nextPage());
    delay(2000);
  }

void U8G_draw()
  {
    u8g.firstPage();
    do
      {
        u8g.setFont(u8g_font_6x10);
        u8g.setFontPosTop();
        u8g.drawStr(0, 2, "T:");
        u8g.setPrintPos(14, 2);
        u8g.print(Temperature);
        u8g.drawStr(44, 2, "\xb0");
        u8g.drawStr(52, 2, "C");
        u8g.drawStr(70, 2, "H:");
        u8g.setPrintPos(82, 2);
        u8g.print(Humidity);
        u8g.drawStr(112, 2, "%");

        switch (subm)
          {
            case 0:
              u8g.setFont(u8g_font_6x10);
              u8g.setFontPosTop();
              {
                char timeText[12];
                buildDisplayTimeText(timeText);
                u8g.drawStr(0, 14, timeText);
              }

              u8g.drawStr(0, 32, "CH4:");
              u8g.drawStr(32, 32, relayStatus4 ? "ON " : "OFF");
              u8g.drawStr(0, 52, "RIGHT: Menu");
              break;

            case 1:
              u8g.setFont(u8g_font_6x10);
              u8g.setFontPosTop();
              u8g.drawStr(40, 14, "MENU");

              u8g.setFontPosTop();
              u8g.drawStr(0, 22, "UP/DN Move");
              u8g.drawStr(12, 32, "1. DOME OPEN");
              u8g.drawStr(12, 40, "2. DOME CLOSE");
              u8g.drawStr(12, 48, "3. CH4 ON/OFF");
              u8g.drawStr(0, 56, ">=SEL <=BACK");
              u8g.drawStr(0, 32 + (menu - 1) * 8, ">");
              break;

            case 2:
              u8g.setFont(u8g_font_6x10);
              u8g.setFontPosTop();

              switch (menu)
                {
                  case 1:
                    u8g.drawStr(16, 14, "DOME OPEN");
                    u8g.setFont(u8g_font_6x10);
                    u8g.drawStr(0, 32, "D5 CH3 Pulse 500ms");
                    u8g.drawStr(0, 44, "UP/RT:RUN");
                    u8g.drawStr(0, 54, "LEFT: Back");
                    break;

                  case 2:
                    u8g.drawStr(16, 14, "DOME CLOSE");
                    u8g.setFont(u8g_font_6x10);
                    u8g.drawStr(0, 32, "D4 CH2 Pulse 500ms");
                    u8g.drawStr(0, 44, "UP/RT:RUN");
                    u8g.drawStr(0, 54, "LEFT: Back");
                    break;

                  case 3:
                    u8g.drawStr(20, 14, "CH4 CTRL");
                    u8g.setFont(u8g_font_6x10);
                    u8g.drawStr(0, 32, "State:");
                    u8g.drawStr(40, 32, relayStatus4 ? "ON " : "OFF");
                    u8g.drawStr(0, 44, "UP:ON DN:OFF");
                    u8g.drawStr(0, 54, "LEFT: Back");
                    break;
                }
              break;
          }

        if (lastRelayMessage && (millis() - lastRelayTime) < RELAY_MSG_DURATION)
          {
            u8g.setFont(u8g_font_6x10);
            u8g.setFontPosTop();
            u8g.drawBox(0, 46, 128, 14);
            u8g.setColorIndex(0);
            u8g.drawStr(centerXForText(lastRelayMessage), 48, lastRelayMessage);
            u8g.setColorIndex(1);
          }
      }
    while (u8g.nextPage());
  }