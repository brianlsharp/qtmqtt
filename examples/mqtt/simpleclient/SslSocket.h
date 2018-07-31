#pragma once

#include "QsslSocket.h"

namespace PC
{ 
class SslSocket :  public QSslSocket
{
    Q_OBJECT
public:
    SslSocket( QObject* parent = nullptr );
    ~SslSocket() {}

private slots:
    // ssl socket slots
    void onSocketStateChanged( QAbstractSocket::SocketState aState );
    void onSocketEncrypted();

};

}
