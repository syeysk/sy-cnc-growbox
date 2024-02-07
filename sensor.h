#include <dht.h>
//#include <DHT.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_AM2320.h>

#ifndef ARDUINO
#error "Arduino version not defined"
#endif

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define SENSOR_PROTOCOL_ANALOGIO 0
#define SENSOR_PROTOCOL_DHT 1
#define SENSOR_PROTOCOL_I2C_AM2320 2

class Sensor {
  private:
  int _protocol;
  int _address;
  int _subsensor;
  DHT _dht;
  Adafruit_AM2320 _am2320;

  public:
  bool has_error;
  Sensor(int protocol, int address, int subsensor);
  void begin();
  void update(char *error_message);
  double value();
  //double temperature();
  //double humidity();
};
