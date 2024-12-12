#include <DHT.h>  // Include the DHT sensor library

// Pin configuration for MQ2, Ultrasonic, DHT Sensor, and LEDs
const int MQ2Pin = A0;        // MQ2 Gas Sensor connected to A0
const int trigPin = 4;        // Ultrasonic trigPin
const int echoPin = 5;        // Ultrasonic echoPin
const int DHTPin = 2;         // DHT sensor connected to pin 2
const int Buzzer_control = 3; // Buzzer pin
const int LED_D12 = 12;       // LED for distance > 50 cm
const int LED_D6 = 6;         // LED for gas value < 200
const int LED_D7 = 7;         // 200 <= Gas <= 360 LED as output
const int LED_D8 = 8;         // Gas > 360 LED as output
const int LED_D9 = 9;         // LED for temperature < 26
const int LED_D10 = 10;       // LED for 26 <= temperature <= 38
const int LED_D11 = 11;       // LED for temperature > 38
const int LED_D13 = 13;       // LED for humidity > 65

// Initialize DHT sensor
#define DHT_TYPE DHT11        // Define sensor type (DHT11 or DHT22)
DHT dht(DHTPin, DHT_TYPE);    // Create DHT sensor object

char command; // Variable for storing serial input

void setup() {
  Serial.begin(9600);          // Initialize serial communication
  pinMode(trigPin, OUTPUT);    // Set trigPin as an output (Ultrasonic)
  pinMode(echoPin, INPUT);     // Set echoPin as an input (Ultrasonic)
  pinMode(Buzzer_control, OUTPUT); // Buzzer as output
  pinMode(LED_D12, OUTPUT);    // Distance LED as output
  pinMode(LED_D6, OUTPUT);     // Gas < 200 LED as output
  pinMode(LED_D7, OUTPUT);     // 200 <= Gas <= 360 LED as output
  pinMode(LED_D8, OUTPUT);     // Gas > 360 LED as output
  pinMode(LED_D9, OUTPUT);     // Temperature < 26 LED as output
  pinMode(LED_D10, OUTPUT);    // 26 <= temperature <= 38 LED as output
  pinMode(LED_D11, OUTPUT);    // Temperature > 38 LED as output
  pinMode(LED_D13, OUTPUT);    // Humidity > 65 LED as output
  dht.begin();                 // Initialize DHT sensor

  Serial.println("System Ready! Send '1' to turn ON buzzer, '2' to turn OFF.");
}

void loop() {
  // Check if data is available in the serial buffer
  if (Serial.available() > 0) {
    command = Serial.read(); // Read the incoming byte

    // Handle buzzer commands
    if (command == '1') {
      digitalWrite(Buzzer_control, HIGH); // Turn buzzer ON
      Serial.println("Buzzer ON");
    } else if (command == '2') {
      digitalWrite(Buzzer_control, LOW); // Turn buzzer OFF
      Serial.println("Buzzer OFF");
    }
  }

  // Read MQ2 Gas Sensor Value (Raw Analog Reading)
  int mq2Value = analogRead(MQ2Pin); // Read analog value from MQ2

  // Read Ultrasonic Sensor Value
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);          // Measure the echo time
  distance = duration * 0.034 / 2;            // Calculate distance in cm

  // Read DHT sensor values (Temperature and Humidity)
  float temperature = dht.readTemperature(); // Read temperature in Celsius
  float humidity = dht.readHumidity();       // Read humidity percentage

  // Check if DHT readings failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.print("Failed to read from DHT sensor!");
    return; // Skip the rest of this loop iteration
  }

  // LED Control for Distance
  if (distance > 50) {
    digitalWrite(LED_D12, HIGH); // Turn ON distance LED
  } else {
    digitalWrite(LED_D12, LOW);  // Turn OFF distance LED
  }

  // LED Control for Gas Levels and Buzzer
  if (mq2Value < 200) {
    digitalWrite(LED_D6, HIGH);  // Turn ON LED for gas < 200
    digitalWrite(LED_D7, LOW);
    digitalWrite(LED_D8, LOW);
    digitalWrite(Buzzer_control, LOW); // Turn OFF buzzer
  } else if (mq2Value >= 200 && mq2Value <= 360) {
    digitalWrite(LED_D6, LOW);
    digitalWrite(LED_D7, HIGH); // Turn ON LED for 200 <= gas <= 360
    digitalWrite(LED_D8, LOW);
    digitalWrite(Buzzer_control, LOW); // Turn OFF buzzer
  } else if (mq2Value > 360) {
    digitalWrite(LED_D6, LOW);
    digitalWrite(LED_D7, LOW);
    digitalWrite(LED_D8, HIGH); // Turn ON LED for gas > 360
    digitalWrite(Buzzer_control, HIGH); // Turn ON buzzer automatically
  }

  // LED Control for Temperature
  if (temperature < 26) {
    digitalWrite(LED_D9, HIGH);  // Turn ON LED for temperature < 26
    digitalWrite(LED_D10, LOW);
    digitalWrite(LED_D11, LOW);
  } else if (temperature >= 26 && temperature <= 32) {
    digitalWrite(LED_D9, LOW);
    digitalWrite(LED_D10, HIGH); // Turn ON LED for 26 <= temperature <= 32
    digitalWrite(LED_D11, LOW);
  } else if (temperature > 32) {
    digitalWrite(LED_D9, LOW);
    digitalWrite(LED_D10, LOW);
    digitalWrite(LED_D11, HIGH); // Turn ON LED for temperature > 32
  }

  // LED Control for Humidity > 60
  if (humidity > 60) {
    digitalWrite(LED_D13, HIGH); // Turn ON LED for humidity > 65
  } else {
    digitalWrite(LED_D13, LOW);  // Turn OFF LED for humidity <= 65
  }

  // Send sensor data continuously in a horizontal direction
  Serial.print(" GAS: ");       // Gas sensor raw analog value
  Serial.print(mq2Value);
  Serial.print(" | ");         // Separator

  Serial.print("DIST: ");      // Ultrasonic distance
  Serial.print(distance);      // No units

  Serial.print(" | ");         // Separator

  Serial.print("TEMP: ");      // Temperature
  Serial.print(temperature, 2); // 2 decimal places

  Serial.print(" | ");         // Separator

  Serial.print("HUM: ");       // Humidity
  Serial.print(humidity, 2);   // 2 decimal places

  delay(100);                  // Wait for 30 ms before repeating
}
