/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*        Dla każdej z klas: MainWindow, Server, Client tworzony jest jeden obiekt. Dla klasy GameCell tworzonych jest 9 obiektów.        */
/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*                        1) Main menu     2) Server menu      3) Client menu     4) Options menu     5) Game menu                        */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

#include "mainwindow.h"
#include <QApplication>

#ifdef Q_OS_ANDROID
    QString os = "Android";

#elif defined(Q_OS_WIN)
    QString os = "Windows";

#else
    QString os = "Unknown";

#endif



/*--------------------------------------------------- F U N K C J E   G L O B A L N E ----------------------------------------------------*/

void setMenuVisible(Ui::MainWindow* ui, int menu, bool value) {

    // Main menu
    if (menu == 1) {

        // Ukrywanie
        if (value == 0) {

            ui->serverButton->hide();
            ui->clientButton->hide();
        }
        // Pokazywanie
        else if (value == 1) {

            ui->serverButton->show();
            ui->clientButton->show();
            activeMenu = 1;
        }
    }
    // Server menu
    else if (menu == 2) {

        // Ukrywanie
        if (value == 0) {

            ui->ipLabel->hide();
            ui->portLabel->hide();
        }
        // Pokazywanie
        else if (value == 1) {

            ui->ipLabel->show();
            ui->portLabel->show();
            activeMenu = 2;
        }
    }
    // Client menu
    else if (menu == 3) {

        // Ukrywanie
        if (value == 0) {

            ui->ipBox1->hide();
            ui->ipBox2->hide();
            ui->ipBox3->hide();
            ui->ipBox4->hide();
            ui->portBox->hide();
            ui->connectButton->hide();
        }
        // Pokazywanie
        else if (value == 1) {

            ui->ipBox1->show();
            ui->ipBox2->show();
            ui->ipBox3->show();
            ui->ipBox4->show();
            ui->portBox->show();
            ui->connectButton->show();
            activeMenu = 3;
        }
    }
    // Options menu
    else if (menu == 4) {

        // Ukrywanie
        if (value == 0) {

            ui->nameLineEdit->hide();
            ui->oButton->hide();
            ui->xButton->hide();
        }
        // Pokazywanie
        else if (value == 1) {

            ui->nameLineEdit->show();
            ui->oButton->show();
            ui->xButton->show();
            activeMenu = 4;
        }
    }
    // Game menu
    else if (menu == 5) {

        // Ukrywanie
        if (value == 0) {

            ui->player1sign->hide();
            ui->player1name->hide();
            ui->player1score->hide();
            ui->player2sign->hide();
            ui->player2name->hide();
            ui->player2score->hide();
        }
        // Pokazywanie
        else if (value == 1) {

            ui->player1sign->show();
            ui->player1name->show();
            ui->player1score->show();
            ui->player2sign->show();
            ui->player2name->show();
            ui->player2score->show();
            activeMenu = 5;
        }
    }

    return;
}

void setMenuEnabled(Ui::MainWindow* ui, int menu, bool value) {

    // Client menu
    if (menu == 3) {

        ui->ipBox1->setEnabled(value);
        ui->ipBox2->setEnabled(value);
        ui->ipBox3->setEnabled(value);
        ui->ipBox4->setEnabled(value);
        ui->portBox->setEnabled(value);

        if (value == 0) {
            ui->connectButton->setStyleSheet(bgColors[value]);
            if (os != "Android") { ui->fullScreenButton->setStyleSheet(bgColors[value]); }
            ui->escapeButton->setStyleSheet(bgColors[value]);
        }
        else if (value == 1) {
            ui->connectButton->setStyleSheet(bgColors[value] + textColors[0]);
            if (os != "Android") { ui->fullScreenButton->setStyleSheet(bgColors[value] + textColors[0]); }
            ui->escapeButton->setStyleSheet(bgColors[value] + textColors[0]);
        }

        ui->connectButton->setEnabled(value);
        if (os != "Android") { ui->fullScreenButton->setEnabled(value); }
        ui->escapeButton->setEnabled(value);
    }
    // Options menu
    else if (menu == 4) {

        ui->nameLineEdit->setEnabled(value);
        ui->oButton->setEnabled(value);
        ui->xButton->setEnabled(value);
    }

    return;
}

void nameValidation(Ui::MainWindow* ui, QString who) {

    // Sprawdza imię wprowadzone na swoim komputerze
    if (who == "self") {

        // Jeśli pole imię zostało nietknięte lub pozostawione puste
        if (ui->nameLineEdit->text() == "" || ui->nameLineEdit->text() == "What's your name?") {
            ui->nameLineEdit->setText("You");
        }
    }
    // Sprawdza imię nadesłane przez przeciwnika
    else if (who == "other") {

        // Jeśli gracze wybrali te same imiona
        if (ui->player1name->text() == ui->player2name->text()) {

            ui->player1name->setText("You");
            ui->player2name->setText("Opponent");
        }
    }

    return;
}

void analyzeSentData(Ui::MainWindow* ui, QByteArray data) {

    // Gdy ruch został wykonany
    if (data[0] == 's') {

        int cellId = GameCell::decToChar(int(data[1]));

        ui->infoBar->setText(infoText[10]);
        GameCell::updateGame("other", cellId);
    }
    // Gdy przesyłane jest imię przeciwnika
    else if (data[0] == 'n') {

        QString name = QString(data);
        name.remove(0, 1);
        if (name == "You") { name = "Opponent"; }
        ui->player2name->setText(name);
    }
    // Gdy przeciwnik wybrał kółko
    else if (data[0] == 'o') {

        ui->oButton->setEnabled(false);
    }
    // Gdy przeciwnik wybrał krzyżyk
    else if (data[0] == 'x') {

        ui->xButton->setEnabled(false);
    }
    // Gdy znaki zostały wybrane po obu stronach i można rozpocząć grę
    else if (data[0] == 'p') {

        ui->infoBar->setText(infoText[10]);
        setMenuVisible(ui, 4, 0);

        GameCell::setCellsEnabled(1);
        GameCell::gameMenuSetup();
        playing = true;
    }

    return;
}



/*--------------------------------------------------- T A B L I C E   G L O B A L N E ----------------------------------------------------*/

QList<QString> bgColors = QList<QString>() << "background-color: rgb(228, 228, 228);"                            // 0 - InfoBar
                                           << "background-color: rgb(66, 66, 255);"                              // 1 - Small buttons
                                           << "background-color: rgb(15, 15, 15);"                               // 2 - Game area unavailable
                                           << "background-color: rgb(255, 255, 255);"                            // 3 - White
                                           << "background-color: rgb(0, 0, 0);"                                  // 4 - Black
;

QList<QString> textColors = QList<QString>() << "color: rgb(255, 255, 255);"                                     // 0 - White
;

QList<QString> infoText = QList<QString>() << "Choose server or client"                                          // 0
                                           << "Waiting for a client ..."                                         // 1
                                           << "Server couldn't start, try again"                                 // 2
                                           << "Enter IP address and port number"                                 // 3
                                           << "Connecting ..."                                                   // 4
                                           << "Connection established!"                                          // 5
                                           << "Could not connect, try again"                                     // 6
                                           << "Connection lost! Waiting ..."                                     // 7
                                           << "Connection lost!"                                                 // 8
                                           << "Waiting for your opponent ..."                                    // 9
                                           << "It's your turn now"                                               // 10
                                           << "Wait for your opponent's move ..."                                // 11
;



/*--------------------------------------------------- Z M I E N N E   G L O B A L N E ----------------------------------------------------*/

bool consoleDebugging = 1;
int clientWaitingTime = 10000;
QFont normalFont = QFont("Arial", 10, 65);
char so = char(NULL);
char role = char(NULL);
char sign = char(NULL);
bool playing = false;
bool myTurn = false;
int activeMenu = 1;
bool busy = false;



/*--------------------------------------------------------------- M A I N ----------------------------------------------------------------*/

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);                                                             // Aplikacja
    MainWindow w;                                                                           // Okno

    if (os != "Android") { w.setFixedSize(w.width(), w.height()); }                         // Żeby nie można było zmieniać rozmiaru okna
    if (os == "Android") { w.showFullScreen(); } else { w.show(); }                         // Pokaż okno
    return a.exec();
}