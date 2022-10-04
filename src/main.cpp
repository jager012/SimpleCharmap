#include <QApplication>
#include <QMessageBox>
#include <QPixmap>
#include <QSplashScreen>
#include <QTranslator>

#include "MainWindow.h"
#include "CharTableView.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QSplashScreen splash(QPixmap(":/SimpleCharmap/splash"));
    splash.show();
    a.processEvents();

    QApplication::setApplicationName("SimpleCharmap");
    QApplication::setOrganizationName("jager012");
    QApplication::setOrganizationDomain("jager012.app");

    QTranslator translator;
    if (translator.load(QLocale::system(), "SimpleCharmap", "_", QCoreApplication::applicationDirPath() + "/translations"))
        a.installTranslator(&translator);
    else {
        if (translator.load(QLocale::English, "SimpleCharmap", "_", QCoreApplication::applicationDirPath() + "/translations"))
            a.installTranslator(&translator);
        else {
            QMessageBox::critical(nullptr, "Translation files not found",
                                  "Unable to find neither your language (" +
                                  QLocale::system().nativeLanguageName() +
                                  ") translation file nor the default english one. "
                                  "Your installation may be corrupt.");
            exit(-1);
        }
    }

    QFont font;
    font.setStyleStrategy(QFont::NoFontMerging);
    QApplication::setFont(font, "CharTableView");

    MainWindow w;
    w.show();

    splash.finish(&w);

    return a.exec();
}
