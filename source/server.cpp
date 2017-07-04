#include "server.h"
#include "mainwindow.h"

/*----------------------------------------------------------------------------------------------------------------------------------------*/

Server::Server(Ui::MainWindow* uiPtr, GameCell** tabPtr, QObject *parent) : QTcpServer(parent) {

    ui = uiPtr;
    table = tabPtr;

    server = new QTcpServer(this);                                          // Serwer
    client = new QTcpSocket(this);                                          // Klient

    connect(server, SIGNAL(newConnection()), this, SLOT(onClientConnected()));
}

Server::~Server(void) {

    ui = NULL;
    table = NULL;

    if (server->isListening()) { server->close(); qDebug() << "You have closed the server."; }
    if (client->isOpen()) { client->close(); }
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

bool Server::startServer(void) {

    // Deklaracje zmiennych
    QHostAddress ip;                                                        // Adres Ip
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();         // Pobiera adresy


    // Jeśli ipList nie jest pusta, czyli znaleziono jakieś Ip na maszynie
    if (ipList.length() > 0) {

        // Wyświetla dostępne adresy Ip
        if (consoleDebugging == true) { printAvailableIps(ipList); }

        // Jeśli znaleziono odpowiednie Ip
        if (searchForValidIp(ipList, ip)) {

            // ip = QHostAddress("6.6.6.6");                   // Podanie błędnego Ip dla testów

            // Jeśli nie udało się stworzyć serwera
            if (!server->listen(ip)) {

                if (consoleDebugging == true) { qDebug() << "\nSomething went wrong. You haven't started the server."; }
                return 0;
            }
            // Jeśli udało się stworzyć serwer
            else {

                if (consoleDebugging == true) {

                    qDebug() << "\nYou have started the server with Ip:" << server->serverAddress() << "on Port:" << server->serverPort();
                    qDebug() << "The server is waiting for a client ...";
                }

                return 1;
            }
        }
        // Jeśli nie znaleziono odpowiedniego Ip
        else { return 0; }
    }
    // Jeśli nie znaleziono żadnych dostępnych Ip na maszynie
    else { return 0; }
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

void Server::printAvailableIps(QList<QHostAddress> &ipList) {

    qDebug() << "\nAvailable Ip's:\n";

    for (int i = 0; i < ipList.count(); ++i) {
        if (ipList[i] != QHostAddress::LocalHost && ipList[i].toIPv4Address()) { qDebug() << ipList[i]; }
    }

    qDebug();
    return;
}

bool Server::searchForValidIp(QList<QHostAddress> &ipList, QHostAddress &ip) {

    QString ipStr;                                                          // Adres Ip jako string
    QString ipStartStr = "192.168";                                         // String, od którego powinien rozpoczynać się adres Ip
    int index;                                                              // Index, od którego rozpoczął się ipStartStr w ipStr

    for (int i = 0; i < ipList.count(); ++i) {
        if (ipList[i] != QHostAddress::LocalHost && ipList[i].toIPv4Address()) {

            ipStr = ipList[i].toString();                   // Konwersja ip do stringa
            index = ipStr.indexOf(ipStartStr);              // Szuka indeksu początkowego ipStartStr w ipStr jeśli wystąpił

            // Jeśli ipStr rozpoczął się od ipStartStr
            if (index == 0) {

                ip = ipList[i];                                                                 // Ip ustanowione
                if (consoleDebugging == true) { qDebug() << "Chosen Ip =" << ip; }              // Wyświetla ustanowione Ip w konsoli
                return 1;                                                                       // Pętla stop
            }
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

void Server::onClientConnected(void) {

    if (consoleDebugging == true) { qDebug() << "The client have connected."; }

    client = server->nextPendingConnection();                                           // Pobiera klienta
    client->write("You're now connected.");                                             // Wysyła informacje o połączeniu do klienta

    /* Połączenia sygnałów i slotów */

    connect(client, SIGNAL(readyRead()), this, SLOT(onClientDataAvailable()));
    connect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));

    /* Zmiana wyglądu */

    ui->infoBar->setText(infoText[5]);

    setMenuVisible(ui, 2, 0);

    ui->nameLineEdit->setText("What's your name?");
    setMenuEnabled(ui, 4, 1);
    setMenuVisible(ui, 4, 1);

    return;
}

void Server::onClientDataAvailable(void) {

    QByteArray data = client->readAll();                        // Czyta dane z bufora
    qDebug() << "From client:" << data;                         // Wyświetla dane w konsoli

    analyzeSentData(ui, data);

    return;
}

void Server::onClientDisconnected(void) {

    sign = char(NULL);
    playing = false;

    setMenuVisible(ui, activeMenu, 0);

    // Gdy klient się rozłącza, ale serwer pozostaje aktywny
    if (server->isListening()) {

        ui->infoBar->setText(infoText[7]);
        setMenuVisible(ui, 2, 1);
    }

    GameCell::setCellsEnabled(0);
    client->close();

    if (consoleDebugging == true) { qDebug() << "Connection lost!"; }
    return;
}