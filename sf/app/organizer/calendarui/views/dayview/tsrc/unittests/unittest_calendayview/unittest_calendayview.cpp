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
 * Description: Test class for CalenDayContainerWidget
 *
 */
#include <QtTest/QtTest>

#include "calenservices.h"
#include "calendateutils.h"
#include "calendaymodelmanager.h"

#define private public

#include "calendayview.h"

qint32 LOCALE_CHANGED;
CalenScrollDirection gDisallowedDirection = ECalenScrollNoDayChange;

class TestCalenDayView : public QObject
{
Q_OBJECT

public:
    TestCalenDayView();
    virtual ~TestCalenDayView();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testPopulationComplete();
    void testDoPopulation();
    void testOnBack();
    void testDayChangeStarted();
    void testRunChangeToAgendaView();
    void testRunLunarData();
    void testRunNewMeeting();
    void testRunGoToToday();
    void testChangeView();
    void testOnLocaleChanged();
    void testGetCurrentDate();
    void testIsDateValid();
    
    
private:
    CalenDayView *mView;
    MCalenServices mServices;
};

/*!
 Constructor
 */
TestCalenDayView::TestCalenDayView() :
	mView(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayView::~TestCalenDayView()
{

}

/*!
 Called before testcase
 */
void TestCalenDayView::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayView::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayView::init()
{
    mServices.IssueCommandL(0);
    mView = new CalenDayView(mServices);
}

/*!
 Called after everyfunction
 */
void TestCalenDayView::cleanup()
{

}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 */
void TestCalenDayView::testConstructors()
{
	MCalenServices services;
    //1)
    CalenDayView *testView = 0;
    QVERIFY(!testView);
    
    //2)
    testView = new CalenDayView(services);
    QVERIFY(testView);
    
    delete testView;
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command population complete is send
 */
void TestCalenDayView::testPopulationComplete()
{
    //1)
    QVERIFY(mServices.lastCommand() == 0);
    
    //2)
    mView->doPopulation();
    
    QVERIFY(mServices.lastCommand() == ECalenNotifyViewPopulationComplete);
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command population complete is send
 */
void TestCalenDayView::testDoPopulation()
{
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->populationComplete();
    
    QVERIFY(mServices.lastCommand() == ECalenNotifyViewPopulationComplete);
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command  month view is send
 */
void TestCalenDayView::testOnBack()
{
#ifndef __WINSCW__
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->onBack();
    
    QVERIFY(mServices.lastCommand() == ECalenMonthView);
#endif /*__WINSCW__*/
}

/*!
   Test if population was done.
   1)Check if next date is set
   2)Check if priovor date is set
 */
void TestCalenDayView::testDayChangeStarted()
{
#ifndef __WINSCW__
    mView->mDate = QDateTime(QDate(2010,9,8),QTime(10,10,10));
    mView->dayChangeStarted(ECalenScrollToNext);
    
    QVERIFY(mView->mDate == QDateTime(QDate(2010,9,9),QTime(10,10,10)));
    
    mView->dayChangeStarted(ECalenScrollToPrev);
        
    QVERIFY(mView->mDate == QDateTime(QDate(2010,9,8),QTime(10,10,10)));
#endif /*__WINSCW__*/
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command go to agenda view is send
 */
void TestCalenDayView::testRunChangeToAgendaView()
{
#ifndef __WINSCW__
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->runChangeToAgendaView();
    
    QVERIFY(mServices.lastCommand() == ECalenAgendaView);
#endif /*__WINSCW__*/
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command regional info taped is send
 */
void TestCalenDayView::testRunLunarData()
{
#ifndef __WINSCW__
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->runLunarData();
    
    QVERIFY(mServices.lastCommand() == ECalenRegionalPluginTapEvent); 
#endif /*__WINSCW__*/  
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command new meeting is send
 */
void TestCalenDayView::testRunNewMeeting()
{
#ifndef __WINSCW__
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->runNewMeeting();
    
    QVERIFY(mServices.lastCommand() == ECalenNewMeeting);   
#endif /*__WINSCW__*/
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command go to today is send
 */
void TestCalenDayView::testRunGoToToday()
{
#ifndef __WINSCW__
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->runGoToToday();
    
    QVERIFY(mServices.lastCommand() != 0);
    QVERIFY(mServices.Context().focusDateAndTime() == CalenDateUtils::today());
#endif /*__WINSCW__*/
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command givenn command is send
 */
void TestCalenDayView::testChangeView()
{
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->changeView(ECalenAgendaView);
    
    QVERIFY(mServices.lastCommand() == ECalenAgendaView); 
}

/*!
   Test function for onLocaleChanged
   1)Test if after system time change date change also
   2)Test if after midnight crosover date change also
   3)In above also check i locale change run in scroll area, should no
   4)Check if locale change run in scroll area after set EChangeLocale
 */
void TestCalenDayView::testOnLocaleChanged()
{
#ifndef __WINSCW__
    QDateTime testDate = QDateTime(QDate(2000, 10, 10));
    //1)
    mServices.Context().setFocusDateAndTime(testDate);
    mView->getCurrentDate();
    QCOMPARE(mView->mDate,testDate);
    mView->onLocaleChanged(EChangesSystemTime);
    QCOMPARE(mView->mDate,CalenDateUtils::today());

    //2)
    mServices.Context().setFocusDateAndTime(testDate);
    mView->getCurrentDate();
    QCOMPARE(mView->mDate,testDate);
    mView->onLocaleChanged(EChangesMidnightCrossover);
    QCOMPARE(mView->mDate,CalenDateUtils::today());
    
    //3)
    mServices.Context().setFocusDateAndTime(testDate);
    mView->getCurrentDate();
    mView->onLocaleChanged(EChangesSystemTime);
    QCOMPARE(mView->mDate,CalenDateUtils::today());
    
    //1)
    mServices.Context().setFocusDateAndTime(testDate);
    mView->getCurrentDate();
    mView->onLocaleChanged(EChangesLocale);
    QCOMPARE(LOCALE_CHANGED,1);
#endif /*__WINSCW__*/
}

/*!
   Test function for getCurrentDate.
   
   Check if date given by context is good
 */
void TestCalenDayView::testGetCurrentDate()
{
#ifndef __WINSCW__
    QDateTime testDate = QDateTime(QDate(2000, 10, 10), QTime(0, 0));
    mServices.Context().setFocusDateAndTime(testDate);
    mView->getCurrentDate();
    QCOMPARE(mView->mDate,testDate);
#endif /*__WINSCW__*/
}

/*!
 Test function for checkDate helper function.
 
 1) date supported
 2) lowest supported date
 3) highest supported date
 */
void TestCalenDayView::testIsDateValid()
{
#ifndef __WINSCW__
    //1)
    mView->mDate = QDateTime(QDate(2010, 9, 22), QTime(0, 0));
    mView->isDateValid();
    QCOMPARE(gDisallowedDirection, ECalenScrollNoDayChange);

    //2)
    mView->mDate = QDateTime(QDate(1900, 1, 1), QTime(0, 0));
    mView->isDateValid();
    QCOMPARE(gDisallowedDirection, ECalenScrollToPrev);

    //3)
    mView->mDate = QDateTime(QDate(2100, 12, 30), QTime(0, 0));
    mView->isDateValid();
    QCOMPARE(gDisallowedDirection, ECalenScrollToNext);
#endif /*__WINSCW__*/
}

QTEST_MAIN(TestCalenDayView);

#include "unittest_calendayview.moc"
