#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Button.h>
#include <EEPROM.h>
#include "main.h"
#include "tools.h"





void setup() 
{
  pinMode(CROSS,INPUT);
  pinMode(SQUARE,INPUT);    
  pinMode(CIRCLE,INPUT);    
  pinMode(TRIANGLE,INPUT);  
  pinMode(RL_AN_PIN,INPUT);     
  pinMode(UD_AN_PIN,INPUT);     
  pinMode(R3_PIN,INPUT);  
  pinMode(BTN_BUILDIN,INPUT_PULLUP);
  pinMode(PHOTO_PIN,INPUT_PULLUP);

  UP_BUTTON.set_delay(80);
  UP_BUTTON.registeranalogfunc(stickisup);

  LEFT_BUTTON.set_delay(80);
  LEFT_BUTTON.registeranalogfunc(stickisleft);

  DOWN_BUTTON.set_delay(80);
  DOWN_BUTTON.registeranalogfunc(stickisdown);

  RIGHT_BUTTON.set_delay(80);
  RIGHT_BUTTON.registeranalogfunc(stickisright);

  R3_BUTTON.set_delay(50);
  R3_BUTTON.begin();
  M_BUTTON.set_delay(50);
  M_BUTTON.begin();
  u8g2.begin();
  Serial.begin(9600);
  while (!Serial);
  u8g2.setFont(u8g2_font_9x15_t_symbols  );	// choose a suitable font
  u8g2.enableUTF8Print();
  u8g2.clearBuffer();
  u8g2.setCursor(0,32);
  u8g2.print(F("READY"));
  u8g2.sendBuffer();
  
  setupTimer1ms();

}



void loop() 
{
  
  if(work_mode==mode_standbye)
  {
    mloop(0);
  }
  else if(work_mode==mode_play)
  {
    current_march=next_march;
    if (next_march==march_miracle) miracle();

    for (int i=0; i<4;i++)
    { 
        calcnextpulse();
        while (remainnextpulse()>0) mloop(i);
        PSBTN(marchs[current_march][i],PRESS);
        while (remainnextpulse()>-150) mloop(i);
        PSBTN(marchs[current_march][i],RELASE);
    }

    synctime+=4; 
    delay(4);

    calcnextpulse(); while (remainnextpulse()>0) mloop(4);
    calcnextpulse(); while (remainnextpulse()>0) mloop(4);
    calcnextpulse(); while (remainnextpulse()>0) mloop(4);
    calcnextpulse(); while (remainnextpulse()>0) mloop(-1);
    
    
    
    
  }
  

}






void mloop(int index)
{
  bool pressed=false;
  if(work_mode==mode_play)
  {
    if(remainnextpulse()>=0) 
    {
      pressed=false;
      if (remainnextpulse()<60) return;
    }
    else 
    {
      pressed=true;
      if (remainnextpulse()<-60) return;
    }
  }
  
  if(R3_BUTTON.pressed()) 
  {
    work_mode=!work_mode;
    if(work_mode==mode_play) 
    {
      if (synchronize())
      {
          next_march=march_attack;
          return;
      }
      else
      {
        work_mode=mode_standbye;
        return;
      }
    }
  }

//---------------------INPUT SECTION-----------------------
  if( work_mode==mode_play)
  {
    /*
    upst=UP_BUTTON.pressed();
    downst=DOWN_BUTTON.pressed();
    leftst=LEFT_BUTTON.pressed();
    rightst=RIGHT_BUTTON.pressed();
    if(upst)    next_march=march_charge; 
    if(downst)  next_march=march_defense; 
    if(rightst) next_march=march_foward; 
    if(leftst)  next_march=march_backward; 
    if(upst && rightst)    next_march=march_attack; 
    */
    if(!stickisup())      next_march=march_charge; 
    if(!stickisdown())    next_march=march_defense; 
    if(!stickisleft())    next_march=march_backward; 
    if(!stickisright())   next_march=march_foward; 

    if(!stickisup() && !stickisright())   next_march=march_attack; 
    //if(stickisup()      &&  stickisleft())    next_march=; 
    //if(stickisdown()    &&  stickisright())   next_march=; 
    //if(stickisdown()    &&  stickisleft())    next_march=; 

    if(M_BUTTON.pressed())      next_march=march_miracle; 


  }
  else
  {
    if(M_BUTTON.pressed())
    {
      //settingmenu();
    }
  }
  


//------------------------LCD SECTION-----------------------------
 
    u8g2.clearBuffer();					// clear the internal memory
    if( work_mode==mode_standbye)
    {
      u8g2.setCursor(0,16);
      u8g2.print(F("STANDBY"));
    }
    else if( work_mode==mode_play)
    {
      u8g2.setCursor(0,16);
      u8g2.print(F("NOW:"));
      printmarchname(current_march);

      u8g2.setCursor(0,36);
      u8g2.print(F("NEX:"));
      printmarchname(next_march);
      
      u8g2.setCursor(0,55);
      if(pressed)
      {
        for(int i=0;i<=index;i++) printbtnname(marchs[current_march][i]);
      }
      else
      {
        for(int i=0;i<=index-1;i++) printbtnname(marchs[current_march][i]);
      }
      
    }
    u8g2.sendBuffer();					// transfer internal memory to the display
  
}

void printbtnname(uint8_t mpin)
{
  if(mpin==CROSS) u8g2.print(F("X "));
  if(mpin==CIRCLE) u8g2.print(F("\u25ef "));
  if(mpin==SQUARE) u8g2.print(F("\u2610 "));
  if(mpin==TRIANGLE) u8g2.print(F("\u25b3 "));
}

void printmarchname(uint8_t march_name)
{
  if (march_name==march_foward) u8g2.print(F("Foward"));
  else if (march_name==march_backward) u8g2.print(F("Backward"));
  else if (march_name==march_defense) u8g2.print(F("Defense"));
  else if (march_name==march_attack) u8g2.print(F("Attack"));
  else if (march_name==march_charge) u8g2.print(F("Charge"));
  else if (march_name==march_miracle) u8g2.print(F("*Miracle*"));
}


void PSBTN_logic(uint8_t pin,uint8_t mstate)//time bas??l?? kalaca???? s??re
{
    if (mstate==PRESS)
    {
      pinMode(pin,OUTPUT);
      digitalWrite(pin,LOW);
      
    }
    else
    {
      digitalWrite(pin,HIGH);
    }
}

void PSBTN(uint8_t pin,uint8_t mstate)//time bas??l?? kalaca???? s??re
{
    if (mstate==PRESS)
    {
      digitalWrite(pin,LOW);
      pinMode(pin,OUTPUT);
    }
    else
    {
      pinMode(pin,INPUT);  
    }
}

uint8_t stickisup()  { if(analogRead(UD_AN_PIN)<350) return 0; else return 1; }
uint8_t stickisdown() { if(analogRead(UD_AN_PIN)>600) return 0; else return 1; }
uint8_t stickisleft() { if(analogRead(RL_AN_PIN)<350) return 0; else return 1; }
uint8_t stickisright() { if(analogRead(RL_AN_PIN)>630) return 0; else return 1; }



ISR(TIMER1_COMPA_vect) 
{
  now++;
}


void settingmenu()
{
  return;
  int menuindex=0;
  uint8_t menucnt=2;
  int *currentsetval;

  currentsetval = &MySettings.light_threshold;

  while(1)
  {
      if(UP_BUTTON.pressed())  next_march=march_attack; 
      if(DOWN_BUTTON.pressed())  next_march=march_defense; 
      if(RIGHT_BUTTON.pressed())  
      {
        menuindex++; if(menuindex>=menucnt) menuindex=0; 
        

      }
      if(LEFT_BUTTON.pressed())  {menuindex--; if(menuindex<0) menuindex=menucnt-1; }
  }
}



void save_settings()
{
 EEPROM.put(0,MySettings);
}

void read_settings()
{
 EEPROM.get(0,MySettings);
}

void miracle()
{
        unsigned long miracle_start = now;
        calcnextpulse();
        while (remainnextpulse()>0) mloop(-1);
        PSBTN(DON,PRESS);
        while (remainnextpulse()>-150) mloop(-1);
        PSBTN(DON,RELASE);   //DON

        calcnextpulse();
        while (remainnextpulse()>0);
        PSBTN(DON,PRESS);
        while (remainnextpulse()>-150);
        PSBTN(DON,RELASE);   //DO

        calcnexthalfpulse();
        while (remainnextpulse()>0);
        PSBTN(DON,PRESS);
        while (remainnextpulse()>-150);
        PSBTN(DON,RELASE);   //DON

        calcnexthalfpulse(); while (remainnextpulse()>0);

        calcnexthalfpulse();
        while (remainnextpulse()>0);
        PSBTN(DON,PRESS);
        while (remainnextpulse()>-150);
        PSBTN(DON,RELASE);   //DO

        calcnexthalfpulse();
        while (remainnextpulse()>0);
        PSBTN(DON,PRESS);
        while (remainnextpulse()>-150);
        PSBTN(DON,RELASE);   //DON

        uint8_t mdots=0;
        while(now<(miracle_start+36200))  // 35200 100ms  birsonraki calculate i??in zaman kals??n diye 37500'de di??er pulse var ????nk??
        {
          u8g2.clearBuffer();
          u8g2.setCursor(0,32);
          u8g2.print(F("*IN MIRACLE*"));
          u8g2.setCursor(0,48);
          if (++mdots>10) mdots=0;
          for(uint8_t i=0;i<mdots;i++) u8g2.print(F("."));
          u8g2.sendBuffer();
          delay(200);
        }
        next_march=march_attack;  //hemen ata??a ge??meli kim bilir belki fever bozulmaz
        current_march = march_attack;
        synctime+=10; 
        delay(10);
}