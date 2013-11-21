


volatile int rate[2][10];                            // used to hold last ten IBI values
<<<<<<< HEAD
volatile unsigned long sampleCounter[] = {0,0};         // used to determine pulse timing
volatile unsigned long lastBeatTime[] = {0,0};    // used to find the inter beat interval
volatile int P[] = {512,512};                     // used to find peak in pulse wave
volatile int T[] = {512,512};                     // used to find trough in pulse wave
volatile int thresh[] = {512, 512};               // used to find instant moment of heart beat
volatile int amp[] = {100,100};                   // used to hold amplitude of pulse waveform
volatile boolean firstBeat[] = {true,true};       // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat[] = {true,true};      // used to seed rate array so we startup with reasonable BPM
=======
volatile unsigned long sampleCounter = 0;         // used to determine pulse timing
volatile unsigned long lastBeatTime[] = {
  0,0};    // used to find the inter beat interval
volatile int P[] = {
  512,512};                     // used to find peak in pulse wave
volatile int T[] = {
  512,512};                     // used to find trough in pulse wave
volatile int thresh[] = {
  512, 512};               // used to find instant moment of heart beat
volatile int amp[] = {
  100,100};                   // used to hold amplitude of pulse waveform
volatile boolean firstBeat[] = {
  true,true};       // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat[] = {
  true,true};      // used to seed rate array so we startup with reasonable BPM
>>>>>>> fe45cc5a2e56e45c5ece9848b55ec9f808c73492


void interruptSetup(){     
  // Initializes Timer2 to throw an interrupt every 2mS.
  TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER 
  OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED      
} 


// THIS IS THE TIMER 2 INTERRUPT SERVICE ROUTINE. 
// Timer 2 makes sure that we take a reading every 2 miliseconds
ISR(TIMER2_COMPA_vect){                         // triggered when Timer2 counts to 124
<<<<<<< HEAD
    cli();                                      // disable interrupts while we do this

    // read both sensors first before processing
    // to know we're reading at the same time
    Signal[0] = analogRead(pulsePin0);          // read the Pulse Sensor 1
    Signal[1] = analogRead(pulsePin1);          // read the Pulse Sensor 2
    sampleCounter += 2;                         // keep track of the time in mS with this variable
    int N[] = {0,0};
    N[0] = sampleCounter - lastBeatTime[0];     // monitor the time since the last beat to avoid noise
    N[1] = sampleCounter - lastBeatTime[1];

    // find the peak and trough of the pulse waves
    // cycle through the two sensors
    for (int i = 0; i<2; i++) {

      if(Signal[i] < thresh[i] && N[i] > (IBI[i]/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
          if (Signal[i] < T[i]){                        // T is the trough
              T[i] = Signal[i];                         // keep track of lowest point in pulse wave 
          }
=======
  cli();                                      // disable interrupts while we do this

  // read both sensors first before processing
  // to know we're reading at the same time
  Signal[0] = analogRead(pulsePin0);          // read the Pulse Sensor 
  Signal[1] = analogRead(pulsePin1);
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  int N[] = {
    0,0  };
  N[0] = sampleCounter - lastBeatTime[0];     // monitor the time since the last beat to avoid noise
  N[1] = sampleCounter - lastBeatTime[1];

  // find the peak and trough of the pulse waves
  // cycle through the two sensors
  for (int i = 0; i<2; i++) {

    if(Signal[i] < thresh[i] && N[i] > (IBI[i]/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
      if (Signal[i] < T[i]){                        // T is the trough
        T[i] = Signal[i];                         // keep track of lowest point in pulse wave 
>>>>>>> fe45cc5a2e56e45c5ece9848b55ec9f808c73492
      }
    }

    if(Signal[i] > thresh[i] && Signal[i] > P[i]){ // thresh condition helps avoid noise
      P[i] = Signal[i];                          // P is the peak
    }                                              // keep track of highest point in pulse wave

    //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
    // signal surges up in value every time there is a pulse
    if (N[i] > 250){                                   // avoid high frequency noise
      if ( (Signal[i] > thresh[i]) && (Pulse[i] == false) && (N[i] > (IBI[i]/5)*3) ){        
        Pulse[i] = true;                               // set the Pulse flag when we think there is a pulse
        digitalWrite(blinkPin[i],HIGH);                // turn on pin 13 LED
        IBI[i] = sampleCounter - lastBeatTime[i];      // measure time between beats in mS
        lastBeatTime[i] = sampleCounter;               // keep track of time for next pulse

        if(firstBeat[i]){                         // if it's the first time we found a beat, if firstBeat == TRUE
          firstBeat[i] = false;                 // clear firstBeat flag
          continue;                             // IBI value is unreliable so move on
        }   

        if(secondBeat[i]){                        // if this is the second beat, if secondBeat == TRUE
          secondBeat[i] = false;                 // clear secondBeat flag
          for(int j=0; j<=9; j++){         // seed the running total to get a realistic BPM at startup
            rate[j][j] = IBI[i];                      
          }
        }

        // keep a running total of the last 10 IBI values
        word runningTotal = 0;                   // clear the runningTotal variable    

        for(int j=0; j<=8; j++){                // shift data in the rate array
          rate[i][j] = rate[i][j+1];              // and drop the oldest IBI value 
          runningTotal += rate[i][j];          // add up the 9 oldest IBI values
        }

        rate[i][9] = IBI[i];                          // add the latest IBI to the rate array
        runningTotal += rate[i][9];                // add the latest IBI to runningTotal
        runningTotal /= 10;                     // average the last 10 IBI values 
        BPM[i] = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
        QS[i] = true;                              // set Quantified Self flag 
        // QS FLAG IS NOT CLEARED INSIDE THIS ISR
      }                       
    } // end search for heart beats

    // cyclic reset
    if (Signal[i] < thresh[i] && Pulse[i] == true){  // when the values are going down, the beat is over
      digitalWrite(blinkPin[i],LOW);            // turn off pin 13 LED
      Pulse[i] = false;                         // reset the Pulse flag so we can do it again
      amp[i] = P[i] - T[i];                     // get amplitude of the pulse wave
      thresh[i] = amp[i]/2 + T[i];                    // set thresh at 50% of the amplitude
      P[i] = thresh[i];                            // reset these for next time
      T[i] = thresh[i];
    } // end cyclic reset

    // reset if no response
    if (N[i] > 2500){                             // if 2.5 seconds go by without a beat
      thresh[i] = 512;                          // set thresh default
      P[i] = 512;                               // set P default
      T[i] = 512;                               // set T default
      lastBeatTime[i] = sampleCounter;          // bring the lastBeatTime up to date        
      firstBeat[i] = true;                      // set these to avoid noise
      secondBeat[i] = true;                     // when we get the heartbeat back
    } // end no response reset
  } // end looping through two sensors

  sei();                                     // enable interrupts when youre done!
}// end isr





