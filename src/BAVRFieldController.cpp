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
  json["timestamp"] = millis();
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
  json["timestamp"] = millis();
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
  json["timestamp"] = millis();
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
  if (strcmp(node_id, "RTT") == 0)
    this->building_name_index = (uint8_t)RTT;

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
  if (strcmp(node_id, "LTT") == 0)
    this->building_name_index = (uint8_t)LTT;

  
  Serial.print(F("building_name_index="));Serial.println(this->building_name_index);
  Serial.print(F("Is Laser="));Serial.println(config_types[this->building_name_index][LASER]);
  Serial.print(F("Is BALL="));Serial.println(config_types[this->building_name_index][BALL]);
  Serial.print(F("Is TRENCH="));Serial.println(config_types[this->building_name_index][TRENCH]);
    Serial.print(F("Is LIGHTONLY="));Serial.println(config_types[this->building_name_index][LIGHTONLY]);

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

  clean_buffers();
  strcpy(topic, "nodered/updategutter/bysegment/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);

  clean_buffers();
  strcpy(topic, "nodered/updategutter/orderedsegments/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);

  clean_buffers();
  strcpy(topic, "nodered/updategutter/full/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);

  clean_buffers();
  strcpy(topic, "nodered/pathlightsoff/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);

   clean_buffers();
  strcpy(topic, "nodered/pathlightson/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);

    clean_buffers();
  strcpy(topic, "nodered/trenchlightsoff/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);
  Serial.print(F("Subscribed to:"));
  Serial.println(topic);

  clean_buffers();
  strcpy(topic, "nodered/trenchlightson/");
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

  // this block is needed apparently to deserialized because payload may not be null terminated? not sure
  if (length > 0)
  {
    strncpy(message, (const char *)payload, length);
    message[length] = 0;
  }

  //Keeping this for now -- it was still getting messages for all nodes, even though subscribed to its uuid

  char uuid_initialization_topic[64];
  memset(uuid_initialization_topic, 0, sizeof(topic));
  strcpy(uuid_initialization_topic, "nodered/initialization/");
  strcat(uuid_initialization_topic, uuid);


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
  else if (prefix("nodered/heateron/",topic))
  {
    heater_on();
    valid_message = true;

  }

  else if (prefix("nodered/heateroff/",topic))
  {
    heater_off();
    valid_message = true;

  }

  //else if (prefix("nodered/initialization/",topic))   //NOTE -- This is still getting all node ids!
  else if (prefix(uuid_initialization_topic,topic))
  {
    Serial.println(F("I'm in the else-if-prefix"));
    Serial.print(F("The topic is "));
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
      int damagedWindows = json["damagedWindows"];
      led_animations->building.set_damaged_windows(side, damagedWindows);
    }
    valid_message = true;
  }
  else if (prefix("nodered/trenchlightson/", topic))
  {
    DeserializationError error = deserializeJson(json, message);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    if (json.containsKey("side_id"))
    {
      int side = json["side_id"];
      led_animations_other->set_active_trench(side);
    }
    valid_message = true;
  }
    else if (prefix("nodered/trenchlightsoff/", topic))
  {
    DeserializationError error = deserializeJson(json, message);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    if (json.containsKey("side_id"))
    {
      int side = json["side_id"];
      led_animations_other->set_inactive_trench(side);
    }
    valid_message = true;
  }
   else if (prefix("nodered/pathlightson/", topic))
  {
    DeserializationError error = deserializeJson(json, message);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    if (json.containsKey("side_id"))
    {
      int side = json["side_id"];
      led_animations_other->set_active_path(side);
    }
    valid_message = true;
  }
  else if (prefix("nodered/pathlightsoff/", topic))
  {
    DeserializationError error = deserializeJson(json, message);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    if (json.containsKey("side_id"))
    {
      int side = json["side_id"];
      led_animations_other->set_inactive_path(side);
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

  else if (prefix("nodered/updategutter/bysegment/", topic))
  {
    DeserializationError error = deserializeJson(json, message);
    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    if (json.containsKey("segment_id") && json.containsKey("color"))
    {
      int segment = json["segment_id"];
      long color_int = strtol(json["color"], 0, 16);
      CRGB color(color_int);
      led_animations->building.set_gutter_segment(segment, color);
    }
    valid_message = true;
  }

  else if (prefix("nodered/updategutter/orderedsegments/", topic))
  {
    DeserializationError error = deserializeJson(json, message);
    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    if (json.containsKey("num_segments") && json.containsKey("color"))
    {
      int segments = json["num_segments"];
      long color_int = strtol(json["color"], 0, 16);
      CRGB color(color_int);
      led_animations->building.set_gutter_progress(segments, color);
    }
    valid_message = true;
  }

  else if (prefix("nodered/updategutter/full/", topic))
  {
    DeserializationError error = deserializeJson(json, message);
    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    if (json.containsKey("color"))
    {
      Serial.println("Json has a color");
      long color_int = strtol(json["color"],0, 16L);
      Serial.println(color_int, 16);
      CRGB color(color_int);
      led_animations->building.set_gutter_full(color);
    }
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
    uint8_t thisone = (uint8_t) whichone;
    Serial.println(F("Turning on laser"));
    led_animations->building.set_inactive_laser(lastone);
    led_animations->building.set_active_laser(thisone);
    lastone = thisone;
    last_laser_time = millis();
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

  if (building_name_index<UNDEFINED_BLDG && config_types[building_name_index][LIGHTONLY]==YES && hasOtherLEDAnimations==false)  {
      Serial.println(F("Switching to other animations..."));

       hasOtherLEDAnimations = true;
       delete(led_animations);
       led_animations_other = new LEDAnimationsOther();
 }

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
   if (hasOtherLEDAnimations && led_animations_other!=NULL) {
     led_animations_other->loop();
   } else if (hasOtherLEDAnimations==false) {
     if (led_animations!=NULL) {
       led_animations->loop();
     }
   }
  
  uint32_t currentms = millis();
  if (hasOtherLEDAnimations==false && currentms-last_laser_time > MAX_LASER_INDICATOR && lastone!=99) {
      led_animations->building.set_inactive_laser(lastone);
      lastone = 99;
  }

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

  // Send request for initialization params (mostly node id)
  clean_buffers();
  json["uuid"] = unique_id;
  json["timestamp"] = millis();
  serializeJson(json, message);
  field_comms->message("edgenode/initialization", (const char *) message);
  return true;
}
