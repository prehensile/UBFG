#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QElapsedTimer>
#include <QDirIterator>
#include <QWhatsThis>
#include <QTextCodec>
#include <QFileDialog>
#include <QSettings>
#include <QColorDialog>
#include <QFontDatabase>
#include <QDebug>

static QStringList _check_for_ttf(const QString &pt, long timeout = 500) {
    QDir dr(pt);
    QStringList filters, r;
    filters << "*.ttf" << "*.TTF" << "*.otf" << "*.OTF";
    dr.setNameFilters(filters);
    QElapsedTimer elps;
    elps.start();
    QStringList entryList = dr.entryList();
    for(int i = 0; i < entryList.size(); ++i) {
        r.append(pt+"/"+entryList.at(i));
        if (elps.elapsed() > timeout)
            break;
    }
    return  r;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // build font list from current directory and subdirs:
    QFontDatabase fdb;
    QDir currentDir;
    QStringList _fnts = _check_for_ttf(currentDir.absolutePath());
    QDirIterator directories(currentDir.absolutePath(), QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QElapsedTimer elps;
    elps.start();
    while(directories.hasNext() && elps.elapsed() < 1000) { // 1s throttle
        directories.next();
        _fnts += _check_for_ttf(directories.filePath());
    }
    if (_fnts.size()) {
        for(int i = 0; i < _fnts.size(); ++i) {
            fdb.addApplicationFont(_fnts.at(i));
        }
        qDebug() << "Added" << _fnts.size() << "external fonts";
    }

    ui->setupUi(this);

    ui->statusBar->showMessage("2.0 / © KomSoft Oprogramowanie 2015-2018");

    connect(ui->listOfFonts, &QListWidget::currentRowChanged, [=](int currentRow){
        ui->pushButtonUpd->setEnabled(currentRow!=-1);
        ui->pushButtonRem->setEnabled(currentRow!=-1);
    });

    ui->bruteForce->hide();
    thread = new FontRender(ui);
    thread->exporting = false;
    connect(ui->updateButton, SIGNAL(clicked()), thread, SLOT(run()));
    qRegisterMetaType<QImage>("QImage");
    connect(thread, SIGNAL(renderedImage(QImage)), ui->widget, SLOT(updatePixmap(QImage)));
    ui->encoding->addItem("UNICODE");
    QList<QByteArray> availableCodecs = QTextCodec::availableCodecs();
    qSort(availableCodecs);
    for(int i = 0; i < availableCodecs.count(); i++)
    {
        ui->encoding->addItem(availableCodecs.at(i));
    }
    ui->outDir->setText(QDir::homePath());
    readSettings();
    thread->run();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        e->accept();
        break;
    default:
        QWidget::changeEvent(e);
        break;
    }
}

void MainWindow::addFont()
{
    QString s = ui->fontComboBox->currentFont().family() +
                QString(", ") +
                ui->spinFontSize->text() + QString(" ") +
                ui->comboPtPx->currentText() +
                (ui->checkFontSmoothing->isChecked()?QString(" smooth"):"") +
                (ui->checkFontBold->isChecked()?QString(" b"):"") +
                (ui->checkFontItalic->isChecked()?QString(" i"):"")+
                (QString(" %1sample").arg(ui->spinSample->value()));
    QListWidgetItem *item = new QListWidgetItem(ui->listOfFonts);
    item->setText(s);
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
}

void MainWindow::updateFont()
{
    removeFont();
    addFont();
}

void MainWindow::removeFont()
{
    ui->listOfFonts->takeItem(ui->listOfFonts->row(ui->listOfFonts->currentItem()));
}

void MainWindow::getFolder()
{
    ui->outDir->setText(QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 ui->outDir->text(),
                                                 QFileDialog::ShowDirsOnly));
}

void MainWindow::exportFont()
{
    thread->exporting = true;
    homeDir = ui->outDir->text();
    outFile = ui->outFile->text();
    thread->run();
}
void MainWindow::bruteForce()
{
    ui->bruteForce->setText("Please, wait...");

    thread->run();
    ui->bruteForce->setText("BRUTE-FORCE");
}

void MainWindow::readSettings()
{
    QSettings settings("Sciprum Plus", "UBFG");

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(800, 600)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    projectDir = settings.value("projectDir", QDir::homePath()).toString();
    homeDir = settings.value("homeDir", QDir::homePath()).toString();
    outFile = settings.value("outFile", ui->outFile->text()).toString();
    ui->outDir->setText(settings.value("outDir", ui->outDir->text()).toString());
    project = settings.value("project", "project.bfg").toString();

    // last used:
    ui->plainTextEdit->setPlainText(settings.value("setup/charList", ui->plainTextEdit->toPlainText()).toString());
    ui->sortOrder->setCurrentIndex(settings.value("setup/sortOrder", ui->sortOrder->currentIndex()).toInt());
    ui->borderTop->setValue(settings.value("setup/borderTop", ui->borderTop->value()).toInt());
    ui->borderLeft->setValue(settings.value("setup/borderLeft", ui->borderLeft->value()).toInt());
    ui->borderRight->setValue(settings.value("setup/borderRight", ui->borderRight->value()).toInt());
    ui->borderBottom->setValue(settings.value("setup/borderBottom", ui->borderBottom->value()).toInt());
    ui->enableDebug->setChecked(settings.value("setup/enableDebug", false).toBool());
    ui->zoomLevel->setCurrentIndex(settings.value("setup/zoom", ui->zoomLevel->currentIndex()).toInt());
    ui->checkFontSmoothing->setChecked(settings.value("setup/fontSmoothing", false).toBool());
    ui->checkFontItalic->setChecked(settings.value("setup/fontItalic", false).toBool());
    ui->checkFontBold->setChecked(settings.value("setup/fontBold", false).toBool());
    ui->spinSample->setValue(settings.value("setup/multisample", ui->spinSample->value()).toInt());

    ui->outputFormat->setCurrentIndex(settings.value("export/outputFormat", ui->outputFormat->currentIndex()).toInt());
    ui->outFormat->setCurrentIndex(settings.value("export/outFormat", ui->outFormat->currentIndex()).toInt());
    ui->encoding->setCurrentIndex(settings.value("export/encoding", ui->encoding->currentIndex()).toInt());
    ui->saveImageInside->setChecked(settings.value("export/embedImages", false).toBool());

    settings.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings settings("Sciprum Plus", "UBFG");

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("projectDir", projectDir);
    settings.setValue("homeDir", homeDir);
    settings.setValue("outFile", outFile);
    settings.setValue("outDir", ui->outDir->text());

    // last used:
    settings.setValue("setup/charList", ui->plainTextEdit->toPlainText());
    settings.setValue("setup/sortOrder", ui->sortOrder->currentIndex());
    settings.setValue("setup/borderTop", ui->borderTop->value());
    settings.setValue("setup/borderLeft", ui->borderLeft->value());
    settings.setValue("setup/borderRight", ui->borderRight->value());
    settings.setValue("setup/borderBottom", ui->borderBottom->value());
    settings.setValue("setup/enableDebug", ui->enableDebug->isChecked());
    settings.setValue("setup/zoom", ui->zoomLevel->currentIndex());
    settings.setValue("setup/fontSmoothing", ui->checkFontSmoothing->isChecked());
    settings.setValue("setup/fontBold", ui->checkFontBold->isChecked());
    settings.setValue("setup/fontItalic", ui->checkFontItalic->isChecked());
    settings.setValue("setup/multisample", ui->spinSample->value());

    settings.setValue("export/outputFormat", ui->outputFormat->currentIndex());
    settings.setValue("export/outFormat", ui->outFormat->currentIndex());
    settings.setValue("export/encoding", ui->encoding->currentIndex());
    settings.setValue("export/embedImages", ui->saveImageInside->isChecked());

    settings.endGroup();
}

void MainWindow::loadProject()
{

    const QString _chsets[] = {
        QStringLiteral(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+\\/():;%&`'*#$=[]@^{}_~\"><"),
        QStringLiteral(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+\\/():;%&`'*#$=[]@^{}_~\"><0x2013 0x20140xAB0xBB0x201C0x201D|0x4300x4310x4320x4330x4340x4350x4510x4360x4370x4380x4390x43A0x43B0x43C0x43D0x43E0x43F0x4400x4410x4420x4430x4440x4450x4460x4470x4480x4490x44A0x44B0x44C0x44D0x44E0x44F0x4100x4110x4120x4130x4140x4150x4010x4160x4170x4180x4190x41A0x41B0x41C0x41D0x41E0x41F0x4200x4210x4220x4230x4240x4250x4270x4260x4280x4290x42A0x42B0x42C0x42D0x42E0x42F0xA90xAE"),
        QStringLiteral("0123456789")
    };

    QString file = QFileDialog::getOpenFileName(this, tr("Open Project"),
                                                 projectDir,
                                                 tr("Projects (*.bfg)"));
    if(file.length())
    {
        QFileInfo fi(file);
        projectDir = fi.path();
        project = fi.fileName();
        QSettings settings(file, QSettings::IniFormat, this);

        ui->plainTextEdit->setPlainText(settings.value("charList", _chsets[0]).toString());
        ui->trim->setChecked(settings.value("trim", true).toBool());
        ui->borderTop->setValue(settings.value("borderTop", 0).toInt());
        ui->borderBottom->setValue(settings.value("borderBottom", 1).toInt());
        ui->borderLeft->setValue(settings.value("borderLeft", 0).toInt());
        ui->borderRight->setValue(settings.value("borderRight", 1).toInt());
        ui->merge->setChecked(settings.value("merge", true).toBool());
        ui->mergeBF->setChecked(settings.value("mergeBF", true).toBool());
        ui->textureW->setValue(settings.value("textureW", 512).toInt());
        ui->textureH->setValue(settings.value("textureH", 512).toInt());
        ui->distanceField->setChecked(settings.value("distanceField", false).toBool());
        ui->comboHeuristic->setCurrentIndex(settings.value("heuristic", 1).toInt());
        ui->sortOrder->setCurrentIndex(settings.value("sortOrder", 2).toInt());
        ui->outputFormat->setCurrentIndex(settings.value("outFormat", 0).toInt());
        //compatible with old format without UNICODE and with export indexes instead of text
        int encodingInt = settings.value("encoding", 0).toInt();
        QString encodingStr = settings.value("encoding", 0).toString();
        if(QString::number(encodingInt) == encodingStr)
            ui->encoding->setCurrentIndex(encodingInt + 1);
        else
            ui->encoding->setCurrentIndex(ui->encoding->findText(encodingStr));
        ui->transparent->setChecked(settings.value("transparent", true).toBool());
        ui->outDir->setText(settings.value("outDir", homeDir).toString());
        ui->outFile->setText(settings.value("outFile", outFile).toString());
        ui->exportKerning->setChecked(settings.value("kerning", true).toBool());
        ui->saveImageInside->setChecked(settings.value("imageInXML", false).toBool());
        int size = settings.beginReadArray("fonts");
        ui->listOfFonts->clear();
        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);
            QListWidgetItem *item = new QListWidgetItem(ui->listOfFonts);
            item->setText(settings.value("font").toString());
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        }
        settings.endArray();
        thread->run();
    }
}

void MainWindow::saveProject()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save Project"),
                                                 projectDir+"/"+project,
                                                 tr("Projects (*.bfg)"));
    if(file.length())
    {
        QFileInfo fi(file);
        projectDir = fi.path();
        project = fi.fileName();
        QSettings settings(file, QSettings::IniFormat, this);

        settings.setValue("charList", ui->plainTextEdit->toPlainText());
        settings.setValue("trim", ui->trim->isChecked());
        settings.setValue("borderTop", ui->borderTop->value());
        settings.setValue("borderBottom", ui->borderBottom->value());
        settings.setValue("borderLeft", ui->borderLeft->value());
        settings.setValue("borderRight", ui->borderRight->value());
        settings.setValue("merge", ui->merge->isChecked());
        settings.setValue("mergeBF", ui->mergeBF->isChecked());
        settings.setValue("textureW", ui->textureW->value());
        settings.setValue("textureH", ui->textureH->value());
        settings.setValue("distanceField", ui->distanceField->isChecked());
        settings.setValue("heuristic", ui->comboHeuristic->currentIndex());
        settings.setValue("sortOrder", ui->sortOrder->currentIndex());
        settings.setValue("outFormat", ui->outputFormat->currentIndex());
        settings.setValue("encoding", ui->encoding->currentText());
        settings.setValue("transparent", ui->transparent->isChecked());
        settings.setValue("outDir", ui->outDir->text());
        settings.setValue("outFile", ui->outFile->text());
        settings.setValue("kerning", ui->exportKerning->isChecked());
        settings.setValue("imageIn", ui->saveImageInside->isChecked());
        settings.beginWriteArray("fonts");
        for (int i = 0; i < ui->listOfFonts->count(); ++i) {
            settings.setArrayIndex(i);
            settings.setValue("font", ui->listOfFonts->item(i)->text());
        }
        settings.endArray();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::bitDepthChanged(int index) {
    if (index == 1)
        ui->transparent->setDisabled(true);
    else
        ui->transparent->setDisabled(false);
}

void MainWindow::changeFontColor() {
    QPalette buttonPal = ui->fontColor->palette();
    QColor selectedColor = QColorDialog::getColor(buttonPal.brush(QPalette::Button).color(), NULL, "Font Color");
    QBrush brush(selectedColor);
    brush.setStyle(Qt::SolidPattern);
    buttonPal.setBrush(QPalette::Active, QPalette::Button, brush);
    buttonPal.setBrush(QPalette::Inactive, QPalette::Button, brush);
    buttonPal.setBrush(QPalette::Disabled, QPalette::Button, brush);
    ui->fontColor->setPalette(buttonPal);
}

void MainWindow::changeBkgColor() {
    QPalette buttonPal = ui->backgroundColor->palette();
    QColor selectedColor = QColorDialog::getColor(buttonPal.brush(QPalette::Button).color(), NULL, "Font Color");
    QBrush brush(selectedColor);
    brush.setStyle(Qt::SolidPattern);
    buttonPal.setBrush(QPalette::Active, QPalette::Button, brush);
    buttonPal.setBrush(QPalette::Inactive, QPalette::Button, brush);
    buttonPal.setBrush(QPalette::Disabled, QPalette::Button, brush);
    ui->backgroundColor->setPalette(buttonPal);
}
