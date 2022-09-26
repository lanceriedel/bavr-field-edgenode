#include "BAVRFieldController.hpp"

BAVRFieldController::BAVRFieldController(LEDAnimations& led_animations, LaserDetect& laser_detect, BAVRFieldComms& fieldComms)
{
    this->laser_detect = &laser_detect;
    this->led_animations = &led_animations;
    this->filed_comms = & fieldComms;
}


//Handle all of the MQTT Messages -- they are being handed off to the controller to do the work
void BAVRFieldController::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String topicstr(topic);
  if (topicstr.equals("windowon")) {
      led_animations->ledanimate(1);
      Serial.println("windowon msg");
  }

  if (topicstr.equals("windowoff")) {
      led_animations->ledanimate(0);
      
      Serial.println("windowoff msg");
  }
  
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

///Everybody do a loop   -- make sure no one is hogging the one thread please!!
void BAVRFieldController::loop() {
    if (!filed_comms->connected()) {
    filed_comms->reconnect();
  }
  filed_comms->loop();
  laser_detect->laser_detect();
  led_animations->ledanimate();
}