#include <ESP8266WiFi.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define MQ2_PIN A0     // Pin analog untuk sensor MQ2
#define PIR_PIN_1 D5   // Pin digital untuk sensor PIR 1
#define PIR_PIN_2 D6   // Pin digital untuk sensor PIR 2
#define SERVO_PIN D7   // Pin digital untuk servo
  // Pin TX untuk komunikasi serial dengan ESP32-CAM
SoftwareSerial mySerial(2, 3);
Servo myServo;
 // RX, TX

// Konfigurasi koneksi WiFi
const char* ssid = "afikaa";
const char* password = "";

void setup() {
  Serial.begin(9600);          // Inisialisasi komunikasi serial untuk debugging
 mySerial.begin(9600);
  // Inisialisasi komunikasi serial dengan ESP32-CAM

  pinMode(PIR_PIN_1, INPUT);   // Setel pin PIR 1 sebagai input
  pinMode(PIR_PIN_2, INPUT);   // Setel pin PIR 2 sebagai input
  myServo.attach(SERVO_PIN);   // Attach servo motor

  // Mulai koneksi WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  // Membaca nilai dari sensor MQ2
  int mq2_value = analogRead(MQ2_PIN);  
  Serial.print("MQ2 Sensor Value: ");
  Serial.println(mq2_value);  

  // Membaca nilai dari sensor PIR 1
  int pir_value_1 = digitalRead(PIR_PIN_1);
  Serial.print("PIR 1 Sensor Value: ");
  Serial.println(pir_value_1 ? "HIGH (Motion Detected)" : "LOW (No Motion)");

  // Membaca nilai dari sensor PIR 2
  int pir_value_2 = digitalRead(PIR_PIN_2);
  Serial.print("PIR 2 Sensor Value: ");
  Serial.println(pir_value_2 ? "HIGH (Motion Detected)" : "LOW (No Motion)");

  // Bergerakkan servo sesuai dengan deteksi PIR
  if (pir_value_1 && !pir_value_2) {
    // Deteksi PIR 1 saja
    Serial.println("Motion detected by PIR 1. Servo moves left.");
    myServo.write(0); // Bergerak ke kiri
  } else if (pir_value_2 && !pir_value_1) {
    // Deteksi PIR 2 saja
    Serial.println("Motion detected by PIR 2. Servo moves right.");
    myServo.write(180); // Bergerak ke kanan
  } else {
    // Tidak ada deteksi atau deteksi bersamaan
    Serial.println("No motion detected or both PIRs detected motion simultaneously.");
    myServo.write(90); // Mengembalikan servo ke posisi tengah
  }

  // Kirim data ke ESP32-CAM
  sendDataToESP32CAM(mq2_value, pir_value_1, pir_value_2);

  delay(5000);  // Tunggu 5 detik sebelum membaca kembali
}

void sendDataToESP32CAM(int mq2, int pir1, int pir2) {
  String data = String(mq2) + "," + String(pir1) + "," + String(pir2);
  Serial.print("Sending data to ESP32-CAM: ");
  Serial.println(data);
  mySerial.println(data); // Send the data over the software serial
}
