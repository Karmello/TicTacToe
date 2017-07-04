#ifndef SERVER_H
#define SERVER_H

#include "ui_mainwindow.h"
#include "gamecell.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QNetworkInterface>

/*-------------------------------------------------------------- K L A S A ---------------------------------------------------------------*/

class Server : public QTcpServer { Q_OBJECT

    friend class MainWindow;

    public:

        explicit Server(Ui::MainWindow* uiPtr, GameCell** tabPtr, QObject *parent = 0);     // Konstruktor
        virtual ~Server(void);                                                              // Dekonstruktor

        bool startServer(void);                                                             // Tworzy serwer
        void printAvailableIps(QList<QHostAddress> &ipList);                                // Wyświetla wszystkie nielokalne IPv4 w konsoli
        bool searchForValidIp(QList<QHostAddress> &ipList, QHostAddress &ip);               // Szuka odpowiedniego Ip

    private slots:

        void onClientConnected(void);                                                       // Kiedy klient połączył się z serwerem
        void onClientDataAvailable(void);                                                   // Kiedy jakieś dane są dostępne ze strony klienta
        void onClientDisconnected(void);                                                    // Kiedy klient rozłączył się lub został rozłączony

    private:

        Ui::MainWindow *ui;                                                                 // Wskaźnik do user interface
        GameCell** table;                                                                   // Wskaźniki na obiekty klasy GameCell - pola gry
        QTcpServer *server;                                                                 // Serwer
        QTcpSocket *client;                                                                 // Klient
};

#endif // SERVER_H