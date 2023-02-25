// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QApplication>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QMessageBox>
#include <QStyleFactory>
#include "window.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(productivity_protector);

    QApplication app(argc, argv);

    qApp->setStyle(QStyleFactory::create("Fusion"));
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(defaultFont.pointSize()+2);
    qApp->setFont(defaultFont);

    QPalette p = qApp->palette();
    p.setColor(QPalette::Window, QColor(53,53,53));
    p.setColor(QPalette::Button, QColor(53,53,53));
    p.setColor(QPalette::Highlight, QColor(142,45,197));
    p.setColor(QPalette::ButtonText, QColor(255,255,255));
    p.setColor(QPalette::WindowText, QColor(255,255,255));
    p.setColor(QPalette::Base,QColor(90,90,90));
    p.setColor(QPalette::Text,Qt::white);
    qApp->setPalette(p);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("Productivity Protector"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    Window window;
    window.show();
    return app.exec();
}

#else

#include <QLabel>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QString text("QSystemTrayIcon is not supported on this platform");

    QLabel *label = new QLabel(text);
    label->setWordWrap(true);

    label->show();
    qDebug() << text;

    app.exec();
}

#endif
