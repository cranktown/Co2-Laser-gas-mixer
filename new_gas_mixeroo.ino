//lcd library/setup
#include<Wire.h>;
#include<LiquidCrystal_I2C.h>;

LiquidCrystal_I2C lcd(0x27, 16, 2);



//button
int buttonPin = 2;

//limit switch
int switchPin = 3;


//relays
int inValve = 11;
int outValve = 12;
int pump = 13;


//debounce button vars
bool lastButton = false;
bool currentButton = false;

//debounce switch vars
bool lastSwitch = false;
bool currentSwitch = false;

//adjustable vars

//container volume in cubic inches
double contVol = 3927 ;

//desired percentages of gases
double hePer = .73;
double co2Per = .09;
double n2Per = .18;

//volume of airshim in cubic inches
double shimVol = 18.85;


//program status vars
bool allsystemsGo = false;
bool systemRunning = false;
bool heDone = false;
bool co2Done = false;
bool n2Done = false;


double currentVol = 0;

//arrays for lcd screen and lcd setup
char array1[] = "So how's that laser going bud";
char array2[] = "attach He then push button";
char array3[] = "attach Co2 then push button";
char array4[] = "attach N2 the push button";
char array5[] = "Yer done dummy! push button to reset";
char runningMsg[] = "filling...";

//reset function
void(* resetFunc) (void) = 0;


void setup() {
  pinMode(switchPin, INPUT);
  pinMode(buttonPin, INPUT);

  pinMode(inValve, OUTPUT);
  pinMode(outValve, OUTPUT);
  pinMode(pump, OUTPUT);

  digitalWrite(inValve, HIGH);
  digitalWrite(outValve, HIGH);
  digitalWrite(pump, HIGH);


  //lcd setup
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(" so how's that");
  lcd.setCursor(0, 1);
  lcd.print("laser going bud?");

  delay(3000);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("   subscribe!");

  delay(1000);

  lcd.clear();

  Serial.begin(9600);

}

//debouce button
boolean debounce (boolean last)
{
  boolean current = digitalRead(buttonPin);
  if (last != current);
  {
    delay(5);
    digitalRead(buttonPin);
  }
  return (current);
}


//debouce switch
boolean debounceLimit (boolean lastLimit)
{
  boolean currentLimit = digitalRead(switchPin);
  if (lastLimit != currentLimit);
  {
    delay(5);
    digitalRead(switchPin);
  }
  return (currentLimit);
}



void loop() {

  //doing maths
  double heVol = contVol * hePer;
  double co2Vol = contVol * co2Per;
  double n2Vol = contVol * n2Per;



  //while loops for display
  if ((heDone == false) && (co2Done == false) && (n2Done == false) && (systemRunning == false)) {
    lcd.setCursor(0, 0);
    lcd.print(" attach He then ");
    lcd.setCursor(0, 1);
    lcd.print("  push button   ");
  }


  if ((heDone == true) && (co2Done == false) && (n2Done == false) && (systemRunning == false)) {
    lcd.setCursor(0, 0);
    lcd.print("attach Co2 then ");
    lcd.setCursor(0, 1);
    lcd.print("  push button   ");

  }


  if ((heDone == true) && (co2Done == true) && (n2Done == false) && (systemRunning == false))  {
    lcd.setCursor(0, 0);
    lcd.print(" attach N2 then ");
    lcd.setCursor(0, 1);
    lcd.print("  push button   ");
  }


  if ((heDone == true) && (co2Done == true) && (n2Done == true) && (systemRunning == false)) {

    lcd.setCursor(0, 0);
    lcd.print(" yer done, push ");
    lcd.setCursor(0, 1);
    lcd.print("button to reset ");
  }

  if (systemRunning == true) {

  }



  //button poosh
  currentButton = debounce(lastButton);
  if (lastButton == LOW && currentButton == HIGH) {
    allsystemsGo = true;
    if (systemRunning == true) {

    }

    while ((heDone == false) && (co2Done == false) && (n2Done == false) && (allsystemsGo == true)) {

      //he fill
      if (systemRunning == false) {
        digitalWrite(inValve, HIGH);
        digitalWrite(outValve, LOW);
        digitalWrite(pump, LOW);
        delay(3000);
        systemRunning = true;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("He level:");
      lcd.setCursor(0, 1);
      lcd.print(currentVol);
      lcd.print("/");
      lcd.print(heVol);

      digitalWrite(inValve, LOW);
      digitalWrite(outValve, HIGH);
      digitalWrite(pump, LOW);
      currentSwitch = debounceLimit(lastSwitch);
      if (lastSwitch == LOW && currentSwitch == HIGH) {
        digitalWrite(inValve, HIGH);
        digitalWrite(outValve, LOW);
        digitalWrite(pump, LOW);
        delay(1000);
        currentVol = currentVol + shimVol;
      }
      lastSwitch = currentSwitch;
      if (currentVol >= heVol) {
        digitalWrite(inValve, HIGH);
        digitalWrite(outValve, HIGH);
        digitalWrite(pump, HIGH);
        heDone = true;
        currentVol = 0;
        systemRunning = false;
        allsystemsGo = false;
      }


    }
    while ((heDone == true) && (co2Done == false) && (n2Done == false) && (allsystemsGo == true)) {

      //co2 fill
      if (systemRunning == false) {
        digitalWrite(inValve, HIGH);
        digitalWrite(outValve, LOW);
        digitalWrite(pump, LOW);
        delay(3000);
        systemRunning = true;
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Co2 level:");
      lcd.setCursor(0, 1);
      lcd.print(currentVol);
      lcd.print("/");
      lcd.print(co2Vol);

      digitalWrite(inValve, LOW);
      digitalWrite(outValve, HIGH);
      digitalWrite(pump, LOW);
      currentSwitch = debounceLimit(lastSwitch);
      if (lastSwitch == LOW && currentSwitch == HIGH) {
        digitalWrite(inValve, HIGH);
        digitalWrite(outValve, LOW);
        digitalWrite(pump, LOW);
        delay(1000);
        currentVol = currentVol + shimVol;
      }
      lastSwitch = currentSwitch;
      if (currentVol >= co2Vol) {
        digitalWrite(inValve, HIGH);
        digitalWrite(outValve, HIGH);
        digitalWrite(pump, HIGH);
        co2Done = true;
        currentVol = 0;
        systemRunning = false;
        allsystemsGo = false;
      }


    }
    while ((heDone == true) && (co2Done == true) && (n2Done == false) && (allsystemsGo == true)) {

      //n2 fill
      if (systemRunning == false) {
        digitalWrite(inValve, HIGH);
        digitalWrite(outValve, LOW);
        digitalWrite(pump, LOW);
        delay(3000);
        systemRunning = true;
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("N2 level:");
      lcd.setCursor(0, 1);
      lcd.print(currentVol);
      lcd.print("/");
      lcd.print(n2Vol);


      digitalWrite(inValve, LOW);
      digitalWrite(outValve, HIGH);
      digitalWrite(pump, LOW);
      currentSwitch = debounceLimit(lastSwitch);
      if (lastSwitch == LOW && currentSwitch == HIGH) {
        digitalWrite(inValve, HIGH);
        digitalWrite(outValve, LOW);
        digitalWrite(pump, LOW);
        delay(1000);
        currentVol = currentVol + shimVol;
      }
      lastSwitch = currentSwitch;
      if (currentVol >= n2Vol) {
        digitalWrite(inValve, HIGH);
        digitalWrite(outValve, HIGH);
        digitalWrite(pump, HIGH);
        n2Done = true;
        currentVol = 0;
        systemRunning = false;
        allsystemsGo = false;
      }
    }
    while ((heDone == true) && (co2Done == true) && (n2Done == true)  && (allsystemsGo == true)) {
      resetFunc();
    }

  }

  //debounce
  lastButton = currentButton;


}
