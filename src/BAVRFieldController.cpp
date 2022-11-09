#include "BAVRFieldController.hpp"
#include "config.hpp"
#include "BAVRFreeMemory.hpp"


void BAVRFieldController::clean_buffers()
{
  memset(topic, 0, sizeof(topic));
  memset(message, 0, sizeof(message));
  //memset(uuid, 0, sizeof(uuid));
  json.clear();
}

BAVRFieldController::BAVRFieldController(LEDAnimations *led_animations, 
LaserDetect *laser_detect, BAVRFieldComms *field_comms, 
BallDetect *ball_detect)
{
  this->laser_detect = laser_detect;
  this->led_animations = led_animations;
  this->field_comms = field_comms;
  this->ball_detect = ball_detect;


  strcpy(node_id, "unnamed-bldg");
}

void BAVRFieldController::check_memory()
{
  if (freeMemory() < worst_memory)
  {
    worst_memory = freeMemory();
  }
}

void BAVRFieldController::laser_hit_message(int hits)
{

  clean_buffers(); // clean buffers before use
  json["num_hits"] = hits;
  json["timestamp"] = millis();
  json["BUILDING_NAME"] = (const char *)node_id;
  json["side_id"] = 1;

  serializeJson(json, message);

  strcpy(topic, "edgenode/laserhit/");
  strcat(topic, node_id);

  // Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);
  check_memory();

  field_comms->message(topic, (const char *)message);
}

void BAVRFieldController::heartbeat_message()
{

  clean_buffers(); // clean buffers before use
  
  json["bldg"] = (const char *)node_id;
  json["timestamp"] = millis();
  json["uuid"] = uuid;
  json["index"]= building_name_index;
  json["heater_on"] = isheater_on;
  json["latest_diff"] = laser_detect->get_latest_diff();
  json["latest_triggered_diff"] = laser_detect->get_latest_triggered_diff();

  json["mem"] = freeMemory();

  serializeJson(json, message);

  strcpy(topic, "edgenode/heartbeat/");
  strcat(topic, node_id);

  // Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);
  check_memory();

  field_comms->message(topic, (const char *)message);
}

void BAVRFieldController::laser_last_raw_reading_message()
{

  clean_buffers(); // clean buffers before use
  uint32_t  avgk = laser_detect->get_lastest_avgk();
  uint16_t  latest_readings = laser_detect->get_lastest_readings();
  
  json["bldg"] = (const char *)node_id;
  json["k_1"] = avgk;


  json["r_1"] = latest_readings;


  serializeJson(json, message);

  strcpy(topic, "edgenode/laserconfig/");
  strcat(topic, node_id);

  // Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);
  check_memory();
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
  check_memory();
  field_comms->message(topic, (const char *)message);
}

bool prefix(const char *pre, const char *str)
{
  return strncmp(pre, str, strlen(pre)) == 0;
}

void BAVRFieldController::reset_match()
{
  Serial.println(F("MATCH RESET!"));
  this->current_fire_score = 0;
  this->heater_off();
  // reset laser detect light sensor?
  this->laser_detect->reset();
  check_memory();
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
  check_memory();
}

void BAVRFieldController::subscribe_all()
{
  // Subscribe to all messages for this
  clean_buffers(); // clean buffers before use
  strcpy(topic, "nodered/firescore/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);

  clean_buffers();
  strcpy(topic, "nodered/laserdiff");
  field_comms->subscribe(topic);

  clean_buffers();
  strcpy(topic, "nodered/weighttare");
  field_comms->subscribe(topic);

  clean_buffers();
  strcpy(topic, "nodered/heateron/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);

  clean_buffers();
  strcpy(topic, "nodered/heateroff/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);

  // subscribe to nodered/reset/match
  clean_buffers();
  strcpy(topic, "nodered/reset/match");
  field_comms->subscribe(topic);

  clean_buffers();
  strcpy(topic, "nodered/updatepixels/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);

  clean_buffers();
  strcpy(topic, "nodered/lightoff/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);

  clean_buffers();
  strcpy(topic, "nodered/lighton/");
  strcat(topic, node_id);
  field_comms->subscribe(topic);

  check_memory();
}

void BAVRFieldController::reset_all()
{
  // ball_detect.reset();

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
  isheater_on = true;
  check_memory();
}

void BAVRFieldController::heater_off()
{
  Serial.print(F("Turn Heater OFF pin:  "));
  Serial.println(HEATER_PIN);
  if (HEATER_PIN==56) {
    Serial.println(F("Serial is 56"));
  }
  isheater_on = false;
  digitalWrite(HEATER_PIN,LOW);
  check_memory();
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
  if (length > sizeof(message)/sizeof(message[0]))
  {
    //we got too long of a message
    Serial.print(F("GOT TOO LONG OF A MESSAGE TO PARSE: "));
    Serial.println(length);
    return;
  }

  clean_buffers(); // clean buffers before use

  // this block is needed apparently to deserialized because payload may not be null terminated? not sure
  if (length > 0)
  {
    strncpy(message, (const char *)payload, length);
    message[length] = 0;
    // Serial.print(F("GOT A MESSAGE TO PARSE on: "));
    // Serial.println(topic);
    check_memory();
  }

  //Keeping this for now -- it was still getting messages for all nodes, even though subscribed to its uuid

  // char uuid_initialization_topic[64];
  // memset(uuid_initialization_topic, 0, sizeof(topic));
  // strcpy(uuid_initialization_topic, "nodered/initialization/");
  // strcat(uuid_initialization_topic, uuid);


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
    check_memory();
  }

  else if (prefix("nodered/reset/match", topic))
  {

    Serial.print(F("Node id  reset: "));
    Serial.println(node_id);
    // now that we know who we are, subscribe to our nodeid
    reset_all();
    valid_message = true;
    check_memory();
  }
  else if (prefix("nodered/heateron/",topic))
  {
    heater_on();
    valid_message = true;
    check_memory();
  }

  else if (prefix("nodered/heateroff/",topic))
  {
    heater_off();
    valid_message = true;
    check_memory();
  }

  else if (prefix("nodered/initialization/",topic))   //NOTE -- This is still getting all node ids!
  //else if (prefix(uuid_initialization_topic,topic))
  {
    if (!configured)
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
      configured = true;
    }

    valid_message = true;
    check_memory();
  }

  else if (prefix("nodered/reset/match", topic))
  {
    reset_match();
    valid_message = true;
    check_memory();
  }

  // else if (prefix("nodered/updatewindow/", topic))
  // {
  //   DeserializationError error = deserializeJson(json, message);
  //   // Test if parsing succeeds.
  //   if (error)
  //   {
  //     Serial.print(F("deserializeJson() failed: "));
  //     Serial.println(error.f_str());
  //     return;
  //   }
  //   if (json.containsKey("side_id") && json.containsKey("activeWindows"))
  //   {
  //     int side = json["side_id"];
  //     int activeWindows = json["activeWindows"];
  //     //led_animations->building.set_active_windows(side, activeWindows);
  //     int damagedWindows = json["damagedWindows"];
  //     //led_animations->building.set_damaged_windows(side, damagedWindows);
  //   }
  //   valid_message = true;
  // }

  else if (prefix("nodered/updatepixels/", topic))
  {

    DeserializationError error = deserializeJson(json, message);
    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    CRGB colors[] = {
      CRGB::Red,
      CRGB::OrangeRed,
      CRGB::Yellow,
      CRGB::Green,
      CRGB::Blue,
      CRGB::Purple
    };

    int strip_index = json["whichLeds"];
    const char* str = json["pixels"];

    // okay listen.. this function is very dangerous...
    // it just blindly expects that there WILL be 30 characters in the
    // pixels key of the json object and if there isnt, well then..
    // we are going to violate some memory safety rules.
    Serial.println(F("ABOUT TO UPDATE PIXELS"));
    for (int i=0; i<30; i++)
    {
      int dec_val = str[i] - 48; //magic conversion from ascii val to decimal
      if (dec_val > 0 && dec_val < 7) //make sure its sane
      {
        led_animations->strips[strip_index].set_pixel_color(i, colors[dec_val - 1]);
      }
    }
    check_memory();
  }

  else if (prefix("nodered/lighton/", topic))
  {
    DeserializationError error = deserializeJson(json, message);
    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    int led_index = json["whichLight"];
    led_animations->leds[led_index].set_led_state(true);
    check_memory();
  }

  else if (prefix("nodered/lightoff/", topic))
  {
    DeserializationError error = deserializeJson(json, message);
    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    int led_index = json["whichLight"];
    led_animations->leds[led_index].set_led_state(false);
    check_memory();
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
    check_memory();
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

void BAVRFieldController::event_trigger(const char *event)
{

  // TODO:
  // Create a backoff and drop events if something is flooding the system
  if (strcmp(event, "laser") == 0)
  {
    this->laser_hit_message(1);
    Serial.println(F("Turning on laser"));
    // led_animations->building.set_inactive_laser(lastone);
    // led_animations->building.set_active_laser(thisone);
    //digitalWrite(LASER_LIGHT_PIN,HIGH);
    led_animations->leds[0].set_led_state(true);

    lastone = thisone;
    last_laser_time = millis();
  }
  
  if (strcmp(event, "ball") == 0)
  {
    this->ball_detect_message(1);
    // Serial.println(F("Controller: Trough event triggered"));
    // field_comms->message("avr-building/1/trough", String(trough_detect->bag_num())); //todo: update to match schema
  }

    if (strcmp(event, "heartbeat") == 0)
  {
    this->heartbeat_message();
    // Serial.println(F("Controller: Trough event triggered"));
    // field_comms->message("avr-building/1/trough", String(trough_detect->bag_num())); //todo: update to match schema
  }
  check_memory();
}

/// Everybody do a loop   -- make sure no one is hogging the one thread please!!
void BAVRFieldController::loop()
{

  field_comms->loop();
  //handle case where we had to re-connect, need to re-sub to everything
  if (configured && field_comms->needs_subscriptions)
  {
    subscribe_all();
    field_comms->needs_subscriptions = false;
  }

//Serial.print(F("building name index:"));Serial.println((uint16_t)this->building_name_index);
//Serial.print("Is Laser:");Serial.println(config_types[building_name_index][LASER]);
  if (building_name_index<UNDEFINED_BLDG && config_types[building_name_index][LASER]==YES) {
    int8_t trigger = laser_detect->laser_detect();
    if (trigger >= 0)
    {
      event_trigger("laser");
    }
  }

  led_animations->loop();
  uint32_t currentms = millis();
  uint32_t diff_ms = currentms-last_laser_time ;
  if (diff_ms> MAX_LASER_INDICATOR ) {
      //led_animations->building.set_inactive_laser(lastone);
      //Serial.println(diff_ms);
      //digitalWrite(LASER_LIGHT_PIN,LOW);
      led_animations->leds[0].set_led_state(false);
  }

  if (building_name_index<UNDEFINED_BLDG && config_types[building_name_index][BALL]==YES) {
    if (ball_detect->ball_detect())
      event_trigger("ball");
  }

  if (currentms-last_heartbeat_time > MAX_HEARTBEAT_WAIT) {
      last_heartbeat_time = currentms;
      event_trigger("heartbeat");
  }
  check_memory();
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
