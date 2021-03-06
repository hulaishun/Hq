﻿#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <QMenu>
#include "profiles.h"
#include "stkmktcap.h"
#include <QDateTime>
#include "qexchangedatemangagedialog.h"
#include "qeastmoneyzjlxthread.h"
#include <QShortcut>
//#include "qthook.h"
#include <QProcess>
#include <QDesktopWidget>
#include <QResizeEvent>
#include "qindexwidget.h"
#include "qeastmonystockcodesthread.h"
#include "qeastmoneychinashareexchange.h"
#include "qeastmoneynorthboundthread.h"
#include "qeastmoneyhsgtdialog.h"
#include "./history/qsharehistoryinfomgr.h"
#include "qhttpget.h"
#include "exchange/qexchangerecordworker.h"
#include "qeastmoneyhsgtshareamount.h"

#define     STK_ZXG_SEC         "0520"
#define     STK_HSJJ_SEC        "4521"
#define     STK_ZXG_NAME        "codes"

class HqTableWidgetItem : public QTableWidgetItem
{
public:
    HqTableWidgetItem(const QString& text, Qt::AlignmentFlag flg = Qt::AlignCenter)
        :QTableWidgetItem(text)
    {
        setTextAlignment(flg);
//        QFont font = this->font();
//        font.setPointSize(10);
//        setFont(font);
    }

    ~HqTableWidgetItem()
    {

    }

};

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),mBlockMgr(NULL)/*,mStockThread(NULL)*/,mSearchThread(NULL),mDisplayCol(0),mMergeThread(0),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setMouseTracking(true);
    mDisplayMode = E_DISPLAY_ALL;
    ui->closeBtn->setIcon(style()->standardPixmap(QStyle::SP_TitleBarCloseButton));
    ui->minBtn->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
    //ui->srchBtn->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    //系统托盘
    QIcon appIcon = QIcon(":/icon/image/Baidu_96px.png");
    if(appIcon.isNull())
    {
        qDebug()<<"icon image is not found";
    }
    this->setWindowIcon(appIcon);
    systemIcon = new QSystemTrayIcon(this);
    systemIcon->setIcon(appIcon);
    systemIcon->show();
    connect(systemIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(setDlgShow(QSystemTrayIcon::ActivationReason)));
    //数据库初始化
    DATA_SERVICE->signalInitDBTables();
    connect(ui->hqtbl, SIGNAL(signalSetFavCode(QString)), DATA_SERVICE, SLOT(slotSetFavCode(QString)));

#if 0
    //开始更新历史信息，龙虎榜信息，沪港通信息
    QShareHistoryInfoMgr *mgr = new QShareHistoryInfoMgr();
    connect(mgr, SIGNAL(signalUpdateProcess(int,int)), this, SLOT(slotUpdate(int,int)));
    connect(mgr, SIGNAL(signalHistoryDataFinished()), this, SLOT(slotHistoryDataFinish()));
    connect(mgr, SIGNAL(signalUpdateAmountProcess(QString)), this, SLOT(slotUpdateMsg(QString)));


    //更新记录

    QExchangeRecordWorker *work = new QExchangeRecordWorker;
    connect(work, SIGNAL(signalSendStkProfitList(StockDataList)), DATA_SERVICE, SIGNAL(signalUpdateStkProfitList(StockDataList)));
    connect(work, SIGNAL(signalSendCodeList(QStringList)), this, SLOT(slotUpdateFavList(QStringList)));
    work->signalStartImport("test.xlsx");
#endif
   QEastMonyStockCodesThread *codesThread = new QEastMonyStockCodesThread;
   connect(codesThread, SIGNAL(signalSendCodesList(QStringList)), this, SLOT(slotUpdateStockCodesList(QStringList)));
//   connect(codesThread, SIGNAL(finished()), codesThread, SLOT(deleteLater()));
   codesThread->start();
    //创建快捷事件
    QShortcut *shotcut = new QShortcut(QKeySequence("Alt+X"), this);  //隐藏
    connect(shotcut, SIGNAL(activated()), this, SLOT(slotWhetherDisplay()));
    QShortcut *shotcut1 = new QShortcut(QKeySequence("Alt+A"), this);
    connect(shotcut1, SIGNAL(activated()), this, SLOT(slotDisplayAll()));
    QShortcut *shotcut2 = new QShortcut(QKeySequence("Alt+S"), this);
    connect(shotcut2, SIGNAL(activated()), this, SLOT(slotDisplayBlock()));
    QShortcut *shotcut3 = new QShortcut(QKeySequence("Alt+D"), this);
    connect(shotcut3, SIGNAL(activated()), this, SLOT(slotDisplayStockMini()));
//    setHook(this);
    mInit = false;
}

void Dialog::setDlgShow(QSystemTrayIcon::ActivationReason val)
{
    qDebug()<<"val:"<<val;
    switch (val) {
    case QSystemTrayIcon::DoubleClick:
        this->setVisible(!this->isVisible());
        break;
    case QSystemTrayIcon::Context:
        qDebug()<<"context coming";
    {
        QMenu *popMenu = new QMenu(this);
        QList<QAction*> actlist;
        QStringList poplist;
        poplist<<QStringLiteral("显示")<<QStringLiteral("退出");
        int index = -1;
        foreach (QString name, poplist) {
            index++;
            QAction *act = new QAction(this);
            act->setText(name);
            act->setData(index);
            connect(act, &QAction::triggered, this, &Dialog::slotSystemTrayMenuClicked);
            actlist.append(act);
        }

        popMenu->addActions(actlist);
        popMenu->popup(QCursor::pos());
    }
        break;
    default:
        break;
    }

}

void Dialog::slotSystemTrayMenuClicked()
{
    QAction *src = (QAction*) sender();
    if(!src) return;
    int val = src->data().toInt();
    if(val == 0)
    {
        //显示
        this->setVisible(true);
    } else
    {
        this->close();
    }

}

void Dialog::displayBlockRealtimeInfo()
{
    ui->updatelbl->clear();
//    MktCapFile::instance()->setValue("Update", "time", QDateTime::currentDateTime().toTime_t());
//    if(mMergeThread)
//    {
//        mMergeThread->start();
//        mMergeThread->setActive(true);
//    }
//    if(mBlockThread)
//    {
//        mBlockThread->SetUpdateBlockCodes(false);
//        if(mBlockThread->isFinished())
//        {
//            mBlockThread->start();
//        }
//    }
}

Dialog::~Dialog()
{
    //unHook();
//    qDebug()<<"close dialog now";
////    if(mStockThread) mStockThread->deleteLater();
//    if(mBlockThread) mBlockThread->deleteLater();
    delete ui;
}

void Dialog::setSortType(int index)
{
    int type = ui->hqtbl->horizontalHeaderItem(index)->data(Qt::UserRole+1).toInt();
    if(mMergeThread && mMergeThread->isActive())
    {
        mMergeThread->setSortType((STK_DISPLAY_TYPE)type);
    }
}

void Dialog::setBlockSort(int val)
{
    qDebug("click val = %d, total = %d", val, ui->blocktbl->rowCount() -1);
    if(val != 1) return;
    if(mBlockMgr) mBlockMgr->reverseSortRule();
}

void Dialog::setBlockName()
{
    QAction *act = (QAction*)sender();
    if(act == NULL) return;
    int index = act->data().toInt();
    //mCurBlockType = index;
    //if(mBlockMgr) mBlockMgr->setCurBlockType(mCurBlockType);
    qDebug()<<"act name:"<<act->text();

}
void Dialog::setDisplayPage()
{
    if(!mMergeThread) return;
    QAction *act = (QAction*)sender();
    if(act == NULL) return;
    int val = act->data().toInt();
    switch (val) {
    case 0:
        mMergeThread->displayFirst();
        break;
    case 1:
        mMergeThread->displayPrevious();
        break;
    case 2:
        mMergeThread->displayNext();
        break;
    case 3:
        mMergeThread->displayLast();
    default:
        break;
    }
}

void Dialog::setDisplayCol(bool isDisplay)
{
    QAction *act = (QAction*)sender();
    if(act == NULL) return;
    TableColDisplayStatus sts = act->data().value<TableColDisplayStatus>();
    int col = sts.mColIndex;
    if(ui->hqtbl->isColumnHidden(col) && isDisplay)
    {
        mDisplayCol++;
        ui->hqtbl->setColumnHidden(col, false);
    } else if((!ui->hqtbl->isColumnHidden(col)) && (!isDisplay))
    {
        mDisplayCol--;
        ui->hqtbl->setColumnHidden(col, true);
    }
    mTargetSize.setWidth(mDisplayCol * mSecSize);
    setTargetSize(mTargetSize);
}

void Dialog::setStockMarket()
{
    //if(mStockThread) mStockThread->setActive(false);
    if(mMergeThread)
    {
        mMergeThread->setActive(true);
        QAction *act = (QAction*)sender();
        if(act == NULL) return;
        qDebug()<<"mkt_type:"<<act->data().toInt();
        MktType type = (MktType)(act->data().toInt());
        if(type != MKT_JJ && type != MKT_ZXG)
        {
            mMergeThread->setMktType((MktType)(act->data().toInt()));
        } else
        {
            qDebug()<<mHSFoundsList;
            if(type ==MKT_JJ)mMergeThread->setSelfCodesList(mHSFoundsList);
            else mMergeThread->setSelfCodesList(mFavStkList);
            mMergeThread->setMktType(MKT_OTHER);
        }
    }

}


void Dialog::on_zjlxBtn_clicked()
{

}

void Dialog::on_lhbBtn_clicked()
{

}

void Dialog::on_closeBtn_clicked()
{
    this->hide();
}

void Dialog::closeEvent(QCloseEvent *event)
{
    this->hide();
}

void Dialog::on_srchBtn_clicked()
{
    //重新更新数据资料

}

void Dialog::slotUpdateMsg(const QString &msg)
{
    ui->updatelbl->setText(msg);
}

void Dialog::on_minBtn_clicked()
{
    this->hide();
}

void Dialog::resizeEvent(QResizeEvent *event)
{
    qDebug()<<"target size:"<<mTargetSize<<"param size:"<<event->size();
    if(mInit)
    {
        //ui->hqframe->setVisible(ui->hqtbl->isVisible());
    } else
    {
        mInit = true;
    }
    QDialog::resizeEvent(event);
}

//void Dialog::HQLISIINFOCBKFUNC(StockDataList& pDataList, void *pUser)
//{
//    Dialog *pDlg = (Dialog*)pUser;
//    if(pDlg == NULL) return;

//    pDlg->updateHqTable(pDataList);
//}

void Dialog::updateHqTable(const StockDataList& pDataList)
{
//    qDebug()<<"data list:"<<pDataList.length();
    ui->hqtbl->setDataList(pDataList);
#if 0
//    qDebug()<<"input";
    //qDebug()<<"main Thread:"<<QThread::currentThreadId();
    //if(pDataList.length() == 0) return;
    ui->hqtbl->setRowCount(pDataList.count());
    int i=0;
    foreach (StockData data, pDataList) {
        if(data.mName.isEmpty()) continue;
        int k =0;
//        qDebug()<<data.code;
        ui->hqtbl->setRowHeight(i, 20);
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(data.mCode, Qt::AlignRight));
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(data.mName));
        QString tempStr;
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f", data.mCur)));
        //ui->hqtbl->setItem(i, k++, new QTableWidgetItem(tempStr.sprintf("%.2f", data.chg)));
//        if(!mStockMap.contains(data.code))
//        {
//            ui->hqtbl->setItem(i, k++, new QTableWidgetItem(tempStr.sprintf("%.2f%%", data.per)));
//        } else {
            double val = mStockMap[data.mCode];
            QString up = QStringLiteral("↑");
            QString down = QStringLiteral("↓");
            if(val > data.mChgPercent)
            {
                ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(QString("%1%2%").arg(down).arg(QString::number(data.mChgPercent, 'f', 2))));
            } else if(val < data.mChgPercent)
            {
               ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(QString("%1%2%").arg(up).arg(QString::number(data.mChgPercent, 'f', 2))));
            } else {
               ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(QString("%1%2%").arg("").arg(QString::number(data.mChgPercent, 'f', 2))));
            }

//        }
        mStockMap[data.mCode] = data.mChgPercent;
        //ui->hqtbl->setItem(i, k++, new QTableWidgetItem(QString::number(data.vol / 10000) + QStringLiteral("万")));
        if(data.mMoney >= 1000){
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f", data.mMoney / 10000.0) + QStringLiteral("亿")));
        } else {
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.0f", data.mMoney) + QStringLiteral("万")));
        }
        //ui->hqtbl->setItem(i, k++, new QTableWidgetItem(QString::number(data.money)));
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f",data.mMoneyRatio)));
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f",data.mLast3DaysChgPers)));
        if(fabs(data.mZJLX) >= 1000){
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f", data.mZJLX / 10000.0) + QStringLiteral("亿")));
        } else {
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.0f", data.mZJLX) + QStringLiteral("万")));
        }
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f%(%.2f)",data.mGXL * 100, data.mXJFH)));
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.0f",data.mSZZG)));
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.0f",data.mTotalCap / 100000000.0 ) + QStringLiteral("亿")));
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.0f",data.mMutalbleCap/ 100000000.0 )+ QStringLiteral("亿")));

        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.0f",data.mProfit)));
        if(data.mForeignVol >= 10000000){
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f", data.mForeignVol / 100000000.0) + QStringLiteral("亿")));
        } else {
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f", data.mForeignVol / 10000.0)  + QStringLiteral("万")));
        }
        if(data.mForeignVolChg >= 10000000){
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f", data.mForeignVolChg / 100000000.0) + QStringLiteral("亿")));
        } else {
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f", data.mForeignVolChg / 10000.0)  + QStringLiteral("万")));
        }
        if(data.mForeignCap >= 1000000){
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f", data.mForeignCap / 100000000.0) + QStringLiteral("亿")));
        } else {
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.0f", data.mForeignCap / 10000.0) + QStringLiteral("万")));
        }

        if(data.mForeignCapChg >= 1000000){
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.2f", data.mForeignCapChg / 100000000.0) + QStringLiteral("亿")));
        } else {
            ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(tempStr.sprintf("%.0f", data.mForeignCapChg / 10000.0) + QStringLiteral("万")));
        }
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(data.mGQDJR.toString("yyyy-MM-dd")));
        ui->hqtbl->setItem(i, k++, new HqTableWidgetItem(data.mYAGGR.toString("yyyy-MM-dd")));
        QString code = data.mCode;
        if(code.left(1) == "5" || code.left(1) == "6")
        {
            code = "sh"+code;
        } else
        {
            code = "sz"+code;
        }
        if(!mFavStkList.contains(code))
        {
           // btn->setText(QStringLiteral("添加"));
            //ui->hqtbl->item(i, 0)->setIcon(QIcon(""));
        } else
        {
            //btn->setText(QStringLiteral("删除"));
            ui->hqtbl->item(i, 0)->setIcon(QIcon(":/icon/image/zxg.ico"));
        }
        //btn->setProperty("code", code);
        //ui->hqtbl->setCellWidget(i, btnindex, btn);
        ui->hqtbl->item(i, 0)->setData(Qt::UserRole, code);
//        qDebug()<<"data.blocklist:"<<data.blocklist;
        if(data.mBlockList.length() == 0) data.mBlockList = mShareBlockList[code];
        ui->hqtbl->item(i, 0)->setData(Qt::UserRole+1, data.mBlockList);
        QColor backColor = Qt::white;
        if(data.mProfit >=5000)
        {
            backColor = QColor(255, 0, 0);
        } else if(data.mProfit >= 1000)
        {
            backColor = QColor(238, 0, 0);
        } else if(data.mProfit >0)
        {
            backColor = QColor(200, 0, 0);
        } else if(data.mProfit <-5000)
        {
            backColor = QColor(0, 255, 0);
        } else if(data.mProfit <-1000)
        {
            backColor = QColor(0,238,0);
        } else if(data.mProfit <0)
        {
            backColor = QColor(0,200,0);
        }
        ui->hqtbl->item(i, 0)->setBackgroundColor(backColor);
        i++;

    }

#endif
}

//void Dialog::HQBLOCKINFOCBKFUNC(BlockDataList& pDataList, void *pUser)
//{
//    Dialog *pDlg = (Dialog*)pUser;
//    if(pDlg == NULL) return;

//    pDlg->updateBlockTable(pDataList);
//}

void Dialog::updateBlockTable(const BlockDataList& pDataList)
{
    ui->blocktbl->setDataList(pDataList);
#if 0
    ui->blocktbl->setRowCount(pDataList.count());

    int i=0;
    foreach (BlockData data, pDataList) {
        mBlockStkList[data.code] = data.stklist;
        int k =0;
        ui->blocktbl->setRowHeight(i, 20);
        ui->blocktbl->setItem(i, k++, new HqTableWidgetItem(data.name));

        //ui->blocktbl->setItem(i, k++, new QTableWidgetItem(QString::number(data.mktkap)));
        QString tempStr = QString("%1%2%");
        QString up = QStringLiteral("↑");
        QString down = QStringLiteral("↓");
        if(mBlockDataMap[data.code].changePer > data.changePer)
        {
            ui->blocktbl->setItem(i, k++, new HqTableWidgetItem(tempStr.arg(down).arg(QString::number(data.changePer, 'f', 2))));
        } else if(mBlockDataMap[data.code].changePer < data.changePer)
        {
            ui->blocktbl->setItem(i, k++, new HqTableWidgetItem(tempStr.arg(up).arg(QString::number(data.changePer, 'f', 2))));
        } else
        {
            ui->blocktbl->setItem(i, k++, new HqTableWidgetItem(tempStr.arg("").arg(QString::number(data.changePer, 'f', 2))));
        }
        //mBlockMap[data.code] = data.changePer;
        QVariant val;
        val.setValue(data);
        ui->blocktbl->item(i, 0)->setData(Qt::UserRole, val);
        i++;

    }
    mBlockDataMap = map;
#endif

}

#if 0
void Dialog::on_blocktbl_itemDoubleClicked(QTableWidgetItem *item)
{
    if(item == NULL) return;
    QTableWidgetItem *wkItem = item;
    if(wkItem->column() != 0){
        wkItem = ui->blocktbl->item(item->row(), 0);
    }
    BlockData data = wkItem->data(Qt::UserRole).value<BlockData>();
    qDebug()<<"code:"<<data.code<<" name:"<<data.name;
    //int code = wkItem->data(Qt::UserRole).toInt();
    qDebug()<<"code:"<<data.stklist;
   displayBlockDetailInfoInTable(data.stklist);
}
#endif

void Dialog::displayBlockDetailInfoInTable(const QStringList& stklist)
{
    if(mMergeThread) /*mMergeThread->setActive(false);*/
    {
        mMergeThread->setMktType(MKT_OTHER);
        mMergeThread->setSelfCodesList(stklist);
    }
//    if(mStockThread)
//    {
//        mStockThread->setActive(true);
//        mStockThread->setStkList(stklist);
//    }


}
#if 0
void Dialog::on_blocktbl_customContextMenuRequested(const QPoint &pos)
{
    qDebug()<<"right menu clicked";
    QMenu *popMenu = new QMenu(this);
    QList<QAction*> actlist;

    QStringList poplist;
    poplist<<QStringLiteral("概念")<<QStringLiteral("行业")<<QStringLiteral("地域");
    int index = 4;
    foreach (QString name, poplist) {
        index--;
        QAction *act = new QAction(this);
        act->setText(name);
        act->setData(index);
        connect(act, &QAction::triggered, this, &Dialog::setBlockName);
        actlist.append(act);
    }

    popMenu->addActions(actlist);
   qDebug()<<"cursor pos:"<<QCursor::pos() <<" param pos:"<<pos;
   // qDebug()<<"parent:"<<((QWidget*)(ui->blkbtn->parent()))->geometry();
   // qDebug()<<"pos:"<<ui->blkbtn->pos();
    popMenu->popup(QCursor::pos());
   // popMenu->popup(ui->blocktbl->mapFromGlobal(pos));
}

void Dialog::on_hqtbl_customContextMenuRequested(const QPoint &pos)
{
    QMenu *popMenu = new QMenu(this);
    popMenu->addMenu(mHqCenterMenu);
    popMenu->addMenu(mHqHeaderMenu);
    popMenu->addMenu(mHqPageMenu);

    //自选股编辑
    QTableWidgetItem *item = ui->hqtbl->itemAt(pos);
    if(item)
    {
        item = ui->hqtbl->item(item->row(), 0);
        QString stkCode = item->data(Qt::UserRole).toString();
        QList<QAction*> actlist;
        int row = item->row();
        item = ui->hqtbl->item(row, 0);
        QStringList poplist;
        poplist<<QStringLiteral("分时图")<<QStringLiteral("日线图")<<QStringLiteral("沪深股通");
        QList<int> Optlist;
        Optlist<<MENU_OPT_MINUTE<<MENU_OPT_DAY<<MENU_OPT_HSGT;
        int index = -1;
        foreach (QString name, poplist) {
            index++;
            QAction *act = new QAction(this);
            act->setText(name);
            HqTableMenuData  data;
            data.mStockCode = stkCode;
            data.mMenuCmd = Optlist[index];
            act->setData(QVariant::fromValue(data));
            connect(act, &QAction::triggered, this, &Dialog::hqMenuOpt);
            actlist.append(act);
        }
        QMenu *submenu = new QMenu(QStringLiteral("所属板块"), this);
        QStringList blocklist = item->data(Qt::UserRole+1).toStringList();
        //qDebug()<<"blocklist:"<<blocklist<<" code:"<<item->data(Qt::UserRole).toString();
        foreach (QString name, blocklist) {
            if(name.trimmed().isEmpty()) continue;
            if(mBlockDataMap[name].name.trimmed().isEmpty()) continue;
            QAction *act = new QAction(this);
            act->setText(QString("%1:%2%").arg(mBlockDataMap[name].name).arg(mBlockDataMap[name].changePer));
            qDebug()<<"subtext:"<<act->text();
            HqTableMenuData data;
            data.mStockCode = stkCode;
            data.mBlockCode = name;
            data.mMenuCmd = MENU_OPT_BLOCK;
            act->setData(QVariant::fromValue(data));
            connect(act, &QAction::triggered, this, &Dialog::hqMenuOpt);
            submenu->addAction(act);
        }
        popMenu->addActions(actlist);
        popMenu->addMenu(submenu);
    }

    popMenu->popup(QCursor::pos());

}
#endif

void Dialog::editFavorite()
{
    QPushButton* send = (QPushButton*) sender();
    if(!send) return;

    QString code = send->property("code").toString();
    if(mFavStkList.contains(code))
    {
        mFavStkList.removeAll(code);
    } else
    {
        mFavStkList.append(code);
    }
    //if(mMergeThread && mMergeThread->getMktType() == MKT_ZXG) mMergeThread->setSelfCodesList(mFavStkList);
    Profiles::instance()->setValue(STK_ZXG_SEC, STK_ZXG_NAME, mFavStkList);
    qDebug()<<"fav:"<<mFavStkList;
}

void Dialog::on_searchTxt_textChanged(const QString &arg1)
{
    if(mSearchThread)
    {
        mSearchThread->signalSetSearchString(arg1);
    }

}

void Dialog::slotUpdate(int cur, int total)
{
 //   qDebug()<<"cur:"<<cur<<" total:"<<total;
    ui->updatelbl->setText(QString("正在更新日线数据：%1/%2").arg(cur).arg(total));
}

void Dialog::hqMenuOpt()
{
    QAction *act = (QAction*)sender();
    if(act == NULL) return;
    HqTableMenuData opt = act->data().value<HqTableMenuData>();
    qDebug()<<"cmd:"<<opt.mMenuCmd<<" code:"<<opt.mStockCode;
    switch (opt.mMenuCmd) {
    case MENU_OPT_MINUTE:
        break;
    case MENU_OPT_DAY:
        break;
    case MENU_OPT_HSGT:
    {
        QEastMoneyHSGTDialog *dlg = new QEastMoneyHSGTDialog;
        emit DATA_SERVICE->signalQueryTop10ChinaStockInfos(QDate(), opt.mStockCode.right(6));
        dlg->setModal(false);
        dlg->show();
    }
        break;
    case MENU_OPT_BLOCK:
        displayBlockDetailInfoInTable(mBlockDataMap[opt.mBlockCode].mShareCodeList);
        break;
    default:
        break;
    }
}

void Dialog::on_DateMgrBtn_clicked()
{
    QExchangeDateMangageDialog *dlg = new QExchangeDateMangageDialog;
    if(dlg == NULL) return;
    connect(dlg, SIGNAL(accepted()), dlg, SLOT(deleteLater()));
    dlg->exec();
}

#if 0
void Dialog::on_hqtbl_itemDoubleClicked(QTableWidgetItem *item)
{
    if(!item) return;
    if(item->column() != 0) return;

    QString code = item->data(Qt::UserRole).toString();
    if(mFavStkList.contains(code))
    {
        mFavStkList.removeAll(code);
    } else
    {
        mFavStkList.append(code);
    }
//    if(mMergeThread && mMergeThread->getMktType() == MKT_ZXG) mMergeThread->setSelfCodesList(mFavStkList);
    Profiles::instance()->setValue(STK_ZXG_SEC, STK_ZXG_NAME, mFavStkList);
    qDebug()<<"fav:"<<mFavStkList;
}

#endif

void Dialog::slotUpdateIndex(const StockDataList &pDataList)
{
#if 0
    if(pDataList.length() == 0) return;
    //更新指数的情况置顶显示
    if(ui->hqtbl->rowCount() == 0)
    {
        ui->hqtbl->setRowCount(pDataList.length());
    }
    //开始更新
    int i=0;
    foreach (StockData data, pDataList) {
        ui->hqtbl->setRowHeight(i, 20);
        QString perstr = QString("").sprintf("%.2f%", data.per);
        QString money = QString("").sprintf("%.2f", data.money / 10000.0) + QStringLiteral("亿");
        QStringList valist;
        valist<<data.code<<data.name<<QString::number(data.cur)<<perstr<<money;
        for(int k=0;k<valist.length(); k++)
        {
            QTableWidgetItem *item = ui->hqtbl->item(i, k);
            if(!item)
            {
                item = new HqTableWidgetItem(valist.at(k), k==0? Qt::AlignRight : Qt::AlignCenter);
                ui->hqtbl->setItem(i, k, item);
            } else
            {
                item->setText(valist.at(k));
            }
        }
        i++;

    }

#endif



}
#if 0
void Dialog::on_hqtbl_itemEntered(QTableWidgetItem *item)
{
    QTableWidgetItem *wkitem = ui->hqtbl->item(item->row(), 0);
    //qDebug()<<"entered item:"<<wkitem->data(Qt::UserRole).toString();
}
#endif

//void Dialog::keyPressEvent(QKeyEvent *e)
//{
//    qDebug()<<__LINE__<<__FUNCTION__;

//}


void Dialog::slotWhetherDisplay()
{
    this->setVisible(!this->isVisible());
}

void Dialog::slotRestartMyself()
{
    QProcess::execute("restart.bat");
}

void Dialog::on_MainDialog_customContextMenuRequested(const QPoint &pos)
{

}

void Dialog::slotDisplayAll()
{
    mDisplayMode = E_DISPLAY_ALL;
    ui->hqtbl->setVisible(true);
    ui->blocktbl->setVisible(true);
    mDisplayCol = 4;
    foreach (QAction* act, mHqColActList) {
        if(act->isChecked())
        {
            mDisplayCol++;
        }
    }
    mTargetSize.setWidth(mDisplayCol * mSecSize);
    setTargetSize(mTargetSize);

}

void Dialog::slotDisplayBlock()
{
    mDisplayMode = E_DISPLAY_BLOCK;

    ui->blocktbl->setVisible(true);
    ui->hqtbl->setVisible(false);
    mDisplayCol = ui->blocktbl->horizontalHeader()->count();
    mTargetSize.setWidth(mDisplayCol * mSecSize);
    setTargetSize(mTargetSize);
}

void Dialog::slotDisplayStockFull()
{
    mDisplayMode = E_DISPLAY_STOCK_FULL;
}


void Dialog::slotDisplayStockMini()
{
    if(mDisplayMode != E_DISPLAY_STOCK_MINI)
    {
        mDisplayMode = E_DISPLAY_STOCK_MINI;
    }
    ui->blocktbl->setVisible(false);
    ui->hqtbl->setVisible(true);
    mDisplayCol = 0;
    for(int i=0; i<ui->hqtbl->columnCount(); i++)
    {
        if(!ui->hqtbl->isColumnHidden(i)) mDisplayCol++;
    }
    mTargetSize.setWidth(mDisplayCol * mSecSize);
    setTargetSize(mTargetSize);
}

void Dialog::setTargetSize(const QSize &size)
{
    int screenW =  QApplication::desktop()->availableGeometry(QApplication::desktop()->primaryScreen()).width();
    int screenH =  QApplication::desktop()->availableGeometry(QApplication::desktop()->primaryScreen()).height();
    this->resize(size);
        windowPos = QPoint(screenW-size.width(), screenH - size.height()-20);
        this->move(windowPos);
        //this->setGeometry(screenW-size.width(), screenH - size.height()-20, size.width(), size.height());

}

void Dialog::mousePressEvent(QMouseEvent *event)

{
    qDebug()<<__FUNCTION__<<__LINE__;

     this->windowPos = this->pos();                // 获得部件当前位置

     this->mousePos = event->globalPos();     // 获得鼠标位置

     this->dPos = mousePos - windowPos;       // 移动后部件所在的位置

}



void Dialog::mouseMoveEvent(QMouseEvent *event)

{
      //  qDebug()<<__FUNCTION__<<__LINE__;
     //this->move(event->globalPos() - this->dPos);

}

//bool Dialog::eventFilter(QObject *obj, QEvent *event)
//{
//    qDebug()<<obj<<" "<<event;
//    if(obj == ui->hqtbl || obj == ui->blocktbl)
//    {
//        if(event->type() == QEvent::DragMove)
//        {
//            return false;
//        }
//    }
//    return true;
//}
#if 0
void Dialog::on_hqtbl_itemClicked(QTableWidgetItem *item)
{
    item->tableWidget()->horizontalHeader()->setHighlightSections(false);
}

void Dialog::on_blocktbl_itemClicked(QTableWidgetItem *item)
{
    item->tableWidget()->horizontalHeader()->setHighlightSections(false);
}
#endif

void Dialog::slotUpdateStockCodesList(const QStringList &list)
{
    qDebug()<<"update code finshed:"<<list.length();
    //更新股本信息等
    QShareHistoryInfoMgr* mgr = new QShareHistoryInfoMgr(list);
    mAllStkList = list;
    //更新指数
    QIndexWidget *indexw = new QIndexWidget(this);
    ui->verticalLayout->insertWidget(0, indexw);
    QStringList indexlist;
    indexlist<<"s_sh000001"<<"s_sh000300"<<"s_sz399001"<<"s_sz399006"<<"s_sh000016"<<"s_sh000010";
    foreach (QString code, indexlist) {
        indexw->insetWidget(code);
    }
    mIndexThread = new QSinaStkInfoThread(0);
    connect(mIndexThread, SIGNAL(sendStkDataList(StockDataList)), indexw, SLOT(updateData(StockDataList)));
    connect(mIndexThread, SIGNAL(finished()), mIndexThread, SLOT(deleteLater()));
    mIndexThread->signalSetStkList(indexlist);
    QEastmoneyNorthBoundThread *north = new QEastmoneyNorthBoundThread();
    connect(north, SIGNAL(signalUpdateNorthBoundList(StockDataList)), indexw, SLOT(updateData(StockDataList)));
    connect(north, SIGNAL(finished()), north, SLOT(deleteLater()));
    north->start();
    //行情中心初始化开始为自选股
    //读取自选
    if(mFavStkList.length() == 0) mFavStkList = Profiles::instance()->value(STK_ZXG_SEC, STK_ZXG_NAME).toStringList();
    mHSFoundsList = Profiles::instance()->value(STK_HSJJ_SEC, STK_ZXG_NAME).toStringList();
    mMergeThread = new QSinaStkResultMergeThread();
    connect(mMergeThread, SIGNAL(sendStkDataList(StockDataList)), this, SLOT(updateHqTable(StockDataList)));
    connect(ui->hqtbl, SIGNAL(signalSetStockMarket(int)), mMergeThread, SLOT(setMktType(int)));
    connect(ui->hqtbl, SIGNAL(signalSetSortType(int)), mMergeThread, SLOT(setSortType(int)));
    connect(ui->hqtbl, SIGNAL(signalDisplayPage(int)), mMergeThread, SLOT(setDisplayPage(int)));
    mMergeThread->setStkList(mAllStkList);
    mMergeThread->setSelfCodesList(mFavStkList);
    mMergeThread->setActive(true);
    mMergeThread->setMktType(MKT_ALL);
    mMergeThread->start();
    //板块行情初始化
    //mCurBlockType = BLOCK_HY;
    mBlockMgr = new QEastMoneyBlockMangagerThread();
    connect(mBlockMgr, SIGNAL(signalBlockDataListUpdated(BlockDataList)), this, SLOT(updateBlockTable(BlockDataList)));
    mBlockMgr->start();

    //查询接口初始化
    mSearchThread = new QSinaSearchThread(this);
    connect(mSearchThread, SIGNAL(sendSearchResult(QStringList)), this, SLOT(displayBlockDetailInfoInTable(QStringList)));
}

void Dialog::on_HSGTBTN_clicked()
{
    ui->HSGTBTN->setStyleSheet("background-color:transparent");
    QEastMoneyHSGTDialog* dlg = new QEastMoneyHSGTDialog;
    dlg->setModal(false);
    dlg->show();

}

void Dialog::slotTodayHSGUpdated()
{
    ui->HSGTBTN->setStyleSheet("background-color:red");
}
//void Dialog::slotStartRealInfo(const QStringList& list)
//{


//}
void Dialog::slotHistoryDataFinish()
{
#if 0
    QShareHistoryInfoMgr *mgr = qobject_cast<QShareHistoryInfoMgr*> (sender());
    if(mgr)
    {
        mAllStkList = mgr->getCodesList();
        mgr->deleteLater();
    }
    ui->updatelbl->clear();
    QEastMoneyChinaShareExchange *tophk = new QEastMoneyChinaShareExchange(QDate::fromString("2017-07-13", "yyyy-MM-dd"));
    connect(tophk, SIGNAL(signalHSGTofTodayTop10Updated()), this, SLOT(slotTodayHSGUpdated()));
    tophk->start();

    //更新指数
    QIndexWidget *indexw = new QIndexWidget(this);
    ui->verticalLayout->insertWidget(0, indexw);
    QStringList indexlist;
    indexlist<<"sh000001"<<"sh000300"<<"sz399001"/*<<"sh000043"*/<<"sz399006"<<"sh000016"<<"sh000010";
    mIndexThread = new QSinaStkInfoThread(this);
    connect(mIndexThread, SIGNAL(sendStkDataList(StockDataList)), indexw, SLOT(updateData(StockDataList)));
    mIndexThread->setStkList(indexlist);
    //mIndexThread->start();
    QEastmoneyNorthBoundThread *north = new QEastmoneyNorthBoundThread(this);
    connect(north, SIGNAL(signalUpdateNorthBoundList(StockDataList)), indexw, SLOT(updateData(StockDataList)));
    north->start();

    //行情中心初始化开始为自选股
    //读取自选
    if(mFavStkList.length() == 0)mFavStkList = Profiles::instance()->value(STK_ZXG_SEC, STK_ZXG_NAME).toStringList();
    mHSFoundsList = Profiles::instance()->value(STK_HSJJ_SEC, STK_ZXG_NAME).toStringList();
    mMergeThread = new QSinaStkResultMergeThread();
    connect(mMergeThread, SIGNAL(sendStkDataList(StockDataList)), this, SLOT(updateHqTable(StockDataList)));
    mMergeThread->setStkList(mAllStkList);
    mMergeThread->setSelfCodesList(mFavStkList);
    mMergeThread->setActive(true);
    mMergeThread->setMktType(MKT_ZXG);
    mMergeThread->start();
    //板块行情初始化
    mBlockMgr = new QEastMoneyBlockMangagerThread();
    connect(mBlockMgr, SIGNAL(signalBlockDataListUpdated(BlockDataList)), this, SLOT(updateBlockTable(BlockDataList)));
    mBlockMgr->start();

    //查询接口初始化
    qDebug()<<"search thread_________________";
    mSearchThread = new QSinaSearchThread(this);
    connect(mSearchThread, SIGNAL(sendSearchResult(QStringList)), this, SLOT(displayBlockDetailInfoInTable(QStringList)));
#endif
}

void Dialog::slotUpdateFavList(const QStringList &list)
{
    if(mFavStkList.length() == 0)
    mFavStkList = list;
}

void Dialog::recvShareBlockDataMap(const QMap<QString, QStringList> &map)
{
    foreach (QString key, map.keys()) {
        //qDebug()<<"key:"<<key<<"   list:"<<map[key];
        //mShareBlockList[key].append(map[key]);
    }
}
