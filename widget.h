#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QNetworkReply>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void readHttpRelpy(QNetworkReply *reply);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_searchbtn_clicked();

private:
    Ui::Widget *ui;
    QPoint offset;      // 鼠标偏移值
    QNetworkReply *reply;
    QNetworkAccessManager *manager;
    void parseWeaterJsonData(QByteArray rawData);       // 处理返回的json数据
    QString getCitycodeFromName(QString cityName);      // 查找城市id
    void drawHighTemLine();
    void drawLowTemLine();
};

#endif // WIDGET_H
