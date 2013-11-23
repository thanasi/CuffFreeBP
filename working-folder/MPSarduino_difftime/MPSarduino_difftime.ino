
/*
>> Pulse Sensor Amped 1.1 <<
This code is for Pulse Sensor Amped by Joel Murphy and Yury Gitman
    www.pulsesensor.com 
    >>> Pulse Sensor purple wire goes to Analog Pin 0 <<<
Pulse Sensor sample aquisition and processing happens in the background via Timer 2 interrupt. 2mS sample rate.
PWM on pins 3 and 11 will not work when using this code, because we are using Timer 2!
The following variables are automatically updated:
Signal :    int that holds the analog signal data straight from the sensor. updated every 2mS.
IBI  :      int that holds the time interval between beats. 2mS resolution.
BPM  :      int that holds the heart rate value, derived every beat, from averaging previous 10 IBI values.
QS  :       boolean that is made true whenever Pulse is found and BPM is updated. User must reset.
Pulse :     boolean that is true when a heartbeat is sensed then false in time with pin13 LED going out.

This code is designed with output serial data to Processing sketch "PulseSensorAmped_Processing-xx"
The Processing sketch is a simple data visualizer. 
All the work to find the heartbeat and determine the heartrate happens in the code below.
Pin 13 LED will blink with heartbeat.
If you want to use pin 13 for something else, adjust the interrupt handler
It will also fade an LED on pin fadePin with every beat. Put an LED and series resistor from fadePin to GND.
Check here for detailed code walkthrough:
http://pulsesensor.myshopify.com/pages/pulse-sensor-amped-arduino-v1dot1

Code Version 02 by Joel Murphy & Yury Gitman  Fall 2012
This update changes the HRV variable name to IBI, which stands for Inter-Beat Interval, for clarity.
Switched the interrupt to Timer2.  500Hz sample rate, 2mS resolution IBI value.
Fade LED pin moved to pin 5 (use of Timer2 disables PWM on pins 3 & 11).
Tidied up inefficiencies since the last version. 

UPDATED BY EMMANUEL LIM
NOVEMBER 2013
Added support for 2 simultaneous measurements & transmissions
*/

//  VARIABLES
int pulsePin0 = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int pulsePin1 = 1;                 // Pulse Sensor purple wire connected to analog pin 1

// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM[2];                     // used to hold the pulse rate
volatile int Signal[2];            // holds the incoming raw data
volatile int IBI[2] = {600,600};           // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse[2] = {false,false}; // true when pulse wave is high, false when it's low
volatile boolean QS[2] = {false,false};   // becomes true when Arduino finds a beat

volatile bool DF = false;     // becomes f
volatile int DIFF = 0;        // holds the difference in ms between the two pulses


void setup(){
  Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal(s) every 2mS 
   // UN-COMMENT THE NEXT LINE IF YOU ARE POWERING The Pulse Sensor AT LOW VOLTAGE, 
   // AND APPLY THAT VOLTAGE TO THE A-REF PIN
   //analogReference(EXTERNAL);   
}


void loop(){
  sendDataToProcessing('S', Signal[0]);     // send Processing the raw Pulse Sensor data
  if (QS[0] == true){                       // Quantified Self flag is true when arduino finds a heartbeat
        sendDataToProcessing('B',BPM[0]);   // send heart rate with a 'B' prefix
        sendDataToProcessing('Q',IBI[0]);   // send time between beats with a 'Q' prefix
        QS[0] = false;                      // reset the Quantified Self flag for next time
  }
  sendDataToProcessing('s', Signal[1]);     // second sensor datapoint prefixed by lower case letters
  if (QS[1] == true){                       // Quantified Self flag is true when arduino finds a heartbeat
      sendDataToProcessing('b',BPM[1]);   // send heart rate with a 'B' prefix
      sendDataToProcessing('q',IBI[1]);   // send time between beats with a 'Q' prefix
      QS[1] = false;                      // reset the Quantified Self flag for next time
  }
  if (DF == true) {
      sendDataToProcessing('D',DIFF); 
      DF = false; 
  }
  delay(20);                             //  take a break
}

void sendDataToProcessing(char symbol, int data ){
    Serial.print(symbol);                // symbol prefix tells Processing what type of data is coming
    Serial.println(data);                // the data to send culminating in a carriage return
}
