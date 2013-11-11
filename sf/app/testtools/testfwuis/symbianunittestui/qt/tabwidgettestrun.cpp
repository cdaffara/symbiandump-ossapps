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
 * Description: TabWidget class to select, run tests and show the result.
 *
 */

#include <QPushButton>
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidget>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPalette>
#include <QTableWidgetItem>
#include <QHeaderView>
#include "tabwidgettestrun.h"
#include "widgettestrunner.h"
#include "settings.h"
#include "testitemlist.h"
#include "dialogmsg.h"

static const QString TOTAL = "Total: %1";
static const QString PASSED = "Passed: %1";
static const QString FAILED = "Failed: %1";
static const QString ERRORSUMMARY = "Error summary";
static const QString NAME = "Case Name";
static const QString REASON = "Reason";
static const int FONTPIXELSIZE = 12;
static const int FONTPIXELSIZE_ITEM = 15;
static const int FONTPIXELSIZE_SUBITEM = 10;
static const QString CASELIST = "Cases List";
static const QString UNMARKED = "  ";
static const QString MARKED = "* ";
static const QString PARTIALLY_MARKED = "# ";
static const int MARK_LEN = MARKED.length();
static const QString CPP_SCOPE_OP = "::";
static const QString EXPAND = "Expand";
static const QString EXPAND_ALL = "Expand All";
static const QString COLLAPSE = "Collapse";
static const QString COLLAPSE_ALL = "Collapse All";

TabWidgetTestRun::TabWidgetTestRun(QWidget* parent) :
    QTabWidget(parent)
    {
    setContextMenuPolicy(Qt::NoContextMenu);
    listTestItems = new TestItemList();
    listSelectedTestItems = new TestItemList();
    load();
    dlgErrMsg = new DialogMsg(this);
    connect(testRunner, SIGNAL(errorOccurred(const QString)), dlgErrMsg, SLOT(showMsg(const QString)));
    }

TabWidgetTestRun::~TabWidgetTestRun()
    {
    delete listTestItems;
    listTestItems = NULL;
    delete listSelectedTestItems;
    listSelectedTestItems = NULL;
    }

void TabWidgetTestRun::createTestsTab()
    {
    tabTests = new QWidget(this);
    createTreeWidgetTests();
    this->addTab(tabTests, tr("Test Cases"));
    }

void TabWidgetTestRun::createExecutionTab()
    {
    tabRunner = new QTabWidget(this);
    createExecutionWidgets();
    this->addTab(tabRunner, tr("Execution"));
    }

void TabWidgetTestRun::createExecutionWidgets()
    {
    labelTotal = new QLabel(tabRunner);
    labelTotal->setText(TOTAL.arg(0));
    labelPassed = new QLabel(tabRunner);
    labelPassed->setText(PASSED.arg(0));
    labelFailed = new QLabel(tabRunner);
    labelFailed->setText(FAILED.arg(0));

    labelErrorSummary = new QLabel(tabRunner);
    labelErrorSummary->setText(ERRORSUMMARY);
    tblWdgtErrorSummary = new QTableWidget(tabRunner);
    tblWdgtErrorSummary->setColumnCount(2);
    tblWdgtErrorSummary->setColumnWidth(0, 200);

    QTableWidgetItem* itemName = new QTableWidgetItem();
    tblWdgtErrorSummary->setHorizontalHeaderItem(0, itemName);
    QTableWidgetItem* itemReason = new QTableWidgetItem();
    tblWdgtErrorSummary->setHorizontalHeaderItem(1, itemReason);
    tblWdgtErrorSummary->horizontalHeaderItem(0)->setText(NAME);
    tblWdgtErrorSummary->horizontalHeaderItem(1)->setText(REASON);
    tblWdgtErrorSummary->setShowGrid(true);
    tblWdgtErrorSummary->setSortingEnabled(false);
    tblWdgtErrorSummary->horizontalHeader()->setStretchLastSection(true);
    testRunner = new WidgetTestRunner(tabRunner);
    testRunner->setHidden(true);
    progressBarExecuting = new QProgressBar(tabRunner);
    progressBarExecuting->setHidden(true);
    }

void TabWidgetTestRun::setTabPageExecutionLayout()
    {
    QWidget* labelsWidget = new QWidget(tabRunner);
    QGridLayout* labelsLayout = new QGridLayout(tabRunner);

    labelsLayout->addWidget(labelPassed, 0, 0);
    labelsLayout->addWidget(labelFailed, 0, 1);
    labelsLayout->addWidget(labelTotal, 0, 2);
    labelsWidget->setLayout(labelsLayout);

    QGridLayout* layout = new QGridLayout(tabRunner);
    layout->addWidget(labelsWidget, 0, 0);
    layout->addWidget(progressBarExecuting, 1, 0);
    layout->addWidget(labelErrorSummary, 2, 0);
    layout->addWidget(tblWdgtErrorSummary, 3, 0);
    tabRunner->setLayout(layout);
    }

void TabWidgetTestRun::load()
    {
    createTestsTab();
    setupTestsTabEventHandlers();
    setTabPageTestsLayout();
    createExecutionTab();
    setTabPageExecutionLayout();
    setupRunnerEventHandlers();
    }

void TabWidgetTestRun::createTreeWidgetTests()
    {
    treeWdgtTests = new QTreeWidget(tabTests);
    treeWdgtTests->headerItem()->setText(0, CASELIST);
    btnRun = new QPushButton(tr("Run"), tabTests);
    btnExpandCollapse = new QPushButton(tr("Expand"), tabTests);
    // Disable buttons on startup
    btnRun->setEnabled(false);

    btnExpandCollapse->setEnabled(false);
    }

void TabWidgetTestRun::setTabPageTestsLayout()
    {
    QWidget* btnsWidget = new QWidget(tabTests);
    QGridLayout* btnsLayout = new QGridLayout(tabTests);
    btnsLayout->addWidget(btnRun, 0, 0);
    btnsLayout->addWidget(btnExpandCollapse, 0, 1);
    btnsWidget->setLayout(btnsLayout);

    QGridLayout* layout = new QGridLayout(tabTests);
    layout->addWidget(treeWdgtTests, 0, 0);
    layout->addWidget(btnsWidget, 1, 0);

    tabTests->setLayout(layout);
    }

void TabWidgetTestRun::setTotal(int num)
    {
    labelTotal->setText(TOTAL.arg(num));
    labelTotal->repaint();
    }

void TabWidgetTestRun::setPassed(int num)
    {
    labelPassed->setText(PASSED.arg(num));
    labelPassed->repaint();
    }

void TabWidgetTestRun::setFailed(QString name, QString reason, int num)
    {
    labelFailed->setText(FAILED.arg(num));
    labelFailed->repaint();
    int curRow = tblWdgtErrorSummary->rowCount();
    tblWdgtErrorSummary->insertRow(curRow);
    QTableWidgetItem* itemName = new QTableWidgetItem(name);
    QTableWidgetItem* itemReason = new QTableWidgetItem(reason);
    tblWdgtErrorSummary->setItem(curRow, 0, itemName);
    tblWdgtErrorSummary->setItem(curRow, 1, itemReason);
    QFont font;
    font.setPixelSize(FONTPIXELSIZE);
    itemName->setFont(font);
    itemReason->setFont(font);
    }

void TabWidgetTestRun::resetTreeWidget()
    {
    for(int i = treeWdgtTests->topLevelItemCount() -1; i >= 0; --i)
        {
        delete treeWdgtTests->topLevelItem(i);
        }
    treeWdgtTests->clear();
    }

void TabWidgetTestRun::resetTestItemList()
    {
    listTestItems->removeAll();
    listSelectedTestItems->removeAll();
    }

void TabWidgetTestRun::addTests(QStringList testCases)
    {
    resetTestItemList();
    resetTreeWidget();
    QFont font, subItemFont;
    font.setPixelSize(FONTPIXELSIZE_ITEM);
    subItemFont.setPixelSize(FONTPIXELSIZE_SUBITEM);
    QStringList fetchedCaseNames;
    QTreeWidgetItem* item = NULL;
    QTreeWidgetItem* subItem = NULL;
    foreach (QString dllName, testCases)
            {
            if (listTestItems->exists(dllName))
                {
                continue;
                }
            
            fetchedCaseNames.clear();
            if (testRunner->listTestCases(dllName, fetchedCaseNames) != 0)
                {
                continue;
                }
            item = new QTreeWidgetItem(treeWdgtTests);
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable
                    | Qt::ItemIsEnabled);
            item->setText(0, UNMARKED + dllName);
            item->setFont(0, font);
            
            foreach(QString testCase, fetchedCaseNames)
                    {
                    subItem = new QTreeWidgetItem(item);
                    subItem->setText(0, UNMARKED + testCase/*getFuncName(testCase)*/);
                    subItem->setFont(0, subItemFont);
                    }
            listTestItems->addItemExclusively(dllName, fetchedCaseNames);
            }

    if (treeWdgtTests->topLevelItemCount() > 0)
        {
        treeWdgtTests->setCurrentItem(treeWdgtTests->topLevelItem(0));
        treeWdgtTests->resizeColumnToContents(0);
        if (this->currentWidget() != tabTests)
            {
            this->setCurrentWidget(tabTests);
            }
        treeWdgtTests->setFocus();
        btnRun->setEnabled(false);
        btnExpandCollapse->setEnabled(true);
        emit this->testsAdded();
        }
    }

QString TabWidgetTestRun::getFuncName(const QString& caseName)
    {
    int index = caseName.indexOf(CPP_SCOPE_OP, 0);
    return caseName.right(caseName.length() - index - CPP_SCOPE_OP.length());
    }

void TabWidgetTestRun::setupRunnerEventHandlers()
    {
    connect(this, SIGNAL(currentChanged(int)), this,
            SLOT(changeCurrentTab(int)));
    connect(btnRun, SIGNAL(clicked()), this, SLOT(runTests()));
    connect(btnExpandCollapse, SIGNAL(clicked()), this,
            SLOT(expandCollapseItem()));
    connect(this, SIGNAL(testRun()), testRunner, SLOT(runTests()));
    connect(testRunner, SIGNAL(testExecuted(int)), progressBarExecuting,
            SLOT(setValue(int)));
    connect(testRunner, SIGNAL(testPassed(int)), this, SLOT(setPassed(int)));
    connect(testRunner, SIGNAL(testFailed(QString, QString, int)), this,
            SLOT(setFailed(QString, QString, int)));
    }

void TabWidgetTestRun::runTests()
    {
    if (this->currentWidget() != tabRunner)
        {
        this->setCurrentWidget(tabRunner);
        }
    tblWdgtErrorSummary->clearContents();
    tblWdgtErrorSummary->setRowCount(0);
    testRunner->addTestDllNames(listSelectedTestItems->items());
    testRunner->addTestCaseNames(listSelectedTestItems->subItems());
    progressBarExecuting->setEnabled(true);
    progressBarExecuting->show();
    progressBarExecuting->setMaximum(
            listSelectedTestItems->subItems().count());
    progressBarExecuting->setValue(0);
    labelTotal->setText(TOTAL.arg(listSelectedTestItems->subItems().count()));
    labelPassed->setText(PASSED.arg(0));
    labelFailed->setText(FAILED.arg(0));
    emit this->testRun();
    progressBarExecuting->setEnabled(false);
    }

void TabWidgetTestRun::changeCurrentTab(int index)
    {
    if (this->widget(index) == tabTests)
        {
        treeWdgtTests->setFocus();
        emit this->currentTabChangedToTests(!listTestItems->empty());
        }
    else
        {
        tblWdgtErrorSummary->setFocus();
        emit this->currentTabChangedToExecution();
        }
    }

void TabWidgetTestRun::setupTestsTabEventHandlers()
    {
    connect(treeWdgtTests, SIGNAL(itemClicked(QTreeWidgetItem* , int)), this,
            SLOT(selectDeselectTestCases(QTreeWidgetItem* , int)));
    connect(treeWdgtTests,
            SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(changeCurItem(QTreeWidgetItem*, QTreeWidgetItem*)));
    connect(treeWdgtTests, SIGNAL(itemExpanded(QTreeWidgetItem*)), this,
            SLOT(setBtnExpandCollapseEnabled(QTreeWidgetItem*)));
    connect(treeWdgtTests, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this,
            SLOT(setBtnExpandCollapseEnabled(QTreeWidgetItem*)));
    }

void TabWidgetTestRun::changeCurItem(QTreeWidgetItem* cur, QTreeWidgetItem*/* prev*/)
    {
    if (cur != NULL)
        {
        setBtnExpandCollapseEnabled(cur);
        emit this->selectedTestsChanged(listSelectedTestItems->empty(),
                isAllCasesSelected(),
                cur->text(treeWdgtTests->currentColumn()).contains(MARKED));
        }
    }

void TabWidgetTestRun::selectCurrentCase()
    {
    QString itemName = treeWdgtTests->currentItem()->text(
            treeWdgtTests->currentColumn());
    if (itemName.contains(UNMARKED))
        {
        selectDeselectTestCases(treeWdgtTests->currentItem(),
                treeWdgtTests->currentColumn());
        }
    }

void TabWidgetTestRun::selectAllCases()
    {
    for (int i = 0; i < treeWdgtTests->topLevelItemCount(); ++i)
        {
        QString itemName = treeWdgtTests->topLevelItem(i)->text(
                treeWdgtTests->currentColumn());
        if (itemName.contains(UNMARKED)
                || itemName.contains(PARTIALLY_MARKED))
            {
            // Deselect current first
            if (itemName.contains(PARTIALLY_MARKED))
                {
                deselectCase(treeWdgtTests->topLevelItem(i),
                        treeWdgtTests->currentColumn());
                }
            selectDeselectTestCases(treeWdgtTests->topLevelItem(i),
                    treeWdgtTests->currentColumn());
            }
        }
    }

void TabWidgetTestRun::deselectCase(QTreeWidgetItem* item, int column)
    {
    QString itemName = item->text(column);
    if (itemName.contains(MARKED) || itemName.contains(PARTIALLY_MARKED))
        {
        selectDeselectTestCases(item, column);
        }
    }

void TabWidgetTestRun::deselectCurrentCase()
    {
    deselectCase(treeWdgtTests->currentItem(), treeWdgtTests->currentColumn());
    }

void TabWidgetTestRun::deselectAllCases()
    {
    for (int i = 0; i < treeWdgtTests->topLevelItemCount(); ++i)
        {
        QString itemName = treeWdgtTests->topLevelItem(i)->text(
                treeWdgtTests->currentColumn());
        if (itemName.contains(MARKED) || itemName.contains(PARTIALLY_MARKED))
            {
            selectDeselectTestCases(treeWdgtTests->topLevelItem(i),
                    treeWdgtTests->currentColumn());
            }
        }
    }

void TabWidgetTestRun::saveSettings(const Settings* settings)
    {
    testRunner->saveSettings(settings);
    }

QString& TabWidgetTestRun::removeMark(QString& src)
    {
    return src.remove(MARKED).remove(UNMARKED).remove(PARTIALLY_MARKED);
    }

void TabWidgetTestRun::selectDeselectTestCases(QTreeWidgetItem* item,
        int column)
    {
    QString itemName = item->text(column);
    QTreeWidgetItem* subItem;
    QString subName;
    if (treeWdgtTests->indexOfTopLevelItem(item) != -1)
        {
        // Top level item
        // Marked. Unmark them.        
        if (itemName.contains(MARKED) || itemName.contains(PARTIALLY_MARKED))
            {
            item->setText(column, itemName.replace(0, MARK_LEN, UNMARKED));
            listSelectedTestItems->removeItem(removeMark(itemName));
            for (int i = 0; i < item->childCount(); ++i)
                {
                subItem = item->child(i);
                subName = subItem->text(column);
                if (subName.contains(MARKED))
                    {
                    subItem->setText(column, subName.replace(0, MARK_LEN,
                            UNMARKED));
                    }
                }
            }
        // Unmarked. Mark them.
        else
            {
            QStringList subNames;
            for (int i = 0; i < item->childCount(); ++i)
                {
                subItem = item->child(i);
                subName = subItem->text(column);
                subItem->setText(column, subName.replace(0, MARK_LEN, MARKED));
                subNames << removeMark(subName);
                }
            item->setText(column, itemName.replace(0, MARK_LEN, MARKED));
            listSelectedTestItems->addItemExclusively(removeMark(itemName),
                    subNames);
            }
        }
    else
        {
        // Child item
        // Marked. Unmark it.
        QTreeWidgetItem* parentItem = item->parent();
        QString parentItemName = parentItem->text(column);
        if (itemName.contains(MARKED))
            {
            item->setText(column, itemName.replace(0, MARK_LEN, UNMARKED));
            listSelectedTestItems->removeOneSubItem(
                    removeMark(parentItemName), removeMark(itemName));
            int i = 0;
            for (; i < parentItem->childCount(); ++i)
                {
                if (parentItem->child(i)->text(column).contains(MARKED))
                    {
                    parentItem->setText(column,
                            parentItem->text(column).replace(0, MARK_LEN,
                                    PARTIALLY_MARKED));
                    break;
                    }
                }
            if (i == parentItem->childCount())
                {
                parentItem->setText(column, parentItem->text(column).replace(
                        0, MARK_LEN, UNMARKED));
                }
            }
        // Unmakred. Mark it.
        else
            {
            item->setText(column, itemName.replace(0, MARK_LEN, MARKED));
            listSelectedTestItems->addOneSubItem(removeMark(parentItemName),
                    removeMark(itemName));
            int i = 0;
            for (; i < parentItem->childCount(); ++i)
                {
                if (parentItem->child(i)->text(column).contains(UNMARKED))
                    break;
                }
            if (i < parentItem->childCount())
                {
                parentItem->setText(column, parentItem->text(column).replace(
                        0, MARK_LEN, PARTIALLY_MARKED));
                }
            else
                {
                parentItem->setText(column, parentItem->text(column).replace(
                        0, MARK_LEN, MARKED));
                }
            }
        }
    btnRun->setEnabled(!listSelectedTestItems->empty());
    emit this->selectedTestsChanged(listSelectedTestItems->empty(),
            isAllCasesSelected(), item->text(column).contains(MARKED));
    }

void TabWidgetTestRun::setBtnExpandCollapseEnabled(QTreeWidgetItem* item)
    {
    if (treeWdgtTests->indexOfTopLevelItem(item) == -1)
        {
        btnExpandCollapse->setEnabled(false);
        }
    else
        {
        btnExpandCollapse->setEnabled(true);
        if (item == treeWdgtTests->currentItem())
            {
            btnExpandCollapse->setText(item->isExpanded() ? COLLAPSE : EXPAND);
            }
        }
    }

void TabWidgetTestRun::expandCollapseItem()
    {
    QTreeWidgetItem* cur = treeWdgtTests->currentItem();
    cur->setExpanded(!cur->isExpanded());
    btnExpandCollapse->setText(cur->isExpanded() ? COLLAPSE : EXPAND);
    }

bool TabWidgetTestRun::isAllCasesSelected()
    {
    return listSelectedTestItems->isEqual(listTestItems);
    }

void TabWidgetTestRun::pressLeftArrow()
    {
    // TODO
    }

void TabWidgetTestRun::pressRightArrow()
    {
    // TODO
    }

void TabWidgetTestRun::pressUpArrow()
    {
    // TODO
    }

void TabWidgetTestRun::pressDownArrow()
    {
    // TODO
    }

void TabWidgetTestRun::pressEnter()
    {
    // TODO
    }
