#define analogPin 0
unsigned int valeurBrute = 0; // Variable résultat de la conversion analogique numérique
float tensionBatterie = 0; // Variable tension de batterie
const float VmaxCAN = 5.0; // Constante tension Max d’entrée convertisseur
const float q = VmaxCAN / 1023; // Constante quantum du convertisseur
const float k = VmaxCAN / 8.4 / 2; // Constante coefficient du diviseur de tension
unsigned int batterie = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(getBatterie());
  delay(100);
}

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