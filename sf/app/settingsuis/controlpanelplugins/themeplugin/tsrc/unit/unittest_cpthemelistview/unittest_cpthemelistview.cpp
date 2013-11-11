/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors: XENT-MV
*
* Description:  unit tests for the CpThemeControl class from themeplugin
*
*/

#include <QtTest/QtTest>
#include <QGraphicsWidget>
#include <QStandardItemModel>

#include "cpthemelistview.h"

class TestCpThemeListView : public QObject
{
    Q_OBJECT

private slots:
    void testConstructor();
    void testSetWidget();
    void testThemeList();
    void testSetModel();
    void testCloseView();
};

void TestCpThemeListView::testConstructor()
{
    CpThemeListView* obj = new CpThemeListView();
    QVERIFY( obj != 0 );
    delete obj;
}

void TestCpThemeListView::testSetWidget()
{
    CpThemeListView *listView = new CpThemeListView();
    QGraphicsWidget *testWidget = new QGraphicsWidget();

    listView->setWidget(testWidget);

    QVERIFY( listView->widget() == testWidget);

    delete testWidget;
    delete listView;
}

void TestCpThemeListView::testThemeList()
{
    CpThemeListView *listView = new CpThemeListView();

    QVERIFY( listView->themeList() != 0 );

    delete listView;
}

void TestCpThemeListView::testSetModel()
{
    CpThemeListView *listView = new CpThemeListView();
    QStandardItemModel *model = new QStandardItemModel(this);

    listView->setModel(model);
    QVERIFY(listView->themeList()->model() == model);

    // model shouldn't be deleted because it is a QObject with a parent.

    delete listView;
}

// null test
void TestCpThemeListView::testCloseView()
{
    CpThemeListView *listView = new CpThemeListView();

    listView->closeView();

    delete listView;
}



QTEST_MAIN(TestCpThemeListView)
#include "unittest_cpthemelistview.moc"
