#include <Adafruit_Sensor.h>

//SSD1306
//#include <SPI.h>
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

//If being initialized
//#define INIT 1
#define serialno 1203

#define cipherkey 0xA831


//u8g2
/*#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>*/

//u8x8
#include <Arduino.h>
#include <Wire.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

//EEPROM
#include <EEPROM.h>

//DHT22
#include <DHT.h>
#include <DHT_U.h>

//Math for Dew Point calc
#include <math.h>

//Timer for motor operation
#include <TimerOne.h>

#define pinDHT 2
#define pinHT1 5
#define pinHT2 6
#define pinDIR 3
#define pinSTEP 4

#define pinM0 8
#define pinM1 9
#define pinM2 10

#define pinL 0
#define pinU 1
#define pinD 2
#define pinR 3
#define pinOLED 4

#define btnthl 256
#define btnthh 768


#define STEPMAX 99999

#define DHTinterval 1000
#define EEPROMinterval 1000
#define EEPROMstepinterval 5000
#define DisplayInterval 250

#define LOOPEVERY 1

#define fwidth1 6 //font width (size 1)
#define fheight1 8 //font height (size 1)
#define fwidth2 12 //font width (size 2)
#define fheight2 16 //font height (size 2)

#define firmver "1.1.1"



//Display: 21*8
//Temp NNN
//IntelliFocus    > (Step UD)
//IntelliHeater   > (Ch1 Auto) > IntelliHeat [v]
//                             > Manual set  [ ] > 0~100%
//                  (Ch2 ...%) > IntelliHeat [ ]
//                             > Manual set  [v] > 0~100%
//Settings > (IntelliFocus) > Step reset > Confirm
//                          > Autosave pos [on]
//                          > Temp comp [off] > Turn on/off
//                                            > Manage > Temp / Step > Delete > Yes
//                                                                                         > No
//                                            > Save value > (Temp/Step) > Yes
//                                                                           > No
//         > (IntelliHeat)  > Adj power 0~100%
//         > Info > Dew point
//                > Firmware ver.
//                > ASCOM

inline int readBtn() {
  int pressedcnt = 0, pressed = -1;
  for(int btnno = 0; btnno < 4; ++btnno) {
    if (analogRead(btnno) > btnthh) {
      pressed = btnno;
      pressedcnt++;
    }
  }
  if (pressedcnt > 1) {
    return -1; //Multiple btn pressed
  } else if (pressedcnt == 0) {
    return -1; //No btn pressed
  } else {
    return pressed;
  }
}

DHT dht(pinDHT, DHT22);
//Adafruit_SSD1306 display(4);
//U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0);
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

int menudepth = 0;
int menuloc[8];

int motspeed = 1;

unsigned long timeprev = 0;
unsigned long timeprev_EEPROM = 0;
unsigned long timeprev_EEPROM_step = 0;
unsigned long timeprev_display = 0;

float h = 0;
float t = 0;

int btnlast = -1;
int btnsave[10];
int btnsave_long[10];

//Settings in EEPROM
unsigned int HTpower[2]; //0-100 //0, 2
unsigned int intellipower = 0; //4
unsigned int smart[2]; //6, 8
unsigned int savepos = 1; //10
unsigned int tempcomp = 0; //12
//14, 16: SerialNo
unsigned long stepnow = 50000; //256
//

unsigned long laststep = 50000;
long tobemoved = 0;

int holdcount = 0; //To check press&hold
int holdcount_long = 0;

unsigned long lastoperation = 0; //for dimming

bool refresh = true;


#define menus_0 "FocusCtl"
#define menus_1 "HeatCtl"
#define menus_2 "Settings"

#define menus0_0  "Step"

#define menus1_0 "CH1"
#define menus1_1 "CH2"

#define menus10_0 "NanoHeat"
#define menus10_1 "Manual"

#define menus2_0 "FocusSet"
#define menus2_1 "Heat Set"
#define menus2_2 "Info"

#define menus20_0 "ResetStp"
#define menus20_1 "AutSav"
#define menus20_2 "Comp"

#define menus202_0 "Turn"
#define menus202_1 "Manage"
#define menus202_2 "SaveVal"

#define menus21_0 "AdjPower"

#define menus22_0 "DewPoint"
#define menus22_1 "Firmware"
#define menus22_2 "ASCOM"

#define menunull ""

String numberpad(int input) {
  if (0 <= input && input < 10) {
    return "  " + String(input);
  } else if (10 <= input && input < 100) {
    return " " + String(input);
  } else if (100 <= input && input < 1000) {
    return String(input);
  } else if (-10 < input && input < 0) {
    return " " + String(input);
  } else if (-100 < input && input <= -10) {
    return String(input);
  } else {
    return String(input);
  }
}

void writeSerialNo(unsigned int srno) {
  unsigned int srno_encoded = srno ^ cipherkey;
  unsigned int srno_chksum = (srno<<8 | srno>>8) ^ cipherkey ^ cipherkey ^ cipherkey;
  EEPROMWriteInt(14, srno_encoded);
  EEPROMWriteInt(16, srno_chksum);
}

unsigned int readSerialNo() {
  unsigned int srno = EEPROMReadInt(14);
  unsigned int srno_chksum = EEPROMReadInt(16);
  unsigned int chksum_decoded = srno_chksum ^ cipherkey ^ cipherkey ^ cipherkey;
  chksum_decoded = (chksum_decoded<<8 | chksum_decoded>>8);
  srno = srno ^ cipherkey;
  if (srno != chksum_decoded) {
    Serial.println("Memory corrupted!");
    delay(3000);
    return 9999;
  }
  return srno;
}
  
/*void clearSerial(){
  for(int i=0; i<15; i++) Serial.println();
}*/

void printInfo(){
  //display.clearDisplay();
  //display.setTextSize(1);

  //Temperature
  //display.setCursor(0, 0);
  //display.print("Temp");
  //display.setCursor(5*fwidth1, 0);
  //display.print(numberpad((int)t));
  //display.setCursor(8*fwidth1, 0);
  //display.print(".");
  //display.setCursor(9*fwidth1, 0);
  //display.print((int)(t*10)%10);
  //display.setCursor(10*fwidth1, 0);
  //display.print("C");

  //Humidity
  //display.setCursor(13*fwidth1, 0);
  //display.print("Hum");
  //display.setCursor(17*fwidth1, 0);
  //display.print(numberpad((int)h));
  //display.setCursor(20*fwidth1, 0);
  //display.print("%");

  //Heaters
  //display.setCursor(0, 1*fheight1);
  //display.print("HT1");
  //display.setCursor(4*fwidth1, 1*fheight1);
  //display.print(numberpad(HTpower[0]));
  //display.setCursor(7*fwidth1, 1*fheight1);
  //display.print("%");
  //display.setCursor(10*fwidth1, 1*fheight1);
  //display.print("HT2");
  //display.setCursor(14*fwidth1, 1*fheight1);
  //display.print(numberpad(HTpower[1]));
  //display.setCursor(17*fwidth1, 1*fheight1);
  //display.print("%");

  /*clearSerial();
  Serial.print("Temp ");
  Serial.print(numberpad((int)t));
  Serial.print(".");
  Serial.print((int)(t*10)%10);
  Serial.print("C ");
  Serial.print("Hum ");
  Serial.print(numberpad((int)h));
  Serial.print("%");
  Serial.println();
  Serial.print("HT1 ");
  Serial.print(numberpad(HTpower[0]));
  Serial.print("% HT2");
  Serial.print(numberpad(HTpower[1]));
  Serial.print("%");
  Serial.println();*/

  u8x8.clearLine(0);

  u8drawstring(0, 0, "T");
  u8drawstring(1, 0, numberpad((int)t));
  u8drawstring(4, 0, ".");
  u8drawstring(5, 0, String((int)(t*10)%10));
  u8drawstring(6, 0, "C");
  u8drawstring(9, 0, "RH");
  u8drawstring(12, 0, numberpad((int)h));
  u8drawstring(15, 0, "%");

  u8x8.clearLine(1);

  u8drawstring(0, 1, "HT1");
  u8drawstring(3, 1, numberpad(HTpower[0]));
  u8drawstring(6, 1, "%  HT2");
  u8drawstring(12, 1, numberpad(HTpower[1]));
  u8drawstring(15, 1, "%");
  
  
}

void printmenu(String menulist1, String menulist2, String menulist3){
  //printInfo();
  
  //Serial.println(menulist1);
  //Serial.println(menulist2);
  //Serial.println(menulist3);

  u8x8.clearLine(2);
  u8x8.clearLine(3);

  u8drawstring2(0, 2, menulist1);
  u8x8.setInverseFont(1);

  u8x8.clearLine(4);
  u8x8.clearLine(5);
  
  u8drawstring2(0, 4, "        ");
  u8drawstring2(0, 4, menulist2);
  u8x8.setInverseFont(0);

  u8x8.clearLine(6);
  u8x8.clearLine(7);
  
  u8drawstring2(0, 6, menulist3);
  
  //display.setTextSize(2);

  //display.setCursor(0, 1*fheight2);
  //display.print(menulist1);

  //display.setCursor(0, 2*fheight2);
  //display.setTextColor(BLACK, WHITE);
  //display.print(menulist2);

  //display.setCursor(0, 3*fheight2);
  //display.print(menulist3);

  //display.display();

  //display.startscrollright(0x08, 0x0B);
}

void printtext(String text1, String text2, String text3) {
  u8x8.clearLine(2);
  u8x8.clearLine(3);

  u8drawstring2(0, 2, text1);

  u8x8.clearLine(4);
  u8x8.clearLine(5);
  
  u8drawstring2(0, 4, text2);

  u8x8.clearLine(6);
  u8x8.clearLine(7);
  
  u8drawstring2(0, 6, text3);
}

void printadjust(String menuname) {
  //printInfo();
  
  //Serial.println();
  //Serial.println(menuname);
  //Serial.println();

  u8x8.clearLine(2);
  u8x8.clearLine(3);
  
  u8drawstring2(4, 2, "(UP)");
  u8x8.setInverseFont(1);
  
  u8x8.clearLine(4);
  u8x8.clearLine(5);
  
  u8drawstring2(0, 4, "        ");                   
  u8drawstring2(0, 4, menuname);
  u8x8.setInverseFont(0);

  u8x8.clearLine(6);
  u8x8.clearLine(7);
  
  u8drawstring2(2, 6, "(DOWN)");
  //u8drawstring2("");
  

  //display.setTextSize(2);

  //display.setCursor(0, 1*fheight2);
  //

  //display.setCursor(0, 2*fheight2);
  //display.setTextColor(BLACK, WHITE);
  //display.print(menulist2);

  //display.setCursor(0, 3*fheight2);
  //

  //display.display();
}

String getPower(int channel) {
  if (smart[channel] == 1) {
    return String("AUTO");
  } else {
    return String(numberpad(HTpower[channel])) + String("%");
  }
}

inline float getDewPoint() {
  // use global var t, h
  double Cb = 18.678, Cc = 257.14, Cd = 234.5;
  double T = (double)t, RH = (double)h;
  //T = random(-30, 50);
  //RH = random(5, 100);
  double gamma = log(RH / 100. * exp((Cb - (T/Cd)) * (T / (Cc+T))));

  /*Serial.print("T: ");
  Serial.print(T);
  Serial.print(" RH: ");
  Serial.println(RH);
  Serial.print("Dpnt1: ");
  Serial.println((Cc * gamma) / (Cb - gamma));
  Serial.print("Dpnt2: ");
  Serial.println(T - ((100-RH)/5));
  delay(1000);*/
  
  return (float) (Cc * gamma) / (Cb - gamma);
}

void EEPROMWriteInt(int p_address, unsigned int p_value) {
  byte lByte = ((p_value >> 0) & 0xFF);
  byte hByte = ((p_value >> 8) & 0xFF);
  
  EEPROM.write(p_address, lByte);
  EEPROM.write(p_address + 1, hByte);
}

unsigned int EEPROMReadInt(int p_address) {
  byte lByte = EEPROM.read(p_address);
  byte hByte = EEPROM.read(p_address + 1);
  
  return ((lByte << 0) & 0xFF) + ((hByte << 8) & 0xFF00);
}

void EEPROMWriteLong(int p_address, unsigned long p_value) {
  byte b0 = ((p_value >> 0) & 0xFF);
  byte b1 = ((p_value >> 8) & 0xFF);
  byte b2 = ((p_value >> 16) & 0xFF);
  byte b3 = ((p_value >> 24) & 0xFF);
  
  
  EEPROM.write(p_address, b0);
  EEPROM.write(p_address + 1, b1);
  EEPROM.write(p_address + 2, b2);
  EEPROM.write(p_address + 3, b3);
}

unsigned long EEPROMReadLong(int p_address) {
  byte b0 = EEPROM.read(p_address);
  byte b1 = EEPROM.read(p_address + 1);
  byte b2 = EEPROM.read(p_address + 2);
  byte b3 = EEPROM.read(p_address + 3);
  
  
  return ((b0 << 0) & 0xFF) + ((b1 << 8) & 0xFF00) + ((b2 << 16) & 0xFF0000) + ((b3 << 24) & 0xFF000000);
}

void movestepper() {
  if (tobemoved > 0) {
    digitalWrite(pinDIR, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinSTEP, HIGH);
    delayMicroseconds(20);
    digitalWrite(pinSTEP, LOW);
    delayMicroseconds(10);
    tobemoved -= 1;
  } else if (tobemoved < 0) {
    digitalWrite(pinDIR, LOW);
    delayMicroseconds(10);
    digitalWrite(pinSTEP, HIGH);
    delayMicroseconds(20);
    digitalWrite(pinSTEP, LOW);
    delayMicroseconds(10);
    tobemoved += 1;
  }
}


void refreshDisplay(){
  String menutmp[3];

  switch (menudepth) {
    case 0: //depth
      switch (menuloc[0]) { //main menu
        case 0: //IntelliFocus selected
          printmenu(menunull, menus_0, menus_1);
          break;
        case 1: //IntelliHeat selected
          printmenu(menus_0, menus_1, menus_2);
          break;
        case 2: //Settings selected
          printmenu(menus_1, menus_2, menunull);
          break;
      }
      break;
      
    case 1: //depth
      switch (menuloc[0]) {
        case 0: //IntelliFocus
          switch (menuloc[1]) {
            case 0: //Step UD mode
              printadjust(/*String(menus0_0) + " " +*/ String(stepnow));
              break;
          }
          break;
        case 1: //IntelliHeater
          menutmp[0] = menus1_0 + String(" ") + getPower(0);
          menutmp[1] = menus1_1 + String(" ") + getPower(1);
          switch (menuloc[1]) {
            case 0: //Ch 1 selected
              printmenu(menunull, menutmp[0], menutmp[1]);
              break;
            case 1: //Ch 2 selected
              printmenu(menutmp[0], menutmp[1], menunull);
              break;
          }
          break;
        case 2: //Settings
          switch (menuloc[1]) {
            case 0: //IntelliFocus selected
              printmenu(menunull, menus2_0, menus2_1);
              break;
            case 1: //IntelliHeat selected
              printmenu(menus2_0, menus2_1, menus2_2);
              break;
            case 2: //Info selected
              printmenu(menus2_1, menus2_2, menunull);
              break;
          }
      }
      break;
      
    case 2: //depth
      switch (menuloc[0]) {
        case 1: //IntelliHeater
          switch (menuloc[1]) {
            case 0: //Channel 1
              if (smart[0] == 1) {
                menutmp[0] = "Auto   v";
                menutmp[1] = "Manual  ";
              } else {
                menutmp[0] = "Auto    ";
                menutmp[1] = "Manual v";
              }
              switch (menuloc[2]) {
                case 0: //IntelliHeat selected
                  printmenu(menunull, menutmp[0], menutmp[1]);
                  break;
                case 1: //Manual set selected
                  printmenu(menutmp[0], menutmp[1], menunull);
                  break;
              }
              break;
            case 1: //Channel 2
              if (smart[1] == 1) {
                menutmp[0] = "Auto [v]";
                menutmp[1] = "Manual[ ";
              } else {
                menutmp[0] = "Auto [ ]";
                menutmp[1] = "Manual[v";
              }
              switch (menuloc[2]) {
                case 0: //IntelliHeat selected
                  printmenu(menunull, menutmp[0], menutmp[1]);
                  break;
                case 1: //Manual set selected
                  printmenu(menutmp[0], menutmp[1], menunull);
                  break;
              }
              break;
          }
          break;
        case 2: //Settings
          switch (menuloc[1]) {
            case 0: //IntelliFocus
              menutmp[0] = menus20_0;
              if (savepos == 1) {
                menutmp[1] = menus20_1 + String(" Y");
              } else {
                menutmp[1] = menus20_1 + String(" N");
              }
              if (tempcomp == 1) {
                menutmp[2] = menus20_2 + String(" Y");
              } else {
                menutmp[2] = menus20_2 + String(" N");
              }
              switch (menuloc[2]) {
                case 0: //Step reset selected
                  printmenu(menunull, menutmp[0], menutmp[1]);
                  break;
                case 1: //Remember pos selected
                  printmenu(menutmp[0], menutmp[1], menutmp[2]);
                  break;
                case 2: //Temp comp selected
                  printmenu(menutmp[1], menutmp[2], menunull);
                  break;
              }
              break;
            case 1: //IntelliHeat
              printadjust("PWR " + String(intellipower) + "%");
              break;
            case 2: //Info
              float dewpnt = getDewPoint();
              menutmp[0] = String("Dew") + String(numberpad((int)dewpnt)) + "." + String((int)(dewpnt*10)%10);
              menutmp[1] = String("FW ") + firmver;
              menutmp[2] = menus22_2;
              
              switch (menuloc[2]) {
                case 0: //Dew point selected
                  printmenu(menunull, menutmp[0], menutmp[1]);
                  break;
                case 1: //firmware ver selected
                  printmenu(menutmp[0], menutmp[1], menutmp[2]);
                  break;
                case 2: //ASCOM selected
                  printmenu(menutmp[1], menutmp[2], menunull);
                  break;
              }
              break;
          }
      }
      break;
    case 3: //depth
      switch (menuloc[0]) {
        case 1: //IntelliHeater
          switch (menuloc[1]) {
            case 0: //Channel 1
              switch (menuloc[2]) {
                case 1: //Manual set
                  printadjust("CH1 " + String(HTpower[0]) + "%"); //Also uses menus21_0 !!
                  break;
              }
              break;
            case 1: //Channel 2
              switch (menuloc[2]) {
                case 1: //Manual set
                  printadjust("CH2 " + String(HTpower[1]) + "%"); //Also uses menus21_0 !!
                  break;
              }
              break;
          }
          break;
        case 2: //Settings
          switch (menuloc[1]) {
            case 0: //IntelliFocus
              switch (menuloc[2]) {
                case 0: //Step reset
                  printtext("ResetStp", "to 50000", "   >>");
                  break;
                case 2: //Temp comp
                  /*switch (menuloc[3]) {
                    case 0: //Turn on/off selected
                      if (tempcomp == 0) {
                        menutmp[0] = menus202_0 + String(" on");
                      } else {
                        menutmp[0] = menus202_0 + String(" off");
                      }
                      menutmp[1] = menus202_1;
                      menutmp[2] = menus202_2;
                      
                      printmenu(menunull, menutmp[0], menutmp[1]);
                      break;
                    case 1: //Manage selected
                      if (tempcomp == 0) {
                        menutmp[0] = menus202_0 + String(" on");
                      } else {
                        menutmp[0] = menus202_0 + String(" off");
                      }
                      menutmp[1] = menus202_1;
                      menutmp[2] = menus202_2;
                      
                      printmenu(menutmp[0], menutmp[1], menutmp[2]);
                      break;
                    case 2: //Save selected
                      if (tempcomp == 0) {
                        menutmp[0] = menus202_0 + String(" on");
                      } else {
                        menutmp[0] = menus202_0 + String(" off");
                      }
                      menutmp[1] = menus202_1;
                      menutmp[2] = menus202_2;
                      
                      printmenu(menutmp[1], menutmp[2], menunull);
                      break;
                  }*/
                  printtext("Please", "use", "ASCOM!");
                  break;
              }
              break;
          }
          break;
      }
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
  }
}

void writeSettings() {
  int tmp;

  //Save only if 
  if (smart[0] == 0) {
    tmp = EEPROMReadInt(0);
    if (HTpower[0] != tmp) {
      EEPROMWriteInt(0, HTpower[0]);
    }
  }

  if (smart[1] == 0) {
    tmp = EEPROMReadInt(2);
    if (HTpower[1] != tmp) {
      EEPROMWriteInt(2, HTpower[1]);
    }
  }

  tmp = EEPROMReadInt(4);
  if (intellipower != tmp) {
    EEPROMWriteInt(4, intellipower);
  }

  tmp = EEPROMReadInt(6);
  if (smart[0] != tmp) {
    EEPROMWriteInt(6, smart[0]);
  }

  tmp = EEPROMReadInt(8);
  if (smart[1] != tmp) {
    EEPROMWriteInt(8, smart[1]);
  }

  tmp = EEPROMReadInt(10);
  if (savepos != tmp) {
    EEPROMWriteInt(10, savepos);
  }

  tmp = EEPROMReadInt(12);
  if (tempcomp != tmp) {
    EEPROMWriteInt(12, tempcomp);
  }
}


/*void u8g2write(int x, int y, char* str) {
  u8g2.firstPage();
  do {
    u8g2.drawStr(x, y, str);
  } while ( u8g2.nextPage() );
}
void u8g2write(int x, int y, String str) {
  u8g2.firstPage();
  char tmp[20] = {0,};
  str.toCharArray(tmp, str.length());
  do {
    u8g2.drawStr(x, y, tmp);
  } while ( u8g2.nextPage() );
}*/

void u8drawstring(int x, int y, char* str) {
  u8x8.drawString(x, y, str);
}

void u8drawstring(int x, int y, String str) {
  char tmp[20] = {0,};
  str.toCharArray(tmp, str.length()+1);
  u8x8.drawString(x, y, tmp);
}

inline void u8drawstring2(int x, int y, char* str) {
  u8x8.draw2x2String(x, y, str);
}

void u8drawstring2(int x, int y, String str) {
  char tmp[15] = {0,};
  str.toCharArray(tmp, str.length()+1);
  u8x8.draw2x2String(x, y, tmp);
}

long powint(long a, long b) {
  long result = 1;
  while (b--) result*=a;
  return result;
}

void ASCOM() {
  byte in[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int i = 0;
  do {
    delay(1); //wait to receive another byte(s)
    in[i] = Serial.read();
    if (in[i] == '#') break; //end of command
    if (i>=18) break; //prevent overflow
    i++;
  } while (Serial.available() > 0);
  
  if (in[0] == ':' && in[1] == 'F') {
    Serial.print(":");
    
    if (in[2] == '?') { //ismoving
      if (tobemoved > 0) {
        Serial.print("1");
      } else {
        Serial.print("0");
      }
    } else if (in[2] == 'M') { //Move
      int digits = 0;
      unsigned long stepmove = 0, stepmove_chk = 0;
      int j, k;
      //in: (:FM(sign)(steps)#)
      for (j = i-1; in[j] != '$'; j--) {
        stepmove += (int)(in[j]-48) * powint(10, digits);
        digits++;
      }
      digits = 0;
        for (k = j-1; k>2; k--) {
        stepmove_chk += (int)(in[k]-48) * powint(10, digits);
        digits++;
       }
      
      //if (in[3] == 49) stepmove *= -1;

      
      tobemoved += (long)stepmove - (long)laststep;
      stepnow = stepmove;
      refresh = true;
      laststep = stepnow;
      Serial.print("M");
      Serial.print(stepmove);


       if (stepmove == stepmove_chk) {
         tobemoved += (long)stepmove - (long)laststep;
         stepnow = stepmove;
         refresh = true;
         laststep = stepnow;
         Serial.print("M");
         Serial.print(stepmove);
        } else {
          Serial.print(stepmove);
          Serial.print("!=");
         Serial.print(stepmove_chk);
        }
 
      
    } else if (in[2] == 'G') { //Get position
      Serial.print(stepnow);
    } else if (in[2] == 'T') { //Get Temperature
      Serial.print(t);
    } else if (in[2] == 'H') { //Halt
      laststep -= tobemoved;
      stepnow -= tobemoved;
      tobemoved = 0;
      refresh = true;
      Serial.print("H");
    }

    Serial.print("#");
  } else {
    Serial.print("$");
  }
}

void setup() {
  //Initialize the display first
  /*display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 3);
  display.println("IntelliFocus");
  display.setCursor(3, 5);
  display.println("Initializing...");
  display.setCursor(4, 6);
  display.print("Firmware v");
  display.setCursor(14, 6);
  display.print(firmver);
  display.display();*/

  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_victoriabold8_r);
  u8drawstring(1, 1, "Nanofocus 1.0");
  u8drawstring(0, 2, "Initializing...");
  u8drawstring(1, 4, "Firmware v");
  u8drawstring(11, 4, firmver);
  u8drawstring(2, 5, "Serial");
  u8drawstring(9, 5, String(readSerialNo()));


  /*u8g2.begin();
  u8g2.setFont(u8g2_font_helvR10_tr);
  
  u8g2.clearBuffer();
  u8g2write(20, 18, "Nanofocus 1.0");
  u8g2write(20, 32, "Initializing...");
  u8drawstring(20, 46, "Firmware v");
  u8drawstring(80, 46, firmver);
  u8drawstring(20, 60, "Serial");
  u8drawstring(65, 60, String(readSerialNo()));
  
  u8g2.sendBuffer();*/
  
  
  pinMode(pinHT1, OUTPUT);
  pinMode(pinHT2, OUTPUT);
  analogWrite(pinHT1, 0);
  analogWrite(pinHT2, 0);

  pinMode(pinDIR, OUTPUT);
  pinMode(pinSTEP, OUTPUT);

  pinMode(pinM0, OUTPUT);
  pinMode(pinM1, OUTPUT);
  pinMode(pinM2, OUTPUT);

  //1/2 microstep
  digitalWrite(pinM0, HIGH);
  digitalWrite(pinM1, HIGH);
  digitalWrite(pinM2, LOW);

/// 확인필요
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
/// 확인필요
  
  Serial.begin(9600);
  dht.begin();
  u8x8.setContrast(255);
  
#ifdef INIT
 // if (EEPROM.read(511) == 0xFF) {
    EEPROMWriteInt(0, 50);
    EEPROMWriteInt(2, 50);
    EEPROMWriteInt(4, 80);
    EEPROMWriteInt(6, 1);
    EEPROMWriteInt(8, 1);
    EEPROMWriteInt(10, 1);
    EEPROMWriteInt(12, 0);
    EEPROMWriteLong(256, 45000);
    writeSerialNo(serialno);
    EEPROM.write(511, 0x00);
 // }
#endif

  //Serial.println("Initializing...");
  //Serial.print("Serial ");
  //Serial.println(readSerialNo());
  
  HTpower[0] = EEPROMReadInt(0);
  HTpower[1] = EEPROMReadInt(2);
  intellipower = EEPROMReadInt(4);
  smart[0] = EEPROMReadInt(6);
  smart[1] = EEPROMReadInt(8);
  savepos = EEPROMReadInt(10);
  tempcomp = EEPROMReadInt(12);
  
  if (savepos == 1) stepnow = EEPROMReadLong(256);
  laststep = stepnow;

  delay(1000); //needed to make sure to not enter the if for overflow of millis()
  
  for (int i = 0; i < 8; i++) {
    u8x8.clearLine(i);
  }

  Timer1.initialize();
  Timer1.attachInterrupt(movestepper, 1000);
}

void loop() {
  unsigned long timenow = millis();
  if (timenow < 600) { //millis() overflowed
    timeprev = 0;
    timeprev_EEPROM = 0;
    timeprev_EEPROM_step = 0;
    timeprev_display = 0;
    lastoperation = 0;
  }
  if (timenow - timeprev >= DHTinterval && btnlast == -1) {
    h = dht.readHumidity();
    t = dht.readTemperature();
    timeprev = timenow;

    if (isnan(h) || isnan(t)) {
      //Failed to read DHT
      //Serial.println("DHT22 failed");
    } else {
      //Serial.print("Temperature: ");
      //Serial.println(t);
      //Serial.print("Humidity: ");
      //Serial.println(h); 
      printInfo();
    }
  }

  if (timenow - timeprev_EEPROM >= EEPROMinterval) {
    timeprev_EEPROM = timenow;
    writeSettings();
  }

  if (timenow - timeprev_EEPROM_step >= EEPROMstepinterval) {
    timeprev_EEPROM_step = timenow;
    unsigned long tmp;
    if (savepos == 1 && tobemoved == 0) {
      tmp = EEPROMReadLong(256);
      if (stepnow != tmp) {
        EEPROMWriteLong(256, stepnow);
      }
    }
  }

  if ((timenow - timeprev_display >= DisplayInterval &&
  refresh == true) || (menuloc[0] == 2 && menuloc[1] == 2 && timenow - timeprev_display >= DHTinterval)) /* Info menu (to update dewpnt) */ {
    timeprev_display = timenow;
    refreshDisplay();
    refresh = false;
  }

  if (timenow - lastoperation > 10000) {
    u8x8.setContrast(0);
  }

  /*if (refresh == true && ) {
    //refreshDisplay();
    refresh = false;
  }*/

  int btnnow = readBtn();

  if (btnnow != -1) {
    refresh = true;
    lastoperation = timenow;
    u8x8.setContrast(255);
    lastoperation = millis();
  }

  if (btnnow == pinL) {
    //Serial.println("Button L");
    if (btnnow != btnlast) {
      if (menudepth > 0) {
        menudepth --;
      }
    }
  }

  if (btnnow == pinU) {
    //Serial.println("Button U");
    if (btnnow != btnlast) {
      switch (menudepth) {
        case 0://Root menu
          if (menuloc[0] > 0) menuloc[0] -= 1;
          break;
        case 1: //Depth 1
          switch (menuloc[0]) {
            case 0: //IntelliFocus
              if (stepnow < STEPMAX) stepnow = stepnow + 1;
              break;
            case 1: //IntelliHeater
              if (menuloc[1] > 0) menuloc[1] -= 1;
              break;
            case 2: //Settings
              if (menuloc[1] > 0) menuloc[1] -= 1;
              break;
          }
          break;
        case 2: //Depth 2
          switch (menuloc[0]) {
            case 1: //IntelliHeater
              
              if (menuloc[2] > 0) menuloc[2] -= 1;
              break;
            case 2: //Settings
              switch (menuloc[1]) {
                case 0: //IntelliFocus
                  if (menuloc[2] > 0) menuloc[2] -= 1;
                  break;
                case 1: //IntelliHeat
                  if (intellipower < 100) intellipower += 1;
                  break;
                case 2: //Info
                  if (menuloc[2] > 0) menuloc[2] -= 1;
                  break;
              }
          }
          break;
        case 3: //Depth 3
          switch(menuloc[0]) {
            case 1: //IntelliHeater
              switch (menuloc[1]) {
                case 0: //Ch1
                  if (HTpower[0] < 100) HTpower[0] += 1;
                  break;
                case 1: //Ch2
                  if (HTpower[1] < 100) HTpower[1] += 1;
                  break;
              }
              break;
            case 2: //Settings
              if (menuloc[0] == 2 && menuloc[1] == 0 && menuloc[2] == 2) { //Temp comp
                //if (menuloc[3] > 0) menuloc[3] -= 1;
              }
              break;
          }
          break;
        /*case 4: //Depth 4
          if (menuloc[0] == 2 && menuloc[1] == 0 && menuloc[2] == 2 && menuloc[3] == 1) { //Manage temp comp
            if(menuloc[4] > 0) menuloc[4] -= 1;
          }
          break;*/
      }
    }
  }

  if (btnnow == pinD) {
    //Serial.println("Button D");
    if (btnnow != btnlast) {
      switch (menudepth) {
        case 0://Root menu
          if (menuloc[0] < 2) menuloc[0] += 1;
          break;
        case 1: //Depth 1
          switch (menuloc[0]) {
            case 0: //IntelliFocus
              if (stepnow > 0) stepnow = stepnow - 1;
              break;
            case 1: //IntelliHeater
              if (menuloc[1] < 1) menuloc[1] += 1;
              break;
            case 2: //Settings
              if (menuloc[1] < 2) menuloc[1] += 1;
              break;
          }
          break;
        case 2: //Depth 2
          switch (menuloc[0]) {
            case 1: //IntelliHeater
              
              if (menuloc[2] < 1) menuloc[2] += 1;
              break;
            case 2: //Settings
              switch (menuloc[1]) {
                case 0: //IntelliFocus
                  if (menuloc[2] < 2) menuloc[2] += 1;
                  break;
                case 1: //IntelliHeat
                  if (intellipower > 0) intellipower -= 1;
                  break;
                case 2: //Info
                  if (menuloc[2] < 2) menuloc[2] += 1;
                  break;
              }
          }
          break;
        case 3: //Depth 3
          switch(menuloc[0]) {
            case 1: //IntelliHeater
              switch (menuloc[1]) {
                case 0: //Ch1
                  if (HTpower[0] > 0) HTpower[0] -= 1;
                  break;
                case 1: //Ch2
                  if (HTpower[1] > 0) HTpower[1] -= 1;
                  break;
              }
              break;
            case 2: //Settings
              if (menuloc[0] == 2 && menuloc[1] == 0 && menuloc[2] == 2) { //Temp comp
                //if (menuloc[3] < 0) menuloc[3] -= 1;
              }
              break;
          }
          break;
        /*case 4: //Depth 4
          if (menuloc[0] == 2 && menuloc[1] == 0 && menuloc[2] == 2 && menuloc[3] == 1) { //Manage temp comp
            if(menuloc[4] > 0) menuloc[4] -= 1;
          }
          break;*/
      }
    }
  }

  if (btnnow == pinR) {
    //Serial.println("Button R");
    if (btnnow != btnlast) {
      switch (menudepth) {
        case 0:
          menudepth = menudepth + 1;
          menuloc[1] = 0;
          break;
        case 1:
          switch (menuloc[0]) {
            case 1:
            case 2:
              menudepth = menudepth + 1;
              menuloc[2] = 0;
              break;
          }
          break;
        case 2:
          if (menuloc[0] == 1 && menuloc[2] == 0) { //IntelliHeater Auto
            smart[menuloc[1]] = 1;
          } else if (menuloc[0] == 1 && menuloc[2] == 1) { //IntelliHeater Manual
            menudepth += 1;
            smart[menuloc[1]] = 0;
            menuloc[3] = 0;
          } else if (menuloc[0] == 2 && menuloc[1] == 0 && menuloc[2] == 0) { //Step reset
            menudepth += 1;
            menuloc[3] = 0;
          } else if (menuloc[0] == 2 && menuloc[1] == 0 && menuloc[2] == 1) { //Save pos
            savepos = 1-savepos;
          } else if (menuloc[0] == 2 && menuloc[1] == 0 && menuloc[2] == 2) { //Temp comp
            menudepth += 1;
            menuloc[3] = 0;
          }
          break;
        case 3:
          if (menuloc[0] == 2 && menuloc[1] == 0 && menuloc[2] == 0) { //Step reset
            stepnow = 50000;
            laststep = 50000;
            tobemoved = 0;
            menuloc[0] = 0;
            menudepth = 1;
          }
          break;
      }
    }
  }

  holdcount = holdcount + 10;
  if (holdcount > 50) {
    holdcount = 0;
    for (int i = 0; i < 9; ++i) {
      btnsave[i] = btnsave[i+1];
    }
    btnsave[9] = btnnow;
  }
  btnlast = btnnow;

  //Check press & hold
  bool diff = false;
  for (int i = 5; i < 10; ++i) {
    if (btnsave[0] != btnsave[i]) diff = true;
  }
  if (diff == false && btnsave[0] == btnlast) {

    if (menudepth == 1 && menuloc[0] == 0) {
      if (btnlast == pinU) {
        if (stepnow < STEPMAX) stepnow += 1;
      }
      if (btnlast == pinD) {
        if (stepnow > 0) stepnow -= 1;
      }
    }
    
    if (menudepth == 3 && menuloc[0] == 1 && menuloc[1] == 0) {
      if (btnlast == pinU) {
        if (HTpower[0] < 100) HTpower[0] += 1;
      }
      if (btnlast == pinD) {
        if (HTpower[0] > 0) HTpower[0] -= 1;
      }
    }

    if (menudepth == 3 && menuloc[0] == 1 && menuloc[1] == 1) {
      if (btnlast == pinU) {
        if (HTpower[1] < 100) HTpower[1] += 1;
      }
      if (btnlast == pinD) {
        if (HTpower[1] > 0) HTpower[1] -= 1;
      }
    }

    if (menudepth == 2 && menuloc[0] == 2 && menuloc[1] == 1) {
      if (btnlast == pinU) {
        if (intellipower < 100) intellipower += 1;
      }
      if (btnlast == pinD) {
        if (intellipower > 0) intellipower -= 1;
      }
    }
  }

  //check longer press & hold
  holdcount_long = holdcount_long + 10;
  if (holdcount_long > 500) {
    holdcount_long = 0;
    for (int i = 0; i < 9; ++i) {
      btnsave_long[i] = btnsave_long[i+1];
    }
    btnsave_long[9] = btnnow;
  }

  diff = false;
  for (int i = 5; i < 10; ++i) {
    if (btnsave_long[0] != btnsave_long[i]) diff = true;
  }
  if (diff == false && btnsave_long[0] == btnlast) {
    if (menudepth == 1 && menuloc[0] == 0) {
      if (btnlast == pinU) {
        if (stepnow < STEPMAX-6) stepnow += 6;
      }
      if (btnlast == pinD) {
        if (stepnow > 0+6) stepnow -= 6;
      }
    }
  }

  //Motor output
  //movestepper(stepnow - laststep);
  tobemoved += (long)stepnow - (long)laststep;
  //Serial.println(stepnow);
  laststep = stepnow;


  //Auto heater
  float dewpoint = getDewPoint();
  float tempdelta = t - dewpoint-2.;
  
  if (tempdelta <= 1) tempdelta = 1.;
  float output = 2./(tempdelta);
  output -= (t-10)/100;
  output *= intellipower;
  if (output >= 100) output = 100;
  if (output <= 0) output = 0;

  if (smart[0] == 1) {
    HTpower[0] = (int)output;
  }
  if (smart[1] == 1) {
    HTpower[1] = (int)output;
  }

  //Heater output
  analogWrite(pinHT1, map(HTpower[0], 0, 100, 0, 255));
  analogWrite(pinHT2, map(HTpower[1], 0, 100, 0, 255));

  if (Serial.available() > 0) {
    ASCOM();
  }
  
  
  delay(LOOPEVERY);
}


//0/1-TXRX (UART)
//2 - DHT22
//5 6 - IRF1010E
//3 4 - DRV8835
//
//a0 a1 a2 a3 - btn
//a4 a5 - OLED (I2C)
