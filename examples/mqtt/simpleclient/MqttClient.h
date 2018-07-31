#pragma once

#include "QMqttClient.h"

namespace PC
{ 
class MqttClient :  public QMqttClient
{
public:
    MqttClient( QObject* parent = nullptr);
    ~MqttClient() {}
};

}
