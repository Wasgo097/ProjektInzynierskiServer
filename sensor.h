#ifndef SENSOR_H
#define SENSOR_H
#include <string>
class Sensor{
    int _id;
    std::string _type;
    std::string _mac;
public:
    Sensor(int id,std::string type,std::string mac);
    int Id()const;
    std::string Type()const;
    std::string Mac()const;
};

#endif // SENSOR_H
