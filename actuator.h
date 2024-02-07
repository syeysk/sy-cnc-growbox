#ifndef ARDUINO
#error "Arduino version not defined"
#endif

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define ACTUATOR_PROTOCOL_ANALOGIO 0
#define ACTUATOR_PROTOCOL_DIGITALIO 1
#define ACTUATOR_PROTOCOL_DAC 2

class Actuator {
  private:
  int _protocol;
  int _address;

  public:
  bool has_error;
  Actuator(int protocol, int address);
  void write(byte value);
};
