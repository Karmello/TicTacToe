#ifndef CLIENT_H
#define CLIENT_H

#include "ui_mainwindow.h"
#include "gamecell.h"

#include <QTcpSocket>
#include <QHostAddress>

/*-------------------------------------------------------------- K L A S A ---------------------------------------------------------------*/

class Client : public QTcpSocket { Q_OBJECT

    friend class MainWindow;

    public:

        explicit Client(Ui::MainWindow* uiPtr, GameCell** tabPtr, QObject *parent = 0);       // Konstruktor
        virtual ~Client(void);                                                                // Dekonstuktor

    private slots:

        void onServerSendingData(void);                                                       // Kiedy serwer wysyła dane
        void onDisconnectedFromServer(void);                                                  // Kiedy połączenie z serwerem padnie

    private:

        Ui::MainWindow *ui;                                                                   // Wskaźnik do user interface
        GameCell** table;                                                                     // Wskaźniki na obiekty klasy GameCell - pola gry
        QTcpSocket *client;                                                                   // Klient
};

#endif // CLIENT_H