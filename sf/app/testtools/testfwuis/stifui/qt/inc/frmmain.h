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
*              frmMain is a QT based Window.
*              Used to display STF executor main GUI.
*
*/
#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>
#include <QtGui>
#include "istfqtuicontroller.h"
#include "istfqtuimodel.h"
#include "dlgoutput.h"
#include "uisetting.h"
#include "dlgsetting.h"
#include "dlgsetselector.h"
#include "uiversion.h"
#include "dlgrepeatrun.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QTabWidget;
class QWidget;
class QPushButton;
class QComboBox;
class QPlainTextEdit;
class QTreeWidget;
class QTreeWidgetItem;
class QListWidget;
class QCheckBox;
class QMessageBox;
class QFileDialog;
QT_END_NAMESPACE



class FrmMain : public QMainWindow, public IStfEventListener, public IStifModelEventListener
{
    Q_OBJECT

public:
    FrmMain();
    ~FrmMain();
    
    
public: //Implement IStfEventListener
    void OnGetMessage(const QString& aMessage);
    void OnSetListChanged();
    void OnCaseOutputChanged(const IStfEventListener::CaseOutputCommand& cmd, const QString& index, const QString& msg);

    
public: //Implement IStifModelEventListener
    void OnCaseStatisticChanged() ;
    void OnRunningCaseChanged() ;
    
protected:
    void paintEvent(QPaintEvent *event);
    
protected slots:
    void onTabWidgetSelectIndexChanged();


private:
    void createMenus();
    void LoadSubMenu();
    void load();
    void loadContent();
    void loadModuleList();
    void loadSetList();
    void loadStatistic();
    QList<CSTFCase> getSelectedCases();
    void reloadStatisticItem(QString name, QTreeWidgetItem* item, TSTFCaseStatusType type);
    void setSetting();
    void startRunning();
    void setItemClicked(QTreeWidgetItem* item);
    
private:
    IStfQtUIController* controller;
    IStfQtUIModel* model;
    DlgOutput* dlgOutput;
    UiSetting* uiSetting; 
    bool layoutType;
    QTreeWidgetItem* lastItemSelected;
    QString currentFilter;
    QString currentFilterCaseSens;

private: //UI Components
    QWidget* centerWidget;
    QGridLayout *mainLayout;
    QGroupBox *groupBox;
    
    //menus and actions
    QMenu *operateMenu;
    //output panel
    QPlainTextEdit *txtOutput;
    //tab control.
    QTabWidget *tabWidget;
    QWidget *tabCase;
    QWidget *tabSet;
    QWidget *tabStarted;
    QWidget *tabStatistic;

    //Cases Tab
    QTreeWidget *treeModuleList;
    
    //Statistic Tab
    QTreeWidget *treeStatistic;
    QTreeWidgetItem *executedItems;
    QTreeWidgetItem *passedItems;
    QTreeWidgetItem *failedItems;
    QTreeWidgetItem *abortedItems;
    QTreeWidgetItem *crashedItems;
        
    
    //Set Tab
    QComboBox *cboSetList;
    QListWidget *lstSetCases;

    //Started Tab
    QListWidget *lstStartedCases;
    QPushButton *btnPauseCase;
    QPushButton *btnAbortCase;
    QPushButton *btnShowOutput;

    //menu actions
    QAction *actExit;
    QAction *actAbout;

    //cases actoins
    QAction *actOpenFile;
    QAction *actRunCaseSeq;
    QAction *actRunCasePar;
    QAction *actReapeatRunSeq;
    QAction *actAddtoSet;
    QAction *actSelectAll;
    QAction *actExpandAll;
    QAction *actCollapseAll;
    QAction *actSetting;
    QMenu *menuRunCase; 
    //sets actions
    QAction *actRunSetSeq;
    QAction *actRunSetPar;
    QAction *actNewSet;
    QAction *actDelSet;
    QMenu *menuRunSet;
    //running actions.
    QAction *actPause;
    QAction *actAbort;
    QAction *actOutput;
    
    //statistics actions
    QAction *actClearStatistics;

private slots:
    void on_cboSetList_currentIndexChanged(QString );
    
    void on_actAbout_triggered();    
    void on_actOpenFile_triggered();
    void on_actRunCaseSeq_triggered();
    void on_actRunCasePar_triggered();
    void on_actReapeatRunSeq_triggered();
    void on_actAddtoSet_triggered();
    void on_actSelectAll_triggered();
    void on_actExpandAll_triggered();
    void on_actCollapseAll_triggered();
    void on_actExpand_triggered();
    void on_actCollapse_triggered();
    void on_actSetting_triggered();
        
    void on_actRunSetSeq_triggered();
    void on_actRunSetPar_triggered();
    void on_actNewSet_triggered();
    void on_actDelSet_triggered();
    void on_actPause_triggered();
    void on_actAbort_triggered();
    void on_actOutput_triggered();
    void on_actClearStatistics_triggered();
    void on_treeModuleList_itemClicked(QTreeWidgetItem* item, int column);

    
        
    



};

#endif // FrmMain_H

// End of File
