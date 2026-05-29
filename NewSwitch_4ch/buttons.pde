// coodinate buttons
int ch_button_x0 = 20 ;
int ch_button_y0 = 240 ;
int ch_button_w = 90 ;
int ch_button_h = 70 ;
int button_sizeX1 = 50 ;
int button_sizeY1 = 20 ;

void set_buttons(){
  cp5.addButton("ch1_on")
    .setPosition(ch_button_x0+ch_button_w*0,ch_button_y0)
    .setSize(button_sizeX1, button_sizeY1)
    ;
  cp5.addButton("ch1_off")
    .setPosition(ch_button_x0+ch_button_w*2,ch_button_y0)
    .setSize(button_sizeX1, button_sizeY1)
    ;
  cp5.addToggle("on1/off1")
    .setPosition(ch_button_x0+ch_button_w*1,ch_button_y0)
    .setSize(50,20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;
  cp5.addButton("ch2_on")
    .setPosition(ch_button_x0+ch_button_w*0,ch_button_y0+ch_button_h*1)
    .setSize(button_sizeX1, button_sizeY1)
    ;
  cp5.addButton("ch2_off")
    .setPosition(ch_button_x0+ch_button_w*2,ch_button_y0+ch_button_h*1)
    .setSize(button_sizeX1, button_sizeY1)
    ;
  cp5.addToggle("on2/off2")
    .setPosition(ch_button_x0+ch_button_w*1,ch_button_y0+ch_button_h*1)
    .setSize(50,20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;  
  cp5.addButton("ch3_on")
    .setPosition(ch_button_x0,ch_button_y0+ch_button_h*2)
    .setSize(button_sizeX1, button_sizeY1)
    ;
  cp5.addButton("ch3_off")
    .setPosition(ch_button_x0+ch_button_w*2,ch_button_y0+ch_button_h*2)
    .setSize(button_sizeX1, button_sizeY1)
    ;
  cp5.addToggle("on3/off3")
    .setPosition(ch_button_x0+ch_button_w*1,ch_button_y0+ch_button_h*2)
    .setSize(50,20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;
  cp5.addButton("ch4_on")
    .setPosition(ch_button_x0+ch_button_w*0,ch_button_y0+ch_button_h*3)
    .setSize(button_sizeX1, button_sizeY1)
    ;
  cp5.addButton("ch4_off")
    .setPosition(ch_button_x0+ch_button_w*2,ch_button_y0+ch_button_h*3)
    .setSize(button_sizeX1, button_sizeY1)
    ;
  cp5.addToggle("on4/off4")
    .setPosition(ch_button_x0+ch_button_w*1,ch_button_y0+ch_button_h*3)
    .setSize(50,20)
    .setValue(false)
    .setMode(ControlP5.SWITCH)
    .lock()
    ;    
}

void set_button_texts(){
  fill(255);
  textSize(14);
  text("CCD Power", 20, ch_button_y0+ch_button_h*0-10);
  text("Anything", 20, ch_button_y0+ch_button_h*1-10);
  text("Anything", 20, ch_button_y0+ch_button_h*2-10);
  text("Mount Motor Power control", 20, ch_button_y0+ch_button_h*3-10); 
}