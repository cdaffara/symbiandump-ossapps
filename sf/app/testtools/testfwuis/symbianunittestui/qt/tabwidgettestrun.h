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
 * Description: widget class to run tests.
 *
 */

#ifndef TABWIDGETTESTRUN_H_
#define TABWIDGETTESTRUN_H_
#include <QTabWidget>
#include <QList>

class QPushButton;
class QWidget;
class QTreeWidget;
class QTableWidget;
class QTreeWidgetItem;
class QLabel;
class WidgetTestRunner;
class QProgressBar;
class Settings;
class TestItemList;
class DialogMsg;

class TabWidgetTestRun : public QTabWidget
    {
Q_OBJECT

public:
    TabWidgetTestRun(QWidget* parent = 0);
    virtual ~TabWidgetTestRun();

protected slots:
    void addTests(QStringList testCases);
    void saveSettings(const Settings*);
    void changeCurrentTab(int index);

private:
    void createTestsTab();
    void createExecutionTab();
    void load();
    void createTreeWidgetTests();
    void setTabPageTestsLayout();
    void createExecutionWidgets();
    void setTabPageExecutionLayout();
    void setupRunnerEventHandlers();
    void setupTestsTabEventHandlers();
    bool isAllCasesSelected();
    QString getFuncName(const QString& caseName);
    QString& removeMark(QString&);
    void resetTreeWidget();
    void resetTestItemList();

signals:
    void testsAdded();
    void testRun();
    void currentTabChangedToTests(bool hasTests);
    void currentTabChangedToExecution();
    void selectedTestsChanged(bool, bool, bool);

private slots:
    void setTotal(int num);
    void setPassed(int num);
    void setFailed(QString name, QString reason, int num);
    void runTests();
    void selectCurrentCase();
    void selectAllCases();
    void deselectCurrentCase();
    void deselectAllCases();
    void selectDeselectTestCases(QTreeWidgetItem* item, int column);
    void changeCurItem(QTreeWidgetItem*, QTreeWidgetItem*);
    void deselectCase(QTreeWidgetItem* item, int column);
    void setBtnExpandCollapseEnabled(QTreeWidgetItem*);
    void expandCollapseItem();
    
    // Key press event received
    void pressLeftArrow();
    void pressRightArrow();
    void pressUpArrow();
    void pressDownArrow();
    void pressEnter();

private:
    // Widgets on "test cases" tab
    QPushButton*    btnRun;
    QPushButton*    btnExpandCollapse;
    QTreeWidget*    treeWdgtTests;
    QWidget*        tabTests;

    // Widgets on "execution" tab
    QWidget*        tabRunner;
    QTableWidget*   tblWdgtErrorSummary;
    QProgressBar*   progressBarExecuting;
    QLabel*         labelTotal;
    QLabel*         labelPassed;
    QLabel*         labelFailed;
    QLabel*         labelErrorSummary;

    // Test runner widget
    WidgetTestRunner* testRunner;
    
    // Test cases    
    TestItemList* listTestItems;
    TestItemList* listSelectedTestItems;
    
    // Error msg dialog
    DialogMsg* dlgErrMsg;
    };

#endif /* TABWIDGETTESTRUN_H_ */
