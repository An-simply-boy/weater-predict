#include "widget.h"
#include "ui_widget.h"

#include <QMouseEvent>
#include <QDebug>
#include <qnetwork.h>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QPainter>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    // QNetworkRequest指定请求的url地址
    QUrl urltianqiv61("http://v1.yiketianqi.com/api?unescape=1&version=v61&appid=12763383&appsecret=ug0kh3iA");
    QUrl urltianqiv63("http://v1.yiketianqi.com/api?unescape=1&version=v63&appid=12763383&appsecret=ug0kh3iA");
    QUrl urlError("http://v1.yiketianqi.co");

    // 由QNetworkAccessManager发起get请求
    manager = new QNetworkAccessManager(this);
    QNetworkRequest res(urltianqiv61);
    reply = manager->get(res);

    //QNetworkReply网络请求后进行数据的处理
    connect(manager, &QNetworkAccessManager::finished, this, &Widget::readHttpRelpy);
    ui->widgetHweater->installEventFilter(this);
    ui->widgetLweater->installEventFilter(this);
}

// 鼠标右键按下移动窗口
void Widget::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        // qDebug() << this->pos() << event->pos();
        offset = event->globalPos() - this->pos();
    }
}
void Widget::mouseMoveEvent(QMouseEvent *event){
    this->move(event->globalPos() - offset);
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->widgetHweater && event->type() == QEvent::Paint){
        drawHighTemLine();
    }
}

// 处理请求结果
void Widget::readHttpRelpy(QNetworkReply *reply)
{
    int resCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(resCode == 200 && reply->error() == QNetworkReply::NoError){
        // 大多数服务器请求返回的字符编码格式是Utf8的，需要进行字符编码转换
        QByteArray data = reply->readAll();
        qDebug() << reply->rawHeaderList();
        qDebug() << QString::fromUtf8(data);
        parseWeaterJsonData(data);
    }else{
        qDebug() << "请求失败" << reply->errorString();
        QMessageBox::warning(this,"错误","网络请求失败",QMessageBox::Ok);
    }
}

// 处理返回的json数据并渲染数据
void Widget::parseWeaterJsonData(QByteArray rawData)
{
    qDebug() << "Json is parsing...";
    QJsonDocument jsonObj = QJsonDocument::fromJson(rawData);
    if(!jsonObj.isNull() && jsonObj.isObject()){
        QJsonObject objRoot = jsonObj.object();
        // 解析数据
        ui->date->setText(objRoot["date"].toString());
        ui->week->setText(objRoot["week"].toString());
        ui->city->setText(objRoot["city"].toString() + "市");
        ui->WD->setText(objRoot["tem"].toString());
        ui->TQ->setText(objRoot["wea"].toString() + "℃");
        ui->WDrange->setText(objRoot["tem2"].toString() + "~" + objRoot["tem1"].toString() + "℃");
        ui->FX->setText(objRoot["win"].toString());
        ui->FXdegree->setText(objRoot["win_speed"].toString());
        ui->SDdegree->setText(objRoot["humidity"].toString());
        ui->KQZLdegree->setText(objRoot["air_level"].toString());
        ui->PMdegree->setText(objRoot["air_pm25"].toString());
        ui->airTip->setText(objRoot["air_tips"].toString());
    }else{
        qDebug() << "replydata is empty!";
    }
}

// 查找城市id
QString Widget::getCitycodeFromName(QString cityName){
    QFile file(":/weather_api/city_cn.json");
    file.open(QIODevice::ReadOnly);
    QByteArray cityJson = file.readAll();
    file.close();
    qDebug() << "btnpush...";
    // 判断，可优化
    QJsonDocument jsonDoc = QJsonDocument::fromJson(cityJson);
    if(jsonDoc.isArray()){
        QJsonArray jsonArr = jsonDoc.array();
        for(QJsonValue val : jsonArr){
            if(val.isObject()){
                QJsonObject jsonObj = val.toObject();
                if (jsonObj.contains("cityZh") && jsonObj["cityZh"].isString()) {
                    if(cityName == jsonObj["cityZh"].toString())
                        return jsonObj["id"].toString();
                }
            }
        }
    }
    return "";
}

void Widget::drawHighTemLine()
{
    QPainter pan(ui->widgetHweater);
    pan.setPen(Qt::red);

    pan.drawLine(QPoint(10,10),QPoint(20,20));
}

// 查找按键按下
void Widget::on_searchbtn_clicked()
{
    QString cityNameFromUser = ui->search->text();
    QString cityCodeFromUser = getCitycodeFromName(cityNameFromUser);
    if(cityCodeFromUser != NULL){
        QUrl urltianqi("http://v1.yiketianqi.com/api?unescape=1&version=v61&appid=12763383&appsecret=ug0kh3iA&cityid=" + cityCodeFromUser);
        qDebug() << urltianqi;
        manager->get(QNetworkRequest(urltianqi));
    }else{
        QMessageBox::warning(this,"错误","请输入正确的城市名称",QMessageBox::Ok);
    }
}

Widget::~Widget()
{
    delete ui;
}
