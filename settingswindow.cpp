#include "settingswindow.h"
#include "ui_settingswindow.h"


settingswindow::settingswindow(QWidget *parent) :
    QWidget(parent),
    settings(QSettings::IniFormat, QSettings::SystemScope, QStringLiteral(APP_ORGANIZATION_NAME), QStringLiteral(APP_PROJECT_NAME)),
    ui(new Ui::settingswindow)
{
    ui->setupUi(this);
    loadSettings();
}

settingswindow::~settingswindow()
{
    delete ui;
}

void settingswindow::loadSettings()
{
//    QSettings settings(settingsFileName, settingsFileFormat);
//    settings.sync();
//    QStringList Groups = settings.childGroups();
//    foreach (QString group, Groups) {
//        QTreeWidgetItem *wi = new QTreeWidgetItem(ui->treeWidget);
//        wi->setForeground(0, QColor(255,255,255));
//        wi->setText(0, group);
//        settings.beginGroup(group);
//        QStringList List = settings.allKeys();
//        foreach (QString item, List) {
//            QTreeWidgetItem *wi2 = new QTreeWidgetItem(wi);
//            wi2->setForeground(0, QColor(180,180,180));
//            wi2->setText(0, item);
//        }
//        settings.endGroup();
//    }
//    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, QStringLiteral(APP_ORGANIZATION_NAME), QStringLiteral(APP_PROJECT_NAME));

    QStorageInfo storage = QStorageInfo::root();
//    QString downloadPath = settings.value("Download/downloadPath", QCoreApplication::applicationDirPath()).toString();
    QString downloadPath = settings.value("Download/downloadPath", storage.rootPath()).toString();

    int debugLevel = settings.value("Debug/Level", 1).toInt();
    int currentLanguage = settings.value("Language", 0).toInt();
    int downLimit = settings.value("libtorrent/download_rate_limit", 0).toInt();
    int upLimit = settings.value("libtorrent/upload_rate_limit", 0).toInt();

    ui->txtDownloadPath->setText(downloadPath);
    qDebug() << "download path " << downloadPath;

    ui->cmbDebugLevel->setCurrentIndex(debugLevel);
    qDebug() << "debug level " << ui->cmbDebugLevel->currentText();

    ui->cmbLanguage->setCurrentIndex(currentLanguage);
    qDebug() << "current language " << ui->cmbLanguage->currentText();

    ui->numLimitDown->setValue(downLimit);
    ui->numLimitUp->setValue(upLimit);
    qDebug() << QString("limits: download %0 MB/s, upload %1 MB/s ").arg(downLimit).arg(upLimit);

    qInfo("settings loaded and applied");
}

void settingswindow::saveSettings()
{
//    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, QStringLiteral(APP_ORGANIZATION_NAME), QStringLiteral(APP_PROJECT_NAME));

    settings.setValue("Language", ui->cmbLanguage->currentIndex());
    qDebug() << "current language " << ui->cmbLanguage->currentText();

    settings.beginGroup("Download");
    settings.setValue("downloadPath", ui->txtDownloadPath->text());
    qDebug() << "download path " << ui->txtDownloadPath->text();
    settings.endGroup();

    settings.setValue("Debug/Level", ui->cmbDebugLevel->currentIndex());
    qDebug() << "debug level " << ui->cmbDebugLevel->currentText();

    settings.beginGroup("libtorrent");
    settings.setValue("download_rate_limit", ui->numLimitDown->value());
    settings.setValue("upload_rate_limit", ui->numLimitUp->value());
    settings.endGroup();

    qInfo("settings saved");
}

void settingswindow::setLastBrowsedDir(const QString &path)
{
//    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, QStringLiteral(APP_ORGANIZATION_NAME), QStringLiteral(APP_PROJECT_NAME));
    settings.setValue("Download/lastBrowsedDir", path);
}

QString settingswindow::getLastBrowsedDir()
{
//    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, QStringLiteral(APP_ORGANIZATION_NAME), QStringLiteral(APP_PROJECT_NAME));
    return settings.value("Download/lastBrowsedDir", QCoreApplication::applicationDirPath()).toString();
}

QString settingswindow::getDownloadPath() const
{
    return ui->txtDownloadPath->text();
}

void settingswindow::on_btnSave_released()
{
//    QSettings settings(QSettings::IniFormat, QSettings::SystemScope, QStringLiteral(APP_ORGANIZATION_NAME), QStringLiteral(APP_PROJECT_NAME));
    int savedLanguage = settings.value("Language", 0).toInt();
    int savedDownLimit = settings.value("libtorrent/download_rate_limit", 0).toInt();
    int savedUpLimit = settings.value("libtorrent/upload_rate_limit", 0).toInt();

    bool needRestart = (ui->cmbLanguage->currentIndex() != savedLanguage);
    bool needRefreshTorrentSettings = ( (ui->numLimitDown->value() != savedDownLimit) || (ui->numLimitUp->value() != savedUpLimit));

    saveSettings();

    // send message settings saved to user
    emit sendMessageToStatusBar(QString("Settings saved"));
    qInfo("Settings saved");

    if (needRestart) {
        qDebug("need restart");
        QMessageBox mbox;
        QString msg = QString("<center>Please restart Tsunami to apply new settings<br/>Do you want to restart now?</center>");
        mbox.setText(msg);
        mbox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );

        bool proceedWithRestart = false;

        switch (mbox.exec()) {
            case QMessageBox::Yes:
                proceedWithRestart = true;
                break;
            default:
                proceedWithRestart = false;
                break;
        }
        if (proceedWithRestart) {
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        } else {
            if (needRefreshTorrentSettings) {
                qDebug("send refresh to libtorrent");
                emit sendRefreshSettings();
            }
        }
    } else {
        qDebug("don't need restart");
        if (needRefreshTorrentSettings) {
            qDebug("send refresh to libtorrent");
            emit sendRefreshSettings();
        }
    }
//    saveSettings();
}

void settingswindow::on_btnCancel_released()
{
    loadSettings();
}

int settingswindow::getDebugLevelIndex() const
{
    return ui->cmbDebugLevel->currentIndex();
}

int settingswindow::getCurrentLanguageIndex() const
{
    return ui->cmbLanguage->currentIndex();
}

void settingswindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange)
    {
        qDebug("received QEvent::LanguageChange, retranslateUi");
        ui->retranslateUi(this);
        loadSettings();
    }
}

void settingswindow::on_btnFolder_released()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose download path"),
                                                 ui->txtDownloadPath->text(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui->txtDownloadPath->setText(dir);
    }
}
