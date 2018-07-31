#include "MqttClient.h"

namespace PC
{

MqttClient::MqttClient( QObject* parent )
    : QMqttClient( parent )
{
    connect( this, SIGNAL(errorChanged( QMqttClient::ClientError )), this, SLOT( onErrorChanged( QMqttClient::ClientError ) ) );
    connect( this, SIGNAL(disconnected()), this, SLOT( brokerDisconnected() ) );

}

void MqttClient::onErrorChanged( QMqttClient::ClientError )
{
    printf( "MainWindow::QMqttClient error: %d\n", error() );
}

void MqttClient::brokerDisconnected()
{
    printf( "MqttClient::brokerDisconnected()\n" );
}

}