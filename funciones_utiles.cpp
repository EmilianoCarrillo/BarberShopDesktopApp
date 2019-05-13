#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QtCore>
#include <QSqlTableModel>
#include <QSqlQuery>

void MainWindow::changeImage() {
    QString randomImage = ":/assets/assets/login_" + QString::number(rand() % (16-1 + 1) + 1) + ".jpg";
    QPixmap logo(randomImage);
    int widWidth = this->ui->loginImg->width();
    int widHeight = this->ui->loginImg->height();
    ui->loginImg->setPixmap(logo.scaled(widWidth, widHeight, Qt::KeepAspectRatioByExpanding));
    timer->start(5000);
}
