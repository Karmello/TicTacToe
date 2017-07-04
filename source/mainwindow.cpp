#include "mainwindow.h"
#include "ui_mainwindow.h"



/*----------------------------------------------------------------------------------------------------------------------------------------*/

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);
    this->installEventFilter(this);

    /* Nadanie wartości zmiennym */
    table = NULL; s = NULL; c = NULL;
    mainWindowPadding = leftPanelSide = limit = 0;
    limitTo = char(NULL);

    /* Inicjalizacja wskaźników obiektami */
    screen = new QDesktopWidget();
    timer = new QTimer(this);

    /* Tworzenie obiektów klasy GameCell */
    table = new GameCell*[GameCell::dimension];
    for (unsigned int i = 0; i < GameCell::dimension; ++i) { table[i] = new GameCell[GameCell::dimension]; }
    GameCell::getPtrs(ui, table);

    /* Tworzenie obiektów związanych z połączeniem */
    s = new Server(ui, table);
    c = new Client(ui, table);

    /* Ustawienia */
    getScreenOrientation();
    windowSetup();
    firstHiding();
    progressBarSetup();
    calculateVariables();
    leftRightPanelsSetup();
    GameCell::tableSetup();
    menusSetup();
    setFontSize(normalFont);

    connectionsSetup();    

    /* Timer do progressBar'u */
    ui->progressBar->show();
    timer->start(15);
}

MainWindow::~MainWindow(void) {

    /* Usuwanie, czyszczenie */

    delete s; s = NULL;
    delete c; c = NULL;

    for (unsigned int i = 0; i < GameCell::dimension; ++i) { delete[] table[i]; table[i] = NULL; }
    delete[] table; table = NULL;

    delete timer; timer = NULL;
    delete screen; screen = NULL;
    delete ui; ui = NULL;
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

bool MainWindow::eventFilter(QObject *object, QEvent *event) {

    // nameLineEdit
    if (object->objectName() == "nameLineEdit") {

        // Focus in
        if (event->type() == QEvent::FocusIn) {

            if (ui->nameLineEdit->text() == "What's your name?") {
                ui->nameLineEdit->setText("");
            }
        }
        // Focus out
        else if (event->type() == QEvent::FocusOut) {

            // Jeśli pole jest puste
            if (ui->nameLineEdit->text() == "") {
                ui->nameLineEdit->setText("What's your name?");
            }
        }
    }

    return 0;
}

void MainWindow::resizeEvent (QResizeEvent *) {

    if (consoleDebugging == true) { qDebug() << "resizeEvent fired!"; }

    /* Nowe rozmiary i położenie obiektów */

    getScreenOrientation();
    progressBarSetup();
    calculateVariables();
    leftRightPanelsSetup();
    GameCell::tableUpdate();
    menusSetup();

    return;
}

void MainWindow::keyPressEvent(QKeyEvent* event) {

    // V - Wyświetla wartości zmiennych w konsoli
    if (event->key() == Qt::Key_V) {

        printVariables();
    }
    // F11 - Fullscreen
    else if (event->key() == Qt::Key_F11) {

        onFullScreenButtonClicked();
    }

    return;
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

void MainWindow::firstHiding(void) {

    for (int i = 2; i <= 5; ++i) {
        setMenuVisible(ui, i, 0);
    }

    ui->progressBar->hide();
    ui->leftPanel->hide();
    ui->rightPanel->hide();

    return;
}

void MainWindow::getScreenOrientation(void) {

    // Kiedy jesteśmy na Windows lub uruchamiamy aplikację na Android poraz pierwszy
    if (os == "Windows" || (os == "Android" && so == char(NULL))) {

        // Portrait
        if (screen->height() >= screen->width()) { so = 'p'; }

        // Landscape
        else if (screen->height() < screen->width()) { so = 'l'; }
    }
    // Kiedy jesteśmy na Android i wykonuje się rotacja ekranu
    else if (os == "Android" && so != char(NULL)) {

        if (so == 'p') { so = 'l'; } else if (so == 'l') { so = 'p'; }
    }

    return;
}

void MainWindow::windowSetup(void) {

    /* a/b - ułamek, procent jaki stanowi szerokość okna w stosunku do szerokości ekranu i wysokość okna do szerokości okna w Windowsie*/
    int a = 2, b = 3;

    if (os != "Android") {
        this->setGeometry(screen->width()/2 - ((screen->width()/b)*a)/2, screen->height()/2 - ((((screen->width()/b)*a)/b)*a)/2, (screen->width()/b)*a, (((screen->width()/b)*a)/b)*a);
    }
    else if (os == "Android") {
        this->setGeometry(0, 0, screen->width(), screen->height());
    }

    return;
}

void MainWindow::progressBarSetup(void) {

    if ((os == "Windows") || (os == "Android" && so == 'l')) {
        ui->progressBar->setGeometry(this->width()/2 - this->width()/3*2/2, this->height()/2 - this->height()/8/2, this->width()/3*2, this->height()/8);
    }
    else if (os == "Android" && so == 'p') {
        ui->progressBar->setGeometry(0, this->height()/2 - this->height()/8/2, this->width(), this->height()/8);
    }

    return;
}

void MainWindow::calculateVariables(void) {

    // Windows lub Android (landscape)
    if (os == "Windows" || (os == "Android" && so == 'l')) {

        // Jeśli wysokość okna jest mniejsza lub równa 2/3 szerokości okna
        if (this->height() <= this->width()/3*2) {

            limitTo = 'h';
            limit = this->height();
        }
        // Jeśli nie
        else {

            limitTo = 'w';
            limit = this->width()/3*2;
        }
    }
    // Android (portrait)
    else if (os == "Android" && so == 'p') {

        // Jeśli szerokość okna jest mniejsza lub równa połowie wysokości okna
        if (this->width() <= this->height()/2) {

            limitTo = 'w';
            limit = this->width();
        }
        // Jeśli nie
        else {

            limitTo = 'h';
            limit = this->height()/2;
        }
    }


    if (so == 'l') { mainWindowPadding = this->height()/50; } else if (so == 'p') { mainWindowPadding = this->width()/50; }
    leftPanelSide = limit - 2*mainWindowPadding;

    // leftPanelSide musi być podzielne przez dimension, żeby wszystko było równo
    while (leftPanelSide % GameCell::dimension != 0) { leftPanelSide -= 1; }

    mainWindowPadding = (limit - leftPanelSide) / 2;
    GameCell::sideLen = leftPanelSide/GameCell::dimension;

    return;
}

void MainWindow::leftRightPanelsSetup(void) {

    /* Left panel */

    if (os == "Windows" && !this->isFullScreen()) {

        ui->leftPanel->setGeometry(mainWindowPadding, mainWindowPadding, leftPanelSide, leftPanelSide);
    }
    else if (os == "Android" && so == 'p') {

        if (limitTo == 'h') {
            ui->leftPanel->setGeometry(this->width()/2 - leftPanelSide/2 , mainWindowPadding, leftPanelSide, leftPanelSide);
        }
        else if (limitTo == 'w') {
            ui->leftPanel->setGeometry(mainWindowPadding, mainWindowPadding, leftPanelSide, leftPanelSide);
        }
    }
    else if ((os == "Windows" && this->isFullScreen()) || (os == "Android" && so == 'l')) {

        int x, y;
        x = y = 0;

        if (limitTo == 'h') { x = (this->width() - leftPanelSide - limit/2)/2; y = mainWindowPadding; }
        else if (limitTo == 'w') { x = mainWindowPadding; y = (this->height() - leftPanelSide)/2; }

        ui->leftPanel->setGeometry(x, y, leftPanelSide, leftPanelSide);
    }



    /* Right panel */

    if (os == "Windows" && !this->isFullScreen()) {

        ui->rightPanel->setGeometry(ui->leftPanel->width() + 2*mainWindowPadding, mainWindowPadding, this->height()/2 - mainWindowPadding, ui->leftPanel->height());
    }
    else if (os == "Android" && so == 'p') {

        ui->rightPanel->setGeometry(this->width()/2 - ui->leftPanel->width()/2, ui->leftPanel->height() + 2*mainWindowPadding, ui->leftPanel->width(), this->height() - ui->leftPanel->height() - 3*mainWindowPadding);
    }
    else if ((os == "Windows" && this->isFullScreen()) || (os == "Android" && so == 'l')) {

        int y = 0;

        if (limitTo == 'h') { y =  mainWindowPadding; }
        else if (limitTo == 'w') { y = (this->height() - leftPanelSide)/2; }

        ui->rightPanel->setGeometry(ui->leftPanel->x() + ui->leftPanel->width() + mainWindowPadding, y, limit/2 - mainWindowPadding, ui->leftPanel->height());
    }

    return;
}

void MainWindow::menusSetup(void) {

    int w, h, m;                            // Szerokość, wysokość małych przycisków, margines
    int s;                                  // Wysokość ServerButton i ClientButton
    int s2;                                 // Wysokość oButton i xButton
    int t;                                  // Pomocnicza zmienna przy ustalaniu pozycji y

    w = h = m = s = s2 = t = 0;

    m = mainWindowPadding;
    if (so == 'l') { h = (GameCell::sideLen/3); } else if (so == 'p') { h = GameCell::sideLen/2.5; }

    w = ui->rightPanel->width();
    t = (ui->rightPanel->height() - 3*h)/2;
    s2 = (ui->rightPanel->height() - 4*h - 3*m)/2;

    s = t - m;

    // Wartość w musi być podzielna przez 4, żeby pola na ip byly ustawione równo
    while (w % 4 != 0) { w -= 1; }



    /* Główne elementy, które są widoczne zawsze */

    ui->infoBar->setGeometry(0, 0, ui->rightPanel->width(), h);
    ui->fullScreenButton->setGeometry(0, ui->rightPanel->height() - h, w, h);
    ui->escapeButton->setGeometry(0, ui->rightPanel->height() - 2*h, w, h);

    /* Main menu */

    ui->serverButton->setGeometry(0, h + t - s, w, s);
    ui->clientButton->setGeometry(0, ui->serverButton->y() + s, w, s);

    ui->serverButton->setIconSize(QSize(s/4*3, s/4*3));
    ui->clientButton->setIconSize(QSize(s/4*3, s/4*3));

    /* Server menu */

    ui->ipLabel->setGeometry(0, h + t - (2*h + m/2)/2, w, h);
    ui->portLabel->setGeometry(0, ui->ipLabel->y() + h + m/2, w, h);

    /* Client menu */

    ui->ipBox1->setGeometry(0, h + t - (3*h + m/2)/2, w/4, h);
    ui->ipBox2->setGeometry(w/4, ui->ipBox1->y(), w/4, h);
    ui->ipBox3->setGeometry(2*(w/4), ui->ipBox1->y(), w/4, h);
    ui->ipBox4->setGeometry(3*(w/4), ui->ipBox1->y(), w/4, h);
    ui->portBox->setGeometry(0, ui->ipBox1->y() + h, w, h);

    ui->connectButton->setGeometry(0, ui->portBox->y() + h + m/2, w, h);

    /* Options menu */

    ui->nameLineEdit->setGeometry(0, h + m, w, h);
    ui->nameLineEdit->installEventFilter(this);

    ui->oButton->setGeometry(0, ui->nameLineEdit->y() + h + m, w, s2);
    ui->xButton->setGeometry(0, ui->oButton->y() + s2, w, s2);

    ui->oButton->setIconSize(QSize(s2/4*3, s2/4*3));
    ui->xButton->setIconSize(QSize(s2/4*3, s2/4*3));

    /* Game menu */

    ui->player1sign->setGeometry(0, h + t - (2*h + m/2)/2, h, h);
    ui->player1name->setGeometry(h, ui->player1sign->y(), w - 2*h, h);
    ui->player1score->setGeometry(w - h, ui->player1name->y(), h, h);

    ui->player2sign->setGeometry(0, ui->player1sign->y() + h + m/2, h, h);
    ui->player2name->setGeometry(h, ui->player2sign->y(), w - 2*h, h);
    ui->player2score->setGeometry(w - h, ui->player2name->y(), h, h);

    /* Kolory małych przycisków */

    if (os == "Android") { ui->fullScreenButton->setStyleSheet(bgColors[0]); ui->fullScreenButton->setEnabled(false); }
    else { ui->fullScreenButton->setStyleSheet(bgColors[1] + textColors[0]); }

    ui->connectButton->setStyleSheet(bgColors[1] + textColors[0]);
    ui->escapeButton->setStyleSheet(bgColors[1] + textColors[0]);

    return;
}

void MainWindow::setFontSize(QFont font) {

    ui->progressBar->setFont(font);

    /* Główne elementy, które są widoczne zawsze */

    ui->infoBar->setFont(font);
    ui->fullScreenButton->setFont(font);
    ui->escapeButton->setFont(font);

    /* Server menu */

    ui->ipLabel->setFont(font);
    ui->portLabel->setFont(font);

    /* Client menu */

    ui->ipBox1->setFont(font);
    ui->ipBox2->setFont(font);
    ui->ipBox3->setFont(font);
    ui->ipBox4->setFont(font);
    ui->portBox->setFont(font);

    ui->connectButton->setFont(font);

    /* Options menu */

    ui->nameLineEdit->setFont(font);

    /* Game menu */

    ui->player1name->setFont(font);
    ui->player1score->setFont(font);
    ui->player2name->setFont(font);
    ui->player2score->setFont(font);

    return;
}

void MainWindow::connectionsSetup(void) {

    connect(timer, SIGNAL(timeout()), this, SLOT(gameLoading()));
    connect(GameCell::cellsGroup, SIGNAL(buttonClicked(int)), this, SLOT(onCellsGroupClicked(int)));

    /* Buttons */

    connect(ui->fullScreenButton, SIGNAL(clicked()), this, SLOT(onFullScreenButtonClicked()));
    connect(ui->escapeButton, SIGNAL(clicked()), this, SLOT(onEscapeButtonClicked()));

    connect(ui->serverButton, SIGNAL(clicked()), this, SLOT(onServerButtonClicked()));
    connect(ui->clientButton, SIGNAL(clicked()), this, SLOT(onClientButtonClicked()));

    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));

    connect(ui->oButton, SIGNAL(clicked()), this, SLOT(onOButtonClicked()));
    connect(ui->xButton, SIGNAL(clicked()), this, SLOT(onXButtonClicked()));

    return;
}

void MainWindow::printVariables(void) {

    qDebug();
    qDebug() << "# Globals:";
    qDebug() << "   so = " << so;
    qDebug() << "   os = " << os;
    qDebug() << "   role =" << role;
    if (!role) { qDebug(); }
    qDebug() << "   sign =" << sign;
    if (!sign) { qDebug(); }
    qDebug() << "   activeMenu:" << activeMenu;
    qDebug() << "   playing:" << playing;
    qDebug() << "   myTurn:" << myTurn;
    qDebug();
    qDebug() << "# Sizes:";
    qDebug() << "   screen_width =" << screen->width();
    qDebug() << "   screen_height =" << screen->height();
    qDebug() << "   window_width =" << this->width();
    qDebug() << "   window_height =" << this->height();
    qDebug() << "   MainWindow::limit =" << limit;
    qDebug() << "   MainWindow::limitTo =" << limitTo;
    qDebug() << "   MainWindow::mainWindowPadding =" << mainWindowPadding;
    qDebug() << "   MainWindow::leftPanelSide =" << leftPanelSide;
    qDebug() << "   GameCell::sideLen =" << GameCell::sideLen;
    qDebug();
    qDebug() << "# Server class:";
    qDebug() << "   isServerListening:" << s->server->isListening();
    qDebug() << "   isClientOpened:" << s->client->isOpen();
    qDebug();
    qDebug() << "# Client class:";
    qDebug() << "   isClientOpened:" << c->client->isOpen();

    return;
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

void MainWindow::gameLoading(void) {

    unsigned int value = ui->progressBar->value();

    if (value == 100) {

        timer->stop();
        this->setStyleSheet(bgColors[4]);
        ui->progressBar->hide();
        ui->leftPanel->show();
        ui->rightPanel->show();

        return;
    }

    value += 1;
    ui->progressBar->setValue(value);

    return;
}

void MainWindow::onCellsGroupClicked(int cellId) {

    // Jeśli komputer ma ruch i kliknięte pole nie jest zaznaczone
    if (myTurn == true && table[int((cellId - 1) % GameCell::dimension)][int((cellId - 1) / GameCell::dimension)].cellSign == char(NULL)) {

        ui->infoBar->setText(infoText[11]);
        sendBytes(QString('s' + QString::number(cellId)).toUtf8());
        GameCell::updateGame("self", cellId);
    }

    return;
}

void MainWindow::onFullScreenButtonClicked(void) {

    if (busy == false) {

        busy = true;

        // Jeśli użytkownik włącza tryb pełnoekranowy
        if (!this->isFullScreen()) {

            ui->fullScreenButton->setText("Fullscreen off");
            this->showFullScreen();
        }
        // Jeśli użytkownik wyłącza tryb pełnoekranowy
        else {

            ui->fullScreenButton->setText("Fullscreen on");
            this->showNormal();
        }

        busy = false;
    }

    return;
}

void MainWindow::onEscapeButtonClicked(void) {

    // Jeśli zamykamy grę (jako serwer lub klient)
    if (ui->escapeButton->text() == "Quit game") {

        QApplication::quit();
    }
    // W innym przypadku
    else {

        setMenuVisible(ui, activeMenu, 0);

        // Jeśli zamykamy serwer
        if (ui->escapeButton->text() == "Close server") {

            s->server->close();

            // Jeśli zamykamy serwer gdy klient jest już połączony (Options menu lub Game menu)
            if (s->client->isOpen()) {

                c->client->close();
                s->client->close();

                ui->nameLineEdit->setText("What's your name?");
                setMenuEnabled(ui, 4, 1);
            }

            ui->infoBar->setText(infoText[0]);
            ui->ipLabel->setText("");
            ui->portLabel->setText("");
            ui->escapeButton->setText("Quit game");
            setMenuVisible(ui, 1, 1);

            role = char(NULL);
            if (consoleDebugging == true) { qDebug() << "You have closed the server."; }

        }
        // Jeśli cofamy się w tył z pozycji client menu
        else if (ui->escapeButton->text() == "Go back") {

            ui->infoBar->setText(infoText[0]);
            ui->escapeButton->setText("Quit game");
            setMenuVisible(ui, 1, 1);
        }
        // Jeśli rozłączamy się z serwerem
        else if (ui->escapeButton->text() == "Disconnect") {

            role = char(NULL);

            ui->escapeButton->setText("Go back");
            setMenuVisible(ui, 3, 1);

            c->client->close();
            s->client->close();
        }
    }

    return;
}

void MainWindow::onServerButtonClicked(void) {

    // Jeśli nie udało się stworzyć serwera
    if (!s->startServer()) {

        ui->infoBar->setText(infoText[2]);
        role = char(NULL);
    }
    // Jeśli udało się utworzyć serwer
    else {

        role = 's';

        ui->infoBar->setText(infoText[1]);
        setMenuVisible(ui, 1, 0);

        ui->ipLabel->setText("IP: " + s->server->serverAddress().toString());
        ui->portLabel->setText("PORT: " + QString::number(s->server->serverPort()));
        ui->escapeButton->setText("Close server");

        setMenuVisible(ui, 2, 1);
    }

    return;
}

void MainWindow::onClientButtonClicked(void) {

    /* Zmiana wyglądu */

    ui->infoBar->setText(infoText[3]);

    ui->ipBox1->setValue(0);
    ui->ipBox2->setValue(0);
    ui->ipBox3->setValue(0);
    ui->ipBox4->setValue(0);
    ui->portBox->setValue(0);

    ui->escapeButton->setText("Go back");

    setMenuVisible(ui, 1, 0);
    setMenuVisible(ui, 3, 1);

    return;
}

void MainWindow::onConnectButtonClicked(void) {

    // Zmiana wyglądu
    ui->infoBar->setText(infoText[4]);
    setMenuEnabled(ui, 3, 0);
    this->repaint();

    QTimer::singleShot(2000, this, SLOT(makingConnection()));

    return;
}

void MainWindow::onOButtonClicked(void) {

    nameValidation(ui, "self");
    sendBytes(QString("n" + ui->nameLineEdit->text()).toUtf8());
    sign = 'o';

    // Jeśli krzyżyk został już wybrany przez drugiego gracza i można zacząć grę
    if (!ui->xButton->isEnabled()) {

        ui->infoBar->setText(infoText[11]);
        setMenuVisible(ui, 4, 0);
        sendBytes("p");

        GameCell::setCellsEnabled(1);
        GameCell::gameMenuSetup();
        playing = true;
        myTurn = false;
    }
    else {

        ui->infoBar->setText(infoText[9]);
        setMenuEnabled(ui, 4, 0);
        sendBytes(QByteArray(&sign, 1));
        myTurn = true;
    }

    return;
}

void MainWindow::onXButtonClicked(void) {

    nameValidation(ui, "self");
    sendBytes(QString("n" + ui->nameLineEdit->text()).toUtf8());
    sign = 'x';

    // Jeśli kółko zostało już wybrane przez drugiego gracza i można zacząć grę
    if (!ui->oButton->isEnabled()) {

        ui->infoBar->setText(infoText[11]);
        setMenuVisible(ui, 4, 0);
        sendBytes("p");

        GameCell::setCellsEnabled(1);
        GameCell::gameMenuSetup();
        playing = true;
        myTurn = false;
    }
    else {

        ui->infoBar->setText(infoText[9]);
        setMenuEnabled(ui, 4, 0);
        sendBytes(QByteArray(&sign, 1));
        myTurn = true;
    }

    return;
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

void MainWindow::makingConnection(void) {

    // Łączenie się z serwerem
    c->client->connectToHost(QHostAddress(QString(ui->ipBox1->text() + '.' + ui->ipBox2->text() + '.' + ui->ipBox3->text() + '.' + ui->ipBox4->text())), ui->portBox->value());

    // Nastąpiło połączenie
    if (c->client->waitForConnected(clientWaitingTime)) {

        role = 'c';

        setMenuEnabled(ui, 3, 1);
        setMenuVisible(ui, 3, 0);

        ui->infoBar->setText(infoText[5]);
        ui->escapeButton->setText("Disconnect");

        ui->nameLineEdit->setText("What's your name?");
        setMenuEnabled(ui, 4, 1);
        setMenuVisible(ui, 4, 1);
        this->repaint();
    }
    // Nie udało się nawiązać połączenia
    else {

        qDebug() << "No connection.";

        role = char(NULL);

        ui->infoBar->setText(infoText[6]);
        setMenuEnabled(ui, 3, 1);
        this->repaint();
    }

    return;
}

void MainWindow::sendBytes(QByteArray data) {

    // Jeśli jesteś serwerem
    if (role == 's') {

        s->client->write(data);
        s->client->flush();
    }
    // Jeśli jesteś klientem
    else if (role == 'c') {

        c->client->write(data);
        c->client->flush();
    }

    return;
}