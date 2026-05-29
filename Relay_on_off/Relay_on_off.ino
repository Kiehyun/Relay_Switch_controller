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
#define serialno 001
#define firmver "1.0b"
#define boardtype 1   //select boardtype....

#if boardtype==1  //standrad  4ch
#define relayPin1  10
#define relayPin2  8
#define relayPin3  6
#define relayPin4  4
#define relayPin5  9      //only 8ch
#define relayPin6  10     //only 8ch
#define relayPin7  11     //only 8ch
#define relayPin8  12     //only 8ch

#elif boardtype==2  //njp  4ch
#define relayPin1  9
#define relayPin2  10
#define relayPin3  11
#define relayPin4  12
#define relayPin5  5      //only 8ch
#define relayPin6  6      //only 8ch
#define relayPin7  7      //only 8ch
#define relayPin8  8      //only 8ch

#elif boardtype==3  //standrad  8ch
#define relayPin1  5
#define relayPin2  6
#define relayPin3  7
#define relayPin4  8
#define relayPin5  9
#define relayPin6  10
#define relayPin7  11
#define relayPin8  12
#endif

int statusPin1 = 0;
int statusPin2 = 0;
int statusPin3 = 0;
int statusPin4 = 0;
int statusPin5 = 0;
int statusPin6 = 0;
int statusPin7 = 0;
int statusPin8 = 0;
int delayTime = 100;

char Key1_0 = 'q';
char Key1_1 = 'a';
char Key2_0 = "w";
char Key2_1 = "s";
char Key3_0 = "q";
char Key3_1 = "d";
char Key4_0 = "r";
char Key4_1 = "f";
char Key5_0 = 't';
char Key5_1 = 'g';
char Key6_0 = "y";
char Key6_1 = "h";
char Key7_0 = "u";
char Key7_1 = "j";
char Key8_0 = "i";
char Key8_1 = "k";

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);
  pinMode(relayPin5, OUTPUT);      //only 8ch
  pinMode(relayPin6, OUTPUT);      //only 8ch
  pinMode(relayPin7, OUTPUT);      //only 8ch
  pinMode(relayPin8, OUTPUT);      //only 8ch
}

void loop() 
{
while(!Serial.available()); //시리얼로 데이터 들어올 때까지 대기
char com = Serial.read(); //들어온 데이터를 변수 com에 저장

if (boardtype==1 or boardtype==2) {
    switch(com) {
      case 'q':
      digitalWrite(relayPin1, 1) ;  statusPin1 = 1;
      delay(delayTime);
       break;
       case 'a':
       digitalWrite(relayPin1, 0) ; statusPin1 = 0;
      delay(delayTime);
       break;
       case 'w':
      digitalWrite(relayPin2, 1) ; statusPin2 = 1;
      delay(delayTime);
       break;
       case 's':
       digitalWrite(relayPin2, 0) ; statusPin2 = 0;
      delay(delayTime);
       break;
       case 'e':
       digitalWrite(relayPin3, 1) ; statusPin3 = 1;
      delay(delayTime);
       break;
       case 'd':
       digitalWrite(relayPin3, 0) ; statusPin3 = 0;
      delay(delayTime);
       break;
       case 'r':
       digitalWrite(relayPin4, 1) ; statusPin4 = 1;
      delay(delayTime);
       break;
       case 'f':
       digitalWrite(relayPin4, 0) ; statusPin4 = 0;
      delay(delayTime);
       break;
    }
  }
else {
  switch(com) {
   case 'q':
   digitalWrite(5, 1) ;  statusPin1 = 1;
   delay(100);
   break;
   case 'a':
   digitalWrite(5, 0) ;  statusPin1 = 0;
   delay(100);
   break;
   case 'w':
   digitalWrite(6, 1) ;   statusPin2 = 1;
   delay(100);
   break;
   case 's':
   digitalWrite(6, 0) ;  statusPin2 = 0;
   delay(5);
   break;
   case 'x':
   digitalWrite(6, 1) ;   statusPin2 = 1;
   delay(100);
   digitalWrite(6, 0) ;   statusPin2 = 0;
   delay(100);
   break;
   case 'e':
   digitalWrite(7, 1) ;   statusPin3 = 1;
   delay(100);
   break;
   case 'd':
   digitalWrite(7, 0) ; statusPin3 = 0;
   delay(100);
   break;
   case 'r':
   digitalWrite(8, 1) ;   statusPin4 = 1;
   delay(100);
   break;
   case 'f':
   digitalWrite(8, 0) ;  statusPin4 = 0;
   delay(100);
   break;

   case 't':
   digitalWrite(9, 1) ;   statusPin5 = 1;
   delay(100);
   break;
   case 'g':
   digitalWrite(9, 0) ;  statusPin5 = 0;
   delay(100);
   break;
   case 'y':
   digitalWrite(10, 1) ;   statusPin6 = 1;
   delay(100);
   digitalWrite(10, 0) ;  statusPin6 = 0;
   delay(10);
   break;
   case 'h':
   digitalWrite(10, 0) ; statusPin6 = 0;
   delay(100);
   break;
   case 'u':
   digitalWrite(11, 1) ;   statusPin7 = 1;
   delay(100);
   digitalWrite(11, 0) ;  statusPin7 = 0;
   delay(10);
   break;
   case 'j':
   digitalWrite(11, 0) ;  statusPin7 = 0;
   delay(100);
   break;
   case 'i':
   digitalWrite(12, 1) ; statusPin8 = 1;
   delay(500);
   digitalWrite(12, 0) ; statusPin8 = 0;
   delay(10);
   break;
   case 'k':
   digitalWrite(12, 0) ; statusPin8 = 0;
   delay(500);
   break;
}
}

Serial.print("(");
Serial.print(statusPin1);
Serial.print(",");
Serial.print(statusPin2);
Serial.print(",");
Serial.print(statusPin3);
Serial.print(",");
Serial.print(statusPin4);
Serial.print(",");
Serial.print(statusPin5);
Serial.print(",");
Serial.print(statusPin6);
Serial.print(",");
Serial.print(statusPin7);
Serial.print(",");
Serial.print(statusPin8);
Serial.println(")");
Serial.print(com);
}
