﻿#include "HqTableWidget.h"
#include "utils/comdatadefines.h"
#include <QHeaderView>
#include <QAction>
#include "qstktablewidgetitem.h"
#include <QDebug>

#define     COL_TYPE_ROLE               Qt::UserRole + 1
#define     COL_SORT_ROLE               Qt::UserRole + 2

HqTableWidget::HqTableWidget(QWidget *parent) : QTableWidget(parent),mCustomContextMenu(0)
{
    initPageCtrlMenu();
    mColDataList.clear();
    mColWidth = 60;
    this->verticalHeader()->setVisible(false);
    this->horizontalHeader()->setDefaultSectionSize(mColWidth);    
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //鼠标右键选择
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomContextMenuRequested(QPoint)));
    connect(this, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(slotCellDoubleClicked(int,int)));
    this->horizontalHeader()->setHighlightSections(false);
    connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slotHeaderClicked(int)));
}

void HqTableWidget::setHeaders(const TableColDataList &list)
{
    QMenu *menu = new QMenu(QStringLiteral("列表标题"), this);
    mColDataList = list;
    this->setColumnCount(list.length());
    for(int i=0; i<mColDataList.size(); i++) {
        mColDataList[i].mColNum = i;
        this->setHorizontalHeaderItem(i, new QStkTableWidgetItem(mColDataList[i].mColStr));
        this->horizontalHeaderItem(i)->setData(COL_TYPE_ROLE, mColDataList[i].mType);
        this->horizontalHeaderItem(i)->setData(COL_SORT_ROLE, QVariant::fromValue((void*) &(mColDataList[i].mRule)));
        QAction *act = new QAction(this);
        act->setText(this->horizontalHeaderItem(i)->text());
        act->setData(QVariant::fromValue((void*) &(mColDataList[i])));
        act->setCheckable(true);
        act->setChecked(mColDataList[i].mIsDisplay);
        connect(act, SIGNAL(triggered(bool)), this, SLOT(slotSetColDisplay(bool)));
        menu->addAction(act);
    }

    insertContextMenu(menu);
}

void HqTableWidget::slotHeaderClicked(int col)
{
    emit signalSetSortType(this->horizontalHeaderItem(col)->data(COL_TYPE_ROLE).toInt());/*
    int *rule = (int*)(this->horizontalHeaderItem(col)->data(COL_SORT_ROLE).value<void*>());
    *rule = !(rule);
    emit signalSetSortRule(*rule);*/
}

void HqTableWidget::slotSetColDisplay(bool isDisplay)
{
    Q_UNUSED(isDisplay)
    QAction *act = (QAction*) sender();
    if(!act) return;
    TableColData *data = (TableColData*)(act->data().value<void*>());
    if(!data) return;
    this->setColumnHidden(data->mColNum, data->mIsDisplay);
    data->mIsDisplay = !(data->mIsDisplay);
}

void HqTableWidget::appendRow()
{
    this->insertRow(this->rowCount());
}

void HqTableWidget::setItemText(int row, int column, const QString &text, Qt::AlignmentFlag flg)
{
    QStkTableWidgetItem *item = (QStkTableWidgetItem*) (this->item(row, column));
    if(item)
    {
        item->setString(text);
    }
    else
    {
        this->setItem(row, column, new QStkTableWidgetItem(text, flg));
    }
}

void HqTableWidget::setFavShareList(const QStringList &list)
{
    mFavShareList = list;
}

void HqTableWidget::appendFavShare(const QString &code)
{
    if(!mFavShareList.contains(code)) mFavShareList.append(code);
}

void HqTableWidget::removeFavShare(const QString &code)
{
    if(mFavShareList.contains(code)) mFavShareList.removeOne(code);
}

void HqTableWidget::updateFavShareIconOfRow(int row, bool isFav)
{
    if(row >= this->rowCount()) return;
    if(isFav) this->item(row, 0)->setIcon(QIcon(":/icon/image/zxg.ico"));
}

void HqTableWidget::prepareUpdateTable(int newRowCount)
{
    int oldRowCount = this->rowCount();
    if(oldRowCount < newRowCount)
    {
        this->setRowCount(newRowCount);
    } else if(oldRowCount > newRowCount)
    {
        removeRows(newRowCount, oldRowCount - newRowCount);
    } else
    {
        //do nothing
    }
}

void HqTableWidget::removeRows(int start, int count)
{
    for(int i=0; i<count; i++)
    {
        for(int k=0; k<this->columnCount(); k++)
        {
            QStkTableWidgetItem *item = (QStkTableWidgetItem*)(this->item(start, k));
            if(item)
            {
                delete item;
                item = 0;
            }
        }
        this->removeRow(start);
    }
}

void HqTableWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    qDebug()<<__FILE__<<__FUNCTION__<<__LINE__;
    mCustomContextMenu->popup(QCursor::pos());
}

void HqTableWidget::initPageCtrlMenu()
{
    QMenu *menu = new QMenu(QStringLiteral("页面控制"), this);
    QList<QAction*> actlist;

    QList<struMenu> itemlist;
    itemlist.append(struMenu(QStringLiteral("首页"), FIRST_PAGE));
    itemlist.append(struMenu(QStringLiteral("前一页"), PRE_PAGE));
    itemlist.append(struMenu(QStringLiteral("后一页"), NEXT_PAGE));
    itemlist.append(struMenu(QStringLiteral("末页"), END_PAGE));

    foreach (struMenu item, itemlist) {
        QAction *act = new QAction(this);
        act->setText(item.mDisplayText);
        act->setData(item.mCmd);
        connect(act, &QAction::triggered, this, &HqTableWidget::slotSetDisplayPage);
        actlist.append(act);
    }

    menu->addActions(actlist);

    insertContextMenu(menu);
}

QAction* HqTableWidget::insertContextMenu(QMenu *menu)
{
    if(!menu) return 0;
    if(!mCustomContextMenu) mCustomContextMenu = new QMenu(this);
    return mCustomContextMenu->addMenu(menu);
}

void HqTableWidget::insertContextMenu(QAction *act)
{
    if(!act) return;
    if(!mCustomContextMenu) mCustomContextMenu = new QMenu(this);
    mCustomContextMenu->addAction(act);
}

void HqTableWidget::slotSetDisplayPage()
{
    QAction *act = (QAction*) sender();
    if(!act) return;

    int val = act->data().toInt();
    emit signalDisplayPage(val);

}

void HqTableWidget::slotCellDoubleClicked(int row, int col)
{
    return;
}

void HqTableWidget::slotCellClicked(int row, int col)
{
    this->horizontalHeader()->setHighlightSections(false);
}



