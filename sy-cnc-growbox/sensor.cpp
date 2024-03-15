#include "sensor.h"

Sensor::Sensor(int protocol, int address=0, int subsensor=0) {
  _protocol = protocol;
  _address = address;
  _subsensor = subsensor;
  has_error = 0;
  if (_protocol == SENSOR_PROTOCOL_DHT) {
    _dht = DHT();
  } else if (_protocol == SENSOR_PROTOCOL_I2C_AM2320) {
    _am2320 = Adafruit_AM2320();
  }
}

void Sensor::begin() {
  if (_protocol == SENSOR_PROTOCOL_DHT) {
    _dht.attach(_address);
  } else if (_protocol == SENSOR_PROTOCOL_I2C_AM2320) {
    _am2320.begin();
  }
}

void Sensor::update(char *error_message) {
  if (_protocol == SENSOR_PROTOCOL_DHT) {
    _dht.update();
    switch (_dht.getLastError()) {
      case DHT_ERROR_OK:
          has_error = 0;
          break;
      case DHT_ERROR_START_FAILED_1:
          has_error = 1;
          sprintf(error_message, "Err: start failed (stage 1)");
          break;
      case DHT_ERROR_START_FAILED_2:
          has_error = 1;
          sprintf(error_message, "Err: start failed (stage 2)");
          break;
      case DHT_ERROR_READ_TIMEOUT:
          has_error = 1;
          sprintf(error_message, "Err: read timeout");
          break;
      case DHT_ERROR_CHECKSUM_FAILURE:
          has_error = 1;
          sprintf(error_message, "Err: checksum error");
          break;
    }
  }
}

double Sensor::value() {
  if (_protocol == SENSOR_PROTOCOL_ANALOGIO) {
    // https://robocraft.ru/arduino/84
    return analogRead(_address) / 1023.0 * 500 - 273.15;
  } else if (_protocol == SENSOR_PROTOCOL_DHT && _subsensor == 0) {
    return (double)_dht.getTemperatureInt();
  } else if (_protocol == SENSOR_PROTOCOL_DHT && _subsensor == 1) {
    return (double)_dht.getHumidityInt();
  } else if (_protocol == SENSOR_PROTOCOL_I2C_AM2320 && _subsensor == 0) {
    return (double)_am2320.readTemperature();
  } else if (_protocol == SENSOR_PROTOCOL_I2C_AM2320 && _subsensor == 1) {
    return (double)_am2320.readHumidity();
  }
}

/*double Sensor::temperature() {
  if (_protocol == SENSOR_PROTOCOL_ANALOGIO) {
    // https://robocraft.ru/arduino/84
    return analogRead(_address) / 1023.0 * 500 - 273.15;
  } else if (_protocol == SENSOR_PROTOCOL_DHT) {
    return (double)_dht.getTemperatureInt();
  } else if (_protocol == SENSOR_PROTOCOL_I2C_AM2320) {
    return (double)_am2320.readTemperature();
  }
}

double Sensor::humidity() {
  if (_protocol == SENSOR_PROTOCOL_DHT) {
    return (double)_dht.getHumidityInt();
  } else if (_protocol == SENSOR_PROTOCOL_I2C_AM2320) {
    return (double)_am2320.readHumidity();
  }
}*/