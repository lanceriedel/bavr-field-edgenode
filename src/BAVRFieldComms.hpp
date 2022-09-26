#include "Adafruit_TCS34725.h"
#include <Ethernet.h>
#include <PubSubClient.h>

class BAVRFieldComms
{
public:
    BAVRFieldComms();
    boolean setup(String unique_id, PubSubClient& client);
    void reconnect();
    boolean connected();
    boolean loop();
    void callback(char* topic, byte* payload, unsigned int length);


private:
    PubSubClient* client;
    String unique_id;

};