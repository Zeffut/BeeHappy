#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <HX711_ADC.h>


//Partie Jauge de contrainte

const int HX711_dout = 7; //mcu > HX711 dout pin
const int HX711_sck = 6; //mcu > HX711 sck pin

HX711_ADC LoadCell(HX711_dout, HX711_sck);

unsigned long t = 0;

//Partie Batterie

#define analogPin 0
unsigned int valeurBrute = 0; // Variable résultat de la conversion analogique numérique
float tensionBatterie = 0; // Variable tension de batterie
const float VmaxCAN = 5.0; // Constante tension Max d’entrée convertisseur
const float q = VmaxCAN / 1023; // Constante quantum du convertisseur
const float k = VmaxCAN / 8.4 / 2; // Constante coefficient du diviseur de tension
float pourcentage = 0;
unsigned int batterie = 0;

//Partie GPS

const int gpsRxPin = 4;
const int gpsTxPin = 5;

String latitude = "";
String longitude = "";
String coords = "";

SoftwareSerial gpsSerial(gpsRxPin, gpsTxPin);

TinyGPSPlus gps;

//Partie GSM

SoftwareSerial SIM900(7, 8);

String command = "";
String phoneNumber = "+33769996016"; //numéro ruche +33769996016
String prefix = "R";

unsigned long previousMillis = 0;
const long interval = 100;

void setup() {
  //Pour debugger
  Serial.begin(9600);
  Serial.println("Demarrage...");

  //Partie jauge de contrainte
  LoadCell.begin();

  float calibrationValue;
  calibrationValue = 696.0;

  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);

  LoadCell.setCalFactor(calibrationValue);
  
  Serial.println("Calibration complete");

  //Partie GPS
  gpsSerial.begin(9600);
  getCoords();
  gpsSerial.end();
  delay(100);
  Serial.println("GPS Pret !");

  //Partie GSM
  //Allumage du module GSM
  startGSM();
  delay(1000);

  SIM900.begin(19200);

  SIM900.print("AT+CMGF=1\r");
  waitForOK();

  SIM900.print("AT+CNMI=2,2,0,0,0\r");
  waitForOK();

  Serial.println("GSM Pret !");
}

void loop() {
  String batterie = getBatterie();
  String poids = String(getMasse());
  SIM900.end();
  delay(1000);
  gpsSerial.begin(9600);
  getCoords();
  gpsSerial.end();
  delay(1000);
  SIM900.begin(19200);
  Serial.print("Latitude: ");
  Serial.println(latitude);
  Serial.print("Longitude: ");
  Serial.println(longitude);
  Serial.print("Batterie: ");
  Serial.println(batterie);
  Serial.print("Poids: ");
  Serial.println(poids);
  sendSMS("{\"coordsLat\":" + latitude + ",\"coordsLng\":" + longitude + ",\"battery\":" + batterie + ",\"weight\":" + poids + "}");
  delay(1000);
  sendDebug("{\"coordsLat\":" + latitude + ",\"coordsLng\":" + longitude + ",\"battery\":" + batterie + ",\"weight\":" + poids + "}");
  Serial.println("Message Envoye !");
  delay(298000);
}

//Fonction pour envoyer un SMS sous forme de variable STRING
void sendSMS(String message) {
  SIM900.print("AT+CMGF=1\r");
  waitForOK();

  SIM900.print("AT+CMGS=\"");
  SIM900.print(phoneNumber);
  SIM900.println("\"");
  waitForOK();

  SIM900.println(message);
  waitForOK();

  SIM900.println((char)26);
  waitForOK();

  SIM900.println();
  waitForOK();
}

void sendDebug(String message) {
  SIM900.print("AT+CMGF=1\r");
  waitForOK();

  SIM900.print("AT+CMGS=\"");
  SIM900.print("+33695468219");
  SIM900.println("\"");
  waitForOK();

  SIM900.println(message);
  waitForOK();

  SIM900.println((char)26);
  waitForOK();

  SIM900.println();
  waitForOK();
}

//Fonctions pour attendre que le module GSM renvoie "OK"
void waitForOK() {
  unsigned long currentMillis = millis();

  while (millis() - currentMillis < interval) {
    if (SIM900.find("OK")) {
      break;
    }
  }
}

//Fonctions pour enregistrer les coordonnées gps dans des variables global
void getCoords() {
  bool status = false;
  while (!status) {
    if (gpsSerial.available() > 0) {
      char c = gpsSerial.read();
      if (gps.encode(c)) {
        if (gps.location.isValid() && gps.location.age() < 2000) {
          latitude = String(gps.location.lat(), 6);
          longitude = String(gps.location.lng(), 6);
          status = true;
          break;
        }
        else {
          Serial.println("Waiting for GPS fix...");
          Serial.println();
        }
      }
    }
  }
  Serial.println("Localisation Effectué !");
}

//Simulation de l'appuie du bouton ON/OFF du module GSM
String startGSM() {
  digitalWrite(9, HIGH);
  delay(1500);
  digitalWrite(9, LOW);
  delay(10000);
}

//Fonction qui retourne le pourcentage de batterie restant entre 0 et 1
String getBatterie(){
  float pourcentage = 0;
  valeurBrute = analogRead(analogPin);
  tensionBatterie = (valeurBrute * q) / k;
  batterie = int(pourcentage);
  if (tensionBatterie < 7.3){
    pourcentage = 8.333 * tensionBatterie - 50;
  }
  if (tensionBatterie > 7.3){
    if (tensionBatterie < 7.94){
      pourcentage = 111.06 * tensionBatterie - 800.88;
    }
  }
  if (tensionBatterie > 7.94){
    pourcentage = 42,233 * tensionBatterie - 263,65;
  }
  return String(pourcentage, 1);
}

//Fonction qui retourne le poids en gramme
float getMasse(){
  LoadCell.update();
  float masse = LoadCell.getData()*5.431971028; // on multiplie la valeur mesuré 
  return fabs(masse);
}