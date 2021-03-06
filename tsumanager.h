#ifndef TSUMANAGER_H
#define TSUMANAGER_H

#include <QSharedPointer>
#include <QObject>
#include <QDebug>
#include <QTimer>

#include "libtorrent/session.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/torrent_handle.hpp"
#include "libtorrent/alert_types.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/write_resume_data.hpp"
#include "libtorrent/read_resume_data.hpp"

#include "tsuitem.h"
#include "tsuevents.h"

#include <fstream>      // std::ofstream

class tsuManager :public QObject
{
    Q_OBJECT
public:
    tsuManager();
//    explicit tsuManager(const lt::settings_pack & sp);
    ~tsuManager();
    static int outstanding_resume_data;

    void addItems(const QStringList &&items, const QString &path);

    QSharedPointer<lt::session> p_session;

signals:
    void updateFromSessionManager(const QVector<tsuEvents::tsuEvent> events);
    void addFromSessionManager(const tsuEvents::tsuEvent &event);
    void torrentDeleted(const std::string &hash);
//    void stopTimer();
    void finished();

public slots:
    void getCancelRequest(const std::string &hash, const bool deleteFilesToo);
    void getPauseRequest(const std::string &hash);
    void getResumeRequest(const std::string &hash);
    void refreshSettings();
    void startManager();
    void stopManager();

private:
    void setNotify();
    void loadSettings();
//    QTimer *timerUpdate;

private slots:
    void alertsHandler();
//    void postUpdates();
};

/*
#include <QObject>
#include <QTimer>
#include <QDebug>


#include "libtorrent/session.hpp"
#include "libtorrent/session_status.hpp"
#include "libtorrent/torrent_handle.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/torrent_status.hpp"
#include "libtorrent/alert_types.hpp"

namespace tsuEvents {
    class tsuEvent
    {
        public:
        tsuEvent()
        { }
        tsuEvent(const std::string &_hash, const QString &_name, const int &_downloaded = 0,
                 const int &_uploaded = 0, const int &_downRate = 0, const int &_upRate = 0,
                 const int &_size = 0, const int &_state = 0, const int &_percentage = 0):
            hash(_hash),
            name(_name),
            downloaded(_downloaded),
            uploaded(_uploaded),
            downloadRate(_downRate),
            uploadRate(_upRate),
            total_size(_size),
            state(_state),
            percentage(_percentage)
        { }
        ~tsuEvent()
        { }

            std::string hash ="";
            QString name ="";
            int downloaded = 0;
            int uploaded = 0;
            int downloadRate = 0;
            int uploadRate = 0;
            int total_size = 0;
            int state = 0;
            int percentage = 0;
    };
}

class Worker : public QObject
{
    Q_OBJECT
//    QThread workerThread;

public slots:
    void doWork(std::vector<libtorrent::alert*> alerts) {
//        std::vector<libtorrent::alert*> alerts;
//        p_session.get()->pop_alerts(&alerts);

    //    qDebug() << QString("processing %1 alerts:").arg(alerts.size());
        for (libtorrent::alert* a : alerts) {
            if (a == nullptr) continue;
    //        qDebug() << QString(" %1").arg(a->message().c_str());

            if (auto st = libtorrent::alert_cast<libtorrent::state_update_alert>(a)) {
                if (st == nullptr) continue;
                if (st->status.empty()) continue;
                for (libtorrent::torrent_status s : st->status) {
                    try {
                        tsuEvents::tsuEvent ev(s.info_hash.to_string(), QString::fromStdString(s.name), s.total_done, s.total_upload, s.download_rate, s.upload_rate, s.total_wanted, s.state, s.progress_ppm);
                        emit eventFromLibtorrent(ev);
                    } catch (std::exception &exc) {
                        qDebug() << QString("timerTick throws %1").arg(exc.what());
                    }
                }
            }

            if (auto aa = libtorrent::alert_cast<libtorrent::torrent_error_alert>(a)) {
                qDebug() << QString("torrent_error_alert %1").arg(QString::fromStdString(aa->message()));
            }

            if (auto aa = libtorrent::alert_cast<libtorrent::add_torrent_alert>(a)) {
                libtorrent::torrent_handle th = aa->handle;
                libtorrent::torrent_status st = th.status(libtorrent::torrent_handle::query_name);
                tsuEvents::tsuEvent ev(st.info_hash.to_string(), QString::fromStdString(st.name), 0, 0, 0, 0, st.total_wanted, 0, 0);
                emit eventFromLibtorrent(ev);
            }

            if (auto aa = libtorrent::alert_cast<libtorrent::torrent_finished_alert>(a)) {
                libtorrent::torrent_handle th = aa->handle;
                libtorrent::torrent_status st = th.status(libtorrent::torrent_handle::query_name);
    //            emit sendPopupInfo(QString("%1 finished!").arg(QString::fromStdString(st.name)));
            }

            if (auto aa = libtorrent::alert_cast<libtorrent::tracker_error_alert>(a)) {
                qDebug() << QString("tracker_error_alert %1").arg(QString::fromStdString(aa->message()));
            }

            if (auto aa = libtorrent::alert_cast<libtorrent::torrent_removed_alert>(a)) {
                emit torrentDeletedFromLibtorrent(aa->info_hash.to_string());
            }

        }
//        p_session.get()->post_torrent_updates(libtorrent::alert::status_notification);
    }


};


*/

Q_DECLARE_METATYPE(tsuEvents::tsuEvent)
Q_DECLARE_METATYPE(std::string)

#endif // TSUMANAGER_H
