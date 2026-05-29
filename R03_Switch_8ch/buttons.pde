void set_buttons() {

  cp5.addButton("ch1_on")
    .setPosition(ch_button_x0+ch_button_w*0, ch_button_y0)
    .setSize(80, 20)
    ;
  cp5.addButton("ch1_off")
    .setPosition(ch_button_x0+ch_button_w*1, ch_button_y0)
    .setSize(80, 20)
    ;
  cp5.addToggle("on1/off1")
    .setPosition(ch_button_x0+ch_button_w*2, ch_button_y0)
    .setSize(50, 20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;
  cp5.addButton("Bulb_on")
    .setPosition(ch_button_x0+ch_button_w*0, ch_button_y0+ch_button_h*1)
    .setSize(80, 20)
    ;
  cp5.addButton("Bulb_off")
    .setPosition(ch_button_x0+ch_button_w*1, ch_button_y0+ch_button_h*1)
    .setSize(80, 20)
    ;
  cp5.addToggle("on2/off2")
    .setPosition(ch_button_x0+ch_button_w*2, ch_button_y0+ch_button_h*1)
    .setSize(50, 20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;
  cp5.addButton("One_shot")
    .setPosition(ch_button_x0+ch_button_w*0, ch_button_y0+ch_button_h*1+25)
    .setSize(80, 20)
    ;

  cp5.addButton("ch3_on")
    .setPosition(ch_button_x0, ch_button_y0+ch_button_h*2)
    .setSize(80, 20)
    ;
  cp5.addButton("ch3_off")
    .setPosition(ch_button_x0+ch_button_w*1, ch_button_y0+ch_button_h*2)
    .setSize(80, 20)
    ;
  cp5.addToggle("on3/off3")
    .setPosition(ch_button_x0+ch_button_w*2, ch_button_y0+ch_button_h*2)
    .setSize(50, 20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;
  cp5.addButton("ch4_on")
    .setPosition(ch_button_x0+ch_button_w*0, ch_button_y0+ch_button_h*3)
    .setSize(80, 20)
    ;
  cp5.addButton("ch4_off")
    .setPosition(ch_button_x0+ch_button_w*1, ch_button_y0+ch_button_h*3)
    .setSize(80, 20)
    ;
  cp5.addToggle("on4/off4")
    .setPosition(ch_button_x0+ch_button_w*2, ch_button_y0+ch_button_h*3)
    .setSize(50, 20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;    
  cp5.addButton("ch5_on")
    .setPosition(ch_button_x0+ch_button_w*0, ch_button_y0+ch_button_h*4)
    .setSize(80, 20)
    ;
  cp5.addButton("ch5_off")
    .setPosition(ch_button_x0+ch_button_w*1, ch_button_y0+ch_button_h*4)
    .setSize(80, 20)
    ;
  cp5.addToggle("on5/off5")
    .setPosition(ch_button_x0+ch_button_w*2, ch_button_y0+ch_button_h*4)
    .setSize(50, 20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;
  cp5.addButton("ch6_on")
    .setPosition(ch_button_x0+210+ch_button_w*1, ch_button_y0+ch_button_h*0)
    .setSize(70, 20)
    ;

  cp5.addButton("res6_on")
    .setPosition(ch_button_x0+ch_button_w*4+30, ch_button_y0+ch_button_h*0)
    .setSize(55, 20)
    ;
  cp5.addButton("res6_off")
    .setPosition(ch_button_x0+ch_button_w*5, ch_button_y0+ch_button_h*0)
    .setSize(55, 20)
    ;

  cp5.addToggle("on6/off6")
    .setPosition(ch_button_x0+230+ch_button_w*2, ch_button_y0+ch_button_h*0+25)
    .setSize(50, 20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;

  cp5.addToggle("opend/closed")
    .setPosition(ch_button_x0+200+ch_button_w*2, ch_button_y0+ch_button_h*0-40)
    .setSize(50, 20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;

  cp5.addButton("ch7_on")
    .setPosition(ch_button_x0+210+ch_button_w*1, ch_button_y0+ch_button_h*1-5)
    .setSize(70, 20)
    ;

  cp5.addButton("res7_on")
    .setPosition(ch_button_x0+ch_button_w*4+30, ch_button_y0+ch_button_h*1-5)
    .setSize(55, 20)
    ;
  cp5.addButton("res7_off")
    .setPosition(ch_button_x0+ch_button_w*5, ch_button_y0+ch_button_h*1-5)
    .setSize(55, 20)
    ;

  cp5.addToggle("on7/off7")
    .setPosition(ch_button_x0+230+ch_button_w*2, ch_button_y0+ch_button_h*1+25)
    .setSize(50, 20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;

  cp5.addButton("ch8_on")
    .setPosition(ch_button_x0+210+ch_button_w*1, ch_button_y0+ch_button_h*2-10)
    .setSize(70, 20)
    ;
  /*
  
   cp5.addButton("ch8_off")
   .setPosition(ch_button_x0+260+ch_button_w*1,ch_button_y0+ch_button_h*2)
   .setSize(80,20)
   ;
   cp5.addToggle("on8/off8")
   .setPosition(ch_button_x0+260+ch_button_w*2,ch_button_y0+ch_button_h*2)
   .setSize(50,20)
   .setValue(false)
   .setMode(ControlP5.SWITCH)
   .lock()
   ;
   */
  cp5.addButton("auto_on")
    .setPosition(ch_button_x0+260+ch_button_w*0, ch_button_y0+ch_button_h*4)
    .setSize(80, 20)
    ;
  cp5.addButton("auto_off")
    .setPosition(ch_button_x0+260+ch_button_w*1, ch_button_y0+ch_button_h*4)
    .setSize(80, 20)
    ;
  cp5.addToggle("on_auto/off_auto")
    .setPosition(ch_button_x0+260+ch_button_w*2, ch_button_y0+ch_button_h*4)
    .setSize(50, 20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;  
  set_button_texts();

  cp5.addButton("res2_on")
    .setPosition(ch_button_x0+ch_button_w*4+30, ch_button_y0+ch_button_h*3-25)
    .setSize(55, 20)
    ;
  cp5.addButton("res2_off")
    .setPosition(ch_button_x0+ch_button_w*5, ch_button_y0+ch_button_h*3-25)
    .setSize(55, 20)
    ;
  cp5.addToggle("on22/off22")
    .setPosition(ch_button_x0+230+ch_button_w*2, ch_button_y0+ch_button_h*3+5)
    .setSize(50, 20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;
}
void set_button_texts() {
  textSize(14);
  text("Anallemma Camera Power", 20, ch_button_y0+ch_button_h*0-10);
  text("Anallemma Camera Shutter", 20, ch_button_y0+ch_button_h*1-10);
  text("Astronomical CCD Power control", 20, ch_button_y0+ch_button_h*3-10);
  text("Mount Motor Power control", 20, ch_button_y0+ch_button_h*4-10);
  text("Roof control", 272+ch_button_w*0, ch_button_y0+ch_button_h*0-10);
  text("Open", 275+ch_button_w*0, ch_button_y0+ch_button_h*0+15);
  text("Close", 275+ch_button_w*0, ch_button_y0+ch_button_h*1+10);
  text("Stop", 275+ch_button_w*0, ch_button_y0+ch_button_h*2+5);
  text("Auto Shutter", 272+ch_button_w*0, ch_button_y0+ch_button_h*3-10);
  text("every 15 sec", 300+ch_button_w*0, ch_button_y0+ch_button_h*3+25);
  text("Anallemma Automation", 272+ch_button_w*0, ch_button_y0+ch_button_h*4-10);
}