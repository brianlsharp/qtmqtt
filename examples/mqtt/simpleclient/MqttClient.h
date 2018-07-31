#pragma once

#include "QMqttClient.h"

namespace PC
{ 
class MqttClient :  public QMqttClient
{
    Q_OBJECT
public:
    MqttClient( QObject* parent = nullptr);
    ~MqttClient() {}

private slots:
void onErrorChanged( QMqttClient::ClientError );
    void brokerDisconnected();
};

}
