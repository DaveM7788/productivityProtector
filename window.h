// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef WINDOW_H
#define WINDOW_H

#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QDialog>
#include <QListWidget>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QListWidget;
class QTimer;
QT_END_NAMESPACE

//! [0]
class Window : public QDialog
{
    Q_OBJECT

public:
    Window();

    void setVisible(bool visible) override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void setIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();
    void addItemToListClicked();
    void deleteItemFromListClicked();
    void saveListClicked();
    void timerTick();
    void onListItemClicked(QListWidgetItem *item);

private:
    void createStatusGroupBox();
    void createFoldersGroupBox();
    void createActions();
    void createTrayIcon();
    void initListFromStorage();
    void updateCheckNumbers();
    bool checkFolderForChange(QString path);

    QGroupBox *statusGroupBox;

    QGroupBox *messageGroupBox;
    QLabel *durationLabel;
    QLabel *durationWarningLabel;
    QSpinBox *durationSpinBox;

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QListWidget *listItemsToWatch;
    QPushButton *addToListButton;
    QPushButton *deleteFromListButton;
    QPushButton *saveListDataButton;

    QLabel *failedChecks;
    QLabel *successfulChecks;
    QCheckBox *enableChecks;

    QTimer *timer;
};
//! [0]

#endif // QT_NO_SYSTEMTRAYICON

#endif
