#ifndef BAVRFieldComms_h
#define BAVRFieldComms_h
#include "Arduino.h"
#include <PubSubClient.h>


class BAVRFieldComms
{
public:
    BAVRFieldComms();
    bool setup(String unique_id, PubSubClient* client);
    void reconnect();
    bool connected();
    bool loop();
    bool message(String topic, String messagestr);
    void callback(char* topic, byte* payload, unsigned int length);


private:
    String unique_id;
    PubSubClient* client;

};

#endif