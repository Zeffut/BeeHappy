#include <SoftwareSerial.h>
#include <TinyGPS++.h>

const int pinAnalogique = A0;
int voltageMaxBatterie = 30;


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
String phoneNumber = "+33695468219";
String prefix = "R";

unsigned long previousMillis = 0;
const long interval = 100;

void setup() {
  //Pour debugger
  Serial.begin(9600);
  Serial.println("Demarrage...");

  //Partie GPS
  gpsSerial.begin(9600);
  getCoords();
  gpsSerial.end();
  delay(100);
  Serial.println("GPS Pret !");

  //Partie GSM
  //Allumage du module GSM
  startGSM();

  SIM900.begin(19200);

  SIM900.print("AT+CMGF=1\r");
  waitForOK();

  SIM900.print("AT+CNMI=2,2,0,0,0\r");
  waitForOK();

  Serial.println("GSM Pret !");
}

void loop() {
  //Boucle de détéction des commandes
  if (SIM900.available() > 0){
    char incoming_char = SIM900.read();
    if (incoming_char == prefix.charAt(0)) {
      Serial.println("Comande reçue !");
      for (int i = 0; i < 3; ++i) {
        while (!SIM900.available()) {}
        incoming_char = SIM900.read();
        command += incoming_char;
      }
      if(command == "bat"){
        Serial.println("Commande Batterie");
        sendSMS(getBatterie());
        Serial.println("Message Envoye !");
      }
      if(command == "gps"){
        Serial.println("Commande GPS");
        SIM900.end();
        gpsSerial.begin(9600);
        getCoords();
        Serial.println("Coordonees Obtenue !");
        gpsSerial.end();
        delay(100);
        SIM900.begin(19200);
        coords = "[" + latitude + ", " + longitude + "]";
        sendSMS(coords);
        Serial.println("Message Envoye !");
      }
      if(command == "pds"){
        Serial.println("Commande Poids");
        sendSMS("Poids");
        Serial.println("Message Envoye !");
      }
    }
  }
  command = "";
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
  int valeurAnalogique = analogRead(pinAnalogique);
  float tension = valeurAnalogique * (5.0 / 1024) * 12.0;
  float pourcentage = (tension / voltageMaxBatterie) * 100.0;
  return String(pourcentage, 1);
}