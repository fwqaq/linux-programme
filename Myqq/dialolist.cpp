#include "dialolist.h"
#include "ui_dialolist.h"
#include <QToolButton>
#include <vector>
#include <widget.h>
#include <QMessageBox>
#include <QDebug>
DialoList::DialoList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialoList)
{
    ui->setupUi(this);
    //设置标题
    setWindowTitle("qq");
    //设置图标
    setWindowIcon(QPixmap(":/images/qq.png"));
    //准备我们的图标和名字
    QList<QString>namelist;
    namelist << "水票奇缘" << "依梦如兰"<<"北京人"<<"Cherry"<<"坦然"
             <<"grilBaby"<<"落水无痕"<<"清末暖暖"<<"无语";
    QStringList iconNameList;//图标资源列表
    iconNameList << "Cherry"<<"dr"<<"ftbz"<<"wy"<<
                    "jj"<<"lswh"<<"qmnn"<<"spqy"<<"ymrl";
    //写入容器 九个按钮
    QVector<QToolButton*> vToolBtn;
    for(int i = 0;i < 9; i++){
        //设置头像
       QToolButton* btn = new QToolButton;
       //设置文字
       btn->setText(namelist[i]);
       //设置头像
       QString str = QString(":/images/%1.png").arg(iconNameList.at(i));
       btn->setIcon(QPixmap(str));
       //设置头像大小
       btn->setIconSize(QPixmap(str).size());
        //设置按钮的风格
       btn->setAutoRaise(true);
       //设置文字和图片一起显示
       btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
       //加到垂直布局中
       ui->vLayout->addWidget(btn);
       //加入到容器中方便以后的点击操作
       vToolBtn.push_back(btn);
       isShow.push_back(false);
    }

   //对九个按钮添加信号槽
   for(int i  =0;i < vToolBtn.size(); i++){
       connect(vToolBtn[i],&QToolButton::clicked,[=](){
            qDebug()<<"i="<<i<<"  "<<isShow[i];
           //弹出聊天对话框,查看是否已经打开了
           if(isShow[i]){
               QString str = QString("%1窗口已经被打开了").arg(vToolBtn[i]->text());
               QMessageBox::warning(this,"警告",str);
               return;
           }
           isShow[i] = true;
           //在构造聊天窗口时候告诉这个窗口他的名字
           //第一个是0表示的是一顶层弹出  参数二:窗口的名字  (修改我们的原构造函数)
           Widget* widget  =  new Widget(0,vToolBtn[i]->text());//点击头像后弹出的界面，
           //设置标题 图片
           widget->setWindowTitle(vToolBtn[i]->text());
           widget->setWindowIcon(vToolBtn[i]->icon());
           widget->show();
           connect(widget,&Widget::closeWidget,[=](){
               isShow[i] = false;
           });
       });
   }
}

DialoList::~DialoList()
{
    delete ui;
}


























