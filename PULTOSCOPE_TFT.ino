//страничка проекта http://srukami.inf.ua/pultoscop_v2.html
//дистплей TFT SPFD5408
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
//пользовательские настройки!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define levo 12
#define ok 11
#define pravo 10
#define vertikal 2   //размер вертикальной сеткаи(если "0" отключениа)
//пользовательские настройки!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
byte mass[501];//массив АЦП
byte massDEL[501];
byte menu=1;//переменная верхнего меню
byte razv=0;//значение развертки
float x=60;//счетчик оси Х
int Vmax=0;// максимальное напряжение 
float Vakb=0;//напряженеиЕКБ
unsigned long t=0;//переменная для ращета развертки
unsigned long pause=0;//переменная для замены Delay
int t_del=0; //значение цены деления развертки в микросекундах
bool opornoe=1; //флаг опорного напряжения
bool paus=0; //флаг режима паузы
bool clen_p=0;//флаг чистки паузы
void setup(){  
    Vakb=analogRead(A6)*5.3/1024;//проверка напряжения на АКБ
    if(opornoe==0){ADMUX = 0b11100101;}//выбор внутреннего опорного 1,1В
    if(opornoe==1){ADMUX = 0b01100101;}//Выбор внешнего опорного
    uint16_t identifier = tft.readID();  
    tft.begin(identifier);
    delay(50);
    tft.fillScreen(BLACK);
    delay(500);
}
void Zamer(){//заполнение буфера АЦП
  if (razv>=6){ADCSRA = 0b11100010;}//delitel 4
  if (razv==5){ADCSRA = 0b11100011;}//delitel 8
  if (razv==4){ADCSRA = 0b11100100;}//delitel 16
  if (razv==3){ADCSRA = 0b11100101;}//delitel 32
  if (razv==2){ADCSRA = 0b11100110;}//delitel 64
  if (razv<2){ADCSRA = 0b11100111;}//delitel 128
  if (razv==0){
      t=micros(); 
      for(int i=0;i<500;i++){ 
          while ((ADCSRA & 0x10)==0);
          ADCSRA|=0x10;
          delayMicroseconds(100);
          mass[i]=ADCH;
      }
      t= micros()-t;
      t_del=t*25/501;
  }
  if (razv>0){
      t=micros();
      for(int i=0;i<500;i++){ 
          while ((ADCSRA & 0x10)==0);
          ADCSRA|=0x10;
          mass[i]=ADCH;
      }
      t= micros()-t;
      t_del=t*25/501;
  }
  
}
void MenuT(){//перерисовка нижнего меню    
    tft.setRotation(0);  
    tft.fillRect(0, 20, 20,320, BLACK);//стирание поля меню низ
    //tft.fillRect(0, 15, 25,75, CYAN);//стирание
    //tft.fillRect(0, 170, 25,65, CYAN);//стирание
    tft.setRotation(1);
    tft.setCursor(0,220);
    tft.print("t=");
    if(razv<7){tft.print(t_del);}
    if(razv==7){tft.print(t_del/2);}
    if(razv==8){tft.print(t_del/3);}
    if(razv==9){tft.print(t_del/4);}
    if(razv==10){tft.print(t_del/5);}
    tft.print("us  ");
    tft.print("Vmax=");
    if(opornoe==0){tft.print(Vmax*1.1/255);}
    if(opornoe==1){tft.print(Vmax*5.3/255);}
    tft.print(" ");
    tft.print("B");
    tft.print(Vakb);  
}

void loop() { 
Zamer();  
//отрисовка и перебор меню###############################
if(menu==0){
  tft.setRotation(1);
  tft.setTextColor(RED);
  tft.setTextSize(2); 
  tft.fillRect(0, 0, 85, 25, CYAN);
  tft.setCursor(0,5);
  if(opornoe==0){tft.print("op-1.1V ");}
  if(opornoe==1){tft.print("op-5.3V ");}
  if(digitalRead(pravo)==HIGH){
    tft.setRotation(0); 
    tft.fillRect(65, 0, 120, 50, BLACK);//стирание напряжения
    tft.fillRect(45, 60, 140, 260, BLACK);//стиране поля графика
    opornoe=!opornoe;}
  if(digitalRead(levo)==HIGH){
    tft.setRotation(0);  
    tft.setRotation(0); 
     tft.fillRect(45, 60, 140, 260, BLACK);//стиране поля графика 
    tft.fillRect(65, 0, 120, 50, BLACK);//стирание напряжения
    opornoe=!opornoe;}
  tft.setRotation(1);  
  tft.print("Razv*");
  tft.print(razv);
  tft.print("   PauseOFF");
}
if(menu==1){
  tft.setRotation(1);
  tft.setTextColor(RED);
  tft.setTextSize(2); 
  tft.fillRect(90, 0,120, 25, CYAN);
  tft.setCursor(0,5);
  if(opornoe==0){tft.print("op-1.1V ");}
  if(opornoe==1){tft.print("op-5.3V ");}
  if(digitalRead(pravo)==HIGH){//доработать скорость
  tft.setRotation(0); 
  tft.fillRect(45, 60, 140, 260, BLACK);//стиране поля графика
  razv++;
  if(razv==11){razv=10;} 
  }
  if(digitalRead(levo)==HIGH){//доработать скорость
  tft.setRotation(0); 
  tft.fillRect(45, 60, 140, 260, BLACK);//стиране поля графика
  razv--;
  if(razv==255){razv=0;} 
  }
  tft.setRotation(1); 
  tft.print("Razv*");
  tft.print(razv);
  tft.print("   PauseOFF");
}
if(menu==2){//пауза
  paus=1;
  if(clen_p==0){
  clen_p=1;  
  tft.setRotation(1);
  tft.setTextColor(RED);
  tft.setTextSize(2); 
  tft.fillRect(210, 0,100, 25, CYAN);
  tft.setCursor(0,5);
  if(opornoe==0){tft.print("op-1.1V ");}
  if(opornoe==1){tft.print("op-5.3V ");}
  tft.print("Razv*");
  tft.print(razv);
  if(paus==0){tft.print("   PauseOFF");}
  if(paus==1){tft.print("   PauseON");}
}
}
if(digitalRead(ok)==HIGH){
  menu++;
  tft.setRotation(0); 
  tft.fillRect(45, 60, 140, 260, BLACK);//стиране поля графика  
  if(menu==3){menu=0;paus=0;clen_p=0;}
  tft.setRotation(1); 
  tft.fillRect(0, 0,320, 25, BLACK);    
}
//отрисовка и перебор меню###############################
//Выбор опорного#########################################  
if(opornoe==0){ADMUX = 0b11100101;}//выбор внутреннего опорного 1,1В
if(opornoe==1){ADMUX = 0b01100101;}//Выбор внешнего опорного
delay(5);
//Выбор опорного#########################################  
tft.setRotation(0); 
//ось напряжения##########################################
tft.drawFastHLine(44,60,140,BLUE);tft.drawFastHLine(44,59,140,BLUE);tft.drawFastHLine(44,58,140,BLUE);
tft.drawFastVLine(50,50,10, BLUE);tft.drawFastVLine(76,50,10, BLUE);tft.drawFastVLine(102,50,10, BLUE);
tft.drawFastVLine(128,50,10, BLUE);tft.drawFastVLine(154,50,10, BLUE);tft.drawFastVLine(175,50,10, BLUE);
//ось времени#############################################
tft.drawFastVLine(44,60,270, RED);tft.drawFastVLine(43,60,270, RED);tft.drawFastVLine(42,60,270, RED);
tft.drawFastHLine(35,60,10,RED);tft.drawFastHLine(35,85,10,RED);tft.drawFastHLine(35,110,10,RED);tft.drawFastHLine(35,135,10,RED);
tft.drawFastHLine(35,160,10,RED);tft.drawFastHLine(35,185,10,RED);tft.drawFastHLine(35,210,10,RED);tft.drawFastHLine(35,235,10,RED);
tft.drawFastHLine(35,260,10,RED);tft.drawFastHLine(35,285,10,RED);tft.drawFastHLine(35,310,10,RED);
//ось времени#############################################
//сетка вертикальная #####################################
tft.drawFastHLine(75,310,5*vertikal,YELLOW);tft.drawFastHLine(110,310,5*vertikal,YELLOW);tft.drawFastHLine(145,310,5*vertikal,YELLOW);tft.drawFastHLine(180,310,5*vertikal,YELLOW);
tft.drawFastHLine(75,285,5*vertikal,YELLOW);tft.drawFastHLine(110,285,5*vertikal,YELLOW);tft.drawFastHLine(145,285,5*vertikal,YELLOW);tft.drawFastHLine(180,285,5*vertikal,YELLOW);
tft.drawFastHLine(75,260,5*vertikal,YELLOW);tft.drawFastHLine(110,260,5*vertikal,YELLOW);tft.drawFastHLine(145,260,5*vertikal,YELLOW);tft.drawFastHLine(180,260,5*vertikal,YELLOW);
tft.drawFastHLine(75,235,5*vertikal,YELLOW);tft.drawFastHLine(110,235,5*vertikal,YELLOW);tft.drawFastHLine(145,235,5*vertikal,YELLOW);tft.drawFastHLine(180,235,5*vertikal,YELLOW);
tft.drawFastHLine(75,210,5*vertikal,YELLOW);tft.drawFastHLine(110,210,5*vertikal,YELLOW);tft.drawFastHLine(145,210,5*vertikal,YELLOW);tft.drawFastHLine(180,210,5*vertikal,YELLOW);
tft.drawFastHLine(75,185,5*vertikal,YELLOW);tft.drawFastHLine(110,185,5*vertikal,YELLOW);tft.drawFastHLine(145,185,5*vertikal,YELLOW);tft.drawFastHLine(180,185,5*vertikal,YELLOW);
tft.drawFastHLine(75,160,5*vertikal,YELLOW);tft.drawFastHLine(110,160,5*vertikal,YELLOW);tft.drawFastHLine(145,160,5*vertikal,YELLOW);tft.drawFastHLine(180,160,5*vertikal,YELLOW);
tft.drawFastHLine(75,135,5*vertikal,YELLOW);tft.drawFastHLine(110,135,5*vertikal,YELLOW);tft.drawFastHLine(145,135,5*vertikal,YELLOW);tft.drawFastHLine(180,135,5*vertikal,YELLOW);
tft.drawFastHLine(75,110,5*vertikal,YELLOW);tft.drawFastHLine(110,110,5*vertikal,YELLOW);tft.drawFastHLine(145,110,5*vertikal,YELLOW);tft.drawFastHLine(180,110,5*vertikal,YELLOW);
tft.drawFastHLine(75,85,5*vertikal,YELLOW);tft.drawFastHLine(110,85,5*vertikal,YELLOW);tft.drawFastHLine(145,85,5*vertikal,YELLOW);tft.drawFastHLine(180,85,5*vertikal,YELLOW);
//сетка вертикальная #####################################
tft.setRotation(1);  
tft.setTextColor(RED);
tft.setTextSize(2);
//шкала напряжения########################################
if(opornoe==0){
    tft.setCursor(30, 190);tft.println("0"); 
    tft.setCursor(0, 155);tft.println("0.22"); 
    tft.setCursor(0, 130);tft.println("0.44"); 
    tft.setCursor(0,105);tft.println("0.66"); 
    tft.setCursor(0, 80);tft.println("0.88"); 
    tft.setCursor(0, 55);tft.println("1.1V");   
}
if(opornoe==1){
    tft.setCursor(30, 190);tft.println("0"); 
    tft.setCursor(0, 155);tft.println("1.00"); 
    tft.setCursor(0, 130);tft.println("2.00"); 
    tft.setCursor(0,105);tft.println("3.00"); 
    tft.setCursor(0, 80);tft.println("4.00"); 
    tft.setCursor(0, 55);tft.println("5.0V");   
}
//шкала напряжения########################################
if(paus==0){MenuT();}
//максимальное значение сигнала##########################
Vmax=0; 
for(int i=0;i<500;i++){  
        if(Vmax<mass[i]){ Vmax=mass[i];} 
}
//максимальное значение сигнала##########################
//отрисовка графика№№№№№№№№№№№№№№№№№№№№№
if(paus==0){
  tft.setRotation(0); 
  //tft.fillRect(45, 60, 140, 260, BLACK);//стиране графика
  x=60;
  for(int y=0;y<260;y++){
      tft.setRotation(1); 
      if(razv<7){x++;}
      if(razv==7){x=x+2;}
      if(razv==8){x=x+3;} 
      if(razv==9){x=x+4;}
      if(razv==10){x=x+5;}
      tft.drawLine(x+1,190-massDEL[y]/2+1, x+2,190-massDEL[y+1]/2+1, BLACK);//стираем график
      tft.drawLine(x,190-massDEL[y]/2, x+1,190-massDEL[y+1]/2, BLACK);//стираем график
      tft.drawLine(x+1,190-mass[y]/2+1, x+2,190-mass[y+1]/2+1, GREEN);//график
      tft.drawLine(x,190-mass[y]/2, x+1,190-mass[y+1]/2, GREEN);//график
         
  }
  for(int i=0;i<500;i++){massDEL[i]=mass[i];}
}
if(paus==1){//режим паузы
    //tft.setRotation(0); 
    //tft.fillRect(45, 60, 140, 260, BLACK);//стиране графика 
    if(digitalRead(pravo)==HIGH){//листаем
        tft.setRotation(0); 
        tft.fillRect(45, 60, 140, 260, BLACK);//стиране графика 
      x=60;
      for(int y=0;y<260;y++){
          tft.setRotation(1);
          if(razv<7){x++;}
          if(razv==7){x=x+2;}
          if(razv==8){x=x+3;}
          if(razv==9){x=x+4;}
          if(razv==10){x=x+5;}
      tft.drawLine(x+1,190-mass[y]/2+1, x+2,190-mass[y+1]/2+1, GREEN);//график
      tft.drawLine(x,190-mass[y]/2, x+1,190-mass[y+1]/2, GREEN);//график
      }
      MenuT();
    }
    if(digitalRead(levo)==HIGH){//листаем
      tft.setRotation(0); 
      tft.fillRect(45, 60, 140, 260, BLACK);//стиране графика 
      x=60;
      for(int y=0;y<260;y++){
          tft.setRotation(1); 
          if(razv<7){x++;}
          if(razv==7){x=x+2;}
          if(razv==8){x=x+3;} 
          if(razv==9){x=x+4;}
          if(razv==10){x=x+5;}
      tft.drawLine(x+1,190-mass[y]/2+1, x+2,190-mass[y+1]/2+1, GREEN);//график
      tft.drawLine(x,190-mass[y]/2, x+1,190-mass[y+1]/2, GREEN);//график
      }
      MenuT();
    }     
}
//отрисовка графика№№№№№№№№№№№№№№№№№№№№№
}
