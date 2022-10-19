#include "uuid.hpp"

UUID::UUID()
{

}

void UUID::init()
{
    byte init_char = EEPROM.read(0);
    if (init_char == 0xFF)
    {
        //we have never run before, gen uuid
        generate_uuid();
        write_uuid();
        EEPROM.write(0, 127);
    }
    fetch_uuid(); //this isnt strictly necessary but whatevs
    pretty_uuid();
}

void UUID::generate_uuid()
//pass in a byte array as &var and let the function modify the array in place
{
    randomSeed(analogRead(7)*analogRead(6)*analogRead(5)+micros());
    long unitId = random();
    long notunitId = -unitId;

    uuid[0] = unitId >> 24 & 0xFF;
    uuid[1] = unitId >> 16 & 0xFF;
    uuid[2] = unitId >> 8  & 0xFF;
    uuid[3] = unitId     & 0xFF;
    uuid[4] = notunitId >> 24 & 0xFF;
    uuid[5] = notunitId >> 16 & 0xFF;
    uuid[6] = notunitId >> 8  & 0xFF;
    uuid[7] = notunitId     & 0xFF;

}

void UUID::write_uuid()
{
    EEPROM.write(1, uuid[0]);
    EEPROM.write(2, uuid[1]);
    EEPROM.write(3, uuid[2]);
    EEPROM.write(4, uuid[3]);
    EEPROM.write(5, uuid[4]);
    EEPROM.write(6, uuid[5]);
    EEPROM.write(7, uuid[6]);
    EEPROM.write(8, uuid[7]);
}

void UUID::fetch_uuid()
{
    uuid[0] = EEPROM.read(0);
    uuid[1] = EEPROM.read(1);
    uuid[2] = EEPROM.read(2);
    uuid[3] = EEPROM.read(3);
    uuid[4] = EEPROM.read(4);
    uuid[5] = EEPROM.read(5);
    uuid[6] = EEPROM.read(6);
    uuid[7] = EEPROM.read(7);
}

void UUID::pretty_uuid()
{
  char b[8];

  memset(b, 0, 8);
  memset(simpl_uuid, 0, 32);

  snprintf(b, 8, "%lX",(long)EEPROM.read(0));  //read a single byte from EEPROM, interpret as ul (4 bytes), write to b
  strcpy((char*)simpl_uuid, b);               //copy b into simpl_uuid
  memset(b, 0, 8);                             //clear b
  snprintf(b, 8, "%lX",(long)EEPROM.read(1));  //read a single byte from EEPROM, interpret as ul (4 bytes), write to b
  strcat((char*)simpl_uuid, b);               //concat b onto simpl_uuid
  memset(b, 0, 8);                             //clear b
  snprintf(b, 8, "%lX",(long)EEPROM.read(2));  //read a single byte from EEPROM, interpret as ul (4 bytes), write to b
  strcat((char*)simpl_uuid, b);               //concat b onto simpl_uuid
  memset(b, 0, 8);                             //clear b
  snprintf(b, 8, "%lX",(long)EEPROM.read(3));  //read a single byte from EEPROM, interpret as ul (4 bytes), write to b
  strcat((char*)simpl_uuid, b);               //concat b onto simpl_uuid
}