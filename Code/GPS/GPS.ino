#include <SoftwareSerial.h>
#include <TinyGPS++.h>

const int gpsRxPin = 4;
const int gpsTxPin = 5;

String latitude = "";
String longitude = "";
String coords = "";

SoftwareSerial gpsSerial(gpsRxPin, gpsTxPin);

TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  Serial.println("GPS Module Testing:");
}

void loop() {
  Serial.println(getCoords());
  delay(5000);
  }

String getCoords() {
  bool status = 0;
  while(!status){
    if(gpsSerial.available() > 0) {
      if (gps.encode(gpsSerial.read())) {
        if (gps.location.isValid()) {
          latitude = String(gps.location.lat(), 6);
          longitude = String(gps.location.lng(), 6);
          coords = "[" + latitude + ", " + longitude + "]";
          status = 1;
          return coords;
        }
        else {
          Serial.println("Waiting for GPS fix...");
        }
      }
    }
  }
}