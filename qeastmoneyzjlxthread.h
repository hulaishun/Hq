#ifndef QEASTMONEYZJLXTHREAD_H
#define QEASTMONEYZJLXTHREAD_H

#include <QThread>
#include "stockdata.h"
#include "qhttpget.h"


class QEastMoneyZjlxThread : public QObject
{
    Q_OBJECT
public:
    explicit QEastMoneyZjlxThread(QObject *parent = 0);
    ~QEastMoneyZjlxThread();
signals:
    void sendZjlxDataList(const QList<zjlxData>& zjlxDataList);
public slots:
    void slotRecvHttpContent(const QByteArray& bytes);
private:
    QThread     mWorkThread;
    QHttpGet    *mHttp;

};

#endif // QEASTMONEYZJLXTHREAD_H
