//--------------------------------------------------------------------------+
//                                                                          *
//               "61 tane okuyucu seçimli Kuran dinletici"                  *
//              Copyright(C) 2021, FB <ironic{at}yaani.com>                 *
//                 https://gitlab.com/fbostanci/furkan                      *
//                            Furkan v1.0                                   *
//                              GPLv3                                       *
//                                                                          *
//--------------------------------------------------------------------------+
//                                                                          *
//    This program is free software: you can redistribute it and/or modify  *
//    it under the terms of the GNU General Public License as published by  *
//    the Free Software Foundation, either version 3 of the License, or     *
//    (at your option) any later version.                                   *
//                                                                          *
//    This program is distributed in the hope that it will be useful,       *
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        *
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
//    GNU General Public License for more details.                          *
//                                                                          *
//    You should have received a copy of the GNU General Public License     *
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.*
//                                                                          *
//--------------------------------------------------------------------------+

#include "furkan.h"
#include "ui_furkan.h"
#include <QMessageBox>
#include <QTime>
#include <QMap>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSlider>
#include <QNetworkRequest>
#include <QUrl>
#include <QRegularExpression>


furkan::furkan(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::furkan>())
    , networkManager(std::make_unique<QNetworkAccessManager>(this))
{
    ui->setupUi(this);
    this->setWindowTitle("Furkan");
    this->setWindowIcon(QIcon(":/images/furkan.png"));
    this->setFixedSize(444, 262);

    QPixmap pm(":/images/furkan.png");
    ui->label_sg->setPixmap(pm);

    ui->comboBox_ok->setCurrentIndex(16);

    // İptal düğmesi
    setupButton(ui->pushButton_ip, ":/images/iptal.png", QSize(40, 40));
    ui->pushButton_ip->setEnabled(false);

    setupButton(ui->pushButton_pv, ":/images/geri.png", QSize(40, 40));
    setupButton(ui->pushButton_nx, ":/images/ileri.png", QSize(40, 40));
    // Dinlet düğmesi
    dinletD();

    // Hakkında düğmesi
    setupButton(ui->pushButton_hk, ":/images/bilgi.png", QSize(16, 16));

    oynatici = std::make_unique<QMediaPlayer>(this);
    audioOutput = std::make_unique<QAudioOutput>(this);
    oynatici->setAudioOutput(audioOutput.get());
    audioOutput->setVolume(0.8);
    ui->slider_se->setValue(80);
    ui->label_lcd->display(80);
    ui->slider_sr->setEnabled(false);

    connect(oynatici.get(), &QMediaPlayer::positionChanged, this, &furkan::setPosition);
    connect(oynatici.get(), &QMediaPlayer::durationChanged, this, &furkan::setDuration);
    connect(ui->slider_se, &QSlider::valueChanged, this, [this](int value) { setVolume(value); });
    connect(oynatici.get(), &QMediaPlayer::mediaStatusChanged, this, &furkan::durumDegisti);
    connect(ui->slider_sr, &QSlider::sliderMoved, this, [this](int value) { oynatici->setPosition(value * 1000); });

    checkInternetConnection();
}

furkan::~furkan()
{

}

void furkan::checkInternetConnection()
{
    QNetworkRequest request(QUrl("https://www.google.com"));
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onNetworkReplyFinished(reply);
    });
}

void furkan::onNetworkReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        // Bağlantı başarılı
        reply->deleteLater();
    } else {
        // Hata oluştu
        showError("İnternet erişimi algılanamadı. Lütfen bağlantınızı denetleyiniz.");
    }
}

void furkan::showError(const QString &message)
{
    QMessageBox::critical(this, "Bağlantı Hatası", message);
}

void furkan::setupButton(QPushButton* button, const QString& iconPath, const QSize& iconSize)
{
    button->setIcon(QIcon(iconPath));
    button->setIconSize(iconSize);
    button->setStyleSheet("QPushButton {border-style: outset; border-width: 0px;}");
}

void furkan::setPosition(qint64 i)
{
    ui->slider_sr->setValue(i / 1000);
    ui->label_gs->setText(sureyiAyarla(i));
}

void furkan::setVolume(int ses)
{
    audioOutput->setVolume(ses / 100.0);
    ui->label_lcd->display(ses);
}

QString furkan::sureyiAyarla(qint64 playtime)
{
    int h, m, s;

    playtime /= 1000;
    h = playtime / 3600;
    m = (playtime - h * 3600) / 60;
    s = playtime - h * 3600 - m * 60;
    QString format = h ? "hh:mm:ss" : "mm:ss";

    QTime time(h, m, s);
    return time.toString(format);
}

void furkan::setDuration(qint64 playtime)
{
    ui->slider_sr->setMaximum(oynatici->duration() / 1000);
    ui->label_dr->setText(sureyiAyarla(playtime));
}

void furkan::on_pushButton_ip_clicked()
{
    oynatici->stop();
    dinletD();
    ui->pushButton_ip->setDisabled(true);
    ui->slider_sr->setEnabled(false);
}

void furkan::on_pushButton_pv_clicked()
{
    ui->slider_sr->setEnabled(true);
    int index = ui->comboBox_sr->currentIndex();
    index--;
    if(index < 0 ) index = 113;
    ui->comboBox_sr->setCurrentIndex(index);
    secimYap();
    ui->pushButton_ip->setEnabled(true);
    duraklatD();
    oynatici->play();
}

void furkan::on_pushButton_nx_clicked()
{
    ui->slider_sr->setEnabled(true);
    int index = ui->comboBox_sr->currentIndex();
    index++;
    if(index > 113) index = 0;
    ui->comboBox_sr->setCurrentIndex(index);
    secimYap();
    ui->pushButton_ip->setEnabled(true);
    duraklatD();
    oynatici->play();
}

void furkan::on_pushButton_di_clicked()
{
    ui->slider_sr->setEnabled(true);
    switch (oynatici->playbackState()) {
    case QMediaPlayer::PlayingState:
        oynatici->pause();
        dinletD();
        ui->pushButton_ip->setEnabled(true);
        break;

    case QMediaPlayer::PausedState:
        oynatici->play();
        duraklatD();
        ui->pushButton_ip->setEnabled(true);
        break;

    default:
        secimYap();
        oynatici->play();
        duraklatD();
        ui->pushButton_ip->setEnabled(true);
        break;
    }
}

void furkan::durumDegisti(QMediaPlayer::MediaStatus durum)
{
    if (durum == QMediaPlayer::EndOfMedia)
    {
        dinletD();
        ui->pushButton_ip->setEnabled(false);
        ui->slider_sr->setEnabled(false);
    }
}

void furkan::on_pushButton_hk_clicked()
{
    QMessageBox::about(this, "Furkan v1.0",
                       "<h4>61 tane okuyucu seçimli Kuran dinletici</h4>"
                       "Copyright (c) 2021, FB <br><br>"
                       "Uygulama sayfası : <a href=\"https://gitlab.com/fbostanci/furkan\">Gitlab.com</a><br>"
                       "Lisans : <a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">GPLv3</a><br>"
                       "İçerik sağlayıcı : <a href=\"https://quranicaudio.com\">quranicaudio.com</a>");
}

void furkan::secimYap()
{
    static const QMap<QString, QString> okuyucuMap {
        {"Abdullah Awad al-Juhani", "abdullaah_3awwaad_al-juhaynee"},
        {"Ali Abdur-Rahman al-Huthaify", "huthayfi"},
        {"AbdulBaset AbdulSamad 1", "abdul_basit_murattal"},
        {"AbdulBaset AbdulSamad 2", "abdulbaset_mujawwad"},
        {"Abdullah Ali Jabir", "abdullaah_alee_jaabir_studio"},
        {"Abdullah Ali Jabir", "abdullaah_alee_jaabir_studio"},
        {"AbdulKareem Al Hazmi", "abdulkareem_al_hazmi"},
        {"Ahmad al-Huthaify", "ahmad_alhuthayfi"},
        {"AbdulWadood Haneef", "abdul_wadood_haneef_rare"},
        {"Ali Hajjaj Alsouasi", "ali_hajjaj_alsouasi"},
        {"Abdullah Basfar", "abdullaah_basfar"},
        {"AbdulMuhsin al-Qasim", "abdul_muhsin_alqasim"},
        {"AbdulAzeez al-Ahmad", "abdulazeez_al-ahmad"},
        {"AbdulWadud Haneef", "abdulwadood_haneef"},
        {"Al-Hussayni Al-Azazy", "alhusaynee_al3azazee_with_children"},
        {"Abdullah Khayat", "khayat"},
        {"Abdul-Munim Abdul-Mubdi", "abdulmun3im_abdulmubdi2"},
        {"Abu Bakr al-Shatri", "abu_bakr_ash-shatri_tarawee7"},
        {"Ahmad Nauina", "ahmad_nauina"},
        {"Abdur-Rahman as-Sudais", "abdurrahmaan_as-sudays"},
        {"AbdulBari ath-Thubaity", "thubaity"},
        {"Abdullah Ali Jabir 2", "abdullaah_alee_jaabir"},
        {"Aziz Alili", "aziz_alili"},
        {"Adel Kalbani", "adel_kalbani"},
        {"Abdur-Rashid Sufi", "abdurrashid_sufi"},
        {"Abdullah Matroud", "abdullah_matroud"},
        {"Akram Al-Alaqmi", "akram_al_alaqmi"},
        {"Bandar Baleela", "bandar_baleela"},
        {"Fares Abbad", "fares"},
        {"Hani ar-Rifai", "rifai"},
        {"Ibrahim Al-Jibrin", "jibreen"},
        {"Imad Zuhair Hafez", "imad_zuhair_hafez"},
        {"Ibrahim Al Akhdar", "ibrahim_al_akhdar"},
        {"Idrees Abkar", "idrees_akbar"},
        {"Khalid al-Qahtani", "khaalid_al-qahtaanee"},
        {"Mishari Rashid al-Afasy", "mishaari_raashid_al_3afaasee"},
        {"Muhammad Siddiq al-Minshawi 1", "muhammad_siddeeq_al-minshaawee"},
        {"Muhammad Jibreel", "muhammad_jibreel/complete"},
        {"Muhammad al-Mehysni",  "mehysni"},
        {"Muhammad Siddiq al-Minshawi 2", "minshawi_mujawwad"},
        {"Muhammad al-Luhaidan", "muhammad_alhaidan"},
        {"Maher al-Muaiqly", "maher_256"},
        {"Muhammad Abdul-Kareem", "muhammad_abdulkareem"},
        {"Mustafa al-Azawi", "mustafa_al3azzawi"},
        {"Muhammad Hassan", "mu7ammad_7assan"},
        {"Mostafa Ismaeel", "mostafa_ismaeel"},
        {"Mohammad Ismaeel Al-Muqaddim", "mohammad_ismaeel_almuqaddim"},
        {"Muhammad Ayyoob", "muhammad_ayyoob_hq"},
        {"Masjid Quba", "masjid_quba_1434"},
        {"Mahmoud Khaleel Al-Husary",  "mahmood_khaleel_al-husaree_iza3a"},
        {"Mahmood Ali Al-Bana",  "mahmood_ali_albana"},
        {"Nabil ar-Rifai", "nabil_rifa3i"},
        {"Nasser Al Qatami", "nasser_bin_ali_alqatami"},
        {"Saud ash-Shuraym", "sa3ood_al-shuraym"},
        {"Saad al-Ghamdi",  "sa3d_al-ghaamidi/complete"},
        {"Sahl Yasin", "sahl_yaaseen"},
        {"Salah Bukhatir", "salaah_bukhaatir"},
        {"Sudais and Shuraym", "sodais_and_shuraim"},
        {"Salah al-Budair", "salahbudair"},
        {"Salah Al-Hashim", "salah_alhashim"},
        {"Wadee Hammadi Al Yamani", "wadee_hammadi_al-yamani"},
        {"Yasser ad-Dussary", "yasser_ad-dussary"}
    };

    static const QRegularExpression dashRegex("-");
    QString sureG = ui->comboBox_sr->currentText();
    QStringList sure = sureG.split(QRegularExpression(dashRegex));
    QString okuyucu = ui->comboBox_ok->currentText();

    QString oky = okuyucuMap.value(okuyucu, "");
    QString dinlet = "https://download.quranicaudio.com/quran/" + oky + "/" + sure.at(0) + ".mp3";
    oynatici->setSource(QUrl(dinlet));
}

void furkan::duraklatD()
{
    setupButton(ui->pushButton_di, ":/images/duraklat.png", QSize(40, 40));
}

void furkan::dinletD()
{
    setupButton(ui->pushButton_di, ":/images/dinlet.png", QSize(40, 40));
}
