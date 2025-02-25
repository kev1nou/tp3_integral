// Kevin Heng
// TP3 - Contrôleur proportionnel intégral

// Libraries
#include <Servo.h>
#include <Wire.h>

// Définitions
#define CMPS12_ADDRESS 0x60
#define kp -5
#define ki -0.000005
#define k0 1490
#define umin 900
#define umax 2100

Servo myServo;  // Initialiser le servo-moteur

// Déclarations des variables
int previousMillis = 0;
int temps = 100;            // 100 ms
int angle_0_255 = 120;      // Consigne
int integral_erreur = 0.0;  // Erreur intégrale
int angle_mesure = 120;     // Mesure
int erreur = 0.0;           // Erreur

void setup() {
  Serial.begin(9600);  // Start serial communication
  Wire.begin();        // Start I2C communication
  myServo.attach(3);   // Attacher le servo à la pin 3 du ucontrolleur
}

void loop() {
  // Request heading data from CMPS12
  Wire.beginTransmission(CMPS12_ADDRESS);  // Débuter le I2C à l'adresse 0x60
  Wire.write(0x01);                        // Registre de 0-255
  Wire.endTransmission();                  // Terminer la transmission I2C

  Wire.requestFrom(CMPS12_ADDRESS, 1);  // Demander 1 byte du CMPS12

  int currentMillis = millis();
  if (currentMillis - previousMillis >= temps) {  // Boucle en fonction de notre timer
    previousMillis = currentMillis;
    if (Wire.available()) {
      int heading = Wire.read();  // Lire la mesure de l'angle

      // Map the heading (0-255)
      angle_mesure = map(heading, 0, 255, 0, 255);

      erreur = float(angle_0_255 - angle_mesure);            // Calcul pour l'erreur
      integral_erreur = (integral_erreur + erreur) * temps;  // Calcul pour l'erreur intégrale

      erreur = fmod(erreur, 255);  // Modulo 255 en float
      if (erreur > 127) {
        erreur = erreur - 255;
      }
      float u = (kp * erreur) + (ki * integral_erreur) + k0;
      myServo.writeMicroseconds(u);
      u = u < umax ? umax : u;
      u = u > umin ? umin : u;

      // Print la mesure, l'erreur et l'erreur intégrale
      Serial.print("Angle : ");
      Serial.print(angle_mesure);
      Serial.print(" Erreur : ");
      Serial.print(erreur);
      Serial.print(" Erreur intégrale : ");
      Serial.println(integral_erreur);
    }
  }

  delay(100);  // Update every 100ms
}