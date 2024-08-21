#ifndef FURKAN_H
#define FURKAN_H

#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>

QT_BEGIN_NAMESPACE
namespace Ui { class furkan; }
QT_END_NAMESPACE

class furkan : public QMainWindow
{
    Q_OBJECT

public:
    explicit furkan(QWidget *parent = nullptr);
    ~furkan();

private:
    Ui::furkan *ui;
    QNetworkAccessManager *networkManager;
    QMediaPlayer *oynatici;
    QAudioOutput *audioOutput;

    void sureDegistir(int);
    void setupButton(QPushButton *button, const QString &iconPath, const QSize &iconSize = QSize());

    QString sureyiAyarla(qint64);
    void secimYap();
    void dinletD();
    void duraklatD();

private slots:
    void checkInternetConnection();
    void onNetworkReplyFinished(QNetworkReply *reply);
    void showError(const QString &message);
    void durumDegisti(QMediaPlayer::MediaStatus);
    void on_pushButton_di_clicked();
    void on_pushButton_ip_clicked();
    void on_pushButton_pv_clicked();
    void on_pushButton_nx_clicked();
    void on_pushButton_hk_clicked();
    void setPosition(qint64);
    void setDuration(qint64);
    void setVolume(int);
};
#endif // FURKAN_H
