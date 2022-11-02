#ifndef BAVRFieldComms_h
#define BAVRFieldComms_h
#include "Arduino.h"
#include <PubSubClient.h>


class BAVRFieldComms
{
public:
    BAVRFieldComms();
    bool setup(const char* unique_id, PubSubClient* client);
    void reconnect();
    bool connected();
    void loop();
    void subscribe(const char* topic);
    bool message(const char* topic, const char* messagestr);
    void callback(char* topic, byte* payload, unsigned int length);

    bool needs_subscriptions = false;

private:
    const char* unique_id;
    PubSubClient* client;
};

#endif