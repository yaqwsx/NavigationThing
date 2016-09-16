#include <Wire.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <toneAC.h>
#include "misc.h"

class Coord {
public:
  Coord(double lat = 0, double lon = 0)
    : _lat(lat), _lon(lon)
  {}

  Coord(TinyGPSLocation l)
    : _lat(l.lat()), _lon(l.lng())
  {}

  double dist(const Coord& other) {
    return TinyGPSPlus::distanceBetween(_lat, _lon, other._lat, other._lon);
  }

  void dump() {
    Serial.print(_lat, 10);
    Serial.print(" ");
    Serial.println(_lon, 10);
  }

  double azimuth(const Coord& other) {
     return TinyGPSPlus::courseTo(_lat, _lon, other._lat, other._lon);
  }

private:
  double _lat, _lon;
};

template <class S>
class Gps {
public:
  template <class... Args>
  Gps(Args...args) :_ser(args...) {}

  void begin() {
    delay(1000);
    _ser.begin(9600);
    // Output TMCGGA
    _ser.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
    // Update 1 Hz
    _ser.println("$PMTK220,1000*1F");
    // Antenna update
    _ser.println("$PGCMD,33,1*6C");
  }

  bool update() {
    while(_ser.available()) {
      char c = _ser.read();
      //Serial.print(c);
      if (_gps.encode(c)) {
        return true;
      }
    }
    return false;
  }

  bool fix() {
    return _gps.location.age() < 3000;
  }

  Coord pos() {
    return {_gps.location};
  }
private:
  S _ser;
  TinyGPSPlus _gps;
};

template <int SAMPLE_COUNT>
class CompassImp {
  public:
    CompassImp(int compensation, CRGB* leds, int count)
      : _comp(compensation), _leds(leds), _count(count), _color(CRGB::Red), _idx(0)
    {
      EEPROM.get(0, _x_comp);
      EEPROM.get(4, _y_comp);

      for (int i = 0; i != SAMPLE_COUNT; i++)
        _headings[i] = 0;
    }

    void calibrate(CRGB color = {0, 0, 128}) {
      for(int i = 4; i != 0; i--)
        toneAC(i * 1000, 10, 200);
      begin();
      delay(3500);
      unsigned long long t = millis();
      int x_max = 0, x_min = 0;
      int y_max = 0, y_min = 0;
      while(millis() - t < 10000) {
        Wire.beginTransmission(0x1E);
        Wire.write(0x03);
        Wire.endTransmission();
  
        int x = 0, y = 0, z = 0;
        Wire.requestFrom(0x1E, 6);
        if(6<=Wire.available()){
          x = Wire.read()<<8; //X msb
          x |= Wire.read(); //X lsb
          z = Wire.read()<<8; //Z msb
          z |= Wire.read(); //Z lsb
          y = Wire.read()<<8; //Y msb
          y |= Wire.read(); //Y lsb
        }

        if (x_min == 0 || x_min > x)
          x_min = x;
        if (x_max == 0 || x_max < x)
          x_max = x;

        if (y_min == 0 || y_min > y)
          y_min = y;
        if (y_max == 0 || y_max < y)
          y_max = y;
        
        for (int i = 0; i != _count; i++)
          _leds[i] = ((millis() - t) % 400) < 200 ? CRGB::Black : color;
        FastLED.show();
      }

      Serial.print("X: "); Serial.print(x_min);
      Serial.print(", "); Serial.println(x_max);
      Serial.print("Y: "); Serial.print(y_min);
      Serial.print(", "); Serial.println(y_max);

      _x_comp = -(x_min + x_max) / 2;
      _y_comp = -(y_min + y_max) / 2;
      toneAC(4000, 10, 100);

      EEPROM.put(0, _x_comp);
      EEPROM.put(4, _y_comp);

      Serial.print("Comp: "); Serial.print(_x_comp);
      Serial.print(", "); Serial.println(_y_comp);
      
      delay(1000);
      toneAC(4000, 10, 500);

      for (int i = 0; i != _count; i++)
        _leds[i] = CRGB::Black;
      FastLED.show();
    }

    void begin() {
      Wire.begin();
      Wire.beginTransmission(0x1E);
      Wire.write(0x02);
      Wire.write(0x00);
      Wire.endTransmission();
    }

    void set_color(CRGB color) {
      _color = color;
    }

    float get_heading() {
      Wire.beginTransmission(0x1E);
      Wire.write(0x03);
      Wire.endTransmission();

      int x = 0, y = 0, z = 0;
      Wire.requestFrom(0x1E, 6);
      if(6<=Wire.available()){
        x = Wire.read()<<8; //X msb
        x |= Wire.read(); //X lsb
        z = Wire.read()<<8; //Z msb
        z |= Wire.read(); //Z lsb
        y = Wire.read()<<8; //Y msb
        y |= Wire.read(); //Y lsb
      }

      x += _x_comp;
      y += _y_comp;

      _len = sqrt((long long)x*x + (long long)y*y);
      // Serial.print("Length: "); Serial.println(_len);

      float head = atan2((float)y, (float)x) * 180 / M_PI + _comp;
      while (head < 0)
        head += 360;
      while (head > 360)
        head -= 360;

      _idx++;
      if (_idx == SAMPLE_COUNT)
        _idx = 0;
      _headings[_idx % SAMPLE_COUNT] = head;

      float sum = 0;
      for (int i = 0; i != SAMPLE_COUNT; i++)
        sum += _headings[i];

      return head;
      return sum / SAMPLE_COUNT;
    }

    void show(int angle) {
      float heading = 360 * 3 - get_heading() - angle;
      float led = (heading * _count + heading / (2 * _count)) / 360;
      int led_pos = led;
      for (int i = 0; i != _count; i++)
        _leds[i] = CRGB::Black;

      if (_len > 140 && _len < 420 || millis() % 400 < 200)
        _leds[led_pos % _count] = _color;

      FastLED.show();
    }

  private:
    int _comp, _x_comp, _y_comp;
    int _len;
    CRGB* _leds;
    int _count;
    CRGB _color;
    float _headings[SAMPLE_COUNT];
    int _idx;
};

using Compass = CompassImp<10>;

template <class GPS>
class Navigation {
public:
  Navigation(GPS& gps, Compass& comp, CRGB* leds, int count)
    : _gps(gps), _comp(comp), _leds(leds), _count(count), _led_state(false), _last_toggle_time(millis()) {}

  void set_target(Coord c) {
    _target = c;
  }

  Coord wait_for_fix() {
    while(true) {
      if (_gps.update()) {
        Serial.print("Fix: "); Serial.println((int)_gps.fix());
        _gps.pos().dump();
  
        if (_gps.fix())
          return _gps.pos();
        else
          toggle_warning();
      }
    }
  }

  void update() {
    if (_gps.update()) {
      Serial.print("Fix: "); Serial.println((int)_gps.fix());
      _gps.pos().dump();

      if (_gps.fix()) {
        Serial.print("Distance: "); Serial.println(_gps.pos().dist(_target));
        Serial.print("Azimuth:  "); Serial.println(_gps.pos().azimuth(_target));
      }

    }

    if (_gps.fix()) {
      hadle_beep(_gps.pos().dist(_target));
    }

    if (_gps.fix() && millis() - _last_comp_update > 200) {
      _last_comp_update = millis();
      _comp.show(_gps.pos().azimuth(_target));
    }
    
    if (!_gps.fix()) {
      toggle_warning();
    }
  }

  bool are_we_there() {
    if (!_gps.fix())
      return false;
    return _gps.pos().dist(_target) < 30;
  }

  void toggle_warning() {
    if (millis() - _last_toggle_time < 400)
      return;
    _last_toggle_time = millis();
    CRGB c = _led_state ? CRGB(16, 0, 0) : CRGB(0, 0, 0);
    _led_state = !_led_state;
    for (int i = 0; i != _count; i++)
      _leds[i] = c;

    FastLED.show();
  }

  void hadle_beep(int d) {
    if (d > 1000)
      _next_beep = 2500;
    else {
      d -= 10;
      if (d < 10)
        d = 10;
      _next_beep = 33 * sqrt(d) + 20;
    }

    if (millis() - _last_beep > _next_beep) {
      _last_beep = millis();
      toneAC(3000, 10, 20);
    }
  }
  
private:
  GPS& _gps;
  Compass& _comp;
  unsigned long long _last_comp_update;
  CRGB* _leds;
  int _count;

  bool _led_state;
  unsigned long long _last_toggle_time;
  Coord _target;

  unsigned long long _last_beep;
  int _next_beep;
};

struct Waypoint{
  Coord coords;
  CRGB color;
  void (*enter_func)();
};

void basic_enter() {
  for (int j = 0; j != 2; j++) {
    for (int i = 0; i != 7; i++) {
      toneAC(600 * i, 10, 150);
    }
  }
}

const int LED_COUNT = 12;
CRGB leds[LED_COUNT];

void fill_with(CRGB col) {
  for (int i = 0; i != LED_COUNT; i++)
        leds[i] = col;
  FastLED.show();
}

Coord secretTrigger = { 49.1043933, 16.1922400 };
Waypoint secretWaypoint = { {49.1029500, 16.1918000}, {16, 16, 16}, nullptr};

Waypoint waypoints[] = {
  { {49.1036661, 16.1809603}, {16, 0, 0}, basic_enter },
  { {49.1037667, 16.1828667}, {0, 16, 0}, basic_enter },
  { {49.1027272, 16.1816669}, {0, 0, 16}, basic_enter },
  { {49.1036239, 16.1833636}, {0, 16, 0}, basic_enter },
  { {49.1059864, 16.1843600}, {16, 0, 0}, basic_enter }
};
const int wp_count = sizeof(waypoints) / sizeof(Waypoint);

using GpsType = Gps<SoftwareSerial>;
GpsType gps(A3,A2);
Compass compass(200, leds, LED_COUNT);
Navigation<GpsType> nav(gps, compass, leds, LED_COUNT);

int wp_num = 0;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, 3, GRB>(leds, LED_COUNT);
  fill_with(CRGB::Black);
  
  pinMode(2, INPUT_PULLUP);
  if (digitalRead(2) == LOW) {
    // Reset game position
    wp_num = 0;
    EEPROM.put(8, wp_num);
    int calib = 1;
    EEPROM.put(12, calib);
    // Calibrate compass
    //compass.calibrate();
    for(int i = 0; i != 20; i++) {
      toneAC(i * 400, 10, 20);
    }
    while(true);
  }

  int calib;
  EEPROM.get(12, calib);
  if (calib == 1) {
    compass.calibrate(CRGB(16, 0, 16));
    calib = 0;
    EEPROM.put(12, calib);
  }
  
  toneAC(1000, 10, 100);
  compass.begin();
  gps.begin();

  // Load last game position
  EEPROM.get(8, wp_num);

  delay(500);

  toneAC(1000, 10, 100);
  toneAC(2000, 10, 100);

  delay(200);
  if (waypoints[0].enter_func)
    (*waypoints[0].enter_func)();
}

void final_trap() {
  for (int i = 0; i != 7; i++) {
    int b = 255 * i / 7;
    fill_with(CRGB(b, b, b));
    toneAC(600 * i, 10, 150);
  }

  for (int i = 0; i != 7; i++) {
    int b = 255 * i / 7;
    fill_with(CRGB(b, 0, 0));
    toneAC(7 * 600 - 600 * i, 10, 150);
  }

  for (int i = 0; i != 7; i++) {
    int b = 255 * i / 7;
    fill_with(CRGB(b, 0, 0));
    toneAC(7 * 600 - 600 * i, 10, 150);
  }

  for (int i = 0; i != 7; i++) {
    int b = 255 * i / 7;
    fill_with(CRGB(b, 0, 0));
    toneAC(7 * 600 - 600 * i, 10, 150);
  }
  fill_with(CRGB::Black);
  while(true);
}

bool first = true;
bool special = false;

void loop() {
  if (gps.fix() && first) {
    if (gps.pos().dist(secretTrigger) < 70) {
      special = true;
      // Navigate to secret waypoint
      for (int i = 0; i != 3; i ++) {
        toneAC(500, 10, 200);
        delay(200);
      }
    }
    first = false;
  }
  
  static unsigned long long t = millis();
  Waypoint& wp = waypoints[wp_num];
  nav.set_target(wp.coords);
  compass.set_color(wp.color);
  
  if (special) {
    nav.set_target(secretWaypoint.coords);
    compass.set_color(secretWaypoint.color);
  }
  
  nav.update();
  if (millis() - t > 1000) {
    t = millis();
    Serial.print("Navigation target: "); Serial.print(wp_num);
    Serial.print(": "); wp.coords.dump();
  }
  
  if (nav.are_we_there()) {
    if (special) {
      for(int i = 0; i != 5; i++) {
        toneAC(i*800, 100, 200);
      }
      fill_with(CRGB::Black);
      FastLED.show();
      while(true);
    }
    
    wp_num++;
    if (wp_num == wp_count) {
      final_trap();
    }
    else {
      EEPROM.put(8, wp_num);
      wp = waypoints[wp_num];
      if (wp.enter_func) {
        (*wp.enter_func)();
      }
    }
  }
}
