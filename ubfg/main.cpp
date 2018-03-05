#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#   include <QtWidgets/QApplication>
#else
#   include <QtGui/QApplication>
#endif

#include <QStandardPaths>
#include <QTextStream>
#include <QDateTime>

#include "mainwindow.h"

// Redirect qDebug() to the text file.
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
   Q_UNUSED(context);

   QString dt = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
   QString txt = QString("[%1] ").arg(dt);

   switch (type)
   {
      case QtInfoMsg:
         txt += QString("{Info} \t %1").arg(msg);
         break;
      case QtDebugMsg:
         txt += QString("{Debug} \t\t %1").arg(msg);
         break;
      case QtWarningMsg:
         txt += QString("{Warning} \t %1").arg(msg);
         break;
      case QtCriticalMsg:
         txt += QString("{Critical} \t %1").arg(msg);
         break;
      case QtFatalMsg:
         txt += QString("{Fatal} \t\t %1").arg(msg);
         abort();
         break;
   }

   QFile outFile(APP_LOG);
   if (!outFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
     outFile.setFileName(APP_LOG_GLOB);
     if (!outFile.open(QIODevice::WriteOnly | QIODevice::Append))
	 return;
   }
   QTextStream textStream(&outFile);
   textStream << txt << endl;
}

// It's safe to call this function on any platform.
// It will only have an effect on the Mac.
static void set_smaller_text_osx(QWidget *w)
{
    Q_ASSERT(w != 0);

    // By default, none of these size attributes are set.
    // If any has been set explicitly, we'll leave the widget alone.
    if (!w->testAttribute(Qt::WA_MacMiniSize) &&
        !w->testAttribute(Qt::WA_MacSmallSize) &&
        !w->testAttribute(Qt::WA_MacNormalSize) &&
        !w->testAttribute(Qt::WA_MacVariableSize))
    {
        // Is the widget is one of a number of types whose default
        // text size is too large?
        if (w->inherits("QLabel") ||
            w->inherits("QLineEdit") ||
            w->inherits("QComboBox") ||
            w->inherits("QCheckBox") ||
            (w->inherits("QPushButton") && ((QPushButton*)w)->icon().isNull()) ||
            w->inherits("QRadioButton") ||
            w->inherits("QAbstractItemView"))
            // Others could be added here...
        {
            // make the text the 'normal' size
            w->setAttribute(Qt::WA_MacSmallSize);
        }
    }
}

class NativeLookingApplication : public QApplication
{
    public:
        NativeLookingApplication(int &argc, char **argv) : QApplication(argc, argv) { }

        virtual bool notify(QObject *receiver, QEvent *event)
        {
            // I tried this fix and had problems that the popup menu of QComboBox
            // had an incorrect size when opening. However using QEvent::PolishRequest
            // as hook instead of QEvent::Polish fixed this issue.
            if (event->type() == QEvent::Polish &&
                receiver &&
                receiver->isWidgetType())
            {
                set_smaller_text_osx(static_cast<QWidget *>(receiver));
            }

            return QApplication::notify(receiver, event);
        }
};

int main(int argc, char *argv[])
{
    NativeLookingApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
