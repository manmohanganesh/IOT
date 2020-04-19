#include <LiquidCrystal.h>
#include <HX711.h>
#include <SoftwareSerial.h>
SoftwareSerial s(8,9);   //8=Rx 9=Tx
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int Contrast = 75;
#define DT A0
#define SCK A1
#define sw 9

//LCD   Ard-UNO
//pin 1 to gnd
//pin 2 5v
//pin 3 6
//pin 4 12
//pin 5 gnd
//pin 6 11
//pin 11 5
//pin 12 4
//pin 13 3
//pin 14 2
//pin 15 5v
//pin 16 gnd

long sample = 0;
float val = 0;
long count = 0;

unsigned long readCount(void)
{
  unsigned long Count;
  unsigned char i;
  pinMode(DT, OUTPUT);
  digitalWrite(DT, HIGH);
  digitalWrite(SCK, LOW);
  Count = 0;
  pinMode(DT, INPUT);
  while (digitalRead(DT));
  for (i = 0; i < 24; i++)
  {
    digitalWrite(SCK, HIGH);
    Count = Count << 1;
    digitalWrite(SCK, LOW);
    if (digitalRead(DT))
      Count++;
  }
  digitalWrite(SCK, HIGH);
  Count = Count ^ 0x800000;
  digitalWrite(SCK, LOW);
  return (Count);
}
void setup()
{
  Serial.begin(9600);
  s.begin(9600);
  pinMode(SCK, OUTPUT);
  pinMode(sw, INPUT_PULLUP);
  analogWrite(6, Contrast);
  lcd.begin(16, 2);
  lcd.print(" Weight ");
  lcd.setCursor(0, 1);
  lcd.print(" Measurement ");
  delay(1000);
  lcd.clear();
  calibrate();
}
void loop()
{
  count = readCount();
  int w = (((count - sample) / val) - 2 * ((count - sample) / val));
  lcd.setCursor(0, 0);
  lcd.print("Measured Weight");
  lcd.setCursor(0, 1);
  lcd.print(w);
  lcd.print("g ");
  int data=100;
  if(s.available()>0)
  {
    s.write(data);                                 //Sending it to nodeMCU
  }
  if (digitalRead(sw) == 0)
  {
    val = 0;
    sample = 0;
    w = 0;
    count = 0;
    calibrate();
  }
}

void calibrate()
{
  lcd.clear();
//  lcd.print("Smart Pantry!"); //
//  lcd.setCursor(0, 1);
//  lcd.print("Project by");
//  delay(2000);
//  lcd.clear();
//  lcd.print("AISHWARYA");
//  lcd.setCursor(0, 1);
//  lcd.print("1847204");
//  delay(2000);
//  lcd.clear();
//  lcd.print("ALWIN");
//  lcd.setCursor(0, 1);
//  lcd.print("1847207 ");
//  delay(2000);
//  lcd.clear();
//  lcd.print("MANMOHAN");
//  lcd.setCursor(0, 1);
//  lcd.print("1847226");

  for (int i = 0; i < 100; i++)
  {
    count = readCount();
    sample += count;
  }
  sample /= 100;
  lcd.clear();
  lcd.print("Put 100g & wait");
  count = 0;
  while (count < 100)
  {
    count = readCount();
    count = sample - count;
  }
  lcd.clear();
  lcd.print("Please Wait....");
  delay(2000);
  for (int i = 0; i < 100; i++)
  {
    count = readCount();
    val += sample - count;
  }
  val = val / 100.0;
  val = val / 100.0; // put here your calibrating weight
  lcd.clear();
}
