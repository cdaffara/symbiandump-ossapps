/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: QT C++ main window Class.
 *
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <QtGui>

// Forward declarations
class QAction;
class QTabWidget;
class QPushButton;
class QWidget;
class QMenu;
class DialogAddTests;
class DialogSettings;
class TabWidgetTestRun;
class QTreeWidgetItem;
class QKeyEvent;
class Settings;
class DialogMsg;

class MainWindow : public QMainWindow
    {
Q_OBJECT

public:
    // ctor and dtor
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    void setTitle();
    void load();
    void createMenu();
    void loadMenu();
    void createTabWidget();
    void loadTabWidget();
    void createDialogs();
    void setupMenuEventHandlers();
    void setupDialogEventHandlers();
    void setupTabWidgetEventHandlers();
    void setupKeyEventHandlers();
    void SetStyle(const QString&);

public:
    void keyPressEvent(QKeyEvent *);
    
signals:
    void testsAdded(QStringList);
    
    // Key press events
    void leftArrowPressed();
    void rightArrowPressed();
    void upArrowPressed();
    void downArrowPressed();
    void enterPressed();

private slots:
    void popupDialogAddTests();
    void popupDialogSettings();
    void popupDialogAbout();

    void changCurrentTabToTests(bool hasTests);
    void changCurrentTabToExecution();
    void enableMenuMarkUnmark();
    void setRunMarkUnmarkEnabled(bool noneSelected, bool allSelected, bool curSelected);

private:
    /*
     * Menu Items.
     */
    // Cases
    QAction* atnAddTests;
    QMenu* menuMarkUnmark;
    QAction* atnMark;
    QAction* atnMarkAll;
    QAction* atnUnmark;
    QAction* atnUnmarkAll;
    QAction* atnRun;
    QAction* atnSettings;
    // App
    QAction* atnAbout;
    QAction* atnExit;

    // Add Tests dialog
    DialogAddTests* dlgAddTests;
    
    // Settings dialog
    DialogSettings* dlgSettings;

    // Tab widget containing cases list and result of execution.
    TabWidgetTestRun* tabWdgtTestRun;
    
    // About dialog
    DialogMsg* dlgAbout;
    };

#endif /* MAINWINDOW_H_ */
