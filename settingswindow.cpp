#include "settingswindow.h"
#include "ui_settingswindow.h"

QString settingswindow::settingsFileName = "tsunami.ini";
QSettings::Format settingswindow::settingsFileFormat = QSettings::IniFormat;

settingswindow::settingswindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingswindow)
{
    ui->setupUi(this);

    ui->txtDownloadPath->installEventFilter(this);

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
    QSettings settings(settingsFileName, settingsFileFormat);
    QString downloadPath = settings.value("Download/downloadPath", QCoreApplication::applicationDirPath()).toString();
    QString debugLevel = settings.value("Debug/Level", "Info").toString();
    setDownloadPath(downloadPath);
    setDebugLevel(debugLevel);
}

void settingswindow::saveSettings()
{
    QSettings settings(settingsFileName, settingsFileFormat);
    settings.beginGroup("Download");
    settings.setValue("downloadPath", getDownloadPath());
    settings.endGroup();
    settings.setValue("Debug/Level", getDebugLevel());
}

void settingswindow::setLastBrowsedDir(const QString &path)
{
    QSettings settings(settingsFileName, settingsFileFormat);
    settings.setValue("Download/lastBrowsedDir", path);
}

QString settingswindow::getLastBrowsedDir()
{
    QSettings settings(settingsFileName, settingsFileFormat);
    return settings.value("Download/lastBrowsedDir", QCoreApplication::applicationDirPath()).toString();
}

QString settingswindow::getDownloadPath() const
{
    return p_downloadPath;
}

void settingswindow::setDownloadPath(const QString &value)
{
    ui->txtDownloadPath->setText(value);
    p_downloadPath = value;
}

bool settingswindow::eventFilter(QObject *object, QEvent *event)
{
    if(object == ui->txtDownloadPath && event->type() == QEvent::MouseButtonPress) {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Choose download path"),
                                                     getDownloadPath(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty()) {
            setDownloadPath(dir);
        }
    }
    return false;
}

void settingswindow::on_btnSave_released()
{
    saveSettings();
}

void settingswindow::on_btnCancel_released()
{
    loadSettings();
}

QString settingswindow::getDebugLevel() const
{
    return p_debugLevel;
}

void settingswindow::setDebugLevel(const QString &value)
{
    ui->cmbDebugLevel->setCurrentText(value);
    p_debugLevel = value;
}
