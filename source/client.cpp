#include "client.h"
#include "mainwindow.h"

/*----------------------------------------------------------------------------------------------------------------------------------------*/

Client::Client(Ui::MainWindow* uiPtr, GameCell** tabPtr, QObject *parent) : QTcpSocket(parent) {

    ui = uiPtr;
    table = tabPtr;

    client = new QTcpSocket(this);

    /* Sygnały i sloty */
    connect(client, SIGNAL(readyRead()), this, SLOT(onServerSendingData()));
    connect(client, SIGNAL(disconnected()), this, SLOT(onDisconnectedFromServer()));
}

Client::~Client(void) {

    ui = NULL;
    table = NULL;

    if (client->isOpen()) { client->close(); qDebug() << "You have disconnected from the server."; }
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

void Client::onServerSendingData(void) {

    QByteArray data = client->readAll();                                        // Czyta dane z bufora
    qDebug() << "From server:" << data;                                         // Wyświetla dane w konsoli

    analyzeSentData(ui, data);

    return;
}

void Client::onDisconnectedFromServer(void) {

    role = char(NULL);
    sign = char(NULL);
    playing = false;

    ui->infoBar->setText(infoText[8]);
    ui->escapeButton->setText("Go back");

    setMenuVisible(ui, activeMenu, 0);
    setMenuVisible(ui, 3, 1);
    GameCell::setCellsEnabled(0);

    client->close();

    qDebug() << "Connection lost!";
    return;
}