
bool readpulse()
{
  int sum=0;
  for(uint8_t i=0;i<3;i++)
  {
      sum+= analogRead(PHOTO_PIN);
  }
  sum/=3;
  
if(sum<880) return true;
else return false;

}



bool photo_sync()
{
    uint8_t mdots=1;
    u8g2.clearBuffer();
    u8g2.setCursor(0,16);
    u8g2.print(F("  PULSE"));
    u8g2.setCursor(0,32);
    u8g2.print(F("SYNCHRONIZING"));
    u8g2.sendBuffer();
    delay(300);
    while(true)
    {

      while(readpulse()==black) if(R3_BUTTON.pressed()) return false;
      while(readpulse()==black) if(R3_BUTTON.pressed()) return false;;
      mstart=now;
      delay(100);
      while(readpulse()==white) if(R3_BUTTON.pressed()) return false;;
      while(readpulse()==white) if(R3_BUTTON.pressed()) return false;;
      while(readpulse()==white) if(R3_BUTTON.pressed()) return false;;
      while(readpulse()==black) if(R3_BUTTON.pressed()) return false;;
      while(readpulse()==black) if(R3_BUTTON.pressed()) return false;;
      mend=now;
      mtmp=mend-mstart;
      delay(100);


      if(mtmp>=499 && mtmp<=501)
      {
        synctime=mstart+355;  //neden bilmiyorum
        delay(100);
        return true;
      }
      else
      {
        u8g2.clearBuffer();
        u8g2.setCursor(0,16);
        u8g2.print(F("  PULSE"));
        u8g2.setCursor(0,32);
        u8g2.print(F("SYNCHRONIZING"));
        u8g2.setCursor(0,48);
        if (++mdots>10) mdots=0;
        for(uint8_t i=0;i<mdots;i++) u8g2.print(F("."));
        u8g2.setCursor(0,64);
        u8g2.print(F("interval:"));
        u8g2.print(mtmp);
        u8g2.sendBuffer();
      }

      while(readpulse()==black) if(R3_BUTTON.pressed()) return false;;
      while(readpulse()==black) if(R3_BUTTON.pressed()) return false;;
      while(readpulse()==white) if(R3_BUTTON.pressed()) return false;;
      while(readpulse()==white) if(R3_BUTTON.pressed()) return false;;
      while(readpulse()==white) if(R3_BUTTON.pressed()) return false;;
    
    }
}

bool pon_sync()
{
    pinMode(PON,INPUT);
    pinMode(DON,INPUT);
    uint8_t mdots=1;
    u8g2.clearBuffer();
    u8g2.setCursor(0,16);
    u8g2.print(F("  PON"));
    u8g2.setCursor(0,32);
    u8g2.print(F("SYNCHRONIZING"));
    u8g2.setCursor(0,48);
    u8g2.print(F("."));
    u8g2.sendBuffer();
    delay(250);
    mtmp=0;

    while(true)
    {
      while(digitalRead(PON)==HIGH) 
      {
        if(R3_BUTTON.pressed()) return false;
        if(digitalRead(DON)==LOW) { synctime=mtmp; delay(500); return true;}
      }
      mtmp=now;

      u8g2.clearBuffer();
      u8g2.setCursor(0,16);
      u8g2.print(F("  PON"));
      u8g2.setCursor(0,32);
      u8g2.print(F("SYNCHRONIZING"));
      u8g2.setCursor(0,48);
      if (++mdots>10) mdots=0;
      for(uint8_t i=0;i<mdots;i++) u8g2.print(F("."));
      u8g2.sendBuffer();

      while(digitalRead(PON)==LOW)
      {
        if(R3_BUTTON.pressed()) return false;
        if(digitalRead(DON)==LOW) { synctime=mtmp; delay(500); return true;}  
      }
    }


}


bool synchronize()
{ 
    if (pon_sync()) return true;
    if (photo_sync()) return true;
    return false;
}








//bir sonraki pulsün saatini hesaplar
void calcnextpulse()
{
    mstart = now;
    mtmp = mstart-synctime;
    nextpulse = mstart+(500-(mtmp%500));
}

//bir sonraki pulsün saatini hesaplar
void calcnexthalfpulse()
{
    mstart = now;
    mtmp = mstart-synctime;
    nextpulse = mstart+(250-(mtmp%250));
}




//en son hesaplanan pulse ne kadar zaman kaldığını döner
long remainnextpulse()
{
    return (long)(nextpulse - now);
    
}


void setupTimer10ms() {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 100 Hz (16000000/((624+1)*256))
  OCR1A = 624;
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 256
  TCCR1B |= (1 << CS12);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

void setupTimer1ms() {
  noInterrupts();
  // Clear registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // 1000 Hz (16000000/((249+1)*64))
  OCR1A = 249;
  // CTC
  TCCR1B |= (1 << WGM12);
  // Prescaler 64
  TCCR1B |= (1 << CS11) | (1 << CS10);
  // Output Compare Match A Interrupt Enable
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

void StopTimer()
{
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));          // clears all clock selects bits
}


  

  void disableMillisInterrupt() 
  {
    _SFR_BYTE(TIMSK0) &= ~_BV(TOIE0);
  }



/*
void mountain()
{
  while(!photo_sync());

  while(digitalRead(DON));

  calcnextpulse();
  while(remainnextpulse()>0);

  PSBTN(PON,PRESS); delay(100) PSBTN(PON,RELASE); delay(400);
  PSBTN(PON,PRESS); delay(100) PSBTN(PON,RELASE); delay(400);
  PSBTN(PON,PRESS); delay(100) PSBTN(PON,RELASE); delay(900);

  PSBTN(PON,PRESS); delay(100) PSBTN(PON,RELASE); delay(150);
  PSBTN(PON,PRESS); delay(100) PSBTN(PON,RELASE); delay(400);
  PSBTN(PON,PRESS); delay(100) PSBTN(PON,RELASE); delay(8900);

}
*/