#include "furkan.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Tekil örnek denetimi için QSharedMemory kullanımı
    QSharedMemory sharedMemory("furkanQtInstance");

    // Paylaşılan bellek oluşturulmaya çalışılıyor
    if (!sharedMemory.create(1)) {
        QMessageBox msgBox;
        msgBox.setWindowIcon(QIcon(":/images/furkan.png"));
        msgBox.setWindowTitle("Furkan");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Uygulamanın başka bir örneği zaten çalışıyor.");
        msgBox.exec();

        return -1;
    }

    furkan mainWindow;
    mainWindow.show();

    return app.exec();
}
