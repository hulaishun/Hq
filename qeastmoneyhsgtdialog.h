#ifndef QEASTMONEYHSGTDIALOG_H
#define QEASTMONEYHSGTDIALOG_H

#include <QDialog>
#include "dbservices/dbservices.h"

namespace Ui {
class QEastMoneyHSGTDialog;
}

class QEastMoneyHSGTDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QEastMoneyHSGTDialog(QWidget *parent = 0);
    ~QEastMoneyHSGTDialog();

private slots:
    void slotRecvTop10Infos(const QList<ChinaShareExchange>& list);
    void on_SearchBTB_clicked();

    void on_MarketCHK_clicked(bool checked);

    void on_DateCHK_clicked(bool checked);

    void on_CodeCHK_clicked(bool checked);

    void on_tableWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::QEastMoneyHSGTDialog *ui;
};

#endif // QEASTMONEYHSGTDIALOG_H
