//wires are for gods!
#include<Wire.h>;

//because I am too broke to afford a real screen
//I'll just use this 1602 and a DFRobot library, I2C saves on wire too.
#include"src/LiquidCrystal_I2C/LiquidCrystal_I2C.h";
LiquidCrystal_I2C lcd(0x27, 16, 2);

//loops are loopy, states are stately
#include <StateMachine.h>
const int STATE_DELAY = 1000;

double contVol = 3927 ; //container volume in cubic inches
double shimVol = 18.85; //volume of airshim in cubic inches
double currentVol = 0;

//desired percentages of gases
double hePer = .73;
double co2Per = .09;
double n2Per = .18;

//debounce button vars
bool lastButton = false;
bool currentButton = false;

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

//debounce switch vars
bool lastSwitch = false;
bool currentSwitch = false;

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

//reset function
void(* resetFunc) (void) = 0;

StateMachine machine = StateMachine();
State* S0 = machine.addState(&S_MachineTurnedOn); 
State* S1 = machine.addState(&S_MachineWaitingHe);
State* S2 = machine.addState(&S_MachineFillingHe);
State* S3 = machine.addState(&S_MachineWaitingCO2);
State* S4 = machine.addState(&S_MachineFillingCO2);
State* S5 = machine.addState(&S_MachineWaitingN2);
State* S6 = machine.addState(&S_MachineFillingN2);
State* S7 = machine.addState(&S_MachineWaitingForShutdown);
State* S8 = machine.addState(&S_MachineSoftwareReset);

void setup() {
  //button
  int buttonPin = 2;
  
  //limit switch
  int switchPin = 3;
  
  //relays
  int inValve = 11;
  int outValve = 12;
  int pump = 13;

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

  S1->addTransition(&transition_MachineWaitingHE_MachineFillingHE,S2);
  S3->addTransition(&transition_MachineWaitingCO2_MachineFillingCO2,S4);
  S5->addTransition(&transition_MachineWaitingN2_MachineFillingN2,S6);
  S7->addTransition(&transition_MachineWaitingForShutdown_MachineSoftwareReset,S8);
  
  Serial.begin(9600);
}

void loop() {
  machine.run();
  delay(STATE_DELAY);
}

void S_MachineTurnedOn {
  if(machine.executeOnce){
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
  }
}

bool transition_MachineTurnedOn_MachineWaitingHe {
  return true;
}

void S_MachineWaitingHe {
  if(machine.executeOnce){
    lcd.setCursor(0, 0);
    lcd.print(" attach He then ");
    lcd.setCursor(0, 1);
    lcd.print("  push button   ");
  }
}

bool transition_MachineWaitingHe_MachineFillingHe {
  currentButton = debounce(lastButton);
  if (lastButton == LOW && currentButton == HIGH) {
    return true;
  }
  return false;
}

void S_MachineFillingHe {
  double heVol = contVol * hePer;
  if(machine.executeOnce){
    digitalWrite(inValve, HIGH);
    digitalWrite(outValve, LOW);
    digitalWrite(pump, LOW);
    delay(3000);
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

    currentVol = 0;
    machine.transitionTo(S3); //S_MachineWaitingCO2
  }
}

void S_MachineWaitingCO2 {
  if(machine.executeOnce){
    lcd.setCursor(0, 0);
    lcd.print("attach Co2 then ");
    lcd.setCursor(0, 1);
    lcd.print("  push button   ");
  }
}

bool transition_MachineWaitingCO2_MachineFillingCO2 {
  currentButton = debounce(lastButton);
  if (lastButton == LOW && currentButton == HIGH) {
    return true;
  }
  return false;
}

void S_MachineFillingCO2 {
  double co2Vol = contVol * co2Per;
  if(machine.executeOnce){
    digitalWrite(inValve, HIGH);
    digitalWrite(outValve, LOW);
    digitalWrite(pump, LOW);
    delay(3000);
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
    
    currentVol = 0;
    machine.transitionTo(S5); //S_MachineWaitingN2
  }
}

void S_MachineWaitingN2 {
  if(machine.executeOnce){
    lcd.setCursor(0, 0);
    lcd.print(" attach N2 then ");
    lcd.setCursor(0, 1);
    lcd.print("  push button   ");
  }
}

bool transition_MachineWaitingN2_MachineFillingN2 {
  currentButton = debounce(lastButton);
  if (lastButton == LOW && currentButton == HIGH) {
    return true;
  }
  return false;
}

void S_MachineFillingN2 {
  double n2Vol = contVol * n2Per;
  if(machine.executeOnce){
    digitalWrite(inValve, HIGH);
    digitalWrite(outValve, LOW);
    digitalWrite(pump, LOW);
    delay(3000);
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
    
    currentVol = 0;
    machine.transitionTo(S7); //S_MachineWaitingForShutdown
  }
}

void S_MachineWaitingForShutdown {
  if(machine.executeOnce){
    lcd.setCursor(0, 0);
    lcd.print(" yer done, push ");
    lcd.setCursor(0, 1);
    lcd.print("button to reset ");
  }
}

bool transition_MachineWaitingForShutdown_MachineSoftwareReset {
  currentButton = debounce(lastButton);
  if (lastButton == LOW && currentButton == HIGH) {
    return true;
  }
  return false;
}

void S_MachineSoftwareReset {
  resetFunc();
}
