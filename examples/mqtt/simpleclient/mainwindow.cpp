/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QDateTime>
#include <QtMqtt/QMqttClient>
#include <QtWidgets/QMessageBox>
#include "SslSocket.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createSocket();
    createClient();

    connect(ui->lineEditHost, &QLineEdit::textChanged, m_client, &QMqttClient::setHostname);
    ui->lineEditHost->setText( "iot.eclipse.org" );
    connect( ui->spinBoxPort, QOverload<int>::of( &QSpinBox::valueChanged ), this, &MainWindow::setClientPort );
    updateLogStateChange();

    connect( ui->btn_CaCertificate, SIGNAL( clicked() ), this, SLOT( onSelectCertFileClicked() ) );
    connect( ui->btn_ClientCert, SIGNAL( clicked() ), this, SLOT( onSelectClientCertificate() ) );
    connect( ui->btn_ClientPrivateKey, SIGNAL( clicked() ), this, SLOT( onSelectClientPrivateKey() ) );
}

void MainWindow::createSocket()
{
    socket = new PC::SslSocket( this );
}

void MainWindow::createClient()
{
    m_client = new PC::MqttClient( this );
    m_client->setHostname( ui->lineEditHost->text() );
    m_client->setPort( ui->spinBoxPort->value() );
    m_client->setTransport( socket, QMqttClient::SecureSocket );

    connect( m_client, &QMqttClient::errorChanged, this, &MainWindow::onErrorChanged );
    connect( m_client, &QMqttClient::stateChanged, this, &MainWindow::updateLogStateChange );
    connect( m_client, &QMqttClient::disconnected, this, &MainWindow::brokerDisconnected );

    connect( m_client, &QMqttClient::messageReceived, this, [this] ( const QByteArray &message, const QMqttTopicName &topic ) {
        const QString content = QDateTime::currentDateTime().toString()
            + QLatin1String( " Received Topic: " )
            + topic.name()
            + QLatin1String( " Message: " )
            + message
            + QLatin1Char( '\n' );
        ui->editLog->insertPlainText( content );
    } );

    connect( m_client, &QMqttClient::pingResponseReceived, this, [this] () {
        ui->buttonPing->setEnabled( true );
        const QString content = QDateTime::currentDateTime().toString()
            + QLatin1String( " PingResponse" )
            + QLatin1Char( '\n' );
        ui->editLog->insertPlainText( content );
    } );
}



void MainWindow::onErrorChanged(  )
{
    printf( "MainWindow::QMqttClient error: %d\n", m_client->error() );
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonConnect_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected) 
    {
        // this is connect
        ui->lineEditHost->setEnabled(false);
        ui->spinBoxPort->setEnabled(false);
        ui->buttonConnect->setText(tr("Disconnect"));

        socket->setLocalCertificate( mClientCertificateFilename );
        socket->setPrivateKey( mClientPrivateKeyFilename );
        if ( !socket->addCaCertificates( mCaCertificateFilename ) )
            printf( " could not successfully add server CA certificate\n" );

        m_client->connectToHostEncrypted( m_client->hostname() );
    } 
    else 
    {
        ui->lineEditHost->setEnabled(true);
        ui->spinBoxPort->setEnabled(true);
        ui->buttonConnect->setText(tr("Connect"));
        m_client->disconnectFromHost();
    }
}

void MainWindow::on_buttonQuit_clicked()
{
    QApplication::quit();
}

void MainWindow::updateLogStateChange()
{
    const QString content = "QMQTTClient State Changed: "
                    + QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(m_client->state())
                    + QLatin1Char('\n');
    ui->editLog->insertPlainText(content);
}

void MainWindow::brokerDisconnected()
{
    ui->lineEditHost->setEnabled(true);
    ui->spinBoxPort->setEnabled(true);
    ui->buttonConnect->setText(tr("Connect"));
}

void MainWindow::setClientPort(int p)
{
    m_client->setPort(p);
}

void MainWindow::on_buttonPublish_clicked()
{
    if (m_client->publish(ui->lineEditTopic->text(), ui->lineEditMessage->text().toUtf8()) == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}

void MainWindow::on_buttonSubscribe_clicked()
{
    auto subscription = m_client->subscribe(ui->lineEditTopic->text());
    if (!subscription) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    }
}

void MainWindow::on_buttonPing_clicked()
{
    ui->buttonPing->setEnabled(false);
    m_client->requestPing();
}


#include "qfiledialog.h"
void MainWindow::onSelectCertFileClicked()
{
    QString lFilename = QFileDialog::getOpenFileName( this, tr( "Select cert file" ) );
    ui->ln_CaCertificate->setText( lFilename );
    mCaCertificateFilename = lFilename;
}

void MainWindow::onSelectClientCertificate()
{
    QString lFilename = QFileDialog::getOpenFileName( this, tr( "Select cert file" ) );
    ui->ln_clientCert->setText( lFilename );
    mClientCertificateFilename = lFilename;
}


void MainWindow::onSelectClientPrivateKey()
{
    QString lFilename = QFileDialog::getOpenFileName( this, tr( "Select cert file" ) );
    ui->ln_ClientPrivateKey->setText( lFilename );
    mClientPrivateKeyFilename = lFilename;
}