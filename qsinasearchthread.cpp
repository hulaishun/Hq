﻿#include "qsinasearchthread.h"
#include <QDateTime>
#include <QDebug>
#include "qhttpget.h"

QSinaSearchThread::QSinaSearchThread(QObject *parent) : QObject(parent)
{
    mHttp = new QHttpGet;
    connect(mHttp, SIGNAL(signalSendHttpConent(QByteArray)), this, SLOT(slotRecvHttpContent(QByteArray)));
    connect(this ,SIGNAL(signalSetSearchString(QString)), this, SLOT(slotRecvSearchString(QString)));
    connect(mHttp, SIGNAL(finished()), this, SLOT(slotSearchCmdFinished()));
    moveToThread(&mWorkThread);
    mWorkThread.start();
}

QSinaSearchThread::~QSinaSearchThread()
{
    qDebug()<<__FUNCTION__<<__LINE__;
    if(mHttp)
    {
        mHttp->deleteLater();
    }
    mWorkThread.quit();
    mWorkThread.wait();
}

void QSinaSearchThread::slotRecvHttpContent(const QByteArray &bytes)
{
    QString result = QString::fromLocal8Bit(bytes.data());
    qDebug()<<"result:"<<result;
    int start = 0;
    QStringList resultlist;
    QRegExp worker("[shz]{2}[0-9]{6}");
    while((start = worker.indexIn(result, start)) != -1)
    {
        //qDebug()<<"start:"<<start<<"   "<<worker.cap();
        resultlist.append("s_"+worker.cap());
        start += worker.cap().length();
    }
    qDebug()<<"search result:"<<resultlist;
    emit sendSearchResult(resultlist);
}

void QSinaSearchThread::slotRecvSearchString(const QString& text)
{
    mSearchList.append(text);
    if(mSearchList.length() == 1)
    {
        if(!mHttp->isRunning())
        {
            startSearchCmd(mSearchList.first());
        }
    }

}

void QSinaSearchThread::slotSearchCmdFinished()
{
    qDebug()<<__FUNCTION__<<__LINE__;
    if(mSearchList.length()) mSearchList.removeFirst();
    if(mSearchList.length()) startSearchCmd(mSearchList.first());
}

void QSinaSearchThread::startSearchCmd(const QString &text)
{
    QString wkURL = QString("http://suggest3.sinajs.cn/suggest/type=11&key=%1&name=suggestdata_%2")
            .arg(text)
            .arg(QDateTime::currentDateTime().toMSecsSinceEpoch());
    mHttp->setUrl(wkURL);
    mHttp->start();
}
