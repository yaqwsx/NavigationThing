#include <FastLED.h>
#include <Wire.h>
#include <toneAC.h>
#include <SoftwareSerial.h>

const int LED_COUNT = 12;
CRGB leds[LED_COUNT];


void setup() {
  FastLED.addLeds<WS2812B, 3, GRB>(leds, LED_COUNT);
  toneAC(1000, 10, 100);
  toneAC(2000, 10, 100);
  Serial.begin(115200);
  Serial.println("Initialized");

  test_led();
  test_compass();
  test_gps();

  for (int j = 0; j != 2; j++) {
    for (int i = 0; i != 7; i++) {
      toneAC(600 * i, 10, 150);
    }
  }
}

void test_led() {
  for (int j = 0; j != 2 * LED_COUNT; j++) {
    Serial.print("Testing LED no: ");
    Serial.println(j % LED_COUNT);
    for (int i = 0; i != LED_COUNT; i++)
      leds[i] = CRGB::Black;
    leds[j % LED_COUNT] = CRGB(32, 0, 0);
    FastLED.show();
    toneAC(1000, 10, 100);
    delay(300);
  }
}

void test_compass() {
  Wire.begin();
  Wire.beginTransmission(0x1E);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();
      
  for (int i = 0; i != 10; i++) { 
    Wire.beginTransmission(0x1E);
    Wire.write(0x03);
    Wire.endTransmission();

    int x, y, z;
    Wire.requestFrom(0x1E, 6);
    if(6<=Wire.available()){
      x = Wire.read()<<8; //X msb
      x |= Wire.read(); //X lsb
      z = Wire.read()<<8; //Z msb
      z |= Wire.read(); //Z lsb
      y = Wire.read()<<8; //Y msb
      y |= Wire.read(); //Y lsb
    }
    Serial.print("Comapass: ");
    Serial.print(x); Serial.print(", ");
    Serial.print(y); Serial.print(", ");
    Serial.println(z);
    for (int j = 0; j != LED_COUNT; j++)
      leds[j] = CRGB(0, (i & 1) ? 16 : 0, 0);
    FastLED.show();
    delay(200);
  }
}

void test_gps() {
  SoftwareSerial s(A3, A2);
  s.begin(9600);
  // Output TMCGGA
  s.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
  // Update 1 Hz
  s.println("$PMTK220,1000*1F");
  // Antenna update
  s.println("$PGCMD,33,1*6C");

  auto t = millis();
  while(millis() - t < 5000) {
    while(s.available()) {
      char c = s.read();
      Serial.print(c);
    }
  }
}

void loop() {

}
