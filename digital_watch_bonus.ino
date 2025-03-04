#define Mode A0
#define Start A1
#define Reset A2

byte text_decode[10]{
  B00111111,  //0
  B00000110,  //1
  B01011011,  //2
  B01001111,  //3
  B01100110,  //4
  B01101101,  //5
  B01111101,  //6
  B00000111,  //7
  B01111111,  //8
  B01101111,  //9
};

int sec_0 = 0;
int sec_1 = 0;
int minute = 0;

int time_0 = 0;
int time_1 = 0;
int time_min = 0;

bool clock_timer = 0;
bool Stop  = 0;

void sec_increase(){
  sec_0++;
  if (sec_0 == 10){
    if (sec_1++ == 5){
      sec_1 = 0;
      minute += (minute == 59) ? -59 : 1;
    }
    sec_0 = 0;
  }
}

void time_decrease(){
  if(time_0-- <= 0){
     time_0 = 9;
     if(time_1-- <= 0){
        time_1 = 5;
        if(time_min-- <= 0){
          Reset_timer();
        }
     }
  }
}


void sec_display(int t0,int t1, int t2, int t3, byte d){
  int a[4] = {t0,t1,t2,t3};
  byte B = B00000001;
  for(byte i = 0; i<4; i++){
    PORTB |= 0x0F;
    PORTB &= ~B;
    PORTD = (text_decode[a[i]]) | (0x80 & (((i == 2) && d) << 7));
    B = B << 1;
    delay(4);
  }
}

void Reset_timer(){
  time_0 = 0;
  time_1 = 0;
  time_min = 10;
  Stop = 1;
}

long p_millis = millis();
long pt_millis = millis();
long pm_millis = millis();

void setup() {
  // put your setup code here, to run once:
  DDRD   = 0xff;
  DDRB  |= B00001111;
  PORTB |= B00001111;
  pinMode(Mode,INPUT);
  pinMode(Start,INPUT);
  pinMode(Reset,INPUT);
  Reset_timer();
  clock_timer = 0;
}

bool M = 1;
bool S = 1;
bool R = 1;
byte d_c, d_t;

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(Mode) && M && (pm_millis - millis() > 1000)){
    clock_timer = !clock_timer;
    M = 0;
    pm_millis = millis();
  }else{
    M = !digitalRead(Mode);
  }
  if(digitalRead(Start) && S){
    if(clock_timer) Stop = !Stop;  //timer mode 
    S = 0;
  }else{
    S = !digitalRead(Start);
  }
  if(digitalRead(Reset) && R){
    if(clock_timer){
      Reset_timer();
    }
    R = 0;
  }else{
    R = !digitalRead(Reset);
  }

  if(!clock_timer){
    sec_display(sec_0,sec_1, minute%10, minute/10,d_c);
  }else{
    sec_display(time_0,time_1, time_min%10, time_min/10,d_t);
  }

  if((millis() - pt_millis > 500) && (!Stop)){
    time_decrease();
    d_t = ~d_t;
    pt_millis = millis();  
  }
  
  if(millis() - p_millis > 500){
    sec_increase();
    d_c = ~d_c;
    p_millis = millis();
  }
}
