#include "BAVRFieldController.hpp"
#include "config.hpp"


void BAVRFieldController::clean_buffers()
{
  memset(topic, 0, sizeof(topic));
  memset(message, 0, sizeof(message));
  //memset(uuid, 0, sizeof(uuid));
  json.clear();
}

BAVRFieldController::BAVRFieldController(LEDAnimations *led_animations, 
LaserDetect *laser_detect, BAVRFieldComms *field_comms, TroughDetect *trough_detect, 
BallDetect *ball_detect)
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

void BAVRFieldController::set_config() {
  //arduino style -- brute force
  if (strcmp(node_id, "RBO") == 0)
    this->building_name_index = (uint8_t)RBO;
  if (strcmp(node_id, "RTO") == 0)
    this->building_name_index = (uint8_t)RTO;
  if (strcmp(node_id, "RBM") == 0)
    this->building_name_index = (uint8_t)RBM;
  if (strcmp(node_id, "RTM") == 0)
    this->building_name_index = (uint8_t)RTM;
  if (strcmp(node_id, "RBI") == 0)
    this->building_name_index = (uint8_t)RBI;
  if (strcmp(node_id, "RTI") == 0)
    this->building_name_index = (uint8_t)RTI;

  if (strcmp(node_id, "LBI") == 0)
    this->building_name_index = (uint8_t)LBI;
  if (strcmp(node_id, "LTI") == 0)
    this->building_name_index = (uint8_t)LTI;
  if (strcmp(node_id, "LBM") == 0)
    this->building_name_index = (uint8_t)LBM;
  if (strcmp(node_id, "LTM") == 0)
    this->building_name_index = (uint8_t)LTM;
  if (strcmp(node_id, "LBO") == 0)
    this->building_name_index = (uint8_t)LBO;
  if (strcmp(node_id, "LTO") == 0)
    this->building_name_index = (uint8_t)LTO;

  
  Serial.print(F("building_name_index="));Serial.println(this->building_name_index);
  Serial.print(F("Is Laser="));Serial.println(config_types[this->building_name_index][LASER]);
  Serial.print(F("Is BALL="));Serial.println(config_types[this->building_name_index][BALL]);
  Serial.print(F("Is TRENCH="));Serial.println(config_types[this->building_name_index][TRENCH]);
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

  clean_buffers();
  strcpy(topic, "nodered/updatewindow/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);

  clean_buffers();
  strcpy(topic, "nodered/heateron/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);

  clean_buffers();
  strcpy(topic, "nodered/heateroff/");
  strcat(topic, node_id);
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

void BAVRFieldController::set_heater_pin(uint8_t p) {
  this->heater_pin = p;
}


void BAVRFieldController::heater_on()
{
  Serial.print(F("Turn Heater On pin:  "));
  Serial.println(HEATER_PIN);
  digitalWrite(HEATER_PIN,HIGH);
}

void BAVRFieldController::heater_off()
{
  Serial.print(F("Turn Heater OFF pin:  "));
  Serial.println(HEATER_PIN);
  if (HEATER_PIN==56) {
    Serial.println("Serial is 56");
  }
  digitalWrite(HEATER_PIN,LOW);
}

void BAVRFieldController::interrupt(int pin)
{
  // Serial.println("");Serial.print("interupt pin:"); Serial.println(pin);
  if (pin == ball_detect->get_pin())
  {
    ball_detect->ball_trigger_interrupt();
  }
}

// Handle all of the MQTT Messages -- they are being handed off to the controller to do the work
void BAVRFieldController::callback(char *topic, byte *payload, unsigned int length)
{


  bool valid_message = false;

  clean_buffers(); // clean buffers before use
  char uuid_initialization_topic[256];
  strcpy(uuid_initialization_topic, "nodered/initialization/");
  strcat(uuid_initialization_topic, uuid);

  char update_window_topic[256];
  strcpy(update_window_topic, "nodered/updatewindow/");
  strcat(update_window_topic, node_id);

  char heateron_topic[256];
  strcpy(heateron_topic, "nodered/heateron/");
  strcat(heateron_topic, node_id);

  char heateroff_topic[256];
  strcpy(heateroff_topic, "nodered/heateroff/");
  strcat(heateroff_topic, node_id);

  // this block is needed apparently to deserialized because payload may not be null terminated? not sure
  if (length > 0)
  {
    strncpy(message, (const char *)payload, length);
    message[length] = 0;
    
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
    valid_message = true;
  }

  else if (prefix("nodered/reset/match", topic))
  {

    Serial.print(F("Node id  reset: "));
    Serial.println(node_id);
    // now that we know who we are, subscribe to our nodeid
    reset_all();
    valid_message = true;
  }
  else if (prefix(heateron_topic,topic))
  {
    heater_on();
    valid_message = true;

  }

  else if (prefix(heateroff_topic,topic))
  {
    heater_off();
    valid_message = true;

  }

  else if (prefix(uuid_initialization_topic,topic))
  {
    Serial.println(F("I'm in the else-if-prefix"));
    Serial.print(F("The uuid_initialization_topic is "));
    Serial.println(uuid_initialization_topic);
    Serial.print(F("The shorter topic is "));
    Serial.println(topic);
    Serial.print(F("Currently, my node is "));
    Serial.println(node_id);
    Serial.print(F("The MQTT message is "));
    Serial.println(message);
    memset(node_id, 0, 128);
    strcpy(node_id, message); // TODO - this should probably be dersialized as json object vs a raw string
    Serial.print(F("Node id initialized: "));
    Serial.println(node_id);
    // now that we know who we are, subscribe to our nodeid
    set_config();
    subscribe_all();
    valid_message = true;
  }

  else if (prefix("nodered/reset/match", topic))
  {
    reset_match();
    valid_message = true;
  }

  else if (prefix(update_window_topic, topic))
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
    valid_message = true;
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
    valid_message = true;
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
    valid_message = true;
  }
  if (valid_message) {
    Serial.print(F("Message arrived ["));
    Serial.print((topic));
    Serial.print(F("] "));
    Serial.println(length);
    Serial.println(message);
  } else {
    //do nothing, it was for someone else, and we don't want too much noise on this node
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
  if (building_name_index<UNDEFINED_BLDG && config_types[building_name_index][TRENCH]==YES) {
    trough_detect->trough_detect();
  if (trough_detect->triggered())
      event_trigger("trough", 0);
  }
//Serial.print(F("building name index:"));Serial.println((uint16_t)this->building_name_index);
//Serial.print("Is Laser:");Serial.println(config_types[building_name_index][LASER]);
  if (building_name_index<UNDEFINED_BLDG && config_types[building_name_index][LASER]==YES) {
    int8_t trigger = laser_detect->laser_detect();
    if (trigger >= 0)
    {
      event_trigger("laser", trigger);
    }
  }

  led_animations->loop();

  if (building_name_index<UNDEFINED_BLDG && config_types[building_name_index][BALL]==YES) {
    if (ball_detect->ball_detect())
      event_trigger("ball", 0);
    }
}

boolean BAVRFieldController::setup(const char *unique_id)
{
  Serial.println(F("Controller setup..."));
  building_name_index = (uint8_t)UNDEFINED_BLDG;
  clean_buffers();
  memset(uuid, 0, sizeof(uuid)); // took this from the original clean_buffers
  strcpy(uuid, unique_id);
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
