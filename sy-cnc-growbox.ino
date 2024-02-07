//#include <dht.h>
#include "U8glib.h"
#include "actuator.h"
#include "sensor.h"

//#include "Adafruit_Sensor.h"
//#include "Adafruit_AM2320.h"
#include "gcode.h"

#define ADD_TEST_G_CODE 1
#define NumberOfCommands 25
#define use_display 0

#define SUNRISE 0
#define DAY 1
#define SUNSET 2
#define NIGHT 3

#define COUNT_SENSORS 2
#define COUNT_ACTUATORS 3

void g_ok() {

}
void g_toggle_automation();
void g_toggle_light_automation();
void g_light_set_duration();
void g_light_get_duration();
void g_part_of_day_data();
void g_set_light_value_part_of_day();
void g_get_light_value_part_of_day();
void g_actuator_write();
void g_get_auto_is_on();
void g_get_light_auto_is_on();
void g_set_duration();
void g_get_duration();
void g_part_of_period_data();
void g_set_value_part_of_period();
void g_get_value_part_of_period();
void g_get_value_from_sensor();
void g_toggle_cc_automation();
void g_get_cc_auto_is_on();
void g_cc_link_sensor();
void g_cc_get_link_sensor();
void g_cc_set_min_value();
void g_cc_get_min_value();
void g_cc_set_max_value();
void g_cc_get_max_value();

commandscallback commands[NumberOfCommands] = {
  {"E0", g_actuator_write},
  {"E2", g_get_value_from_sensor},
  {"E10",g_ok},

  {"E100", g_toggle_automation},
  {"E1001", g_get_auto_is_on},
  {"E101", g_set_duration},
  {"E1011", g_get_duration},
  {"E102", g_part_of_period_data},
  {"E103", g_set_value_part_of_period},
  {"E1031", g_get_value_part_of_period},

  {"E150", g_toggle_light_automation},
  {"E1501", g_get_light_auto_is_on},
  {"E151", g_light_set_duration},
  {"E1511", g_light_get_duration},
  {"E152", g_part_of_day_data},
  {"E153", g_set_light_value_part_of_day},
  {"E1531", g_get_light_value_part_of_day},

  {"E200", g_toggle_cc_automation},
  {"E2001", g_get_cc_auto_is_on},
  {"E201", g_cc_link_sensor},
  {"E2011", g_cc_get_link_sensor},
  {"E202", g_cc_set_min_value},
  {"E2021", g_cc_get_min_value},
  {"E203", g_cc_set_max_value},
  {"E2031", g_cc_get_max_value},
};
gcode Commands(NumberOfCommands,commands);


// setup u8g object, please remove comment from one of the following constructor calls
// IMPORTANT NOTE: The following list is incomplete. The complete list of supported 
// devices with all constructor calls is here: https://github.com/olikrau90s/u8glib/wiki/device

#if use_display == 1
U8GLIB_PCD8544 u8g(13, 11, 10, 9, 8);		// SPI Com: SCK (CLK) = 13, MOSI (Din) = 11, CS (CE) = 10, A0 (DC, RS) = 9, Reset = 8
#endif

/*
Исполнительные устройства:
- освещение (вкл/выкл, яркость в люменах, спектры по времени суток (рассвет, день, закат), спектры по развитию растения (рост, цетение, плодоношение)) - для изменения освещения
  - люксометр (он же датчик освещённости)
  - спектрометр
- вентиляция (вкл/выкл, мощность) - для изменения состава воздуха (вывод кислорода, приток углекислого газа), контроль температуры (если температура окружающей среды отличается)
  - датчик кислорода
  датчик углекислого газа
- нагреватель и охладитель (вкл/выкл, мощность) - для изменения температуры воздуха
- увлажнитель и осушитель - для изменения влажности воздуха
- насос - для полива растения
  датчик влажности грунт

Датчики:
- фотокамера - для контроля заболеваний растений, стадии развития растения,
- датчик температуры грунта
- бесконтактный датчик температуры листвы

*/

int prev_minutes = 0;

struct DaySettings {
  bool auto_is_on = false;
  byte part_of_day = 0;
  int counter_minutes = 0;
  byte day_sun_value = 255;
  byte night_sun_value = 0;
  int part_of_day_durations[4] = {
    0,   // sunrise (max = 255)
    840, // day
    0,   // sunset (max = 255)
    600, // night
  };
};

struct CycleSettings {
  bool auto_is_on = false;
  bool is_on = true;
  int counter_minutes = 0;
  byte value_on = 255;
  byte value_off = 0;
  int duration_on = 1;
  int duration_off = 719;
};

struct ClimateControlSettings {
  bool auto_is_on = false;
  byte sensor_id = 0;
  double min_value = 0;
  double max_value = 0;
};

DaySettings day_settings[COUNT_ACTUATORS];
CycleSettings cycle_settings[COUNT_ACTUATORS];
ClimateControlSettings cc_settings[COUNT_ACTUATORS];

// DHT sensor_dht = DHT();

char msg[128];
//Sensor sensor_lm335 = Sensor(PROTOCOL_ANALOGIO, A1);
//Sensor sensor_dht2 = Sensor(PROTOCOL_DHT, A0);
Sensor sensors[COUNT_SENSORS] = {
  Sensor(SENSOR_PROTOCOL_I2C_AM2320, 0, 0),
  Sensor(SENSOR_PROTOCOL_I2C_AM2320, 0, 1),
};
Actuator actuators[COUNT_ACTUATORS] = {
  Actuator(ACTUATOR_PROTOCOL_DIGITALIO, LED_BUILTIN), // humidifier
  Actuator(ACTUATOR_PROTOCOL_ANALOGIO, 5), // extractor
  Actuator(ACTUATOR_PROTOCOL_ANALOGIO, 3), // light
};

// void draw(void) {
//   // graphic commands to redraw the complete screen should be placed here  
//   u8g.setFont(u8g_font_unifont);
//   //u8g.setFont(u8g_font_osb21);
//   u8g.drawStr( 0, 22, "Hello World!");
// }


//void draw2(int x, int y, String text) {
//  u8g.firstPage();
//  do {
//    u8g.drawStr(x, y, text);
//  } while( u8g.nextPage() );
//}


void setup(void) {
  for (byte i=0; i < COUNT_SENSORS; i++) {
    sensors[i].begin();
  }

  Commands.begin(FinishedCommandLine);

  cc_settings[0].auto_is_on = 1;
  cycle_settings[1].auto_is_on = 1;
  day_settings[2].auto_is_on = 1;

  cycle_settings[1].value_on = 1;

  cc_settings[0].sensor_id = 1;
  cc_settings[0].min_value = 65;
  cc_settings[0].max_value = 70;

  for (byte i=0; i < COUNT_ACTUATORS; i++) {
    if (day_settings[i].auto_is_on) {
      actuators[i].write(day_settings[i].day_sun_value);
    }
    if (cycle_settings[i].auto_is_on) {
      actuators[i].write(cycle_settings[i].is_on ? cycle_settings[i].value_on : cycle_settings[i].value_off);
    }
  }
  //sensor_dht.attach(DHT11_Analog_IN);

  // flip screen, if required
  // u8g.setRot180();
  
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);
  //Serial.begin(9600);

  #if use_display == 1
  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  //draw();
  u8g.setFont(u8g_font_5x8);
  #endif
  delay(1000);
}

void calc_counter_minutes(void) {
  int current_minutes = millis() / 60000;
  if (current_minutes != prev_minutes) {
    for (byte i=0; i < COUNT_ACTUATORS; i++) {
      if (day_settings[i].auto_is_on) {
        day_settings[i].counter_minutes += 1;
      }
      if (cycle_settings[i].auto_is_on) {
        cycle_settings[i].counter_minutes += 1;
      }
    }
  }
  prev_minutes = current_minutes;
}

void check_actuator_sun(struct DaySettings &settings, Actuator &actuator) {
  int duration = settings.part_of_day_durations[settings.part_of_day];
  if (settings.counter_minutes >= duration) {
    settings.counter_minutes = 0;
    settings.part_of_day = (settings.part_of_day >= (sizeof(settings.part_of_day_durations) / sizeof(int) - 1)) ? 0 : settings.part_of_day + 1;
  }

  if (duration != 0) {
    byte sun_value = 0;
    if (settings.part_of_day == SUNRISE) {
      sun_value = settings.counter_minutes * (settings.day_sun_value / duration);
    } else if (settings.part_of_day == DAY) {
      sun_value = settings.day_sun_value;
    } else if (settings.part_of_day == SUNSET) { 
      sun_value = settings.day_sun_value - settings.counter_minutes * (settings.day_sun_value / duration);
    } else if (settings.part_of_day == NIGHT) {
      sun_value = settings.night_sun_value;
    }
    actuator.write(sun_value);
  }
}

void check_actuator(struct CycleSettings &settings, Actuator &actuator) {
  int duration = settings.is_on ? settings.duration_on : settings.duration_off;
  if (settings.counter_minutes >= duration) {
    settings.counter_minutes = 0;
    settings.is_on = !(settings.is_on);
  }
  actuator.write(settings.is_on ? settings.value_on : settings.value_off);
}

void check_actuator_climate_control(struct ClimateControlSettings &settings, Actuator &actuator) {
  double current_value = sensors[settings.sensor_id].value();
  if (current_value <= settings.min_value) {
    actuator.write(255);
  } else if (current_value >= settings.max_value) {
    actuator.write(0);
  }
}

/*double calc_celsius(int LM335_value) {
    double LM335_voltage = (LM335_value / 1023.0) * 5000;
    double LM335_celsius = LM335_voltage / 10.0 - 273.15;
    return LM335_celsius;
}*/

void loop(void) {
    Commands.available();
    calc_counter_minutes();

    for (byte i=0; i < COUNT_ACTUATORS; i++) {
      if (day_settings[i].auto_is_on) {
        check_actuator_sun(day_settings[i], actuators[i]);
      } else if (cycle_settings[i].auto_is_on) {
        check_actuator(cycle_settings[i], actuators[i]);
      } else if (cc_settings[i].auto_is_on) {
        check_actuator_climate_control(cc_settings[i], actuators[i]);
      }
    }

    #if use_display == 1
    sensor_dht2.update(msg);
    u8g.firstPage();
    do {
        if (sensor_dht2.has_error == 0) {
          sprintf(msg, "%dC %d%%", (int)sensor_dht2.temperature(), (int)sensor_dht2.humidity());
        }
        u8g.drawStr( 0, 11, msg);
        /*switch (sensor_dht.getLastError()) {
            case DHT_ERROR_OK:
                //sprintf(msg, "%dC %d%%", sensor_dht.getTemperatureInt(), sensor_dht.getHumidityInt());
                sprintf(msg, "%dC %d%%", (int)sensor_dht2.temperature(), (int)sensor_dht2.humidity());
                u8g.drawStr( 0, 11, msg);
                break;
            case DHT_ERROR_START_FAILED_1:
                u8g.drawStr( 0, 11, "Err: start failed (stage 1)");
                break;
            case DHT_ERROR_START_FAILED_2:
                u8g.drawStr( 0, 11, "Err: start failed (stage 2)");
                break;
            case DHT_ERROR_READ_TIMEOUT:
                u8g.drawStr( 0, 11, "Err: read timeout");
                break;
            case DHT_ERROR_CHECKSUM_FAILURE:
                u8g.drawStr( 0, 11, "Err: checksum error");
                break;
        }*/

        sprintf(msg, "%dC", (int)sensor_lm335.temperature());
        u8g.drawStr( 0, 22, msg);

        sprintf(msg, part_of_day ? "Day: %d minutes" : "Night: %d minutes", counter_minutes);
        u8g.drawStr( 0, 33, msg);

    } while (u8g.nextPage());
    #endif

    
    // picture loop
    //u8g.firstPage();  
    //do {
    //  u8g.drawStr( 0, 22, "Hello World!");
    //} while( u8g.nextPage() );
    //u8g.nextPage();
    // rebuild the picture after some delay
    delay(1000);
}

