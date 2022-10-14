#ifndef uuid_H
#define uuid_H

#include <Arduino.h>
#include <EEPROM.h>

class UUID
{
private:
    void generate_uuid();
    void write_uuid();

public:
    UUID();
    void init();
    void fetch_uuid();
    void pretty_uuid();
    byte uuid[8];
    byte simpl_uuid[32];
};
#endif