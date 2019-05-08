#ifndef DIALOLIST_H
#define DIALOLIST_H

#include <QWidget>

namespace Ui {
class DialoList;
}

class DialoList : public QWidget
{
    Q_OBJECT

public:
    explicit DialoList(QWidget *parent = nullptr);
    ~DialoList();
    //标识号，表示是否已经打开了
private:
    Ui::DialoList *ui;
    QVector<bool>isShow;
};

#endif // DIALOLIST_H
