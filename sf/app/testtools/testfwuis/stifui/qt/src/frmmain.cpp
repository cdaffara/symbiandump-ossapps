/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: QT C++ based Class.
 *              
 */
#include <QtGui>
#include "frmmain.h"
#include "stfqtuicontroller.h"
#include "stfqtuimodel.h"
#include <QList>
#include "version.h"
#include <QCursor>
#include <QDesktopWidget>

const QString SELECTITEMHEADER = " * ";
const QString UNSELECTITEMHEADER = "   ";

FrmMain::FrmMain()
    {
    lastItemSelected = NULL;
    uiSetting = new UiSetting();
    createMenus();
    load();
    LoadSubMenu();
    
    QFile file(uiSetting->ReadSetting(KStyleSheet));
    bool rst = file.open(QFile::ReadOnly);    
    if(rst)
        {
        QString styleSheet = QLatin1String(file.readAll());    
        qApp->setStyleSheet(styleSheet);    
        }
        
    model = new StfQtUIModel();
    model->AddStifModelEventListener(this);
    controller = new StfQtUIController(model);
    controller->AddStfEventListener(this);
    loadContent();
    dlgOutput = new DlgOutput(controller);
    setSetting();
    }

FrmMain::~FrmMain()
    {
    model->AbortCase();
    controller->RemoveStfEventListener(this);
    model->RemoveStifModelEventListener(this);
    
    //lastItemSelected does not own any memory, don't need to delete.
    lastItemSelected = NULL;
    delete uiSetting;
    delete dlgOutput;
    delete controller;
    delete model;
    }

void FrmMain::paintEvent(QPaintEvent* event)
    {
    
    if(mainLayout != NULL)
        {
        QDesktopWidget* desktop = QApplication::desktop();
        QRect rect = desktop->screenGeometry(0);
        bool temp = false;
        if(rect.height() > rect.width())
            {
            temp = true;
            }
        
        if(temp != layoutType)
            {
            mainLayout->removeWidget(tabWidget);
            mainLayout->removeWidget(groupBox);
            if(temp)
                {
                mainLayout->addWidget(tabWidget, 0, 0);
                mainLayout->addWidget(groupBox, 1, 0, Qt::AlignBottom);
                mainLayout->setRowStretch(0,4);
                mainLayout->setRowStretch(1,1);
                }
            else
                {
                mainLayout->addWidget(tabWidget, 0, 0);
                mainLayout->addWidget(groupBox, 0, 1);//Qt::AlignRight
                //groupBox->setFixedSize(60,0);
                mainLayout->setColumnStretch(0,1);
                mainLayout->setColumnStretch(1,1);
                }
            layoutType = temp;
            
            }
        
        }
    event->accept();
    }

void FrmMain::setSetting()
    {
    controller->SetShowOutput(uiSetting->ReadSetting(KShowOutput) == "true");    
    // Apply filter changes
    QString newFilter = uiSetting->ReadSetting(KFilter);
    QString newFilterCaseSens = uiSetting->ReadSetting(KFilterCaseSens); 
    if(currentFilter != newFilter || currentFilterCaseSens != newFilterCaseSens)
        {
        // Store new filter for further use
        currentFilter = newFilter;
        currentFilterCaseSens = newFilterCaseSens;
        
        // Create and setup regular expression for wildcard searching
        QRegExp filter;
        filter.setPattern((newFilter == "") ? ("*") : (tr("*") + newFilter + tr("*")));
        filter.setCaseSensitivity((newFilterCaseSens == "true") ? (Qt::CaseSensitive) : (Qt::CaseInsensitive));
        filter.setPatternSyntax(QRegExp::Wildcard);

        // Go through top level entries (modules)
        bool isAnythingHidden = false;
        for(int i = 0; i < treeModuleList->topLevelItemCount(); i++)
            {
            QTreeWidgetItem* top = treeModuleList->topLevelItem(i);
            // And through test cases for each module
            for(int j = 0; j < top->childCount(); j++)
                {                
                QTreeWidgetItem *child = top->child(j);
                // Remove first three chars to get valid test case title
                QString title = (child->text(0)).mid(3);
                // Check if title is matching to filter and show or hide it
                if(filter.exactMatch(title))
                    {
                    child->setHidden(false);
                    }
                else
                    {
                    child->setHidden(true);
                    child->setText(0, child->text(0).replace(0, 3, UNSELECTITEMHEADER));
                    isAnythingHidden = true;
                    }
                }
            }

        if(isAnythingHidden)
            treeModuleList->headerItem()->setText(0, tr("Module List (filtered)"));
        else
            treeModuleList->headerItem()->setText(0, tr("Module List"));
        }
    }

void FrmMain::OnGetMessage(const QString& aMessage)
    {
    txtOutput->appendPlainText(aMessage);
    }

void FrmMain::OnRunningCaseChanged()
    {
    QList<CSTFCase> caseList = controller->GetCasesByStatus(EStatusRunning);
    lstStartedCases->clear();
    foreach(CSTFCase aCase, caseList)
            {
            lstStartedCases->addItem(aCase.Name());
            }
    if (caseList.size() != 0)
        {
        btnPauseCase->setEnabled(true);
        btnAbortCase->setEnabled(true);
        btnShowOutput->setEnabled(true);
        actPause->setEnabled(true);
        actAbort->setEnabled(true);
        actOutput->setEnabled(true);
        }
    else
        {
        btnPauseCase->setEnabled(false);
        btnAbortCase->setEnabled(false);
        btnShowOutput->setEnabled(false);
        actPause->setEnabled(false);
        actAbort->setEnabled(false);
        actOutput->setEnabled(false);
        }
    }

void FrmMain::OnCaseOutputChanged(const IStfEventListener::CaseOutputCommand& /*cmd*/, const QString& /*index*/, const QString& /*msg*/)
    {
    //nothing to do.
    }

void FrmMain::OnSetListChanged()
    {
    loadSetList();
    }

void FrmMain::OnCaseStatisticChanged()
    {
    loadStatistic();
    }

void FrmMain::createMenus()
    {
    //operateMenu = menuBar()->addMenu(tr("&Operation"));
    actAbout = new QAction(tr("&About"), this);
    connect(actAbout, SIGNAL(triggered()), this,
            SLOT(on_actAbout_triggered()));

    actExit = new QAction(tr("&Exit"), this);
    connect(actExit, SIGNAL(triggered()), this, SLOT(close()));

    actOpenFile = new QAction(tr("&Open Ini File"), this);
    connect(actOpenFile, SIGNAL(triggered()), this,
            SLOT(on_actOpenFile_triggered()));

    menuRunCase = new QMenu(tr("Run Selected Case(s)"), this->menuBar());
    
    actRunCaseSeq = new QAction(tr("Sequentially"), this);
    connect(actRunCaseSeq, SIGNAL(triggered()), this,
            SLOT(on_actRunCaseSeq_triggered()));

    actRunCasePar = new QAction(tr("Parallel"), this);
    connect(actRunCasePar, SIGNAL(triggered()), this,
            SLOT(on_actRunCasePar_triggered()));

    ////////////////////
    actReapeatRunSeq = new QAction(tr("Repeat run sequentially"), this);
    connect(actReapeatRunSeq, SIGNAL(triggered()), this,
            SLOT(on_actReapeatRunSeq_triggered()));
    
    actAddtoSet = new QAction(tr("Add cases to Set"), this);
    connect(actAddtoSet, SIGNAL(triggered()), this,
            SLOT(on_actAddtoSet_triggered()));

    actSelectAll = new QAction(tr("Select All"), this);
    connect(actSelectAll, SIGNAL(triggered()), this,
            SLOT(on_actSelectAll_triggered()));

    actExpandAll = new QAction(tr("Expand All"), this);
    connect(actExpandAll, SIGNAL(triggered()), this,
            SLOT(on_actExpandAll_triggered()));

    actCollapseAll = new QAction(tr("Collapse All"), this);
    connect(actCollapseAll, SIGNAL(triggered()), this,
            SLOT(on_actCollapseAll_triggered()));

    actSetting = new QAction(tr("Settings"), this);
    connect(actSetting, SIGNAL(triggered()), this,
            SLOT(on_actSetting_triggered()));

    menuRunSet = new QMenu(tr("Run Case(s) in Selected Set"), this->menuBar());
    
    actRunSetSeq = new QAction(tr("Sequentially"), this);
    connect(actRunSetSeq, SIGNAL(triggered()), this,
            SLOT(on_actRunSetSeq_triggered()));

    actRunSetPar = new QAction(tr("Parallel"), this);
    connect(actRunSetPar, SIGNAL(triggered()), this,
            SLOT(on_actRunSetPar_triggered()));

    actNewSet = new QAction(tr("Create New Set"), this);
    connect(actNewSet, SIGNAL(triggered()), this,
            SLOT(on_actNewSet_triggered()));

    actDelSet = new QAction(tr("Delete Set"), this);
    connect(actDelSet, SIGNAL(triggered()), this,
            SLOT(on_actDelSet_triggered()));

    actPause = new QAction(tr("Pause"), this);
    actPause->setEnabled(false);
    connect(actPause, SIGNAL(triggered()), this,
            SLOT(on_actPause_triggered()));

    actAbort = new QAction(tr("Abort"), this);
    actAbort->setEnabled(false);
    connect(actAbort, SIGNAL(triggered()), this,
            SLOT(on_actAbort_triggered()));
    
    actOutput = new QAction(tr("Output"), this);
    actOutput->setEnabled(false);
    connect(actAbort, SIGNAL(triggered()), this,
            SLOT(on_actOutput_triggered()));

    actClearStatistics = new QAction(tr("Clear Statistics"), this);
    connect(actClearStatistics, SIGNAL(triggered()), this,
            SLOT(on_actClearStatistics_triggered()));

    }

void FrmMain::load()
    {
    this->setContextMenuPolicy(Qt::NoContextMenu);
    
    this->setWindowTitle(QtUIName);
    centerWidget = new QWidget(this);
    this->setCentralWidget(centerWidget);
    
    mainLayout = new QGridLayout(this);
    mainLayout->setVerticalSpacing(2);
    mainLayout->setHorizontalSpacing(2);
    mainLayout->setSpacing(2);
    mainLayout->setMargin(2);
    
    this->centralWidget()->setContextMenuPolicy(Qt::NoContextMenu);
    
    //tab control
    tabWidget = new QTabWidget(this);
    tabWidget->setContextMenuPolicy(Qt::NoContextMenu);
    tabCase = new QWidget(tabWidget);
    tabCase->setContextMenuPolicy(Qt::NoContextMenu);
    tabWidget->addTab(tabCase, tr("Cases"));
    tabSet = new QWidget(tabWidget);
    tabSet->setContextMenuPolicy(Qt::NoContextMenu);
    tabWidget->addTab(tabSet, tr(" Set "));
    tabStarted = new QWidget(tabWidget);
    tabStarted->setContextMenuPolicy(Qt::NoContextMenu);
    tabWidget->addTab(tabStarted, tr("Running"));
    tabStatistic = new QWidget(tabWidget);
    tabStatistic->setContextMenuPolicy(Qt::NoContextMenu);
    tabWidget->addTab(tabStatistic, tr("Statistics"));
    connect(tabWidget, SIGNAL(currentChanged(int)), this,
            SLOT(onTabWidgetSelectIndexChanged()));

    //output panel
    groupBox = new QGroupBox(this);
    //groupBox->setFixedHeight(150);
    groupBox->setContextMenuPolicy(Qt::NoContextMenu);
    groupBox->setTitle(tr("Information"));
    txtOutput = new QPlainTextEdit(groupBox);
    txtOutput->setContextMenuPolicy(Qt::NoContextMenu);
    txtOutput->setReadOnly(true);
    txtOutput->setFocusPolicy(Qt::NoFocus);
    //txtOutput->setEditFocus(false);
    QGridLayout *groupBoxLayout = new QGridLayout(this);
    groupBoxLayout->setVerticalSpacing(2);
    groupBoxLayout->setHorizontalSpacing(2);
    groupBoxLayout->setSpacing(2);
    groupBoxLayout->setMargin(2);   
    groupBoxLayout->addWidget(txtOutput, 0, 0);
    groupBox->setLayout(groupBoxLayout);

    QDesktopWidget* desktop = QApplication::desktop();
    QRect rect = desktop->screenGeometry(0);
    if(rect.height() > rect.width())
        {
        mainLayout->addWidget(tabWidget, 0, 0);
        mainLayout->addWidget(groupBox, 1, 0, Qt::AlignBottom);
        mainLayout->setRowStretch(0,4);
        mainLayout->setRowStretch(1,1); 
        layoutType = true;
        }
    else
        {
        mainLayout->addWidget(tabWidget, 0, 0);
        mainLayout->addWidget(groupBox, 0, 1);//Qt::AlignRight
        //groupBox->setFixedSize(60,0);
        mainLayout->setColumnStretch(0,1);
        mainLayout->setColumnStretch(1,1);    
        layoutType = false;
        }
    
    //Create MainLayout and MainWidget
    this->centralWidget()->setLayout(mainLayout);
    mainLayout->addWidget(tabWidget, 0, 0);
    mainLayout->addWidget(groupBox, 1, 0, Qt::AlignBottom);
    mainLayout->setRowStretch(0,4);
    mainLayout->setRowStretch(1,1);
    

    //Tab page: Case
    QGridLayout *tabCaseLayout = new QGridLayout(this);
    tabCaseLayout->setVerticalSpacing(2);
    tabCaseLayout->setHorizontalSpacing(2);
    tabCaseLayout->setSpacing(2);
    tabCaseLayout->setMargin(2);   
    treeModuleList = new QTreeWidget(tabCase);
    treeModuleList->setContextMenuPolicy(Qt::NoContextMenu);
    treeModuleList->headerItem()->setText(0, tr("Module List"));
    treeModuleList->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeModuleList->setEditFocus(false);
    connect(treeModuleList, SIGNAL(itemClicked(QTreeWidgetItem* , int)), this, 
            SLOT(on_treeModuleList_itemClicked(QTreeWidgetItem* , int)));
    

    QWidget *caseToolWidget = new QWidget(tabCase);
    caseToolWidget->setContextMenuPolicy(Qt::NoContextMenu);
    QGridLayout *caseToolWidgetLayout = new QGridLayout(this);
    QPushButton *btnRunCase = new QPushButton(tr("Run"), caseToolWidget);
    btnRunCase->setContextMenuPolicy(Qt::NoContextMenu);
    connect(btnRunCase, SIGNAL(clicked()), this,
            SLOT(on_actRunCaseSeq_triggered()));
    QPushButton *btnExpandAll = new QPushButton(tr("Expand"), caseToolWidget);
    btnExpandAll->setContextMenuPolicy(Qt::NoContextMenu);
    connect(btnExpandAll, SIGNAL(clicked()), this,
            SLOT(on_actExpand_triggered()));
    QPushButton *btnCollapseAll = new QPushButton(tr("Collapse"),
            caseToolWidget);
    btnCollapseAll->setContextMenuPolicy(Qt::NoContextMenu);
    connect(btnCollapseAll, SIGNAL(clicked()), this,
            SLOT(on_actCollapse_triggered()));

    caseToolWidgetLayout->addWidget(btnRunCase, 0, 0);
    caseToolWidgetLayout->addWidget(btnExpandAll, 0, 1);
    caseToolWidgetLayout->addWidget(btnCollapseAll, 0, 2);
    caseToolWidget->setLayout(caseToolWidgetLayout);

    tabCaseLayout->addWidget(caseToolWidget, 1, 0);
    tabCaseLayout->addWidget(treeModuleList, 0, 0);
    tabCase->setLayout(tabCaseLayout);

    //Tab page: Set

    QGridLayout *tabSetLayout = new QGridLayout(this);
    tabSetLayout->setVerticalSpacing(2);
    tabSetLayout->setHorizontalSpacing(2);
    tabSetLayout->setSpacing(2);
    tabSetLayout->setMargin(2);   

    QGridLayout *tabSetMainLayout = new QGridLayout(this);
    tabSetMainLayout->setVerticalSpacing(2);
    tabSetMainLayout->setHorizontalSpacing(2);
    tabSetMainLayout->setSpacing(2);
    tabSetMainLayout->setMargin(2);   
    QWidget *tabSetMainWidget = new QWidget(tabSet);
    tabSetMainWidget->setContextMenuPolicy(Qt::NoContextMenu);
    QLabel *lblSet = new QLabel(tr("Test Set:"), tabSetMainWidget);
    lblSet->setContextMenuPolicy(Qt::NoContextMenu);
    QLabel *lblCase = new QLabel(tr("Cases:"), tabSetMainWidget);
    lblCase->setContextMenuPolicy(Qt::NoContextMenu);
    cboSetList = new QComboBox(tabSetMainWidget);
    cboSetList->setContextMenuPolicy(Qt::NoContextMenu);
    cboSetList->setEditable(false);
    connect(cboSetList, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(on_cboSetList_currentIndexChanged(QString)));
    lstSetCases = new QListWidget(tabSetMainWidget);
    lstSetCases->setContextMenuPolicy(Qt::NoContextMenu);
    tabSetMainLayout->addWidget(lblSet, 0, 0);
    tabSetMainLayout->addWidget(cboSetList, 0, 1);
    tabSetMainLayout->addWidget(lblCase, 1, 0,
            (Qt::AlignTop | Qt::AlignRight));
    tabSetMainLayout->addWidget(lstSetCases, 1, 1);
    tabSetMainWidget->setLayout(tabSetMainLayout);

    QWidget *setToolWidget = new QWidget(tabSet);
    setToolWidget->setContextMenuPolicy(Qt::NoContextMenu);
    QGridLayout *setToolWidgetLayout = new QGridLayout(this);
    setToolWidgetLayout->setVerticalSpacing(2);
    setToolWidgetLayout->setHorizontalSpacing(2);
    setToolWidgetLayout->setSpacing(2);
    setToolWidgetLayout->setMargin(2);     
    QPushButton *btnRunSetCase = new QPushButton(tr("Run"), setToolWidget);
    btnRunSetCase->setContextMenuPolicy(Qt::NoContextMenu);
    connect(btnRunSetCase, SIGNAL(clicked()), this,
            SLOT(on_actRunSetSeq_triggered()));
    QPushButton *btnNewSet = new QPushButton(tr("New Set"), setToolWidget);
    btnNewSet->setContextMenuPolicy(Qt::NoContextMenu);
    connect(btnNewSet, SIGNAL(clicked()), this,
            SLOT(on_actNewSet_triggered()));
    QPushButton *btnDelSet = new QPushButton(tr("Delete Set"), setToolWidget);
    btnDelSet->setContextMenuPolicy(Qt::NoContextMenu);
    connect(btnDelSet, SIGNAL(clicked()), this,
            SLOT(on_actDelSet_triggered()));

    setToolWidgetLayout->addWidget(btnRunSetCase, 0, 0);
    setToolWidgetLayout->addWidget(btnNewSet, 0, 1);
    setToolWidgetLayout->addWidget(btnDelSet, 0, 2);
    setToolWidget->setLayout(setToolWidgetLayout);

    tabSetLayout->addWidget(tabSetMainWidget, 0, 0);
    tabSetLayout->addWidget(setToolWidget, 1, 0);
    tabSet->setLayout(tabSetLayout);

    //Tab Started
    QGridLayout *tabStartedLayout = new QGridLayout(this);
    tabStartedLayout->setVerticalSpacing(2);
    tabStartedLayout->setHorizontalSpacing(2);
    tabStartedLayout->setSpacing(2);
    tabStartedLayout->setMargin(2);     
    lstStartedCases = new QListWidget(tabStarted);
    lstStartedCases->setContextMenuPolicy(Qt::NoContextMenu);
    QWidget *startedToolWidget = new QWidget(tabStarted);
    startedToolWidget->setContextMenuPolicy(Qt::NoContextMenu);
    QGridLayout *startedToolWidgetLayout = new QGridLayout(this);
    startedToolWidgetLayout->setVerticalSpacing(2);
    startedToolWidgetLayout->setHorizontalSpacing(2);
    startedToolWidgetLayout->setSpacing(2);
    startedToolWidgetLayout->setMargin(2);
    btnPauseCase = new QPushButton(tr("Pause"), startedToolWidget);
    btnPauseCase->setContextMenuPolicy(Qt::NoContextMenu);
    connect(btnPauseCase, SIGNAL(clicked()), this,
            SLOT(on_actPause_triggered()));
    btnPauseCase->setEnabled(false);

    btnAbortCase = new QPushButton(tr("Abort"), startedToolWidget);
    btnAbortCase->setContextMenuPolicy(Qt::NoContextMenu);
    connect(btnAbortCase, SIGNAL(clicked()), this,
            SLOT(on_actAbort_triggered()));
    btnAbortCase->setEnabled(false);
        
    btnShowOutput = new QPushButton(tr("Output"), startedToolWidget);
    connect(btnShowOutput, SIGNAL(clicked()), this,
                SLOT(on_actOutput_triggered()));
    btnShowOutput->setEnabled(false);


    startedToolWidgetLayout->addWidget(btnPauseCase, 0, 0);
    startedToolWidgetLayout->addWidget(btnAbortCase, 0, 1);
    startedToolWidgetLayout->addWidget(btnShowOutput, 0, 2);
    startedToolWidget->setLayout(startedToolWidgetLayout);

    tabStartedLayout->addWidget(lstStartedCases, 0, 0);
    tabStartedLayout->addWidget(startedToolWidget, 1, 0);
    tabStarted->setLayout(tabStartedLayout);

    //Tab Statistic
    QGridLayout *tabStatisticLayout = new QGridLayout(this);
    tabStatisticLayout->setVerticalSpacing(2);
    tabStatisticLayout->setHorizontalSpacing(2);
    tabStatisticLayout->setSpacing(2);
    tabStatisticLayout->setMargin(2);

    treeStatistic = new QTreeWidget(tabStatistic);
    treeStatistic->setContextMenuPolicy(Qt::NoContextMenu);
    treeStatistic->headerItem()->setText(0, tr("Statistics"));
    tabStatisticLayout->addWidget(treeStatistic, 0, 0);
    tabStatistic->setLayout(tabStatisticLayout);

    executedItems = new QTreeWidgetItem(treeStatistic);
    executedItems->setText(0, tr("Executed Cases(0)"));
    passedItems = new QTreeWidgetItem(treeStatistic);
    passedItems->setText(0, tr("Passed Cases(0)"));
    failedItems = new QTreeWidgetItem(treeStatistic);
    failedItems->setText(0, tr("Failed Cases(0)"));
    crashedItems = new QTreeWidgetItem(treeStatistic);
    crashedItems->setText(0, tr("Crashed Cases(0)"));
    abortedItems = new QTreeWidgetItem(treeStatistic);
    abortedItems->setText(0, tr("Aborted Cases(0)"));
    
    //this->repaint();


    }



void FrmMain::LoadSubMenu()
    {
    menuBar()->clear();
    menuBar()->setContextMenuPolicy(Qt::NoContextMenu);
    if (tabWidget->currentIndex() == 0)
        {
        //Cases Tab
        menuBar()->addAction(actOpenFile);
        menuBar()->addMenu(menuRunCase);
        menuRunCase->addAction(actRunCaseSeq);
        menuRunCase->addAction(actRunCasePar);
        menuBar()->addAction(actReapeatRunSeq);
        menuBar()->addSeparator();
        menuBar()->addAction(actAddtoSet);
        menuBar()->addSeparator();
        menuBar()->addAction(actSelectAll);
        menuBar()->addAction(actExpandAll);
        menuBar()->addAction(actCollapseAll);
        }
    else if (tabWidget->currentIndex() == 1)
        {
        //Set Tab
        menuBar()->addMenu(menuRunSet);
        menuRunSet->addAction(actRunSetSeq);
        menuRunSet->addAction(actRunSetPar);
        menuBar()->addSeparator();
        menuBar()->addAction(actNewSet);
        menuBar()->addAction(actDelSet);
        }
    else if (tabWidget->currentIndex() == 2)
        {
        //Started Tab
        menuBar()->addAction(actPause);
        menuBar()->addAction(actAbort);
        menuBar()->addAction(actOutput);
        
        }
    else
        {
        //Staticstic tab
        menuBar()->addAction(actClearStatistics);
        }
    menuBar()->addSeparator();
    menuBar()->addAction(actSetting);
    menuBar()->addAction(actAbout);
    menuBar()->addAction(actExit);

    }

void FrmMain::onTabWidgetSelectIndexChanged()
    {
    LoadSubMenu();
    }

void FrmMain::loadContent()
    {
    //Load ModuleList
    loadModuleList();
    //Load SetList
    loadSetList();
    //Load Statistic List
    loadStatistic();
    }

void FrmMain::loadModuleList()
    {
    treeModuleList->clear();
    
    QList<QString> moduleList = controller->GetModuleList();
    foreach(QString moduleName, moduleList)
            {
            QTreeWidgetItem* item = new QTreeWidgetItem(treeModuleList);
            item->setText(0, UNSELECTITEMHEADER + moduleName);
            
            QList<QString> caseList = controller->GetCaseListByModule(
                    moduleName);

            foreach(QString caseName, caseList)
                    {
                    QTreeWidgetItem* caseItem = new QTreeWidgetItem(item);        
                    caseItem->setText(0, UNSELECTITEMHEADER + caseName);
                    }
            }
    if (moduleList.size() > 0)
        {
        treeModuleList->setCurrentItem(treeModuleList->topLevelItem(0));
        }
    }

void FrmMain::reloadStatisticItem(QString name, QTreeWidgetItem* item,
        TSTFCaseStatusType type)
    {
    QList<CSTFCase> caseList = controller->GetCasesByStatus(type);
    while (item->childCount() != 0)
        {
        item->removeChild(item->child(0));
        }
    item->setText(0, name + "(" + QString::number(caseList.size(), 10) + ")");
    foreach(CSTFCase aCase, caseList)
            {
            QTreeWidgetItem* child = new QTreeWidgetItem(item);
            child->setText(0, aCase.Name());
            }
    }

void FrmMain::loadStatistic()
    {
    //executedItems;
    reloadStatisticItem("Executed Cases", executedItems, EStatusExecuted);

    //passedItems;
    reloadStatisticItem("Passed Cases", passedItems, EStatusPassed);

    //failedItems;
    reloadStatisticItem("Failed Cases", failedItems, EStatusFailed);

    //crashedItems;
    reloadStatisticItem("Crashed Cases", crashedItems, EStatusCrashed);

    //abortedItems;
    reloadStatisticItem("Aborted Cases", abortedItems, EStatusAborted);

    }

void FrmMain::loadSetList()
    {
    cboSetList->clear();

    QList<QString> setList = controller->GetSetList();
    foreach(QString setName, setList)
            {
            cboSetList->addItem(setName);
            }
//    if (setList.size() > 0)
//        {
//        //cboSetList->setCurrentIndex(0);
//        on_cboSetList_currentIndexChanged(setList.at(0));
//        }
    }

QList<CSTFCase> FrmMain::getSelectedCases()
    {
    int index = 0;
    QTreeWidgetItem* item = treeModuleList->topLevelItem(index);
    QList<CSTFCase> caseList;
    while (item != 0)
        {
        for (int i = 0; i < item->childCount(); i++)
            {
            QTreeWidgetItem* child = item->child(i);
            if (child->text(0).startsWith(SELECTITEMHEADER))
                {
                CSTFCase aCase(child->text(0).remove(0,3), i);
                aCase.SetIndex(i);
                //aCase.SetModuleName(moduleBox->text());
                aCase.SetModuleName(item->text(0).remove(0,3));
                caseList.append(aCase);
                }
            }
        index++;
        item = treeModuleList->topLevelItem(index);
        }
    return caseList;
    }

void FrmMain::on_cboSetList_currentIndexChanged(QString item)
    {
    lstSetCases->clear();
    QList<QString> list = controller->GetCaseListBySet(item);
    foreach(QString caseName, list)
            {
            lstSetCases->addItem(caseName);
            }
    }

void FrmMain::startRunning()
    {
    setSetting();
    tabWidget->setCurrentWidget(tabStarted);    
    }

void FrmMain::on_actRunCaseSeq_triggered()
    {
    //run case seq
    startRunning();
    controller->RunCases(getSelectedCases(), Sequentially);
    }

void FrmMain::on_actRunCasePar_triggered()
    {
    startRunning();
    controller->RunCases(getSelectedCases(), Parallel);
    }

void FrmMain::on_actReapeatRunSeq_triggered()
    {
    DlgRepeatRun dlgRepeatRun(this);
    int result = dlgRepeatRun.exec();
    if(result == QDialog::Accepted)
        {
        QList<CSTFCase> selectedCases = getSelectedCases();
        if(selectedCases.count() > 0)
            {
            startRunning();
            controller->RepeatRunCases( selectedCases, 
                                        dlgRepeatRun.isRepeatInfinitely(),
                                        dlgRepeatRun.GetLoopTimes() );
            }
        
        }
    }

void FrmMain::on_actAddtoSet_triggered()
    {
    QList<CSTFCase> list = getSelectedCases();
    if (list.size() == 0)
        {
        QErrorMessage *errorMessageDialog = new QErrorMessage(this);
        errorMessageDialog->setAutoFillBackground(true);
        errorMessageDialog->showMessage(tr(
                "<font color =black>Please select cases you want to added to set.</font>"));
        return;
        }

    QList<QString> setList = controller->GetSetList();

    DlgSetSelector dlgSet(setList, this);
    int result = dlgSet.exec();
    QString setName;
    if(result == QDialog::Accepted)
        {
        setName = dlgSet.SelectName();
        }
    else
        {
        return;
        }
    bool rst = false;
    if(setName == "")
        {
        setName = "temp.set";
        rst = controller->CreateSet(setName);
        if(!rst)
            {
            return;
            }
        }
    controller->AddCaseToSet(list, setName);
//
//     bool ok;
//     QString setName = QInputDialog::getItem(this, tr(
//     "\r\nAdd select cases to Set"), tr("\r\n\r\nSets:"), setList, 0, false, &ok, Qt::Dialog);
//     if (ok && !setName.isEmpty())
//     {
//     if(setName == newSet)
//         {
//         ok = controller->CreateSet(setName);
//         if(!ok)
//             {
//             return;
//             }
//         }
//     controller->AddCaseToSet(list, setName);
//     }
    tabWidget->setCurrentIndex(1);
    int index = -1;
    for(int i=0;i<cboSetList->count();i++)
        {
        if(cboSetList->itemText(i) == setName)
            {
            index = i;
            break;
            }
        }
    if(index != -1)
        {
        cboSetList->setCurrentIndex(index);
        }

    
    }

void FrmMain::on_actSelectAll_triggered()
    {
    QString header = UNSELECTITEMHEADER;
    if(actSelectAll->text() == "Select All")
        {
        actSelectAll->setText("UnSelect All");
        header = SELECTITEMHEADER;
        }
    else
        {
        actSelectAll->setText("Select All");    
        }
    
    int index = 0;
    QTreeWidgetItem* item = treeModuleList->topLevelItem(index);
    while (item != 0)
        {
        if(!item->isHidden())
            item->setText(0, item->text(0).replace(0,3, header));
        for (int i = 0; i < item->childCount(); i++)
            {
            QTreeWidgetItem* child = item->child(i);
            if(!child->isHidden())
                child->setText(0,child->text(0).replace(0,3,header));
            }
        index++;
        item = treeModuleList->topLevelItem(index);
        }
    }

void FrmMain::on_actExpandAll_triggered()
    {
    QTreeWidgetItem* item = treeModuleList->currentItem();
    treeModuleList->expandAll();
    if(item != NULL)
        {
        treeModuleList->setCurrentItem(item);
        }
        
    }

void FrmMain::on_actCollapseAll_triggered()
    {
    QTreeWidgetItem* item = treeModuleList->currentItem();
    if(item != NULL)
        {
        if(item->parent() != NULL)
            {
            item = item->parent();
            }
        }
    treeModuleList->collapseAll();
    if(item != NULL)
        {
        treeModuleList->setCurrentItem(item);
        }
    
    }

void FrmMain::on_actSetting_triggered()
    {
    DlgSetting dlgSet(uiSetting);
    currentFilter = uiSetting->ReadSetting(KFilter);
    currentFilterCaseSens = uiSetting->ReadSetting(KFilterCaseSens);
    int result = dlgSet.exec();
    if(result == QDialog::Accepted)
        {
        setSetting();
        }
    }

void FrmMain::on_actRunSetSeq_triggered()
    {
    startRunning();
    QString setName = cboSetList->currentText();
    controller->RunSets(setName, Sequentially);
    }

void FrmMain::on_actRunSetPar_triggered()
    {
    startRunning();
    QString setName = cboSetList->currentText();
    controller->RunSets(setName, Parallel);
    }

void FrmMain::on_actNewSet_triggered()
    {
    QString name;
    bool rst = controller->CreateSet(name);
    if(rst)
        {
        QMessageBox::information(this, 
                tr("Create Set Successfully"), 
                "Create a new test set, named: " + name);

        int index = -1;
        for(int i=0;i<cboSetList->count();i++)
            {
            if(cboSetList->itemText(i) == name)
                {
                index = i;
                break;
                }
            }
        if(index != -1)
            {
            cboSetList->setCurrentIndex(index);
            }

        
        }
    else
        {
        QMessageBox::information(this, 
                tr("Create Set Failed"), 
                tr("Please check the log for more information."));
    
        }
    
    }

void FrmMain::on_actDelSet_triggered()
    {
        QString setName = cboSetList->currentText();
        QMessageBox msgBox(QMessageBox::Warning, tr("Delete a Set"), tr(
        "Do you really want to delete the set?"), 0, this);
        msgBox.addButton(tr("&Delete"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);
        if (msgBox.exec() == QMessageBox::AcceptRole)
        {
        controller->DeleteSet(setName);
        }

    }

void FrmMain::on_actPause_triggered()
    {
    if (btnPauseCase->text() == "Pause")
        {
        controller->PauseCase();
        btnPauseCase->setText(tr("Resume"));
        actPause->setText(tr("Resume"));
        }
    else
        {
        controller->ResumeCase();
        btnPauseCase->setText(tr("Pause"));
        actPause->setText(tr("Pause"));
        }
    }

void FrmMain::on_actAbort_triggered()
    {
    controller->AbortCase();
    }

void FrmMain::on_treeModuleList_itemClicked(QTreeWidgetItem* item, int /*column*/)
    {
    //Check if shift key is pressed
    bool isShiftPressed = false;
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
    isShiftPressed=keyMod.testFlag(Qt::ShiftModifier);
    
    //Handle shift key.
    //Shift not pressed.
    if(!isShiftPressed)
        {
        setItemClicked(item);
        }
    //Shift pressed.
    else
        {
        enum Direction
            {
            Item_NoDirection,
            Item_Above,
            Item_Below
            };
        Direction direction = Item_NoDirection;
        QTreeWidgetItem* tempItem = item;
        //check direction of last selected item comparing current one.
        while(tempItem)
            {
            tempItem = treeModuleList->itemAbove(tempItem);
            if(tempItem == lastItemSelected)
                {
                direction = Item_Above;
                break;
                }
            }
        if (direction != Item_Above)
            {
            tempItem = item;
            while(tempItem)
                {
                tempItem = treeModuleList->itemBelow(tempItem);
                if(tempItem == lastItemSelected)
                    {
                    direction = Item_Below;
                    break;
                    }
                }
            }
        
        // Select all items between current item and last selected item.
        tempItem = item;
        if(direction != Item_NoDirection)
            {
            while(tempItem)
                {
                //check if this item been selected.
                bool isItemSelected = false;
                if ( tempItem->text(0).left(3).compare(SELECTITEMHEADER)==0 )
                    {
                    isItemSelected = true;
                    }
                // If not selected, set to selected.
                if (!isItemSelected )
                    {
                    setItemClicked(tempItem);
                    }
                
                //Go above/below
                if (direction == Item_Above)
                    {
                    tempItem = treeModuleList->itemAbove(tempItem);             
                    }
                if (direction == Item_Below)
                    {
                    tempItem = treeModuleList->itemBelow(tempItem);             
                    }
                
                if (tempItem == lastItemSelected)
                    {
                    break;
                    }
                }
            }
        }
    
    // Set current clicked item to last selected item.
    lastItemSelected = item;

      
    }

void FrmMain::setItemClicked(QTreeWidgetItem* item)
    {
    QString header = UNSELECTITEMHEADER;
    if(item->text(0).startsWith(UNSELECTITEMHEADER))
        {
            header = SELECTITEMHEADER;
        }
    item->setText(0 , item->text(0).replace(0, 3, header));
    for(int i=0;i<item->childCount();i++)
        {
            item->child(i)->setText(0, item->child(i)->text(0).replace(0, 3, header));
        }
    }

void FrmMain::on_actAbout_triggered()
    {
    QString str = QtUIName + "<&nbsp;>" + QtUIVersion;
    str.append("<br>").append("engine version:");
    
    str.append(QString::number(STIF_MAJOR_VERSION, 10)).append(".");
    str.append(QString::number(STIF_MINOR_VERSION, 10)).append(".");
    str.append(QString::number(STIF_BUILD_VERSION, 10));
    str.append("  --").append(STIF_REL_DATE).append("<br>");
    str.append("---");
    str.append("Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. ");

    QErrorMessage *errorMessageDialog = new QErrorMessage(this);
    errorMessageDialog->showMessage("<font color =black size=12px><b>" + str + "</b></font>");
    
    }

void FrmMain::on_actOpenFile_triggered()
    {
    QString fileName = QFileDialog::getOpenFileName(this, tr(
            "Select ini file"), tr("c:\\"), tr(
            "Ini Files (*.ini);;All Files (*)"));
    if (!fileName.isEmpty())
        {
        bool result = controller->OpenEngineIniFile(fileName);
        if(result)
            {
            this->loadModuleList();   
            QMessageBox::information(this, tr("Open INI File"), "Load Engine INI file successfully!");
            }
        else
            {
            QMessageBox::warning(this, tr("Open INI File"),"Failed to Load Engine INI file. Please check the file format and its path.");
            }
        
        }
    }

void FrmMain::on_actClearStatistics_triggered()
    {
    model->ClearCasesStatus();
    }


void FrmMain::on_actExpand_triggered()
    {
    QTreeWidgetItem* item = treeModuleList->currentItem();
    if(item != NULL)
        {
        item->setExpanded(true);
        }
    }

void FrmMain::on_actCollapse_triggered()
    {

    QTreeWidgetItem* item = treeModuleList->currentItem();
    if(item != NULL)
        {
        item->setExpanded(false);
        }
    }

void FrmMain::on_actOutput_triggered()
    {
    controller->SetShowOutput(true);
    }

// End of File
