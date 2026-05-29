public void ch1_on() {
  if (connectedSerial) {
      serial.write('q');  println("type 'q'");  status_text("Ch1 on");   delay(20); 
      serial.write('q');  println("type 'q'");  status_text("Ch1 on");   delay(20);
      twitter_send("The Ch1 is turned on !! This is a tweet sent from SRCROOF_R04!! ");
  }
if (isPressedCh1Button == false) {
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController("on1/off1")).setState(true);
  messageBoxResult = -1;
}
}

public void ch1_off() {
  createModalDialog("Are you sure to turn Ch1 off?");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('a'); println("type 'a'"); status_text("Ch1 off"); delay(20);
    serial.write('a'); println("type 'a'"); status_text("Ch1 off"); delay(20);
    twitter_send("The Ch1 is turned off!! This is a tweet sent from SRCROOF_R04!! ");
  }
  if (isPressedCh1Button == true) {
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController("on1/off1")).setState(false);
  messageBoxResult = -1;
  }
}

public void ch2_on() {
  if (connectedSerial) {
      serial.write('w');  println("type 'w'");  status_text("Ch2 on");   delay(20); 
      serial.write('w');  println("type 'w'");  status_text("Ch2 on");   delay(20);
      twitter_send("The Ch2 is turned on !! This is a tweet sent from SRCROOF_R04!! ");
  }
if (isPressedCh2Button == false) {
  isPressedCh2Button = !isPressedCh2Button;
  ((Toggle)cp5.getController("on2/off2")).setState(true);
  messageBoxResult = -1;
}
}

public void ch2_off() {
  createModalDialog("Are you sure to turn Ch2 off?");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('s'); println("type 's'"); status_text("Ch2 off"); delay(20);
    serial.write('s'); println("type 's'"); status_text("Ch2 off"); delay(20);
    twitter_send("The Ch2 is turned off!! This is a tweet sent from SRCROOF_R04!! ");
  }
  if (isPressedCh2Button == true) {
  isPressedCh2Button = !isPressedCh2Button;
  ((Toggle)cp5.getController("on2/off2")).setState(false);
  messageBoxResult = -1;
  }
}

public void ch3_on() {
  if (connectedSerial) {
      serial.write('e');  println("type 'e'");  status_text("Ch3 on");   delay(20); 
      serial.write('e');  println("type 'e'");  status_text("Ch3 on");   delay(20);
      twitter_send("The Ch3 is turned on !! This is a tweet sent from SRCROOF_R04!! ");
  }
if (isPressedCh3Button == false) {
  isPressedCh3Button = !isPressedCh3Button;
  ((Toggle)cp5.getController("on3/off3")).setState(true);
  messageBoxResult = -1;
}
}

public void ch3_off() {
  createModalDialog("Are you sure to turn Ch3 off?");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('d'); println("type 'd'"); status_text("Ch3 off"); delay(20);
    serial.write('d'); println("type 'd'"); status_text("Ch3 off"); delay(20);
    twitter_send("The Ch3 is turned off!! This is a tweet sent from SRCROOF_R04!! ");
  }
  if (isPressedCh3Button == true) {
  isPressedCh3Button = !isPressedCh3Button;
  ((Toggle)cp5.getController("on3/off3")).setState(false);
  messageBoxResult = -1;
  }
}

public void ch4_on() {
  if (connectedSerial) {
      serial.write('r');  println("type 'r'");  status_text("Ch4 on");   delay(20); 
      serial.write('r');  println("type 'r'");  status_text("Ch4 on");   delay(20);
      twitter_send("The Ch4 is turned on !! This is a tweet sent from SRCROOF_R04!! ");
  }
if (isPressedCh4Button == false) {
  isPressedCh4Button = !isPressedCh4Button;
  ((Toggle)cp5.getController("on4/off4")).setState(true);
  messageBoxResult = -1;
}
}

public void ch4_off() {
  createModalDialog("Are you sure to turn Ch4 off?");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('f'); println("type 'f'"); status_text("Ch4 off"); delay(20);
    serial.write('f'); println("type 'f'"); status_text("Ch4 off"); delay(20);
    twitter_send("The Ch4 is turned off!! This is a tweet sent from SRCROOF_R04!! ");
  }
  if (isPressedCh4Button == true) {
  isPressedCh4Button = !isPressedCh4Button;
  ((Toggle)cp5.getController("on4/off4")).setState(false);
  messageBoxResult = -1;
  }
}


void status_text(String status_message){
  fill(255);
  rect(status_text_x,status_text_y,200,25);
  textSize(11);
  fill(0);
  text(status_message, status_text_x+20, status_text_y+17);
}