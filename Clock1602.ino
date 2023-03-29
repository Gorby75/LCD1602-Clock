#include "EEPROM.h"
#include "RTClib.h"
#include "Seeed_BME280.h"
#include <RCSwitch.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Wtv020sd16p.h>
#define mn 60UL
int resetPin = 3;  // The pin number of the reset pin.
int clockPin = 4;  // The pin number of the clock pin.
int dataPin = 5;  // The pin number of the data pin.
int busyPin = 6;  // The pin number of the busy pin.
Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin);
int god1,mou1,day1,mnh1,cmh1,god1a,mou1a,day1a,mnh1a,cmh1a;
LiquidCrystal_I2C lcd(0x3F,16,2);
unsigned long alert433;
unsigned long timing,timingsw,timinglcd;
BME280 bme280;
uint32_t TimeAdjustPeriod = 60*mn; //корректирровка времени на -1 сек за 38 мин
uint32_t TimeCorrection = -2;
float datt1,datt2,dath1,dath2;
float datt3,dath3,datp3;
byte alarm=0, tt=0;
byte tt1=0;
int mm,yc,mc,dc,hc,ns;
byte trigsw=0;
byte triag=0;
float yy=0;
byte nmode;
int sng;
byte trigarm=0;
int minual,houral,secc, kl,kl2,kl3,timerkn0=0;
int klv;
byte bat1=0;
RCSwitch mySwitch = RCSwitch();
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
byte LT[8] = 
{
  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte UB[8] =
{
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte RT[8] =
{

  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte LL[8] =
{


  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B01111,
  B00111
};
byte LB[8] =
{
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};
byte LR[8] =
{


  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11110,
  B11100
};
byte MB[8] =
{
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111
};
byte block[8] =
{
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
// loop counter
int count = 0;



void setup() {
pinMode(11, OUTPUT);
digitalWrite(11,LOW);  
pinMode(10, OUTPUT);
analogWrite(10,0);//Яркость4
pinMode(9, OUTPUT);
digitalWrite(9,HIGH);//Яркость4
wtv020sd16p.reset();
Serial.begin(115200);
mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
bme280.init();
rtc.begin();
//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
lcd.init();
lcd.backlight();
lcd.home();
lcd.createChar(0,LT);
lcd.createChar(1,UB);
lcd.createChar(2,RT);
lcd.createChar(3,LL);
lcd.createChar(4,LB);
lcd.createChar(5,LR);
lcd.createChar(6,MB);
lcd.createChar(7,block);
lcd.clear();
alarm=EEPROM.read(3);
nmode=EEPROM.read(4);

}

void loop() { 
DateTime now = rtc.now();  
if (mySwitch.available()) {
  
alert433= mySwitch.getReceivedValue();
Serial.println(alert433);
if ((alert433>910000)&& (alert433 < 911999)) datt1=(float(int(alert433-910000-500))/10);
if ((alert433>912000)&& (alert433 < 912999)) dath1=(float(int(alert433-912000))/10);
mySwitch.resetAvailable();
if ((alert433>5510000)&& (alert433 < 5513000)) {
god1=int(now.year());
mou1=int(now.month());
day1=int(now.day());
mnh1a=int(now.minute());
cmh1a=int(now.hour());
mnh1=(alert433-5510000)-(((int(alert433-5510000)/100))*100);
cmh1=int((alert433-5510000)/100);
if((cmh1!=cmh1a)||(mnh1!=mnh1a)){
  rtc.adjust(DateTime(god1, mou1, day1, cmh1, mnh1, 0));
rtc.begin();
}
}    
if ((alert433>5500000)&& (alert433 < 5509000)) {
//god1=int(now.year());
//mou1=int(now.month());
god1a=int(now.year());
mou1a=int(now.month());
day1=int(now.day());
mnh1=int(now.minute());
cmh1=int(now.hour());
mou1=(alert433-5500000)-(((int(alert433-5500000)/100))*100);
god1=2000+int((alert433-5500000)/100);
if ((mou1!=mou1a)||(god1!=god1a)){
rtc.adjust(DateTime(god1, mou1, day1, cmh1, mnh1, 0));
rtc.begin();
}
}  
if ((alert433>5520000)&& (alert433 < 5529000)) {
god1=int(now.year());
mou1=int(now.month());
day1a=int(now.day());
mnh1=int(now.minute());
cmh1=int(now.hour());
day1=int((alert433-5520000)/100);
if (day1!=day1a){
rtc.adjust(DateTime(god1, mou1, day1, cmh1, mnh1, 0));
rtc.begin();
}
}      
  }
if (tt==0) {
  triag=0;
  digitalClockDisplay();  
}

if (tt>0) {
if (millis() - timing > 5000){
st1:
lcd.clear();   
if (tt>0)alarmon();  


if (tt==1){
  if (now.hour()<10){
 lcd.setCursor(2,0);
lcd.print("0"); 
lcd.setCursor(3,0);
lcd.print(now.hour());
 }
 else
 {
 lcd.setCursor(2,0);
lcd.print(now.hour());
 }
 lcd.setCursor(4,0);
lcd.print(":");

if (now.minute()<10){
 lcd.setCursor(5,0);
lcd.print("0"); 
lcd.setCursor(6,0);
lcd.print(now.minute());
 }
 else
 {
 lcd.setCursor(5,0);
lcd.print(now.minute());
 }
 if ((datt1==0) && (dath1==0)) {
 lcd.setCursor(1,1);
  lcd.print(">");  
  lcd.setCursor(2,1);
lcd.print(bme280.getTemperature()/1.13);
lcd.setCursor(7,1);
lcd.print("c");
lcd.setCursor(9,1);
lcd.print(bme280.getHumidity()*1.3); 
 }
 else {
  lcd.setCursor(2,1);
lcd.print(datt1);
lcd.setCursor(7,1);
lcd.print("c");
lcd.setCursor(9,1);
lcd.print(dath1);
 }
}
if (tt==2){
  if (now.hour()<10){
 lcd.setCursor(2,0);
lcd.print("0"); 
lcd.setCursor(3,0);
lcd.print(now.hour());
 }
 else
 {
 lcd.setCursor(2,0);
lcd.print(now.hour());
 }
 lcd.setCursor(4,0);
lcd.print(":");

if (now.minute()<10){
 lcd.setCursor(5,0);
lcd.print("0"); 
lcd.setCursor(6,0);
lcd.print(now.minute());
 }
 else
 {
 lcd.setCursor(5,0);
lcd.print(now.minute());
 }
  lcd.setCursor(1,1);
  lcd.print(">");  
  lcd.setCursor(2,1);
lcd.print(bme280.getTemperature()/1.13);
lcd.setCursor(7,1);
lcd.print("c");
lcd.setCursor(9,1);
lcd.print(bme280.getHumidity()*1.3);
}
if (tt==3){
  if (now.hour()<10){
 lcd.setCursor(2,0);
lcd.print("0"); 
lcd.setCursor(3,0);
lcd.print(now.hour());
 }
 else
 {
 lcd.setCursor(2,0);
lcd.print(now.hour());
 }
 lcd.setCursor(4,0);
lcd.print(":");

if (now.minute()<10){
 lcd.setCursor(5,0);
lcd.print("0"); 
lcd.setCursor(6,0);
lcd.print(now.minute());
 }
 else
 {
 lcd.setCursor(5,0);
lcd.print(now.minute());
 }
  lcd.setCursor(2,1);
lcd.print("Pres");
lcd.setCursor(9,1);
lcd.print(bme280.getPressure()/133);
}
if (tt==0){
  digitalClockDisplay();  
}


  tt=tt+1;
  if (tt==4) tt=1;
  tt1=tt1-1;
 
  if (tt1==0) {
  tt=0;
  lcd.clear();
  }
  timing = millis(); 
 
}
}

 
if ((analogRead(A3)&&(tt>0))>0){ 
kl=analogRead(A3);
klava();
}
if (analogRead(A3)==0) timerkn0=0;
if ((analogRead(A2)>750)&&(triag==0)){  
triag=1;
tt1=6;
tt=1;
goto st1;
};

if ((analogRead(A7)>300)&&(bat1==0)){  
tt1=6;
tt=1;
goto st1;
};

if ((analogRead(A2)>750)&&(trigarm==2))alarmoff();
if ((analogRead(A7)>300)&&(trigarm==2))alarmoff();


if (analogRead(A7)>300){
timingsw=millis(); 
trigsw=120;
analogWrite(10,trigsw);
}
if (analogRead(A2)>750){
analogWrite(10,0);
trigsw=0;
}
if ((analogRead(A2)>750)&&(nmode==1)){
timinglcd=millis();
digitalWrite(9,HIGH); 
}
if ((analogRead(A7)>300)&&(nmode==1)){
timinglcd=millis();
digitalWrite(9,HIGH);
} 


if ((millis() - timingsw > 20000)&&(trigsw>0)){
trigsw=trigsw-1; 
timingsw=millis();  
analogWrite(10,trigsw);
}

if ((millis() - timinglcd > 30000)&&(nmode==1)){
if (digitalRead(9)==HIGH) digitalWrite(9,LOW); 
timinglcd=millis();  
}

if ((analogRead(A0)>300)&&(bat1==1)) {
  digitalWrite(11,HIGH);
  delay(3000);
  digitalWrite(11,LOW);
  bat1=0;
}
if ((analogRead(A0)<300)&&(bat1==0)) {
  bat1=1;
}



sng=now.day(); 
secc=now.second();
houral=EEPROM.read(1);
minual=EEPROM.read(2);
alarm=EEPROM.read(3);
if ((now.hour()==houral)&&(now.minute()==minual)&&(alarm==1)){
if ((now.second()<2)&&(trigarm==0)) {
  trigarm=1;
  yy=0;  
}
  alarmm();

}

if ((now.hour()==houral)&&(now.minute()==minual+5)&&(alarm>0))alarmoff();
}

void alarmoff(){
trigarm=0;
trigsw=0;
yy=0;
analogWrite(10,0);  
wtv020sd16p.stopVoice();
delay(2000);      
}

void alarmm(){  
  
if (trigarm>0) {
  analogWrite(10,int (yy));
  yy=yy+0.02;
  if (yy>120) yy=120;
}
if (trigarm==1){
trigarm=2;
wtv020sd16p.playVoice(sng);
timinglcd=millis();
digitalWrite(9,HIGH);  
}
}


void klava()
{
if (kl>800) { 
klv=1;
timerkn0=timerkn0+1; 
if(timerkn0>150){
menu1();
timerkn0=0;
}
}
if ((kl<800)&&(kl>600)) { 
  klv=2;
timerkn0=timerkn0+1; 
if(timerkn0>100){
menu2();
timerkn0=0;
}
}
if ((kl<600)&&(kl>450)) {
  klv=3;
  timerkn0=timerkn0+1; 
if(timerkn0>100){
menu2();
}
} 

if ((kl<450)&&(kl>360)) {
  klv=4;
  timerkn0=0;
  } 
if ((kl<360)&&(kl>100)) {
  klv=5;
  timerkn0=0;
  }
kl=0; 
klv=0;
}

void menu1()
{
  lcd.setCursor(9,0);
  lcd.print("@");
  alarm=!alarm; 
  EEPROM.write(3,alarm);
  if (alarm==0){
    lcd.setCursor(8,0);
lcd.print("        ");  
  }  
else
{
  alarmon();}  

}



void custom0(int x)
{ // uses segments to build the number 0

  lcd.setCursor(x,0); // set cursor to column 0, line 0 (first row)
  lcd.write(0);  // call each segment to create
  lcd.write(1);  // top half of the number
  lcd.write(2);
  lcd.setCursor(x, 1); // set cursor to colum 0, line 1 (second row)
  lcd.write(3);  // call each segment to create
  lcd.write(4);  // bottom half of the number
  lcd.write(5);
}

void custom1(int x)
{
  lcd.setCursor(x,0);
  lcd.write(1);
  lcd.write(2);
  lcd.print(" ");
  lcd.setCursor(x,1);
  lcd.write(4);
  lcd.write(7);
  lcd.write(4);
}

void custom2(int x)
{
  lcd.setCursor(x,0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(4);
}

void custom3(int x)
{
  lcd.setCursor(x,0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5); 
}

void custom4(int x)
{
  lcd.setCursor(x,0);
  lcd.write(3);
  lcd.write(4);
  lcd.write(7);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);
}

void custom5(int x)
{
  lcd.setCursor(x,0);
  lcd.write(3);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5);
}

void custom6(int x)
{
  lcd.setCursor(x,0);
  lcd.write(0);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}

void custom7(int x)
{
  lcd.setCursor(x,0);
  lcd.write(1);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);
}

void custom8(int x)
{
  lcd.setCursor(x,0);
  lcd.write(0);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}

void custom9(int x)
{
  lcd.setCursor(x,0);
  lcd.write(0);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);

}

void digitalClockDisplay(){
  // digital clock display of the time
  DateTime now = rtc.now();

  printDigits(now.hour()/10,0); 
  printDigits(now.hour()%10,4); 

  printDigits(now.minute()/10,8);
  printDigits(now.minute()%10,12);

  

  if (now.second()%10%2==0){
    lcd.setCursor(7, 0);
   if (alarm==0) lcd.print("+");
   if (alarm==1) lcd.print("!");
    lcd.setCursor(7, 1);
   if (alarm==0) lcd.print("+");
   if (alarm==1) lcd.print("!");
  }
  else
  {
    lcd.setCursor(7, 0);
    lcd.print(" ");
    lcd.setCursor(7, 1);
    lcd.print(" ");
  }
}


void printDigits(int digits, int x){
  // utility function for digital clock display: prints preceding colon and leading 0

  switch (digits) {
  case 0:  
    custom0(x);
    break;
  case 1:  
    custom1(x);
    break;
  case 2:  
    custom2(x);
    break;
  case 3:  
    custom3(x);
    break;
  case 4:  
    custom4(x);
    break;
  case 5:  
    custom5(x);
    break;
  case 6:  
    custom6(x);
    break;
  case 7:  
    custom7(x);
    break;
  case 8:  
    custom8(x);
    break;
  case 9:  
    custom9(x);
    break;

  }
}

void alarmon(){
  if (alarm==1){
lcd.setCursor(8,0);
lcd.print("@");

houral=EEPROM.read(1);
if (houral<10){
lcd.setCursor(9,0);
lcd.print("0");
lcd.setCursor(10,0);
lcd.print(houral); 
}
else
{
lcd.setCursor(9,0);
lcd.print(houral);  
}
minual=EEPROM.read(2);
if (minual<10){
lcd.setCursor(12,0);
lcd.print("0");
lcd.setCursor(13,0);
lcd.print(minual);  
}  
else
{
lcd.setCursor(12,0);
lcd.print(minual);  
}
lcd.setCursor(11,0);
lcd.print(":"); 
} 
}

void menu2()
{
  lcd.clear();
  delay(1000);
  lcd.setCursor(5,0);
  lcd.print("SET ALARM");
  houral=EEPROM.read(1);
  minual=EEPROM.read(2);
  
start3:;
if (houral<10){
lcd.setCursor(5,1);
lcd.print("0");
lcd.setCursor(6,1);
lcd.print(houral);  
}
else
{
lcd.setCursor(5,1);
lcd.print(houral);  
}

if (minual<10){
lcd.setCursor(8,1);
lcd.print("0");
lcd.setCursor(9,1);
lcd.print(minual);  
}  
else
{
lcd.setCursor(8,1);
lcd.print(minual);  
}
lcd.setCursor(7,1);
lcd.print(":");  


if ((analogRead(A3)>0)&&(tt>0)&&(alarm=1)){ 
kl2=analogRead(A3);
if (kl2>800) { 
timerkn0=timerkn0+1; 
if(timerkn0>100){
tt=0;
timerkn0=0;
EEPROM.write(1,houral);
EEPROM.write(2,minual);
goto start2;

}
}

if ((kl2<450)&&(kl2>360)) { //left
ajj();  
goto start2;
  } 
  
if ((kl2<360)&&(kl2>100)) { //right
ajj(); 
goto start2;
  }



if ((kl2<800)&&(kl2>600)) { //down
timerkn0=timerkn0+1; 
if(timerkn0>30){
minual=minual-10;
if (minual<0) {
minual=60+minual;
houral=houral-1;
if (houral<0) houral=23; 
}
timerkn0=0;
}
}
if ((kl2<600)&&(kl2>450)) { //up
  timerkn0=timerkn0+1; 
if(timerkn0>30){
minual=minual+10;
if (minual>=60) {
  minual=minual-60;
  houral=houral+1;
  if (houral==24) houral=0;
}
timerkn0=0;
}
} 



}
goto start3;



start2:;
timinglcd=millis(); 
lcd.clear();
}

void ajj(){
  lcd.clear();
  delay(1000);
lcd.setCursor(3,0);
lcd.print("Time ajust");
delay(2000);
  
  
  DateTime now = rtc.now();
  timingsw=millis();
  mm=now.minute();
yc=now.year();
mc=now.month();
dc=now.day();
hc=now.hour();
ns=now.second(); 
okk:;  
kl2=analogRead(A3);
if (mm<10){ 
lcd.setCursor(9,1);
lcd.print("0");
lcd.setCursor(10,1);
lcd.print(mm);
}
else
{
lcd.setCursor(9,1);
lcd.print(mm);
}
if (hc<10){ 
lcd.setCursor(6,1);
lcd.print("0");
lcd.setCursor(7,1);
lcd.print(hc);
}
else
{
lcd.setCursor(6,1);
lcd.print(hc);
}
if ((kl2<800)&&(kl2>600)){
delay(100); 
timingsw=millis();
mm=mm-1;
if (mm<0) {
  hc=hc-1;
  mm=59;
  if (hc<0) hc=23;
}
}
if ((kl2<600)&&(kl2>450)){
delay(100);  
timingsw=millis();
mm=mm+1;
if (mm>59) {
  hc=hc+1;
  mm=0;
  if (hc>23) hc=0;
}
}
if (kl2>800){
delay(100); 
rtc.adjust(DateTime(yc, mc, dc, hc, mm, ns));
rtc.begin();
lcd.clear();
  delay(1000);
lcd.setCursor(3,0);
lcd.print("OK");
delay(2000);
lcd.clear();
goto ok1;
}

if (millis()-timingsw>30000) goto ok1;
goto okk;

ok1:;
lcd.clear();
}

   
