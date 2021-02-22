#include "downloadts.h"
#include "ui_downloadts.h"

/**
  * DownloadCode 状态码:
  *  0 表示空闲状态或完成状态
  *  1 表示下载状态
  *  2 表示暂停状态
  *
**/

/**
 * @brief DownloadTS::DownloadTS
 * @param parent
 */
DownloadTS::DownloadTS(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadTS) {
    ui->setupUi(this);
    this->checkNetWorkOnline();
    /*
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *item;
    item = new QStandardItem("https://img.ivsky.com/img/tupian/pre/202003/12/xiaoji-001.jpg");
    model->appendRow(item);
    item = new QStandardItem("https://img.ivsky.com/img/tupian/pre/202003/12/xiaoji-002.jpg");
    model->appendRow(item);
    item = new QStandardItem("https://img.ivsky.com/img/tupian/pre/202003/12/xiaoji-113.jpg");
    model->appendRow(item);
    item = new QStandardItem("https://img.ivsky.com/img/tupian/pre/202003/12/xiaoji-024.jpg");
    model->appendRow(item);
    item = new QStandardItem("https://img.ivsky.com/img/tupian/pre/202003/12/xiaoji-005.jpg");
    model->appendRow(item);
    item = new QStandardItem("https://img.ivsky.com/img/tupian/pre/202003/12/xiaoji-006.jpg");
    model->appendRow(item);
    ui->URLList->setModel(model);
    */
}

DownloadTS::~DownloadTS() {
    delete ui;
}
/**
 * @brief DownloadTS::on_IFileButton_clicked
 * @description:导入文件按钮槽函数
 */
void DownloadTS::on_IFileButton_clicked() {
    if (CheckDownload("IFileButton") == 1) {
        return;
    }
    emit SystemInit(1);
    QStandardItemModel *model = new QStandardItemModel();
    QString location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), location, tr("PHP files (*.php);;TXT files (*.txt);;Any files (*)"));
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        qDebug() << "Cannot open file: " << file.errorString();
    } else {
        //qDebug()<<"currentTime--"<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            //去掉换行符
            QString str(line);
            //去掉\n
            str.simplified();
            str.remove(QChar('\n'), Qt::CaseInsensitive);
            if ((str.startsWith("https:") || (str.startsWith("http:"))) && ((str.contains("GQ") && str.endsWith("ts")) || str.endsWith("jpg"))) {
                QStandardItem *item = new QStandardItem(str);
                model->appendRow(item);
            }
        }
    }
    file.close();
    ui->currentFilePath->setText(fileName);
    ui->URLList->setModel(model);
}
/**
 * @brief DownloadTS::on_StartDownload_clicked
 * @description:开始下载按钮槽函数
 */
void DownloadTS::on_StartDownload_clicked() {
    QDir dir;
    unsigned int flag = CheckDownload("StartDownload");
    if (flag == 1 && this->DownloadCode == 2) {
        qDebug() << "-------------------继续下载";
    } else if (flag == 1 && this->DownloadCode == 1) {
        return;
    } else {
        emit SystemInit(2);
        //判断是否导入文件
        if (!ui->URLList->model()) {
            QMessageBox::warning(this, "Warning", "未导入文件!");
            return;
        }
        //根据时间戳获取下载目录路径
        currentPath = QCoreApplication::applicationDirPath() + "/";
        currentPath = currentPath + (QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
        //网络连接检测
        this->checkNetWorkOnline();
        if (!Networkflag) {
            QMessageBox::warning(this, "Warning", "没有网络连接!");
            return;
        }
        QStandardItemModel *model = new QStandardItemModel();
        QModelIndexList modelIndexList = ui->URLList->selectionModel()->selectedIndexes();
        if (modelIndexList.count() <= 0) {
            QMessageBox::warning(this, "Warning", "没有选中列表中某一项或多项!");
            return;
        }
        if (!dir.mkdir(currentPath)) {
            QMessageBox::warning(this, "Warning", "文件夹 " + currentPath + " 创建失败!");
            return;
        }
        foreach (QModelIndex modelIndex, modelIndexList)    {
            CDownloadList.append(modelIndex.data().toString());
            QStandardItem *item = new QStandardItem(modelIndex.data().toString());
            model->appendRow(item);
        }
        ui->LDownStatus->setText(QString::number(this->SDownloadCount, 10) + "/" + QString::number(CDownloadList.count(), 10));
        ui->DownloadList->setModel(model);
        this->DownloadCode = 1;
        this->append(this->CDownloadList);
    }
}

/**
 * @brief DownloadTS::on_DirDownload_clicked
 * @description:打开下载目录槽函数
 */
void DownloadTS::on_DirDownload_clicked() {
    if (!currentPath.isEmpty()) {
        QString currentDir = currentPath.replace("/", "\\");
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir))) {
            QMessageBox::warning(this, "Warning", "目录: " + currentDir + " 打开失败!");
        }
    } else {
        QMessageBox::warning(this, "Warning", "未下载文件，目录打开失败!");
    }
}
/**
 * @brief DownloadTS::on_StopDownload_clicked
 * @description:终止下载槽函数
 */
void DownloadTS::on_StopDownload_clicked() {
    switch (this->DownloadCode) {
        case 0:
            QMessageBox::warning(this, "Warning", "未下载任何内容!");
            break;
        case 1:
            this->DownloadCode = 0;
            currentDownload->abort();
            emit SystemInit(4);
            break;
        case 2:
            QMessageBox::warning(this, "Warning", "下载将由暂停状态转为终止状态!");
            this->DownloadCode = 0;
            currentDownload->abort();
            emit SystemInit(4);
            break;
        default:
            QMessageBox::warning(this, "Warning", "异常信号值!");
            break;
    }
}
/**
 * @brief DownloadTS::on_PauseDownload_clicked
 * @description:暂停下载槽函数
 */
void DownloadTS::on_PauseDownload_clicked() {
    switch (this->DownloadCode) {
        case 0:
            QMessageBox::warning(this, "Warning", "未下载任何内容!");
            break;
        case 1:
            this->DownloadCode = 2;
            currentDownload->abort();
            emit SystemInit(3);
            break;
        case 2:
            QMessageBox::warning(this, "Warning", "下载已经处于暂停状态!");
            break;
        default:
            QMessageBox::warning(this, "Warning", "异常信号值!");
            break;
    }
}
/**
 * @brief DownloadTS::checkNetWorkOnline
 * @description:网络连接检测
 */
void DownloadTS::checkNetWorkOnline() {
    QHostInfo::lookupHost("www.baidu.com", this, SLOT(lookedUp(QHostInfo)));
}
/**
 * @brief DownloadTS::lookedUp
 * @param host
 * @description:网络连接检测
 */
void DownloadTS::lookedUp(QHostInfo host) {
    if (host.error() != QHostInfo::NoError) {
        Networkflag = false;
    } else {
        Networkflag = true;
    }
}
/**
 * @brief DownloadTS::SystemInit
 * @description:系统界面初始化
 *   flag 标记:
 *        导入文件按钮发出标记 : 1
 *        开始下载按钮发出标记 : 2
 *        暂停下载按钮发出标记 : 3
 *        终止下载按钮&完成下载发出标记 : 4
 *        某一项下载完成发出标记 : 5
 *
 */
void DownloadTS::SystemInit(int flag) {
    if (flag != 5 && flag != 3) {
        DownloadCode = 0;
        downloadQueue.clear();
        if (flag != 4) {
            CDownloadList.clear();
            SDownloadCount = 0;
            FinshDownlodmodel = new QStandardItemModel();
            if (ui->DownloadList->model()) {
                ui->DownloadList->model()->deleteLater();
            }
            if (ui->FinishDownloadList->model()) {
                ui->FinishDownloadList->model()->deleteLater();
            }
            if (flag == 1 && ui->URLList->model()) {
                ui->URLList->model()->deleteLater();
            }
        }
    }
    ui->LCDownload->setText("");
    ui->LRate->setText("");
    ui->LScheduleBar->setValue(0);
    if (flag == 4 && (unsigned int)CDownloadList.count() > SDownloadCount) {
        ui->LDownStatus->setText(QString::number(this->SDownloadCount, 10) + "/" + QString::number(CDownloadList.count(), 10) + "  下载失败: " + QString::number(CDownloadList.count() - this->SDownloadCount, 10));
    } else if (flag == 3) {
        ui->LDownStatus->setText(QString::number(this->SDownloadCount, 10) + "/" + QString::number(CDownloadList.count(), 10) + "  已暂停下载");
    } else {
        ui->LDownStatus->setText(QString::number(this->SDownloadCount, 10) + "/" + QString::number(CDownloadList.count(), 10));
    }
}
/**
 * @brief DownloadTS::append
 * @param urls
 * @description:添加下载集合并进入下载
 */
void DownloadTS::append(const QStringList &urls) {
    for (const QString &urlAsString : urls) {
        if (downloadQueue.isEmpty()) {
            QTimer::singleShot(0, this, &DownloadTS::startNextDownload);
        }
        downloadQueue.enqueue(QUrl::fromEncoded(urlAsString.toLocal8Bit()));
    }
}
/**
 * @brief DownloadTS::saveFileName
 * @param url
 * @return 文件名称
 * @description:获取文件名称
 */
QString DownloadTS::saveFileName(const QUrl &url) {
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();
    if (basename.isEmpty()) {
        basename = "download";
    }
    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        while (QFile::exists(basename + QString::number(i))) {
            ++i;
        }
        basename = QString("%1%2.%3").arg(basename.split(".")[0]).arg("(" + QString::number(i) + ")").arg(basename.split(".")[1]);
    }
    return basename;
}
/**
 * @brief DownloadTS::startNextDownload
 * @description:开始进行下载
 * @description:下载完成槽函数
 */
void DownloadTS::startNextDownload() {
    if (downloadQueue.isEmpty()) {
        SystemInit(4);
        return;
    }
    QUrl url = downloadQueue.dequeue();
    QString filename = saveFileName(url);
    output.setFileName(currentPath + "/" + filename);
    if (!output.open(QIODevice::WriteOnly)) {
        //QMessageBox::warning(this, "Warning", "Problem opening save file "+currentPath+"/"+filename);
        qDebug() << "Problem opening save file " << currentPath << "/" << filename;
        startNextDownload();
        return;                 // skip this download
    }
    QNetworkRequest request(url);
    currentDownload = manager.get(request);
    connect(currentDownload, &QNetworkReply::downloadProgress, this, &DownloadTS::downloadProgress);
    connect(currentDownload, &QNetworkReply::finished, this, &DownloadTS::downloadFinished);
    connect(currentDownload, &QNetworkReply::readyRead, this, &DownloadTS::downloadReadyRead);
    // prepare the output
    QModelIndex index = ui->DownloadList->model()->index(ui->DownloadList->model()->rowCount() - downloadQueue.count() - 1, 0);
    ui->DownloadList->setCurrentIndex(index);
    ui->LCDownload->setText(index.data().toString());
    downloadTimer.start();
}
/**
 * @brief DownloadTS::CheckDownload
 * @description:判断当前下状态下弹出提示框
 */
unsigned int DownloadTS::CheckDownload(QString click_flag) {
    int flag = 0;
    if (DownloadCode == 1) {
        QMessageBox::StandardButton rb = QMessageBox::warning(this, "Warning", "文件正在下载,请确认是否继续下载？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (rb == QMessageBox::Yes) {
            flag = 1;
        }
        if (rb == QMessageBox::No) {
            flag = 2;
            if (this->DownloadCode == 1) {
                this->DownloadCode = 0;
                currentDownload->abort();
                emit SystemInit(4);
            }
        }
    }
    if (DownloadCode == 2) {
        QMessageBox::StandardButton rb;
        if (click_flag == "IFileButton") {
            rb = QMessageBox::warning(this, "Warning", "文件下载暂停,请确认是否导入新的下载链接(若导入新的下载链接，原下载将终止)？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        } else if (click_flag == "StartDownload") {
            rb = QMessageBox::warning(this, "Warning", "文件下载暂停,请确认继续下载还是创建新的下载？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        }
        if (rb == QMessageBox::Yes) {
            flag = 1;
        }
        if (rb == QMessageBox::No) {
            flag = 2;
        }
    }
    return flag;
}
/**
 * @brief DownloadTS::downloadProgress
 * @param bytesReceived
 * @param bytesTotal
 * @description:下载进度
 */
void DownloadTS::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    // calculate the download speed
    double speed = bytesReceived * 1000.0 / downloadTimer.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024 * 1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024 * 1024;
        unit = "MB/s";
    }
    ui->LScheduleBar->setValue(bytesReceived * 100.0 / bytesTotal);
    ui->LRate->setText(QString::fromLatin1("%1 %2").arg(speed, 3, 'f', 1).arg(unit));
}
/**
 * @brief DownloadTS::downloadFinished
 * @description:下载完成槽函数
 */
void DownloadTS::downloadFinished() {
    output.close();
    if (currentDownload->error()) {
        qDebug() << "Failed: " << currentDownload->errorString();
        // download failed
        output.remove();
    } else {
        // let's check if it was actually a redirect
        if (isHttpRedirect()) {
            reportRedirect();
            output.remove();
        } else {
            qDebug() << ui->DownloadList->currentIndex().data().toString() << " 下载完成!";
            QStandardItem *item = new QStandardItem(ui->DownloadList->currentIndex().data().toString());
            ui->DownloadList->model()->removeRow(ui->DownloadList->currentIndex().row());
            this->FinshDownlodmodel->appendRow(item);
            ui->FinishDownloadList->setModel(this->FinshDownlodmodel);
            ++this->SDownloadCount;
        }
    }
    emit SystemInit(5);
    currentDownload->deleteLater();
    if (this->DownloadCode == 1) {
        startNextDownload();
    }
}
/**
 * @brief DownloadTS::downloadReadyRead
 * @description:下载写入操作函数
 */
void DownloadTS::downloadReadyRead() {
    output.write(currentDownload->readAll());
}
/**
 * @brief DownloadTS::isHttpRedirect
 * @return
 * @description:判断是否http请求是否重定向函数
 */
bool DownloadTS::isHttpRedirect() const {
    int statusCode = currentDownload->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    return statusCode == 301 || statusCode == 302 || statusCode == 303
           || statusCode == 305 || statusCode == 307 || statusCode == 308;
}
/**
 * @brief DownloadTS::reportRedirect
 * @description:HTTP请求重定向函数
 */
void DownloadTS::reportRedirect() {
    int statusCode = currentDownload->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QUrl requestUrl = currentDownload->request().url();
    //QMessageBox::warning(this, "Warning", "Request: "+requestUrl.toDisplayString()+" was redirected with code: "+statusCode);
    QTextStream(stderr) << "Request: " << requestUrl.toDisplayString()
                        << " was redirected with code: " << statusCode
                        << '\n';
    QVariant target = currentDownload->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!target.isValid()) {
        return;
    }
    QUrl redirectUrl = target.toUrl();
    if (redirectUrl.isRelative()) {
        redirectUrl = requestUrl.resolved(redirectUrl);
    }
    //QMessageBox::warning(this, "Warning", "Redirected to: "+redirectUrl.toDisplayString());
    QTextStream(stderr) << "Redirected to: " << redirectUrl.toDisplayString() << '\n';
}


