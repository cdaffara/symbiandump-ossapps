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
 * Description:
 *
 */
#include <QGraphicsItem>
#include <hbmodeliterator.h>
#include <QtTest/QtTest>

#include "calenservices.h"

#include <hbabstractitemview.h>
#include <hbmenu.h>

#ifndef __WINSCW__
#define private public
#endif

#include "calendayitemview.h"
#include "calendaymodel.h"

// Test variables
QRectF gTestWindowRect = QRectF(0, 0, 10, 20);
Qt::Orientation gTestOrientation = Qt::Horizontal;
quint32 SELECTED_COMMAND = 0; 

class TestCalenItemView : public QObject
{
Q_OBJECT

public:
    TestCalenItemView();
    virtual ~TestCalenItemView();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testIssueCommandOnSelectedItem();
    void testOpenSelectedItem();
    void testEditSelectedItem();
    void testDeleteSelectedItem();
    void testItemActivated();
    void testSetContextMenu();
    void testContextMenu();
    void testModelReset();
private:
    MCalenServices   mMCalenServices;
    CalenDayItemView *mCalenDayItemView;

};

/*!
 Constructor
 */
TestCalenItemView::TestCalenItemView()
{

}

/*!
 Destructor
 */
TestCalenItemView::~TestCalenItemView()
{

}

/*!
 Called before testcase
 */
void TestCalenItemView::initTestCase()
{
    
}

/*!
 Called after testcase
 */
void TestCalenItemView::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenItemView::init()
{
    HbModelIterator *iterator = new HbModelIterator();
    mCalenDayItemView = new CalenDayItemView(mMCalenServices,iterator,0);
    mCalenDayItemView->setModel(new CalenDayModel( 
            QDateTime(QDate(12,07,2010),QTime(6,0)), mMCalenServices));
    SELECTED_COMMAND = 0;
}

/*!
 Called after everyfunction
 */
void TestCalenItemView::cleanup()
{
    if(mCalenDayItemView){
        delete mCalenDayItemView;
        mCalenDayItemView = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 */
void TestCalenItemView::testConstructors()
{
    //1)
    CalenDayItemView *testItemView = 0;
    QVERIFY(!testItemView);
    
    //2)
    HbModelIterator *iterator = new HbModelIterator();
    testItemView = new CalenDayItemView(mMCalenServices,iterator,0);
    QVERIFY(testItemView);
    delete testItemView;
}

/*!
 Test issueing command on selected item.
 */
void TestCalenItemView::testIssueCommandOnSelectedItem()
{
#ifndef __WINSCW__
    mCalenDayItemView->model()->insertRow(0, QModelIndex());
    QModelIndex modelIndex = mCalenDayItemView->model()->index(0, 0,
        QModelIndex());
    mCalenDayItemView->selectionModel()->setCurrentIndex(modelIndex,
        QItemSelectionModel::Select);
    mCalenDayItemView->issueCommandOnSelectedItem((quint32)ECalenEventView);
    QCOMPARE(SELECTED_COMMAND,(quint32)ECalenEventView);
#endif
}

/*!
 Test opening command on selected item.
 */
void TestCalenItemView::testOpenSelectedItem()
{
#ifndef __WINSCW__
    mCalenDayItemView->model()->insertRow(0, QModelIndex());
    QModelIndex modelIndex = mCalenDayItemView->model()->index(0, 0, 
        QModelIndex());
    mCalenDayItemView->selectionModel()->setCurrentIndex(modelIndex, 
        QItemSelectionModel::Select);
    mCalenDayItemView->openSelectedItem();
    QCOMPARE(SELECTED_COMMAND,(quint32)ECalenEventView);
#endif 
}

/*!
 Test edit command on selected item.
 */
void TestCalenItemView::testEditSelectedItem()
{
#ifndef __WINSCW__
    mCalenDayItemView->model()->insertRow(0, QModelIndex());
    QModelIndex modelIndex = mCalenDayItemView->model()->index(0, 0, 
        QModelIndex());
    mCalenDayItemView->selectionModel()->setCurrentIndex(modelIndex, 
        QItemSelectionModel::Select);
    mCalenDayItemView->editSelectedItem();
    QCOMPARE(SELECTED_COMMAND,(quint32)ECalenEditCurrentEntry);
#endif
}

/*!
 Test delete command on selected item.
 */
void TestCalenItemView::testDeleteSelectedItem()
{
#ifndef __WINSCW__
    mCalenDayItemView->model()->insertRow(0, QModelIndex());
    QModelIndex modelIndex = mCalenDayItemView->model()->index(0, 0, 
        QModelIndex());
    mCalenDayItemView->selectionModel()->setCurrentIndex(modelIndex, 
        QItemSelectionModel::Select);
    mCalenDayItemView->deleteSelectedItem();
    QCOMPARE(SELECTED_COMMAND,(quint32)ECalenDeleteCurrentEntry);
#endif
}

/*!
 Test activation of item.
 */
void TestCalenItemView::testItemActivated()
{
#ifndef __WINSCW__
    mCalenDayItemView->model()->insertRow(0, QModelIndex());
    QModelIndex modelIndex = mCalenDayItemView->model()->index(0, 0, 
        QModelIndex());
    mCalenDayItemView->selectionModel()->setCurrentIndex(modelIndex, 
        QItemSelectionModel::Select);
    mCalenDayItemView->itemActivated( modelIndex );
    QCOMPARE(SELECTED_COMMAND,(quint32)ECalenEventView);
#endif
}

/*!
 Test setting context menu
 */
void TestCalenItemView::testSetContextMenu()
{
    HbMenu* contextMenu = new HbMenu(NULL);
    mCalenDayItemView->setContextMenu(contextMenu);
    QCOMPARE(contextMenu, mCalenDayItemView->contextMenu());
    delete contextMenu;
}

/*!
 Test context menu
 1. Testing existing context menu
 2. Setting new context menu
 */
void TestCalenItemView::testContextMenu()
{
    //1)
    QVERIFY(mCalenDayItemView->contextMenu() != NULL);
    
    //2)
    HbMenu* contextMenu = new HbMenu(NULL);
    mCalenDayItemView->setContextMenu(contextMenu);
    QCOMPARE(contextMenu, mCalenDayItemView->contextMenu());
    delete contextMenu;
}

/*!
 Test model reset
 1. Inserting timed event directly
 2. Inserting timed event to model
 3. Inserting al day event to model
 4. Setting model
 5. Setting day event info
 */
void TestCalenItemView::testModelReset()
{
#ifndef __WINSCW__
    //1)
    SCalenApptInfo dayInfo;
    mCalenDayItemView->mInfo->InsertTimedEvent(dayInfo);
    mCalenDayItemView->modelReset();
    QCOMPARE(mCalenDayItemView->mInfo->RegionList().count(), 0);
    
    //2)
    // insert timed event
    mCalenDayItemView->model()->insertRow(0, QModelIndex());
    mCalenDayItemView->modelReset();
    QCOMPARE(mCalenDayItemView->mInfo->RegionList().count(), 1);
    
    //3)
    // insert all day event
    mCalenDayItemView->model()->insertRow(1, QModelIndex());
    mCalenDayItemView->modelReset();
    QCOMPARE(mCalenDayItemView->mInfo->AlldayCount(), 1);

    //4)
    mCalenDayItemView->setModel(NULL);
    mCalenDayItemView->modelReset();
    QCOMPARE(mCalenDayItemView->mInfo->RegionList().count(), 0);
    
    //5)
    mCalenDayItemView->mInfo = NULL;
    mCalenDayItemView->modelReset();
    QVERIFY(mCalenDayItemView->mInfo == NULL);
#endif
}

QTEST_MAIN(TestCalenItemView);
#include "unittest_calendayitemview.moc"
