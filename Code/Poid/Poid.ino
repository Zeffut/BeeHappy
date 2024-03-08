#include <HX711_ADC.h>

//pins:
const int HX711_dout = 7; //mcu > HX711 dout pin
const int HX711_sck = 6; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

unsigned long t = 0;

void setup() {
  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();

  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);

  LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
  Serial.println("Startup is complete");
}




void loop(){
  Serial.println(getMasse());
}

float getMasse(){
  LoadCell.update();
  float masse = LoadCell.getData()*5.431971028; // on multiplie la valeur mesuré 
  return fabs(masse);
}