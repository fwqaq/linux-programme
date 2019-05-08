#include "widget.h"
#include "ui_widget.h"
#include <QDataStream>
#include <QMessageBox>
#include <QDateTime>
#include <QColorDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
Widget::Widget(QWidget *parent,QString name) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //初始化操作
    udpSocket = new QUdpSocket(this);
    //用户名的获取
    uName = name;
    //端口号
    this->port = 9999;
    //绑定端口号  (参数是共享地址加上短线重连)
    udpSocket->bind(this->port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    //发送新用户进入
    sndMsg(UsrEnter);

    //点击发送按钮发送消息
    connect(ui->sendButton,&QPushButton::clicked,[=](){
       sndMsg(Msg);
    });
    //监听别人发送的数据
    connect(udpSocket,&QUdpSocket::readyRead,this,&Widget::ReceiveMessage);

    //点击按钮,退出窗口
    connect(ui->exitBtn,&QPushButton::clicked,[=](){
       this->close();
    });

    //字体设置
    connect(ui->fontCbx,&QFontComboBox::currentFontChanged,[=](const QFont &font){
       ui->msgTxtEdit->setCurrentFont(font);
       ui->msgBrowser->setFocus();
    });
    //设置字号

    void(QComboBox::*cbxsiganl)(const QString&text)= &QComboBox::currentIndexChanged;
    connect(ui->sizeCbx,cbxsiganl,[=](const QString &text){
        ui->msgTxtEdit->setFontPointSize(text.toDouble());
        ui->msgBrowser->setFocus();
    });
    //加粗
    connect(ui->boldTBun,&QToolButton::clicked,[=](bool isCheck){
        if(isCheck){
              ui->msgTxtEdit->setFontWeight(QFont::Bold);
        }
        else {
            ui->msgTxtEdit->setFontWeight(QFont::Normal);
        }
    });

    //倾斜
    connect(ui->italicTBtn,&QToolButton::clicked,[=](bool check){
       ui->msgTxtEdit->setFontItalic(check);
    });
    //下划线
    connect(ui->underlineTBtn,&QToolButton::clicked,[=](bool check){
       ui->msgTxtEdit->setFontUnderline(check);
    });
    //字体的颜色
    connect(ui->colorTBtn,&QToolButton::clicked,[=](){
        QColor color = QColorDialog::getColor(Qt::red);
        ui->msgTxtEdit->setTextColor(color);
    });
    //清空聊天记录
    connect(ui->clearTBtn,&QToolButton::clicked,[=](){
       ui->msgBrowser->clear();
    });

    //保存liao'tia聊天记录
    connect(ui->saveTBtn,&QToolButton::clicked,[=](){
        if(ui->msgBrowser->document()->isEmpty()){
            QMessageBox::warning(this,"警告","聊天记录是空的");
            return;
        }
      QString path = QFileDialog::getSaveFileName(this,"保存记录","聊天记录","*.txt");
      if(path.isEmpty()){
          QMessageBox::warning(this,"警告","路径是空的");
      }else{
          QFile file(path);
          //在打开模式中加上换行操作
          file.open(QIODevice::WriteOnly | QIODevice::Text);
          QTextStream stream(&file);
          stream << ui->msgBrowser->toPlainText();
          file.close();
      }
    });
}
void Widget::ReceiveMessage(){
    //拿到数据报文
    //获取长度
    qint64 size = udpSocket->pendingDatagramSize();
    QByteArray array = QByteArray(10000,0);
    udpSocket->readDatagram(array.data(),size);
    //  解析数据
    //第一段 类型  第二段 用户名  第三段 具体的数据
    QDataStream stream (&array,QIODevice::ReadOnly);
    int msgType;//读取到类型
    QString usrName;
    QString msg;
    //获取当前事件
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    stream>>msgType;
    switch(msgType){
    case UsrLeft:
    stream >>usrName;
    usrLeft(usrName,time);
        break;
        case Msg:
            stream >>usrName>>msg;
            //追加聊天记录
            ui->msgBrowser->setTextColor(Qt::blue);
            ui->msgBrowser->append("["+usrName+"]"+time);
            ui->msgBrowser->append(msg);
             break;
        case UsrEnter://新用户进入
            //更新右侧TableWidget
            stream>>usrName;
            bool isEmpty =  ui->userTblWidget->findItems(usrName,Qt::MatchExactly).isEmpty();
            if(isEmpty){
             QTableWidgetItem* usr = new QTableWidgetItem(usrName);
             //插入行
             ui->userTblWidget->insertRow(0);
             ui->userTblWidget->setItem(0,0,usr);
             //追加聊天记录
             ui->msgBrowser->setTextColor(Qt::red);
             ui->msgBrowser->append(QString("%1 于 %2 上线了").arg(usrName).arg(time));
             //在先人数更新,
             ui->userNumLbl->setText(QString("在线用户：%1人").arg(ui->userTblWidget->rowCount()));

             //把自身的信息广播出去
             sndMsg(UsrEnter);
             }
        break;

     }

}
void Widget::usrLeft(QString usrname, QString time){
    //更新我们的右侧table
    bool isempty = ui->userTblWidget->findItems(usrname,Qt::MatchExactly).isEmpty();
    if(!isempty){

        int row = ui->userTblWidget->findItems(usrname,Qt::MatchExactly).first()->row();
        ui->userTblWidget->removeRow(row);
        //追加聊天记录
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->append(QString("%1 于 %2 离开").arg(usrname).arg(time));
        //在线人数更新
        ui->userNumLbl->setText(QString("在线用户：%1人").arg(ui->userTblWidget->rowCount()));
    }

}
void Widget::closeEvent(QCloseEvent *event){

    emit this->closeWidget();
    sndMsg(UsrLeft);
    //断开套接字
    udpSocket->close();
    udpSocket->destroyed();

}
void Widget::sndMsg(MsgType type){
         //发送的信息分为三种类型
        //发送到数据作为分段处理。1.类型 2.用户名 3.具体的内容
        QByteArray array;
        QDataStream stream(&array,QIODevice::WriteOnly);//往数据流中写数据
        stream <<type<<getUsr();//type就是第一段，类型 和第二段用户名
        switch(type){
        case Msg://普通消息的处理
            if(ui->msgTxtEdit->toPlainText() == ""){
                QMessageBox::warning(this,"警告","发送内容不能为空！");
                return;
            }
            stream<<getMsg();
            break;
        case UsrEnter://新用户进入消息

            break;
        case UsrLeft://用户离开的消息

            break;
        }
    //书写报文  因为所有人都要看到，所以使用广播发送
    udpSocket->writeDatagram(array,QHostAddress::Broadcast,port);
}
QString Widget::getUsr(){

    return this->uName;
}
QString Widget::getMsg(){
    QString str = ui->msgTxtEdit->toHtml();
    //清空输入框
    ui->msgTxtEdit->clear();
    ui->msgTxtEdit->setFocus();
    return str;
}
Widget::~Widget()
{
    delete ui;
}













