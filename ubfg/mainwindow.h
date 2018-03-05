#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGlobal>

#ifdef Q_OS_MAC
# define APP_INI "UBFG.ini"
# define APP_LOG "UBFG.log" // log created in current directory
# define APP_LOG_GLOB (QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).arg(APP_LOG))
#else
# define APP_INI "UBFG.ini"
# define APP_LOG "UBFG.log"
# define APP_LOG_GLOB "UBFG.log"
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#   include <QtWidgets/QMainWindow>
#else
#   include <QMainWindow>
#endif

#include "fontrender.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
    void writeSettings();
    void readSettings();

private:
    Ui::MainWindow *ui;
    FontRender *thread;
    QString projectDir;
    QString project;
    QString homeDir, outFile;

private Q_SLOTS:
    void addFont();
    void updateFont();
    void removeFont();
    void bruteForce();
    void getFolder();
    void exportFont();
    void saveProject();
    void loadProject();
    void bitDepthChanged(int index);
    void changeFontColor();
    void changeBkgColor();
    void zoom100() { ui->zoomLevel->setCurrentIndex(0); }
    void zoom200() { ui->zoomLevel->setCurrentIndex(1); }
    void zoom400() { ui->zoomLevel->setCurrentIndex(2); }
    void zoom800() { ui->zoomLevel->setCurrentIndex(3); }
};

#endif // MAINWINDOW_H
