
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
  int redOut, irOut, i, blankOut;
  float ratio;
 
  redOut = readingRED();
  irOut = readingIR();
  ratio = calculateSPO2((float) redOut,(float) irOut);
 
  Serial.println(ratio);
}

int readingRED() {
  //--Initializations--
  int raw, avgRED, i;
  //-------------------
  
  analogWrite(RED, 60);         //turn on RED LED
  for (i=0; i<AVGBUFFER; i++) { //start runnning average
    raw=analogRead(0);          //read photodiode signal
    avgRED = avgRED + raw; //calculate average
  }
  digitalWrite(RED,HIGH);       //turn off RED LED
  return avgRED / AVGBUFFER;                //return total average
}

int readingBlank() {
  int avgBlank;
  int raw, i;
  for (i=0; i < AVGBUFFER; i++) {
     raw=analogRead(0);
     avgBlank=avgBlank + raw;
  }
  return avgBlank / AVGBUFFER;
}

int readingIR() {
  //--Initializations--
  int raw, avgIR, i;
  int lastAve = 0;
  //-------------------
  
  analogWrite(IR, 250);          //turn on IR LED
  for (i=0;i<AVGBUFFER;i++) {    // start running average
    raw=analogRead(0);           //read photodiode signal
    avgIR = avgIR+raw;  //calculate average
  }
  digitalWrite(IR,HIGH);         //turn off IR LED
  return avgIR / AVGBUFFER;                  //return total average
}

float calculateSPO2(float rawIR, float rawRED) {
  float rmsIR, rmsRED, ratio;
  rmsIR = 0.5*rawIR/sqrt(2);
  rmsRED = 0.5*rawRED/sqrt(2);
  ratio = log10(rmsRED)/log10(rmsIR);
  return ratio;
}

