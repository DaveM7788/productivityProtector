// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "window.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>

int listSelectIdx = -1;
//! [0]
Window::Window()
{
    createIconGroupBox();
    createMessageGroupBox();

    iconLabel->setMinimumWidth(durationLabel->sizeHint().width());

    createActions();
    createTrayIcon();

    // connect(showMessageButton, &QAbstractButton::clicked, this, &Window::showMessage);
    connect(addToListButton, &QAbstractButton::clicked, this, &Window::addItemToListClicked);
    connect(deleteFromListButton, &QAbstractButton::clicked, this, &Window::deleteItemFromListClicked);
    connect(saveListDataButton, &QAbstractButton::clicked, this, &Window::saveListClicked);
    connect(listItemsToWatch, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onListItemClicked(QListWidgetItem*)));


    connect(showIconCheckBox, &QAbstractButton::toggled, trayIcon, &QSystemTrayIcon::setVisible);
    connect(iconComboBox, &QComboBox::currentIndexChanged,
            this, &Window::setIcon);
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Window::messageClicked);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(iconGroupBox);
    mainLayout->addWidget(messageGroupBox);
    setLayout(mainLayout);

    iconComboBox->setCurrentIndex(1);
    trayIcon->show();

    setWindowTitle(tr("Productivity Protector"));
    resize(400, 300);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));
    timer->start(1 * 60 * 1000);

    initListFromStorage();
}
//! [0]

//! [1]
void Window::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}
//! [1]

//! [2]
void Window::closeEvent(QCloseEvent *event)
{
    if (!event->spontaneous() || !isVisible())
        return;
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("Productivity Protector"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}
//! [2]

//! [3]
void Window::setIcon(int index)
{
    QIcon icon = iconComboBox->itemIcon(index);
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->setToolTip(iconComboBox->itemText(index));
}
//! [3]

//! [4]
void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1) % iconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}
//! [4]

//! [5]
void Window::showMessage() {
    QString minutes = QString::number(durationSpinBox->value());
    QString message = "No changes detected within last " + minutes + " minutes";
    QIcon msgIcon = trayIcon->icon();
    trayIcon->showMessage("Productivity Protector", message, msgIcon, 15 * 1000);
}
//! [5]

//! [6]
void Window::messageClicked() {
    QMessageBox::information(nullptr, tr("Productivity Protector"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}
//! [6]

void Window::createIconGroupBox() {
    iconGroupBox = new QGroupBox(tr("Tray Icon"));

    iconLabel = new QLabel("Icon:");

    iconComboBox = new QComboBox;
    iconComboBox->addItem(QIcon(":/images/bad.png"), tr("Bad"));
    iconComboBox->addItem(QIcon(":/images/heart.png"), tr("Heart"));
    iconComboBox->addItem(QIcon(":/images/trash.png"), tr("Trash"));

    showIconCheckBox = new QCheckBox(tr("Show icon"));
    showIconCheckBox->setChecked(true);

    QHBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(iconLabel);
    iconLayout->addWidget(iconComboBox);
    iconLayout->addStretch();
    iconLayout->addWidget(showIconCheckBox);
    iconGroupBox->setLayout(iconLayout);
}

void Window::addItemToListClicked() {
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Folder", QDir::homePath());

    QListWidgetItem *item = new QListWidgetItem;
    item->setText(folderPath);
    item->setCheckState(Qt::Unchecked);
    listItemsToWatch->addItem(item);
}

void Window::deleteItemFromListClicked() {
    if (listSelectIdx != -1) {
        QListWidgetItem *toDelete = listItemsToWatch->takeItem(listSelectIdx);
        delete toDelete;
        listSelectIdx = -1;
    }
}

void Window::saveListClicked() {
    QFile storeFile("storeFile.txt");
    if (storeFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&storeFile);
        for (int i = 0; i < listItemsToWatch->count(); ++i) {
            QListWidgetItem *item = listItemsToWatch->item(i);
            QString path = item->text();
            QString isChecked = (item->checkState()) ? "1" : "0";
            QString lineToWrite = isChecked + "," + path + '\n';
            stream << lineToWrite;
        }
        storeFile.close();
    }
}

void Window::initListFromStorage() {
    QFile storeFile("storeFile.txt");
    if (storeFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&storeFile);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            QString isChecked = line.left(1);
            QString actualPath = line.mid(2, line.length());
            auto checkState = (isChecked == "1") ? Qt::Checked : Qt::Unchecked;

            QListWidgetItem *item = new QListWidgetItem;
            item->setText(actualPath);
            item->setCheckState(checkState);
            listItemsToWatch->addItem(item);
        }
        storeFile.close();
    }
}

void Window::timerTick() {
    bool needsToNotify = true;
    for (int i = 0; i < listItemsToWatch->count(); ++i) {
        QListWidgetItem *item = listItemsToWatch->item(i);
        if (item->checkState() == Qt::Checked) {
            QString checkThisPath = item->text();
            bool foundRecentChange = checkFolderForChange(checkThisPath);
            if (foundRecentChange) {
                needsToNotify = false;
                break;
            }
        }
    }

    if (needsToNotify) {
        showMessage();
    }
}

void Window::onListItemClicked(QListWidgetItem *item) {
    for (int i = 0; i < listItemsToWatch->count(); ++i) {
        if (listItemsToWatch->item(i) == item) {
            listSelectIdx = i;
            break;
        }
    }
}

bool Window::checkFolderForChange(QString path) {
    int minutes = durationSpinBox->value();
    qint64 mustBeAfter = QDateTime::currentMSecsSinceEpoch() - (minutes * 60 * 1000);

    QDirIterator it(QDir(path), QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFile file(it.next());
        QFileInfo fileInfo(it.fileInfo());
        QDateTime lastModified = fileInfo.lastModified();

        //qDebug() << file.fileName() << "last modified " << lastModified;
        qDebug() << "mustBeAfter" << mustBeAfter << lastModified.toMSecsSinceEpoch();
        if (lastModified.toMSecsSinceEpoch() > mustBeAfter) {
            qDebug() << file.fileName() << " last modified time GREATER!!!! than target. good means recent change here ";
            return true;
        }
    }
    return false;
}

void Window::createMessageGroupBox() {
    messageGroupBox = new QGroupBox(tr("Folders To Check"));

    listItemsToWatch = new QListWidget;

    addToListButton = new QPushButton(tr("Add"));
    deleteFromListButton = new QPushButton(tr("Delete"));
    saveListDataButton = new QPushButton(tr("Save"));

    durationLabel = new QLabel(tr("Changes must occur within previous"));
    durationSpinBox = new QSpinBox;
    durationSpinBox->setRange(5, 60);
    durationSpinBox->setSuffix(" m");
    durationSpinBox->setValue(15);

    durationWarningLabel = new QLabel(tr("(some systems might ignore this "
                                         "hint)"));
    durationWarningLabel->setIndent(10);

    QGridLayout *messageLayout = new QGridLayout;
    messageLayout->addWidget(listItemsToWatch, 0, 0, 1, 5);
    messageLayout->addWidget(addToListButton, 1, 0);
    messageLayout->addWidget(deleteFromListButton, 1, 1);
    messageLayout->addWidget(saveListDataButton, 1, 2);
    messageLayout->addWidget(durationLabel, 2, 0);
    messageLayout->addWidget(durationSpinBox, 2, 1);
    messageGroupBox->setLayout(messageLayout);
}

void Window::createActions() {
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void Window::createTrayIcon() {
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

#endif
