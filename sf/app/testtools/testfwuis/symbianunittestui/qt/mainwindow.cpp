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

#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QGridLayout>
#include <QTreeWidgetItem>
#include <QKeyEvent>
#include <QFile>
#include <QLatin1String>
#include <QApplication>
#include "mainwindow.h"
#include <symbianunittestversion.h>
#include "dialogaddtests.h"
#include "tabwidgettestrun.h"
#include "dialogsettings.h"
#include "dialogmsg.h"

static const QString APPNAME = "symbianunittestqt";
static const QString STYLESHEET = ":/qss/coffee.qss";
//Symbian EKeyLeftArrow = 0x807
static const int KeyLeftArrow = 63495;
//Symbian EKeyRightArrow = 0x808
static const int KeyRightArrow = 63496;
//Symbian EKeyUpArrow = 0x809
static const int KeyUpArrow = 63497;
//Symbian EKeyDownArrow = 0x80a
static const int KeyDownArrow = 63498;
//Symbian EKeyEnter = 0x10
static const int KeyEnter = 0x10;

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent)
    {
    setTitle();
    QWidget* centralWidget;
    centralWidget = new QWidget(this);
    centralWidget->setContextMenuPolicy(Qt::NoContextMenu);
    setCentralWidget(centralWidget);
    load();
    }

MainWindow::~MainWindow()
    {

    }

void MainWindow::SetStyle(const QString& styleFile)
    {
    QFile file(styleFile);
    bool rst = file.open(QFile::ReadOnly);    
    if(rst)
        {
        QString styleSheet = QLatin1String(file.readAll());    
        qApp->setStyleSheet(styleSheet);    
        }
    file.close();
    }

void MainWindow::popupDialogAddTests()
    {
    dlgAddTests->clear();
    dlgAddTests->showMaximized();
    }

void MainWindow::popupDialogSettings()
    {
    dlgSettings->restoreSettings();
    dlgSettings->showMaximized();
    }

void MainWindow::popupDialogAbout()
    {
    dlgAbout->showMsg(tr("%1 v%2.%3.%4").arg(APPNAME) .arg(
            SUT_MAJOR_VERSION) .arg(SUT_MINOR_VERSION).arg(SUT_BUILD_VERSION));
    }

void MainWindow::setTitle()
    {
    this->setWindowTitle(tr("%1 v%2.%3.%4").arg(APPNAME) .arg(
            SUT_MAJOR_VERSION) .arg(SUT_MINOR_VERSION).arg(SUT_BUILD_VERSION));
    }

void MainWindow::load()
    {
    createTabWidget();
    loadTabWidget();
    setupTabWidgetEventHandlers();
    createDialogs();
    setupDialogEventHandlers();
    createMenu();
    setupMenuEventHandlers();
    loadMenu();
    setupKeyEventHandlers();
    SetStyle(STYLESHEET);
    }

void MainWindow::createMenu()
    {
    atnAddTests = new QAction(tr("Add Tests"), this);
    menuMarkUnmark = new QMenu(tr("Mark / Unmark"), this);
    menuMarkUnmark->setEnabled(false);
    atnMark = new QAction(tr("Mark"), menuMarkUnmark);
    atnMarkAll = new QAction(tr("Mark All"), menuMarkUnmark);
    atnUnmark = new QAction(tr("Unmark"), menuMarkUnmark);
    atnUnmarkAll = new QAction(tr("Unmark All"), menuMarkUnmark);
    atnRun = new QAction(tr("Run"), this);
    atnRun->setEnabled(false);
    atnSettings = new QAction(tr("Settings"), this);
    atnAbout = new QAction(tr("About"), this);
    atnExit = new QAction(tr("Exit"), this);
    }

void MainWindow::setupMenuEventHandlers()
    {
    // add tests
    connect(atnAddTests, SIGNAL(triggered()), this,
            SLOT(popupDialogAddTests()));
    // Mark / Unmark single / all
    connect(atnMark, SIGNAL(triggered()), tabWdgtTestRun,
            SLOT(selectCurrentCase()));
    connect(atnMarkAll, SIGNAL(triggered()), tabWdgtTestRun,
            SLOT(selectAllCases()));
    connect(atnUnmark, SIGNAL(triggered()), tabWdgtTestRun,
            SLOT(deselectCurrentCase()));
    connect(atnUnmarkAll, SIGNAL(triggered()), tabWdgtTestRun,
            SLOT(deselectAllCases()));
    // Run tests
    connect(atnRun, SIGNAL(triggered()), tabWdgtTestRun, SLOT(runTests()));
    // set options to run cases
    connect(atnSettings, SIGNAL(triggered()), this,
            SLOT(popupDialogSettings()));

    connect(atnAbout, SIGNAL(triggered()), this, SLOT(popupDialogAbout()));
    connect(atnExit, SIGNAL(triggered()), this, SLOT(close()));
    }

void MainWindow::createTabWidget()
    {
    tabWdgtTestRun = new TabWidgetTestRun(this);
    }

void MainWindow::loadMenu()
    {
    menuBar()->clear();
    menuBar()->addAction(atnAddTests);
    menuMarkUnmark->addAction(atnMark);
    menuMarkUnmark->addAction(atnMarkAll);
    menuMarkUnmark->addAction(atnUnmark);
    menuMarkUnmark->addAction(atnUnmarkAll);
    menuBar()->addAction(menuMarkUnmark->menuAction());
    menuBar()->addAction(atnRun);
    menuBar()->addAction(atnSettings);
    menuBar()->addAction(atnAbout);
    menuBar()->addAction(atnExit);
    }

void MainWindow::loadTabWidget()
    {
    QGridLayout* tabLayout = new QGridLayout(this);
    tabLayout->setVerticalSpacing(2);
    tabLayout->setHorizontalSpacing(2);
    tabLayout->setSpacing(2);
    tabLayout->setMargin(2);
    tabLayout->addWidget(tabWdgtTestRun, 0, 0);
    centralWidget()->setLayout(tabLayout);
    }

void MainWindow::setupTabWidgetEventHandlers()
    {
    connect(tabWdgtTestRun, SIGNAL(testsAdded()), this,
            SLOT(enableMenuMarkUnmark()));
    connect(tabWdgtTestRun, SIGNAL(selectedTestsChanged(bool, bool, bool)), this,
            SLOT(setRunMarkUnmarkEnabled(bool, bool, bool)));
    // current tab changed
    connect(tabWdgtTestRun, SIGNAL(currentTabChangedToTests(bool)), this,
            SLOT(changCurrentTabToTests(bool)));
    connect(tabWdgtTestRun, SIGNAL(currentTabChangedToExecution()), this,
            SLOT(changCurrentTabToExecution()));
    }

void MainWindow::createDialogs()
    {
    dlgAddTests = new DialogAddTests(this);
    dlgSettings = new DialogSettings(this);
    dlgAbout = new DialogMsg(this);
    }

void MainWindow::setupDialogEventHandlers()
    {
    connect(dlgAddTests, SIGNAL(testsSaved(QStringList)), tabWdgtTestRun,
            SLOT(addTests(QStringList)));
    connect(dlgSettings, SIGNAL(settingsSaved(const Settings*)),
            tabWdgtTestRun, SLOT(saveSettings(const Settings*)));
    }

void MainWindow::changCurrentTabToTests(bool hasTests)
    {
    if (hasTests)
        {
        menuMarkUnmark->setEnabled(true);
        }
    }

void MainWindow::changCurrentTabToExecution()
    {
    menuMarkUnmark->setEnabled(false);
    }

void MainWindow::setRunMarkUnmarkEnabled(bool noneSelected, bool allSelected, bool curSelected)
    {
    atnRun->setEnabled(!noneSelected);
    atnUnmarkAll->setEnabled(!noneSelected);
    atnUnmark->setEnabled(curSelected);
    atnMark->setEnabled(!curSelected);
    atnMarkAll->setEnabled(noneSelected || !allSelected);
    }

void MainWindow::enableMenuMarkUnmark()
    {
    menuMarkUnmark->setEnabled(true);
    atnMarkAll->setEnabled(true);
    atnMark->setEnabled(true);
    atnUnmark->setEnabled(false);
    atnUnmarkAll->setEnabled(false);
    }

void MainWindow::keyPressEvent(QKeyEvent* event)
    {
    // Handle arrow keys and selection key events
    switch (event->nativeVirtualKey())
        {
        case KeyLeftArrow:
            {
            // TODO
            emit this->leftArrowPressed();
            break;
            }
        case KeyRightArrow:
            {
            // TODO
            emit this->rightArrowPressed();
            break;
            }
        case KeyUpArrow:
            {
            // TODO
            emit this->upArrowPressed();
            break;
            }
        case KeyDownArrow:
            {
            // TODO
            emit this->downArrowPressed();
            break;
            }
        case KeyEnter:
            {
            // TODO
            emit this->enterPressed();
            break;
            }
        default:
            {
            break;
            }
        }
    }

void MainWindow::setupKeyEventHandlers()
    {
    connect(this, SIGNAL(leftArrowPressed()), tabWdgtTestRun,
            SLOT(pressLeftArrow()));
    connect(this, SIGNAL(rightArrowPressed()), tabWdgtTestRun,
            SLOT(pressRightArrow()));
    connect(this, SIGNAL(upArrowPressed()), tabWdgtTestRun,
            SLOT(pressUpArrow()));
    connect(this, SIGNAL(downArrowPressed()), tabWdgtTestRun,
            SLOT(pressDownArrow()));
    connect(this, SIGNAL(enterPressed()), tabWdgtTestRun, SLOT(pressEnter()));
    }
