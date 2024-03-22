const int pinAnalogique = A0;

int voltageMaxBatterie = 7.4;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int valeurAnalogique = analogRead(pinAnalogique);

  float tension = valeurAnalogique * (5.0 / 1024) * 12.0;
  float pourcentage = (tension / voltageMaxBatterie) * 100.0;

  Serial.println(pourcentage, 1);

  delay(100);
}
