#ifndef DOWNLOADTS_H
#define DOWNLOADTS_H

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QStandardPaths>
#include <QStringListModel>
#include <QStandardItem>
#include <QDebug>
#include <QDateTime>
#include <QHostInfo>
#include <QThread>
#include <QtCore>
#include <QtNetwork>
#include <QDesktopServices>
#include <QMouseEvent>

namespace Ui {
class DownloadTS;
}

class DownloadTS : public QWidget {
    Q_OBJECT

  public:
    explicit DownloadTS(QWidget *parent = 0);
    ~DownloadTS();
    void checkNetWorkOnline();
    void SystemInit(int flag);
    void append(const QUrl &url);
    void append(const QStringList &urls);
    static QString saveFileName(const QUrl &url);
    unsigned int CheckDownload(QString click_flag);

  private slots:
    void on_IFileButton_clicked();
    void on_DirDownload_clicked();
    void lookedUp(QHostInfo host);
    void on_StartDownload_clicked();
    void startNextDownload();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadReadyRead();
    void on_StopDownload_clicked();

    void on_PauseDownload_clicked();

  private:
    Ui::DownloadTS *ui;
    QString currentPath;
    QStringList CDownloadList;
    bool Networkflag = false;
    unsigned int DownloadCode = 0;
    unsigned int SDownloadCount = 0;
    QNetworkAccessManager manager;
    QQueue<QUrl> downloadQueue;
    QNetworkReply *currentDownload = nullptr;
    QFile output;
    QElapsedTimer downloadTimer;
    QStandardItemModel *FinshDownlodmodel;
    bool isHttpRedirect() const;
    void reportRedirect();
};

#endif // DOWNLOADTS_H
