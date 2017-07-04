/*----------------------------------------------------------------------------------------------------------------------------------------*/
/*                         Większość funkcji i zmiennych w tej klasie są statyczne - wspólne dla każdego obiektu                          */
/*----------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef GAMECELL_H
#define GAMECELL_H

#include "ui_mainwindow.h"
#include <QWidget>

/*-------------------------------------------------------------- K L A S A ---------------------------------------------------------------*/

class GameCell : public QWidget { Q_OBJECT

    friend class MainWindow;
    friend class Server;
    friend class Client;

    public:

        explicit GameCell(QWidget *parent = 0);                                                  // Konstruktor
        ~GameCell(void);                                                                         // Dekonstruktor

        static void tableSetup(void);                                                            // Główne ustawienia pól gry
        static void tableUpdate(void);                                                           // Zmiana ustawień pól gry
        static void setCellsEnabled(bool value);                                                 // Tablica aktywna lub nie
        static void gameMenuSetup(void);                                                         // Początkowe ustawienia Game menu
        static void updateGame(QString whom, int cellId);                                        // Aktualizuje kliknięte pole gry
        static char checkingGameResult(void);                                                    // Sprawdza stan gry (wygrana, remis)

        static int decToChar(int number);                                                        // Dec na char, np 48 na 0, zwraca int
        static void getPtrs(Ui::MainWindow* uiPtr, GameCell** tabPtr) {                          // Przyjmuje niezbędne wskaźniki
            ui = uiPtr; table = tabPtr; return;
        }

    private:

        static Ui::MainWindow *ui;                                                               // Windows i Android (landscape) user interface
        static GameCell** table;                                                                 // Wskaźniki na obiekty klasy GameCell - pola gry

        static const unsigned int dimension;                                                     // Wymiar lewego panelu z polami gry
        static unsigned int sideLen;                                                             // Długość boku jednego pola w pikselach
        static QButtonGroup *cellsGroup;                                                         // Grupa wszystkich pól (Pushbuttons)

        QPushButton *cell;                                                                       // PushButton - jedno pole gry
        char cellSign;                                                                           // 'o' - kółko, 'x' - krzyżyk, NULL - pusto
};

#endif // GAMECELL_H