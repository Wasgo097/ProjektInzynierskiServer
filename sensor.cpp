#include "sensor.h"

Sensor::Sensor(int id, std::string type, std::string mac):_id{id},_type{type},_mac{mac}{}
int Sensor::Id() const{
    return _id;
}
std::string Sensor::Type() const{
    return _type;
}
std::string Sensor::Mac() const{
    return _mac;
}
