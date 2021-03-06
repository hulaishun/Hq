#ifndef QEASTMONEYBLOCKTHREAD_H
#define QEASTMONEYBLOCKTHREAD_H

#include <QThread>
#include <QMap>
#include <QList>


#include "blockdata.h"
//typedef enum enBlockType
//{
//    BLOCK_DISTRICT = 1 ,
//    BLOCK_INDUSTORY,
//    BLOCK_CONCEPT,
//}BLOCK_OPT_TYPE;

typedef enum enBlockDisplayRule
{
    BLOCK_DISPLAY_SORT_NONE = 0,
    BLOCK_DISPLAY_SORT_DESCEND,
    BLOCK_DISPLAY_SORT_ASCEND
}BLOCK_DISPLAY_RULE;

class QEastMoneyBlockThread : public QObject
{
    Q_OBJECT
public:
    explicit QEastMoneyBlockThread(int pBlockID, QObject *parent = 0);
    ~QEastMoneyBlockThread();
    void    setSortRule(BLOCK_DISPLAY_RULE rule);
    void    reverseSortRule();
    void    stop();
signals:
    void    sendBlockDataList(const BlockDataList& list);
    void    sendStkinfoUpdateProgress(int cur, int total);
    void    signalUpdateMsg(const QString& msg);
    void    start();
    void    sendShareBlockDataMap(const QMap<QString, BlockDataList>& map);
public slots:
    void    slotUpdateBlockShare();
    void    slotUpdateBlockInfos();
    void    slotUpdateBlockShareCodeList(const QString& pBlockCode, const QStringList& pShareCodesList);
    void    slotBlockShareThreadFinished();

private:
    int         mSortRule;
    int         mUserBlockType;
    int         mWebBlockType;
    QMap<QString,   BlockData*> mBlockDataList;
    QList<QThread*>     mWorkThreadList;
    QThread     mWorkthread;
    bool        mUpdateRealInfo;
};

#endif // QEASTMONEYBLOCKTHREAD_H
