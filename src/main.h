
#define CROSS       9
#define SQUARE      8
#define CIRCLE      7
#define TRIANGLE    6
#define DON         CROSS
#define PATA        SQUARE
#define PON         CIRCLE
#define CHAKA       TRIANGLE
#define PHOTO_PIN   A2

#define white true
#define black false

#define RL_AN_PIN     A0
#define UD_AN_PIN     A1
#define R3_PIN        5
#define BTN_BUILDIN   3

#define RELASE        0
#define PRESS         1

#define mode_standbye     false
#define mode_play         true

volatile unsigned long now=0;
bool work_mode =0;
unsigned long synctime=0;
unsigned long mstart=0;
unsigned long mtmp=0;
unsigned long mend=0;
unsigned long nextpulse=0;
bool upst=false;
bool downst=false;
bool leftst=false;
bool rightst=false;

struct SettingStruct {

  int syncshift;
  int light_threshold;
};

SettingStruct MySettings = {
    355,
    880,
  };


const  uint8_t  marchs[6][4]={
  {PATA,PATA,PATA,PON},
  {PON,PON,PATA,PON},
  {CHAKA,CHAKA,PATA,PON},
  {PON,PATA,PON,PATA},
  {PON,PON,CHAKA,CHAKA},
  {PON,PON,PON,PON}
};

uint8_t current_march=1;
uint8_t next_march=1;

#define march_foward    0
#define march_attack    1
#define march_defense   2
#define march_backward  3
#define march_charge    4
#define march_miracle   5


U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Button R3_BUTTON(R3_PIN,button_type_pulledDown,INPUT);
Button M_BUTTON(BTN_BUILDIN,button_type_pulledUp,INPUT_PULLUP);
Button UP_BUTTON(0,button_type_analog,INPUT);
Button LEFT_BUTTON(0,button_type_analog,INPUT);
Button DOWN_BUTTON(0,button_type_analog,INPUT);
Button RIGHT_BUTTON(0,button_type_analog,INPUT);




void mloop(int index);
void printbtnname(uint8_t mpin);
void PSBTN(uint8_t pin,uint8_t mstate);
void printmarchname(uint8_t march_name);
void settingmenu();
void miracle();
uint8_t stickisup();
uint8_t stickisleft();
uint8_t stickisdown();
uint8_t stickisright();