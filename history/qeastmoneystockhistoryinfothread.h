﻿#ifndef QEASTMONEYSTOCKHISTORYINFOTHREAD_H
#define QEASTMONEYSTOCKHISTORYINFOTHREAD_H

#include <QThread>
#include <QStringList>
#include <QDate>


class QEastmoneyStockHistoryInfoThread : public QThread
{
    Q_OBJECT
public:
    explicit QEastmoneyStockHistoryInfoThread(const QString& code, QObject *parent = 0);
    ~QEastmoneyStockHistoryInfoThread();
    QString getCode();
signals:
protected:
    void run();
private:
    QString         mCode;
    QThread         mWorkThread;
};

#endif // QEASTMONEYSTOCKHISTORYINFOTHREAD_H


//http://nufm.dfcfw.com/EM_Finance2014NumericApplication/JS.aspx?type=CT&cmd=P.[(x)]|3000592&sty=MPICT&st=z&sr=&p=&ps=&cb=callback&js=&token=aaf32a88de888ea7b0ea63e017ecb049&_=1481245370248
