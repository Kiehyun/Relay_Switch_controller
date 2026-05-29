public void ch_on(char but, char msg1, String msg2, String name_but) {
  if (connectedSerial) {
    serial.write(but); println(msg1); status_text(msg2); delay(50); 
    }
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController(name_but)).setState(true);
  messageBoxResult = -1;
}

public void ch1_on() {
  if (connectedSerial) {
    serial.write('q'); println("type 'q'"); status_text("Ch1 on"); delay(50); serial.write('q'); println("type 'q'"); status_text("Ch1 on"); delay(50);
    }
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController("on1/off1")).setState(true);
  messageBoxResult = -1;
}

public void ch1_off() {
  createModalDialog("Are you sure to turn Ch1 off?");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('a'); println("type 'a'");
    status_text("Ch1 off");
  }
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController("on1/off1")).setState(false);
  messageBoxResult = -1;
}

public void ch2_on() {
  if (connectedSerial) {
    serial.write('w'); println("type 'w'"); status_text("Ch2 on"); delay(50); serial.write('w'); println("type 'w'"); status_text("Ch2 on"); delay(50);
    }
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController("on2/off2")).setState(true);
  messageBoxResult = -1;
}

public void ch2_off() {
  createModalDialog("Are you sure to turn Ch2 off?");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('s'); println("type 's'");
    status_text("Ch2 off");
  }
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController("on2/off2")).setState(false);
  messageBoxResult = -1;
}

public void ch3_on() {
  if (connectedSerial) {
    serial.write('e'); println("type 'e'"); status_text("Ch3 on"); delay(50); serial.write('e'); println("type 'e'"); status_text("Ch3 on"); delay(50);
    }
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController("on3/off3")).setState(true);
  messageBoxResult = -1;
}

public void ch3_off() {
  createModalDialog("Are you sure to turn Ch3 off?");
  if (messageBoxResult >= 1) return;
  if (connectedSerial) {
    serial.write('d'); println("type 'd'");
    status_text("Ch3 off");
  }
  isPressedCh1Button = !isPressedCh1Button;
  ((Toggle)cp5.getController("on3/off3")).setState(false);
  messageBoxResult = -1;
}

public void ch4_on() {
  if (connectedSerial) {
    serial.write('r'); println("type 'r'");
    status_text("Ch4 on");
  }
  isPressedCh4Button = !isPressedCh4Button;
  ((Toggle)cp5.getController("on4/off4")).setState(true);
  messageBoxResult = -1;
}

public void ch4_off() {
  createModalDialog("Are you sure to turn CCD Power off?");
   if (messageBoxResult >= 1)
    return;
  if (connectedSerial) {
    serial.write('f'); println("type 'f'");
    status_text("Ch4 off");
  }
  isPressedCh4Button = !isPressedCh4Button;
  ((Toggle)cp5.getController("on4/off4")).setState(false);
  messageBoxResult = -1;
}


void status_text(String status_message){
  fill(255);
  rect(status_text_x,status_text_y,200,25);
  textSize(11);
  fill(0);
  text(status_message, status_text_x+20, status_text_y+17);
}