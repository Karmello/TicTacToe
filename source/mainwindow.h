/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*                   Gracz, który pierwszy wybierze kółko lub krzyżyk blokuje możliwość wyboru znaku drugiemu graczowi                    */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "server.h"
#include "client.h"
#include "gamecell.h"

#include <QMainWindow>
#include <QDesktopWidget>
#include <QDebug>
#include <QKeyEvent>
#include <QTimer>

/*----------------------------------------------------------------------------------------------------------------------------------------*/

namespace Ui { class MainWindow; }

/*------------------------------------------ Globalne funkcje i zmienne zdefiniowane w main.cpp ------------------------------------------*/

// Funkcje:
void setMenuVisible(Ui::MainWindow*, int, bool);                        // Pokazuje/chowa menu
void setMenuEnabled(Ui::MainWindow*, int, bool);                        // Zmienia menu w aktywne lub nieaktywne
void nameValidation(Ui::MainWindow*, QString who);                      // Sprawdza pole nameLineEdit lub imię nadesłane przez przeciwnika
void analyzeSentData(Ui::MainWindow*, QByteArray);                      // Analizuje odebrane dane i podejmuje dalsze kroki

// Zmienne const:
extern QString os;                                                      // Operating System
extern bool consoleDebugging;                                           // Wyświetlanie info w konsoli lub nie
extern int clientWaitingTime;                                           // Tyle ms klient czeka na połączenie z serwerem
extern QFont normalFont;                                                // Normalna czcionka
extern QList<QString> bgColors;                                         // Background colors
extern QList<QString> textColors;                                       // Text colors
extern QList<QString> infoText;                                         // Info wyświetlane w infoText podczas gry

// Zmienne non-const:
extern char so;                                                         // Screen orientation: 'p' - portrait, 'l' - landscape
extern char role;                                                       // 's' - server, 'c' - client
extern char sign;                                                       // 'o' - kółko, 'x' - krzyżyk
extern bool playing;                                                    // True - gdy gracze grają
extern bool myTurn;                                                     // True - gdy komputer ma ruch
extern int activeMenu;                                                  // Numer aktualnie wyświetlanego menu
extern bool busy;                                                       // Aby wychwycić czy jakiś event się zakończył czy jest w trakcie



/*-------------------------------------------------------------- K L A S A ---------------------------------------------------------------*/

class MainWindow : public QMainWindow { Q_OBJECT

    friend class GameCell;

    public:

        explicit MainWindow(QWidget *parent = 0);                             // Konstruktor
        ~MainWindow(void);                                                    // Dekonstruktor

        bool eventFilter(QObject *, QEvent *);                                // Różne zdarzenia
        void resizeEvent(QResizeEvent*);                                      // Kiedy okno zmienia rozmiar
        void keyPressEvent(QKeyEvent*);                                       // Kiedy klawisz wciśnięty

        void firstHiding(void);
        void getScreenOrientation(void);                                      // Sprawdza orientację ekranu
        void windowSetup(void);                                               // Ustawia opcje związane z oknem
        void progressBarSetup(void);                                          // Ustawia progressBar
        void calculateVariables(void);                                        // Oblicza wartości pewnym zmiennych
        void leftRightPanelsSetup(void);                                      // Ustawia lewą i prawą część okna
        void menusSetup(void);                                                // Ustawia rozmiary i pozycje elementów prawego menu
        void setFontSize(QFont font);                                         // Ustawia czcionkę
        void connectionsSetup(void);                                          // Ustawia sygnały i sloty

        void printVariables(void);                                            // Wyświetla info w konsoli (naciśnij v w aplikacji)

    private slots:

        void gameLoading(void);                                               // Animacja ładowania gry

        // Kliknięcia w Buttons
        void onCellsGroupClicked(int);                                        // Kiedy jakaś pole zostaje kliknięte
        void onFullScreenButtonClicked(void);                                 // FullButton
        void onEscapeButtonClicked(void);                                     // EscapeButton: Quit game, Close server, Go back, Disconnect
        void onServerButtonClicked(void);                                     // ServerButton
        void onClientButtonClicked(void);                                     // ClientButton
        void onConnectButtonClicked(void);                                    // ConnectButton
        void onOButtonClicked(void);                                          // Kiedy gracz wybierze kółko
        void onXButtonClicked(void);                                          // Kiedy gracz wybierze krzyżyk

        void makingConnection();
        void sendBytes(QByteArray data);                                      // Wysyła bajty do drugiego kompa

    private:

        char limitTo;                                                         // 'w' - width, 'h' - height, w zależności od zmiennej limit

        /* Rozmiary */
        unsigned int limit;                                                   // screenwidth/3*2 lub screenheight w zależności co mniejsze
        unsigned int leftPanelSide;                                           // Długość krawędzi panelu gry
        unsigned int mainWindowPadding;                                       // Odstęp między panelami, a krawędzią okna

        /* Wskaźniki na obiekty moich pozostałych klas */
        Server *s;                                                            // Wskaźnik na obiekt klasy Server
        Client *c;                                                            // Wskaźnik na obiekt klasy Client
        GameCell **table;                                                     // Wskaźniki na obiekty klasy GameCell - pola gry

        /* Pozostałe */

        Ui::MainWindow *ui;                                                   // Windows i Android (landscape) user interface
        QDesktopWidget *screen;                                               // Ekran
        QTimer *timer;                                                        // Timer


};

#endif // MAINWINDOW_H