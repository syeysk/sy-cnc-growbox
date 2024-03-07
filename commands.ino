CycleSettings *get_cycle_settings(char parametr) {
  return &cycle_settings[(byte)Commands.GetValue(parametr)];
}

DaySettings *get_day_settings(char parametr) {
  return &day_settings[(byte)Commands.GetValue(parametr)];
}

ClimateControlSettings *get_cc_settings(char parametr) {
  return &cc_settings[(byte)Commands.GetValue(parametr)];
}

/* Cycle automatics g-code */

void g_toggle_automation() {
  if(Commands.availableValue('A') && Commands.availableValue('B'))
    get_cycle_settings('A')->auto_is_on = (byte)Commands.GetValue('B');
}

void g_get_auto_is_on() {
  if(Commands.availableValue('A'))
    Commands.comment('B', get_cycle_settings('A')->auto_is_on);
}

void g_set_duration() {
  if(Commands.availableValue('A') && Commands.availableValue('B') && Commands.availableValue('D')) {
    int duration = (int)Commands.GetValue('D');
    if (Commands.GetValue('B'))
      get_cycle_settings('A')->duration_on = duration;
    else
      get_cycle_settings('A')->duration_off = duration;
  }
}

void g_get_duration() {
  if(Commands.availableValue('A') && Commands.availableValue('B')) {
    Commands.comment('D', Commands.GetValue('B') ? get_cycle_settings('A')->duration_on : get_cycle_settings('A')->duration_off);
  }
}

void g_part_of_period_data() {
  if(Commands.availableValue('A')) {
    Commands.comment('B', get_cycle_settings('A')->is_on);
    Commands.comment('D', get_cycle_settings('A')->counter_minutes);
  }
}

void g_set_value_part_of_period() {
  if(Commands.availableValue('A') && Commands.availableValue('B') && Commands.availableValue('V')) {
    byte p = (byte)Commands.GetValue('B');
    if (p) {
      get_cycle_settings('A')->value_on = (byte)Commands.GetValue('V');
    } else {
      get_cycle_settings('A')->value_off = (byte)Commands.GetValue('V');
    }
  }
}

void g_get_value_part_of_period() {
  if(Commands.availableValue('A') && Commands.availableValue('B')) {
    Commands.comment('V', (byte)Commands.GetValue('B') ? get_cycle_settings('A')->value_on : get_cycle_settings('A')->value_off);
  }
}

/* Day automatics g-code */

void g_toggle_light_automation() {
  if(Commands.availableValue('A') && Commands.availableValue('B'))
    get_day_settings('A')->auto_is_on = (byte)Commands.GetValue('B');
}

void g_get_light_auto_is_on() {
  if(Commands.availableValue('A'))
    Commands.comment('B', get_day_settings('A')->auto_is_on);
}

void g_light_set_duration() {
  if(Commands.availableValue('A') && Commands.availableValue('P') && Commands.availableValue('D'))
    get_day_settings('A')->part_of_day_durations[(int)Commands.GetValue('P')] = (int)Commands.GetValue('D');
}

void g_light_get_duration() {
  if(Commands.availableValue('A') && Commands.availableValue('P'))
    Commands.comment('D', get_day_settings('A')->part_of_day_durations[(int)Commands.GetValue('P')]);
}

void g_part_of_day_data() {
  if(Commands.availableValue('A')) {
    Commands.comment('P', get_day_settings('A')->part_of_day);
    Commands.comment('D', get_day_settings('A')->counter_minutes);
  }
}

void g_set_light_value_part_of_day() {
  if(Commands.availableValue('A') && Commands.availableValue('P') && Commands.availableValue('V')) {
    byte p = (byte)Commands.GetValue('P');
    if (p == DAY) {
      get_day_settings('A')->day_sun_value = (byte)Commands.GetValue('V');
    } else if (p == NIGHT) {
      get_day_settings('A')->night_sun_value = (byte)Commands.GetValue('V');
    }
  }
}

void g_get_light_value_part_of_day() {
  if(Commands.availableValue('A') && Commands.availableValue('P')) {
    byte p = (byte)Commands.GetValue("P");
    if (p == DAY) {
      Commands.comment('V', get_day_settings('A')->day_sun_value);
    } else if (p == NIGHT) {
      Commands.comment('V', get_day_settings('A')->night_sun_value);
    }
  }
}

/* Climate-control automatics g-code */

void g_toggle_cc_automation() {
  if(Commands.availableValue('A') && Commands.availableValue('B'))
    get_cc_settings('A')->auto_is_on = (byte)Commands.GetValue('B');
}

void g_get_cc_auto_is_on() {
  if(Commands.availableValue('A'))
    Commands.comment('B', get_cc_settings('A')->auto_is_on);
}

void g_cc_link_sensor() {
  if(Commands.availableValue('A') && Commands.availableValue('S'))
    get_cc_settings('A')->sensor_id = (byte)Commands.GetValue('S');
}

void g_cc_get_link_sensor() {
  if(Commands.availableValue('A'))
    Commands.comment('S', get_cc_settings('A')->sensor_id);
}

void g_cc_set_min_value() {
  if(Commands.availableValue('A') && Commands.availableValue('V'))
    get_cc_settings('A')->min_value = (byte)Commands.GetValue('V');
}

void g_cc_get_min_value() {
  if(Commands.availableValue('A'))
    Commands.comment('V', get_cc_settings('A')->min_value);
}

void g_cc_set_max_value() {
  if(Commands.availableValue('A') && Commands.availableValue('V'))
    get_cc_settings('A')->max_value = (byte)Commands.GetValue('V');
}

void g_cc_get_max_value() {
  if(Commands.availableValue('A'))
    Commands.comment('V', get_cc_settings('A')->max_value);
}

/* Others g-code */

void g_turn_off_all_auto() {
  for (byte actuator_code=0; actuator_code < COUNT_ACTUATORS; actuator_code++) {
    day_settings[actuator_code].auto_is_on = 0;
    cycle_settings[actuator_code].auto_is_on = 0;
    cc_settings[actuator_code].auto_is_on = 0;
  }
}

void g_actuator_write() {
  if(Commands.availableValue('A') && Commands.availableValue('V'))
    actuators[(int)Commands.GetValue('A')].write((byte)Commands.GetValue('V'));
}

void g_get_value_from_sensor() {
  if(Commands.availableValue('S')) {
    Commands.comment('V', sensors[(byte)Commands.GetValue('S')].value());
  }
}

void FinishedCommandLine() {
  Commands.comment("ok");
}