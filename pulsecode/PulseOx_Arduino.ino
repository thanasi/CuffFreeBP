
#include <math.h>

//--GLOBAL VARIABLES--
int RED = 6;
int IR = 3;
int AVGBUFFER = 100;

void setup() {
  pinMode(RED, OUTPUT);    //set RED pin to output
  pinMode(IR, OUTPUT);     //set IR pin to output
  digitalWrite(RED, HIGH); //turn off RED LED
  digitalWrite(IR, HIGH);  //turn off IR LED
  Serial.begin(9600);
}

void loop() {
  int redOut, irOut, i;
  float ratio;
  
  redOut = readingRED();
  irOut = readingIR();
  ratio = calculateSPO2((float) redOut,(float) irOut);
  
//  Serial.print(redOut);
//  Serial.print(", ");
//  Serial.print(irOut);
//  Serial.print(", ");
  Serial.println(ratio);
}

int readingRED() {
  //--Initializations--
  int raw, avgRED, i;
  int lastAve = 0;
  //-------------------
  
  analogWrite(RED, 60);         //turn on RED LED
  for (i=0; i<AVGBUFFER; i++) { //start runnning average
    raw=analogRead(0);          //read photodiode signal
    avgRED = (lastAve + raw)/2; //calculate average
  }
  digitalWrite(RED,HIGH);       //turn off RED LED
  return avgRED;                //return total average
}

int readingIR() {
  //--Initializations--
  int raw, avgIR, i;
  int lastAve = 0;
  //-------------------
  
  analogWrite(IR, 250);          //turn on IR LED
  for (i=0;i<AVGBUFFER;i++) {    // start running average
    raw=analogRead(0);           //read photodiode signal
    avgIR = (lastAve + raw)/2;  //calculate average
  }
  digitalWrite(IR,HIGH);         //turn off IR LED
  return avgIR;                  //return total average
}

float calculateSPO2(float rawIR, float rawRED) {
  float rmsIR, rmsRED, ratio;
  rmsIR = 0.5*rawIR/sqrt(2);
  rmsRED = 0.5*rawRED/sqrt(2);
  ratio = log10(rmsRED)/log10(rmsIR);
  return ratio;
}

