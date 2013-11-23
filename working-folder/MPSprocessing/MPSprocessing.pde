import processing.serial.*;
/*
THIS PROGRAM WORKS WITH PulseSensorAmped_Arduino-xx ARDUINO CODE
 THE PULSE DATA WINDOW IS SCALEABLE WITH SCROLLBAR AT BOTTOM OF SCREEN
 PRESS 'S' OR 's' KEY TO SAVE A PICTURE OF THE SCREEN IN SKETCH FOLDER (.jpg)
 MADE BY JOEL MURPHY AUGUST, 2012
 */


PFont font;
Scrollbar scaleBar;

Serial port;

int [] Sensor;      // HOLDS PULSE SENSOR DATA FROM ARDUINO
int [] IBI;         // HOLDS TIME BETWEN HEARTBEATS FROM ARDUINO
int [] BPM;         // HOLDS HEART RATE VALUE FROM ARDUINO
int [][] RawY;      // HOLDS HEARTBEAT WAVEFORM DATA BEFORE SCALING
int [][] ScaledY;   // USED TO POSITION SCALED HEARTBEAT WAVEFORM
int [][] rate;      // USED TO POSITION BPM DATA WAVEFORM
float zoom;      // USED WHEN SCALING PULSE WAVEFORM TO PULSE WINDOW
float offset;    // USED WHEN SCALING PULSE WAVEFORM TO PULSE WINDOW
color eggshell = color(255, 253, 248);
int heart = 0;   // This variable times the heart image 'pulse' on screen
//  THESE VARIABLES DETERMINE THE SIZE OF THE DATA WINDOWS
int PulseWindowWidth = 490;
int PulseWindowHeight = 512; 
int BPMWindowWidth = 180;
int BPMWindowHeight = 340;
boolean beat = false;    // set when a heart beat is detected, then cleared when the BPM graph is advanced


void setup() {
  size(700, 600);  // Stage size
  frameRate(100);  
  font = loadFont("Arial-BoldMT-24.vlw");
  textFont(font);
  textAlign(CENTER);
  rectMode(CENTER);
  ellipseMode(CENTER);  
  // Scrollbar constructor inputs: x,y,width,height,minVal,maxVal
  scaleBar = new Scrollbar (400, 575, 180, 12, 0.5, 1.0);  // set parameters for the scale bar
  RawY = new int[2][PulseWindowWidth];          // initialize raw pulse waveform array
  ScaledY = new int[2][PulseWindowWidth];       // initialize scaled pulse waveform array
  rate = new int[2][BPMWindowWidth];           // initialize BPM waveform array
  zoom = 0.75;                               // initialize scale of heartbeat window

  Sensor = new int[2];      // HOLDS PULSE SENSOR DATA FROM ARDUINO
  IBI = new int[2];         // HOLDS TIME BETWEN HEARTBEATS FROM ARDUINO
  BPM = new int[2];         // HOLDS HEART RATE VALUE FROM ARDUINO

  for (int j=0; j<2; j++) {
    Sensor[j] = 0;
    IBI[j] = 0;
    BPM[j] = 0;

    // set the visualizer lines to 0
    for (int i=0; i<rate[j].length; i++) {
      rate[j][i] = 555;      // Place BPM graph line at bottom of BPM Window
    }
    for (int i=0; i<RawY[j].length; i++) {
      RawY[j][i] = height/2; // initialize the pulse window data line to V/2
    }
  }

  // GO FIND THE ARDUINO
  
  String [] slist = Serial.list();
  println(slist);    // print a list of available serial ports
 
  // choose the number between the [] that is connected to the Arduino
  port = new Serial(this, Serial.list()[7], 115200);  // make sure Arduino is talking serial at this baud rate
  port.clear();            // flush buffer
  port.bufferUntil('\n');  // set buffer full flag on receipt of carriage return
}

void draw() {
  background(  0);
  noStroke();
  // DRAW OUT THE PULSE WINDOW AND BPM WINDOW RECTANGLES  
  fill(eggshell);  // color for the window background
  rect(255, height/2, PulseWindowWidth, PulseWindowHeight);
  rect(600, 385, BPMWindowWidth, BPMWindowHeight);

  // DRAW THE PULSE WAVEFORM for both sensors
  for (int j=0; j<2; j++) {
    // prepare pulse data points    
    RawY[j][RawY[j].length-1] = (1023 - Sensor[j]) - 212;   // place the new raw datapoint at the end of the array
    zoom = scaleBar.getPos();                      // get current waveform scale value
    offset = map(zoom, 0.5, 1, 150, 0);                // calculate the offset needed at this scale
    for (int i = 0; i < RawY[j].length-1; i++) {      // move the pulse waveform by
      RawY[j][i] = RawY[j][i+1];                         // shifting all raw datapoints one pixel left
      float dummy = RawY[j][i] * zoom + offset + (float) Math.pow(-1,j) * 50;       // adjust the raw data to the selected scale
      ScaledY[j][i] = constrain(int(dummy), 44, 556);   // transfer the raw data array to the scaled array
    }
    stroke(250-j*127,j*127, 0);                               // red is a good color for the pulse waveform
    noFill();
    beginShape();                                  // using beginShape() renders fast
    for (int x = 1; x < ScaledY[j].length-1; x++) {    
      vertex(x+10, ScaledY[j][x]);                    //draw a line connecting the data points
    }
    endShape();

    // DRAW THE BPM WAVE FORM
    // first, shift the BPM waveform over to fit then next data point only when a beat is found
    if (beat == true) {   // move the heart rate line over one pixel every time the heart beats 
      beat = false;      // clear beat flag (beat flag waset in serialEvent tab)
      for (int i=0; i<rate[j].length-1; i++) {
        rate[j][i] = rate[j][i+1];                  // shift the bpm Y coordinates over one pixel to the left
      }
      // then limit and scale the BPM value
      BPM[j] = min(BPM[j], 200);                     // limit the highest BPM value to 200
      float dummy = map(BPM[j], 0, 200, 555, 215);   // map it to the heart rate window Y
      rate[j][rate[j].length-1] = int(dummy);       // set the rightmost pixel to the new data point value
    } 
    // GRAPH THE HEART RATE WAVEFORM
    stroke(250, 0, 0);                          // color of heart rate graph
    strokeWeight(2);                          // thicker line is easier to read
    noFill();
    beginShape();
    for (int i=0; i < rate[j].length-1; i++) {    // variable 'i' will take the place of pixel x position   
      vertex(i+510, rate[j][i]);                 // display history of heart rate datapoints
    }
    endShape();

    // DRAW THE HEART AND MAYBE MAKE IT BEAT
    fill(250, 0, 0);
    stroke(250, 0, 0);
    // the 'heart' variable is set in serialEvent when arduino sees a beat happen
    heart--;                    // heart is used to time how long the heart graphic swells when your heart beats
    heart = max(heart, 0);       // don't let the heart variable go into negative numbers
    if (heart > 0) {             // if a beat happened recently, 
      strokeWeight(8);          // make the heart big
    }
    smooth();   // draw the heart with two bezier curves
    bezier(width-100, 50, width-20, -20, width, 140, width-100, 150);
    bezier(width-100, 50, width-190, -20, width-200, 140, width-100, 150);
    strokeWeight(1);          // reset the strokeWeight for next time


    // PRINT THE DATA AND VARIABLE VALUES
    fill(eggshell);                                       // get ready to print text
    text("Pulse Sensor Amped Visualizer 1.1", 245, 30);     // tell them what you are
    text("IBI " + IBI[0] + "mS", 600, 585);                    // print the time between heartbeats in mS
    text(BPM[0] + " BPM", 600, 200);                           // print the Beats Per Minute
    text("Pulse Window Scale " + nf(zoom, 1, 2), 150, 585); // show the current scale of Pulse Window

    //  DO THE SCROLLBAR THINGS
    scaleBar.update (mouseX, mouseY);
    scaleBar.display();
  } // end cycling through both sensors

}  //end of draw loop
