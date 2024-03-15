#include "actuator.h"


Actuator::Actuator(int protocol, int address) {
  _protocol = protocol;
  _address = address;
  has_error = 0;
  if (_protocol == ACTUATOR_PROTOCOL_ANALOGIO || _protocol == ACTUATOR_PROTOCOL_DIGITALIO) {
    pinMode(_address, OUTPUT);
  }
}

void Actuator::write(byte value) {
  if (_protocol == ACTUATOR_PROTOCOL_ANALOGIO) {
    analogWrite(_address, value);
  } else if (_protocol == ACTUATOR_PROTOCOL_DIGITALIO) {
    digitalWrite(_address, value > 0 ? HIGH : LOW);
  } else if (_protocol == ACTUATOR_PROTOCOL_DAC) {
  }
}
