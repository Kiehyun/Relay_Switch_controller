public void ch1_on() {
  if (connectedSerial) {
      serial.write('q');  println("type 'q'");  status_text("Ch1 on");   delay(30); 
      twitter_send("The Camera is turned on for captuering anallemma!! This is a tweet sent from SRCROOF_R03!! ");
  }
if (isPressedCh1Button == false) {
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController("on1/off1")).setState(true);
  messageBoxResult = -1;
}
}

public void ch1_off() {
  createModalDialog("Are you sure to turn Anallemma Camera Power off?");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('a'); println("type 'a'"); status_text("Ch1 off"); delay(30);
  }
  if (isPressedCh1Button == true) {
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController("on1/off1")).setState(false);
  messageBoxResult = -1;
  }
}


public void One_shot() { //One shot
  if (connectedSerial) {
    serial.write('x'); 
    println("type 'x'"); 
    status_text("One shot"); 
    delay(50);
  }
}

public void Bulb_on() { //Bulb shot start
  if (connectedSerial) {
    serial.write('w'); 
    println("type 'w'"); 
    status_text("Bulb shot on");
  }
}

public void Bulb_off() { //Bulb shot stop
  if (connectedSerial) {
    serial.write('s');
    println("type 's'");
    status_text("Bulb shot off");
  }
}

public void ch3_on() {
  if (connectedSerial) {
    serial.write('e'); status_text("Ch3 on"); delay(50);
  }
  if (isPressedCh3Button == false) {
  isPressedCh3Button = !isPressedCh3Button;
  ((Toggle)cp5.getController("on3/off3")).setState(true);
  messageBoxResult = -1;
  }
}

public void ch3_off() {
  createModalDialog("Ch3 off");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('d'); 
    println("type 'd'");
    status_text("Ch3 off");
  }
  if (isPressedCh3Button == true) {
  isPressedCh3Button = !isPressedCh3Button;
  ((Toggle)cp5.getController("on3/off3")).setState(false);
  messageBoxResult = -1;
  }
}

public void ch4_on() {
  if (connectedSerial) {
    serial.write('r'); 
    println("type 'r'");
    status_text("Ch4 on");
  }
  if (isPressedCh4Button == false) {
  isPressedCh4Button = !isPressedCh4Button;
  ((Toggle)cp5.getController("on4/off4")).setState(true);
  messageBoxResult = -1;
  }
}

public void ch4_off() {
  createModalDialog("Are you sure to turn CCD Power off?");
  if (messageBoxResult >= 1)
    return;
  if (connectedSerial) {
    serial.write('f'); 
    println("type 'f'");
    status_text("Ch4 off");
  }
  if (isPressedCh4Button == true) {
  isPressedCh4Button = !isPressedCh4Button;
  ((Toggle)cp5.getController("on4/off4")).setState(false);
  messageBoxResult = -1;
  }
}

public void ch5_on() {
  if (connectedSerial) {
    serial.write('t'); 
    println("type 't'");
    status_text("Ch5 on");
  }
  if (isPressedCh5Button == false) {
  isPressedCh5Button = !isPressedCh5Button;
  ((Toggle)cp5.getController("on5/off5")).setState(true);
  messageBoxResult = -1;
  }
}

public void ch5_off() {
  createModalDialog("Are you sure to turn Motor focuser Power off?");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('g'); println("type 'g'"); status_text("Ch5 off");
  }
  if (isPressedCh5Button == true) {
  isPressedCh5Button = !isPressedCh5Button;
  ((Toggle)cp5.getController("on5/off5")).setState(false);
  messageBoxResult = -1;
  }
}
public void ch6_on() {
  createModalDialog("Are you sure to open roof?");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('y'); 
    println("type 'y'"); 
    status_text("Roof open");  
    delay(50);
  twitter_send("The SRCROOF is opened!! This is a tweet sent from SRCROOF_R03!! ");
  }
  if (isPressedroofButton == false) {
  isPressedroofButton = !isPressedroofButton;
  ((Toggle)cp5.getController("opend/closed")).setState(true);
  messageBoxResult = -1;
  }
}

public void ch6_on_on() {
  if (connectedSerial) {
    serial.write('y'); println("type 'y'"); status_text("Roop Open"); delay(30); 
    twitter_send("The SRCROOF is opened autometically!! This is a tweet sent from SRCROOF_R03!! ");
  }
    if (isPressedroofButton == false) {
  isPressedroofButton = !isPressedroofButton;
  ((Toggle)cp5.getController("opend/closed")).setState(true);
}
}

public void res6_on() {
  if (connectedSerial) {
    println("type 'Res6 on'"); status_text("Res6 on");
    twitter_send("The SRCROOF open reservation is on. !! This is a tweet sent from SRCROOF_R03!! ");
    delay(50);
  }
  if (isPressedCh6Button == false) {
  isPressedCh6Button = !isPressedCh6Button;
  ((Toggle)cp5.getController("on6/off6")).setState(true);
  }
}

public void res6_off() {
  if (connectedSerial) {
    println("type 'Res6 off'"); status_text("Res6 off"); delay(30);
    twitter_send("The SRCROOF open reservation is off. !! This is a tweet sent from SRCROOF_R03!! ");
  }
  if (isPressedCh6Button == true) {
  isPressedCh6Button = !isPressedCh6Button;
  ((Toggle)cp5.getController("on6/off6")).setState(false);
  }
}

public void ch7_on() {
  createModalDialog("Are you sure to close roof?");
  if (connectedSerial) {
    serial.write('u'); println("type 'u'"); status_text("Roof close");
twitter_send("The SRCROOF is closed!! This is a tweet sent from SRCROOF_R01!! ");
  }
if (isPressedroofButton == true) {
  isPressedroofButton = !isPressedroofButton;
  ((Toggle)cp5.getController("opend/closed")).setState(false);
   messageBoxResult = -1;
  }
}
public void ch7_on_on() {
  if (connectedSerial) {
    serial.write('u'); println("type 'u'"); delay(20);
    serial.write('u'); println("type 'u'"); delay(20);
    status_text("Roop Close");
  twitter_send("The SRCROOF is closed automatically!! This is a tweet sent from SRCROOF_R03!! ");
  }
  if (isPressedroofButton == true) {
    isPressedroofButton = !isPressedroofButton;
  ((Toggle)cp5.getController("opend/closed")).setState(false);
  }
  }
  
public void res7_on() {
  if (connectedSerial) {
    println("type 'Res7 on'"); status_text("Res7 on");
    twitter_send("The SRCROOF close reservation is on. !! This is a tweet sent from SRCROOF_R03!! ");
  }
  if (isPressedCh7Button == false) {
  isPressedCh7Button = !isPressedCh7Button;
  ((Toggle)cp5.getController("on7/off7")).setState(true);
  }
  messageBoxResult = -1;
}

public void res7_off() {
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    println("type 'Res7 off'"); status_text("Res7 off");
    twitter_send("The SRCROOF close reservation is off. !! This is a tweet sent from SRCROOF_R03!! ");
  }
  if (isPressedCh7Button == true) {
  isPressedCh7Button = !isPressedCh7Button;
  ((Toggle)cp5.getController("on7/off7")).setState(false);
  }
  messageBoxResult = -1;
}

public void res2_on() {
  if (connectedSerial) {
    println("type 'Res2 on'"); status_text("Res2 on");
    twitter_send("The Anallemma auto shutter is on. !! This is a tweet sent from SRCROOF_R03!! ");
  }
  if (isPressedCh22Button == false) {
  isPressedCh22Button = !isPressedCh22Button;
  ((Toggle)cp5.getController("on22/off22")).setState(true);
  }
  messageBoxResult = -1;
}

public void res2_off() {
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    println("type 'Res2 off'");  status_text("Res2 off");
    twitter_send("The Anallemma auto shutter is off. !! This is a tweet sent from SRCROOF_R03!! ");
  }
  if (isPressedCh22Button == true) {
  isPressedCh22Button = !isPressedCh22Button;
  ((Toggle)cp5.getController("on22/off22")).setState(false);
  }
  messageBoxResult = -1;
}

public void ch8_on() {
  if (connectedSerial) {
    serial.write('i'); println("type 'i'");
    status_text("Roof stop");
  }
}

void status_text(String status_message) {
  fill(255);
  rect(status_text_x-50, 15, 150, 25);
  textSize(11);
  fill(0);
  text(status_message, status_text_x, status_text_y);
}