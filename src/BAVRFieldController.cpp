#include "BAVRFieldController.hpp"


void BAVRFieldController::clean_buffers()
{
  memset(topic, 0, sizeof(topic));
  memset(message, 0, sizeof(message));
  json.clear();
}

BAVRFieldController::BAVRFieldController(LEDAnimations *led_animations, LaserDetect *laser_detect, BAVRFieldComms *field_comms, TroughDetect *trough_detect, BallDetect *ball_detect)
{
  this->laser_detect = laser_detect;
  this->led_animations = led_animations;
  this->field_comms = field_comms;
  this->trough_detect = trough_detect;
  this->ball_detect = ball_detect;

  strcpy(node_id, "unnamed-bldg");
}

void BAVRFieldController::laser_hit_message(int hits, int whichone)
{

  clean_buffers(); // clean buffers before use
  json["num_hits"] = hits;
  json["timestamp"] = 100000;
  json["BUILDING_NAME"] = (const char *)node_id;
  json["side_id"] = whichone + 1;

  serializeJson(json, message);

  strcpy(topic, "edgenode/laserhit/");
  strcat(topic, node_id);

  // Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);

  field_comms->message(topic, (const char *)message);
}

void BAVRFieldController::laser_last_raw_reading_message()
{

  clean_buffers(); // clean buffers before use
  uint32_t * avgk = laser_detect->get_lastest_avgk();
  uint16_t * latest_readings = laser_detect->get_lastest_readings();
  
  json["bldg"] = (const char *)node_id;
  json["k_1"] = avgk[0];
  json["k_2"] = avgk[1];
  json["k_3"] = avgk[2];
  json["k_4"] = avgk[3];

  json["r_1"] = latest_readings[0];
  json["r_2"] = latest_readings[1];
  json["r_3"] = latest_readings[2];
  json["r_4"] = latest_readings[3];

  serializeJson(json, message);

  strcpy(topic, "edgenode/laserconfig/");
  strcat(topic, node_id);

  // Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);

  field_comms->message(topic, (const char *)message);
}

void BAVRFieldController::last_weight_tare_reading_message()
{

  clean_buffers(); // clean buffers before use
  long last_stable_tare = trough_detect->get_last_weight_stable();
  
  json["bldg"] = (const char *)node_id;
  json["weight_tare"] = last_stable_tare;
 
  serializeJson(json, message);

  strcpy(topic, "edgenode/latesttare/");
  strcat(topic, node_id);

  // Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);

  field_comms->message(topic, (const char *)message);
}

void BAVRFieldController::ball_detect_message(int drops)
{

  clean_buffers(); // clean buffers before use
  json["num_drops"] = drops;
  json["timestamp"] = 100000;
  json["BUILDING_NAME"] = (const char *)node_id;
  json["side_id"] = 0;

  serializeJson(json, message);

  strcpy(topic, "edgenode/balldrop/");
  strcat(topic, node_id);

  // Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);

  field_comms->message(topic, (const char *)message);
}

void BAVRFieldController::trough_detect_message(int bags)
{

  clean_buffers(); // clean buffers before use
  json["num_bags"] = bags;
  json["timestamp"] = 100000;
  json["BUILDING_NAME"] = (const char *)node_id;
  json["side_id"] = 0;

  serializeJson(json, message);

  strcpy(topic, "edgenode/troughbags/");
  strcat(topic, node_id);

  // led_animations->windows[1].on_fire = !led_animations->windows[1].on_fire;

  // Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);

  field_comms->message(topic, (const char *)message);
}

bool prefix(const char *pre, const char *str)
{
  return strncmp(pre, str, strlen(pre)) == 0;
}

void BAVRFieldController::reset_match()
{
  Serial.println("MATCH RESET!");
  this->current_fire_score = 0;
  // reset laser detect light sensor?
  this->laser_detect->reset();
}

void BAVRFieldController::subscribe_all()
{
  clean_buffers(); // clean buffers before use
  strcpy(topic, "nodered/firescore/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);

  clean_buffers();
  
  strcpy(topic, "nodered/laserdiff");
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);

  clean_buffers();
  
  strcpy(topic, "nodered/weighttare");
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);



  // Subscribe to all messages for this
}

void BAVRFieldController::reset_all()
{
  // ball_detect.reset();
  trough_detect->reset();

  // Subscribe to all messages for this
}

void BAVRFieldController::interrupt(int pin)
{
  // Serial.println("");Serial.print("interupt pin:"); Serial.println(pin);
  if (pin == ball_detect->get_pin())
  {
    ball_detect->ball_trigger();
  }
}

// Handle all of the MQTT Messages -- they are being handed off to the controller to do the work
void BAVRFieldController::callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print(F("Message arrived ["));
  Serial.print((topic));
  Serial.print(F("] "));
  Serial.println(length);

  clean_buffers(); // clean buffers before use

  // this block is needed apparently to deserialized because payload may not be null terminated? not sure
  if (length > 0)
  {
    strncpy(message, (const char *)payload, length);
    message[length] = 0;
    Serial.println(message);
  }

  if (prefix("nodered/firescore/", topic))
  {
    DeserializationError error = deserializeJson(json, message);

    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    int newfirescore = json["FCS"];
    Serial.println(F("FCS:"));
    Serial.println(newfirescore);
    current_fire_score = newfirescore;
  }

  else if (prefix("nodered/reset/match", topic))
  {

    Serial.print(F("Node id  reset: "));
    Serial.println(node_id);
    // now that we know who we are, subscribe to our nodeid
    reset_all();
  }

  else if (prefix("nodered/initialization", topic))
  {
    memset(node_id, 0, 128);
    strcpy(node_id, message); // TODO - this should probably be dersialized as json object vs a raw string
    Serial.print(F("Node id initialized: "));
    Serial.println(node_id);
    // now that we know who we are, subscribe to our nodeid
    subscribe_all();
  }

  else if (prefix("nodered/reset/match", topic))
  {
    reset_match();
  }

  else if (prefix("nodered/updatewindow/", topic))
  {
    DeserializationError error = deserializeJson(json, message);
    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    if (json.containsKey("side_id") && json.containsKey("activeWindows"))
    {
      int side = json["side_id"];
      int activeWindows = json["activeWindows"];
      led_animations->building.set_active_windows(side, activeWindows);
    }
  }

  else if (prefix("nodered/laserdiff", topic))
  {
    DeserializationError error = deserializeJson(json, message);

    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    int newdiff = json["DETECTOR_DIFF"];
    Serial.println(F("DETECTOR_DIFF:"));
    Serial.println(newdiff);
    
    laser_detect->set_diff(newdiff);
    laser_last_raw_reading_message();
  }

  else if (prefix("nodered/weighttare", topic))
  {
    DeserializationError error = deserializeJson(json, message);

    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    last_weight_tare_reading_message();
  }
}

void BAVRFieldController::event_trigger(const char *event, int whichone)
{

  // TODO:
  // Create a backoff and drop events if something is flooding the system
  if (strcmp(event, "laser") == 0)
  {
    this->laser_hit_message(1, whichone);
  }
  if (strcmp(event, "trough") == 0)
  {
    this->trough_detect_message(trough_detect->bag_num());
    // Serial.println(F("Controller: Trough event triggered"));
    // field_comms->message("avr-building/1/trough", String(trough_detect->bag_num())); //todo: update to match schema
  }
  if (strcmp(event, "ball") == 0)
  {
    this->ball_detect_message(1);
    // Serial.println(F("Controller: Trough event triggered"));
    // field_comms->message("avr-building/1/trough", String(trough_detect->bag_num())); //todo: update to match schema
  }
}

/// Everybody do a loop   -- make sure no one is hogging the one thread please!!
void BAVRFieldController::loop()
{

  field_comms->loop();
  trough_detect->trough_detect();

  int8_t trigger = laser_detect->laser_detect();
  if (trigger >= 0)
  {
    event_trigger("laser", trigger);
  }

  led_animations->loop();

  if (trough_detect->triggered())
    event_trigger("trough", 0);

  if (ball_detect->ball_detect())
    event_trigger("ball", 0);
}

boolean BAVRFieldController::setup(const char *unique_id)
{
  Serial.println(F("Controller setup..."));
  clean_buffers();
  strcpy(topic, "nodered/initialization/");
  strcat(topic, unique_id);
  field_comms->subscribe(topic);

  // subscribe to nodered/reset/match
  clean_buffers();
  strcpy(topic, "nodered/reset/match");
  field_comms->subscribe(topic);

  // TODO - create second round of subscribing once assigned node id
  clean_buffers();
  strcpy(topic, "nodered/updatewindow/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);

  // Hand build JSON for now
  // Send request for initialization params (mostly node id)
  clean_buffers();

  strcpy(topic, "{\"uuid\":\"");
  strcat(topic, unique_id);
  strcat(topic, "\", \"timestamp\":10000000}");

  field_comms->message("edgenode/initialization", (const char *)topic);
  return true;
}
