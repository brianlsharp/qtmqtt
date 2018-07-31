#include "SslSocket.h"
#include <QMetaEnum>

namespace PC
{

//==========================================================
SslSocket::SslSocket( QObject* parent )
    : QSslSocket( parent )
{
    connect( this, SIGNAL( encrypted() ), this, SLOT( onSocketEncrypted() ) );
    connect( this, SIGNAL( stateChanged( QAbstractSocket::SocketState ) ), this, SLOT( onSocketStateChanged( QAbstractSocket::SocketState ) ) );
    connect( this, QOverload<QAbstractSocket::SocketError>::of( &QAbstractSocket::error ),
        [=] ( QAbstractSocket::SocketError )
    {
        printf( "socket error: %s\n", errorString().toStdString().c_str() );
    } );
    connect( this, QOverload<const QList<QSslError> &>::of( &QSslSocket::sslErrors ),
        [=] ( const QList<QSslError> &errors )
    {
        for ( QSslError lError : errors )
            printf( "SSL Error: %s\n", lError.errorString().toStdString().c_str() );
    } );
}


//==========================================================
void SslSocket::onSocketEncrypted()
{
    printf( "successfully established an encrypted socket connection.\n" );
}


//==========================================================
void SslSocket::onSocketStateChanged( QAbstractSocket::SocketState aState )
{
    printf( "socket state changed to %s\n", QMetaEnum::fromType<QAbstractSocket::SocketState>().valueToKey( aState ) );
}

}