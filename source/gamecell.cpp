#include "gamecell.h"
#include "mainwindow.h"

/*----------------------------------------------------------------------------------------------------------------------------------------*/

/* Definicje statycznych zmiennych */

Ui::MainWindow* GameCell::ui = NULL;
GameCell** GameCell::table = NULL;
const unsigned int GameCell::dimension = 3;
unsigned int GameCell::sideLen = 0;
QButtonGroup* GameCell::cellsGroup = new QButtonGroup();

/*----------------------------------------------------------------------------------------------------------------------------------------*/

GameCell::GameCell(QWidget *parent) : QWidget(parent) {

    cell = new QPushButton(this);
    cellSign = char(NULL);
}

GameCell::~GameCell(void) {

    if (cellsGroup) { delete cellsGroup; cellsGroup = NULL; }
    ui = NULL;
    table = NULL;
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

void GameCell::tableSetup(void) {

    int cellId = 1;

    for (unsigned int i = 0; i < dimension; ++i) {
        for (unsigned int j = 0; j < dimension; ++j) {

            cellsGroup->addButton(table[j][i].cell, cellId);
            cellsGroup->button(cellId)->setCursor(Qt::PointingHandCursor);
            cellsGroup->button(cellId)->setParent(ui->leftPanel);
            cellsGroup->button(cellId)->setGeometry(sideLen * j, sideLen * i, sideLen, sideLen);
            cellsGroup->button(cellId)->setIconSize(QSize(sideLen/4*3, sideLen/4*3));

            cellId += 1;
        }
    }

    setCellsEnabled(0);

    return;
}

void GameCell::tableUpdate(void) {

    for (unsigned int i = 0; i < dimension; ++i) {
        for (unsigned int j = 0; j < dimension; ++j) {

            table[j][i].cell->setGeometry(sideLen * j, sideLen * i, sideLen, sideLen);

            // Jeśli gra się toczy i sprawdzane pole jest oznakowane
            if (playing && table[j][i].cellSign != char(NULL)) {
                table[j][i].cell->setIconSize(QSize(sideLen/4*3, sideLen/4*3));
            }
        }
    }

    return;
}

void GameCell::setCellsEnabled(bool value) {

    unsigned int cellId = 1;

    for (unsigned int i = 0; i < dimension; ++i) {
        for (unsigned int j = 0; j < dimension; ++j) {

            cellsGroup->button(cellId)->setEnabled(value);
            cellsGroup->button(cellId)->setStyleSheet(bgColors[value + 2]);
            cellsGroup->button(cellId)->setIcon(QIcon());
            table[j][i].cellSign = char(NULL);

            cellId += 1;
        }
    }

    return;
}

void GameCell::gameMenuSetup(void) {

    QString o = ":/images/kolko.png";
    QString x = ":/images/krzyzyk.png";

    QString self, other;

    if (sign == 'o') { self = o; other = x; } else if (sign == 'x') { self = x; other = o; }

    ui->player1sign->setPixmap(QPixmap(self));
    ui->player1name->setText(ui->nameLineEdit->text());
    ui->player1score->setText("0");

    ui->player2sign->setPixmap(QPixmap(other));
    ui->player2score->setText("0");

    nameValidation(ui, "other");

    setMenuVisible(ui, 5, 1);

    return;
}

void GameCell::updateGame(QString whom, int cellId) {

    QString o = ":/images/kolko.png";
    QString x = ":/images/krzyzyk.png";

    QString selectedPath;
    char selectedSign = char(NULL);

    char result = char(NULL);



    // Aktualizacja ekranu gry na danym kompie
    if (whom == "self") {

        if (sign == 'o') {

            selectedPath = o;
            selectedSign = 'o';
        }
        else if (sign == 'x') {

            selectedPath = x;
            selectedSign = 'x';
        }
    }
    // Aktualizacja ekranu gry na kompie przeciwnika
    else if (whom == "other") {

        if (sign == 'o') {

            selectedPath = x;
            selectedSign = 'x';
        }
        else if (sign == 'x') {

            selectedPath = o;
            selectedSign = 'o';
        }
    }

    // Aktualizacja ekranu
    cellsGroup->button(cellId)->setIcon(QIcon(selectedPath));
    cellsGroup->button(cellId)->setIconSize(QSize(GameCell::sideLen/4*3, GameCell::sideLen/4*3));

    // Pole oznaczone odpowiednim znakiem
    table[int((cellId - 1) % GameCell::dimension)][int((cellId - 1) / GameCell::dimension)].cellSign = selectedSign;

    // Zmiana ruchu
    if (myTurn == true) { myTurn = false; } else if (myTurn == false) { myTurn = true; }

    result = checkingGameResult();

    // Jeśli ktoś wygrał
    if (result != '0' && result != '1') {

        if (result == sign) {

            ui->player1score->setText(QString::number(ui->player1score->text().toInt() + 1));
        }
        else if (result != sign) {

            ui->player2score->setText(QString::number(ui->player2score->text().toInt() + 1));
        }

        setCellsEnabled(1);
    }
    // Jeśli padł remis
    else if (result == '1') {

        setCellsEnabled(1);
    }

    return;
}

char GameCell::checkingGameResult(void) {

    /* Funkcja zwraca:

            char = '0'                  - jeśli gra się wciąż toczy
            char = '1'                  - jeśli padł remis
            char = 'o' lub 'x'          - jeśli ktoś wygrał
    */

    char cellSigns[3];
    unsigned int n;
    int valueDiff;
    int cellId;
    int numOfSelectedCells = 0;



    // a = 0: wiersze, a = 1: kolumny, a = 2: przekątne
    for (unsigned int a = 0; a < 3; ++a) {

        // Określenie ilości iteracji kolejnej pętli wewnętrznej
        if (a == 0 || a == 1) { n = 3; } else if (a == 2) { n = 2; }

        // i = 0: wiersz/kolumna/przekątna nr1, i = 1: wiersz/kolumna/przekątna nr2, i = 3: wiersz/kolumna nr3
        for (unsigned int i = 0; i < n; ++i) {

            // Reset tablicy znakowej cellSigns
            for (unsigned int k = 0; k < 3; ++k) { cellSigns[k] = char(NULL); }

            /* Ustalenie wartości początkowej cellId i wartości valueDiff w oparciu o zmienną a i zmienną i */
            // Wiersze
            if (a == 0) {

                valueDiff = 1;

                if (i == 0) { cellId = 1; }                                 // Wiersz nr1
                else if (i == 1) { cellId = 4; }                            // Wiersz nr2
                else if (i == 2) { cellId = 7; }                            // Wiersz nr3
            }
            // Kolumny
            else if (a == 1) {

                valueDiff = 3;

                if (i == 0) { cellId = 1; }                                 // Kolumna nr1
                else if (i == 1) { cellId = 2; }                            // Kolumna nr2
                else if (i == 2) { cellId = 3; }                            // Kolumna nr3
            }
            // Przekątne
            else if (a == 2) {

                if (i == 0) { valueDiff = 4; cellId = 1; }                  // Przekątna '\'
                else if (i == 1) { valueDiff = 2; cellId = 3; }             // Przekątna '/'
            }
            /* -------------------------------------------------------------------------------------------- */

            // j = 0: pole nr1, j = 1: pole nr2, j = 2: pole nr3
            for (unsigned int j = 0; j < 3; ++j) {

                // Jeśli sprawdzane pole posiada znak
                if (table[int((cellId - 1) % GameCell::dimension)][int((cellId - 1) / GameCell::dimension)].cellSign != char(NULL)) {

                    cellSigns[j] = table[int((cellId - 1) % GameCell::dimension)][int((cellId - 1) / GameCell::dimension)].cellSign;
                    cellId += valueDiff;

                    // Ostatnia iteracja
                    if (j == 2) {

                        // Jeśli wszystkie pola danego wiersza/kolumny/przekątnej mają ten sam znak
                        if ((cellSigns[0] == cellSigns[1]) && (cellSigns[1] == cellSigns[2])) {

                            // Ktoś wygrał
                            qDebug() << "Win!";
                            return cellSigns[0];
                        }
                    }
                }
                // Jeśli sprawdzane pole jest puste
                else { break; }
            }
        }
    }



    // Sprawdza ilość zaznaczonych pól
    for (int i = 0; i < 9; ++i) {

        // Jeśli sprawdzane pole posiada znak
        if (table[int(i % GameCell::dimension)][int(i / GameCell::dimension)].cellSign != char(NULL)) { numOfSelectedCells += 1; }
    }

    // Jeśli gra zakończyła się remisem
    if (numOfSelectedCells == 9) {

        qDebug() << "Draw!";
        return '1';
    }

    // Jeśli nikt nie wygrał, ale gra się jeszcze nie skończyła
    qDebug() << "Still playing ...";
    return '0';
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

int GameCell::decToChar(int number) {

    if (number == 48) { return 0; }
    else if (number == 49) { return 1; }
    else if (number == 50) { return 2; }
    else if (number == 51) { return 3; }
    else if (number == 52) { return 4; }
    else if (number == 53) { return 5; }
    else if (number == 54) { return 6; }
    else if (number == 55) { return 7; }
    else if (number == 56) { return 8; }
    else if (number == 57) { return 9; }

    return -1;
}