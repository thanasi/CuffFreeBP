
void serialEvent(Serial port) { 
  // port.bufferUntil('\n');
  // String inData = port.readString();
  
  String inData = new String(port.readBytesUntil('\n'));

  inData = trim(inData);                 // cut off white space (carriage return)

  // sensor
  if (inData.charAt(0) == 'S') {          // leading 'S' for sensor data
    inData = inData.substring(1);        // cut off the leading 'S'
    Sensor[0] = int(inData);             // convert the string to usable int
  }
  if (inData.charAt(0) == 'B') {          // leading 'B' for BPM data
    inData = inData.substring(1);        // cut off the leading 'B'
    BPM[0] = int(inData);                // convert the string to usable int
    beat = true;                      // set beat flag to advance heart rate graph
    heart = 20;                          // begin heart image 'swell' timer
  }  
  if (inData.charAt(0) == 'Q') {         // leading 'Q' means IBI data 
    inData = inData.substring(1);        // cut off the leading 'Q'
    IBI[0] = int(inData);                // convert the string to usable int
  }

  // second sensor
  if (inData.charAt(0) == 's') {          // leading 'S' for sensor data
    inData = inData.substring(1);        // cut off the leading 'S'
    Sensor[1] = int(inData);             // convert the string to usable int
  }
  if (inData.charAt(0) == 'b') {          // leading 'B' for BPM data
    inData = inData.substring(1);        // cut off the leading 'B'
    BPM[1] = int(inData);                // convert the string to usable int
    beat = true;                      // set beat flag to advance heart rate graph
    heart = 20;                         // begin heart image 'swell' timer
  }
  if (inData.charAt(0) == 'q') {           // leading 'Q' means IBI data 
    inData = inData.substring(1);        // cut off the leading 'Q'
    IBI[1] = int(inData);                // convert the string to usable int
  }
}
