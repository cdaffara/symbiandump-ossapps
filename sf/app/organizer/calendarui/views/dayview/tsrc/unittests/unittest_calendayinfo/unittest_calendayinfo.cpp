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
 * Description: Test class for CalenDayContentWidget
 *
 */
#include <QGraphicsItem>
#include <QtTest/QtTest>
#include <QDebug>


#include <HbMainWindow>
#include "calendaycommonheaders.h"
#include "calendayutils.h"

#define private public

#include "calendayinfo.h"

class TestCalenDayInfo : public QObject
{
Q_OBJECT

public:
    TestCalenDayInfo();
    virtual ~TestCalenDayInfo();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testReset();
    void testInsertTimedEvent();
    void testInsertUntimedEvent();
    void testInsertAlldayEvent();
    void testSuggestedUntimedSlotPos();
    void testNeededUntimedSlotCount();
    void testFirstOccupiedSlot();
    void testLastOccupiedSlot();
    void testEarliestEndSlot();
    void testLastStartSlot();
    void testSlotIndexForStartTime();
    void testSlotIndexForEndTime();
    void testAlldayCount();
    void testTodoCount();
    void testTimeColumnContainsEvent();
    void testTimeColumnAddOffset();
    void testSlotIntervalAddOffset();
    void testSlotIntervalOverlaps();
    void testSlotIntervalUnion();
    void testSlotIntervalAdjacent();
    void testRegionAddOffset();
    void testRegionOverlaps();
    void testIsAlldayEvent_data();
    void testIsAlldayEvent();
    void testGetLocation();
    void testGetSelectedSlot();
    void testFindRegion();
    void testFindEvent();
    void testIsAlldayEventSelected();
    void testAlldayEvent();
    
    void testConstructors();

private:
    CalenDayInfo *mInfo;
};

/*!
 Constructor
 */
TestCalenDayInfo::TestCalenDayInfo() :
   mInfo(NULL)
{
	qDebug() << "test start";
}

/*!
 Destructor
 */
TestCalenDayInfo::~TestCalenDayInfo()
{
	
}

/*!
 Called before testcase
 */
void TestCalenDayInfo::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayInfo::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayInfo::init()
{
    mInfo = new CalenDayInfo(CalenDayInfo::ETwo);
}

/*!
 Called after everyfunction
 */
void TestCalenDayInfo::cleanup()
{
    if (mInfo) {
        delete mInfo;
        mInfo = NULL;
    }
}

void TestCalenDayInfo::testReset()
	{
		SCalenApptInfo info;
		mInfo->InsertAlldayEvent(info);
		mInfo->InsertTimedEvent(info);
		
		mInfo->Reset();
		
		QCOMPARE (mInfo->iAlldayEvents.length(), 0);
		QCOMPARE (mInfo->iRegionList.length(), 0);
	}

void TestCalenDayInfo::testInsertTimedEvent()
	{
		SCalenApptInfo info;
		info.iStartTime = QDateTime(QDate(2010, 7, 10), QTime(23, 45, 45));
			
		info.iEndTime = QDateTime(QDate(2010, 7, 10), QTime(23, 49, 45));
		mInfo->InsertTimedEvent(info);
		
		QCOMPARE (mInfo->iRegionList.count(), 1);
	}

void TestCalenDayInfo::testInsertUntimedEvent()
	{

	}

void TestCalenDayInfo::testInsertAlldayEvent()
	{
		SCalenApptInfo info;
		info.iId = TCalenInstanceId::nullInstanceId();
		info.iStatus = AgendaEntry::StatusUnknown; 
	
		mInfo->InsertAlldayEvent(info);
		
		QCOMPARE(mInfo->iAlldayEvents.length(), 1);
	}

void TestCalenDayInfo::testSuggestedUntimedSlotPos()
	{
		mInfo->iSlotsInHour = CalenDayInfo::ETwo;
		//iregionlist.length  =  0
		mInfo->iRegionList.clear();
		QCOMPARE(mInfo->SuggestedUntimedSlotPos(), 16);
		
		//iregionlist.length  <>  0
		CalenTimeRegion region;
		region.iStartSlot = 10;
		mInfo->iRegionList.append(region);
		
		QCOMPARE(mInfo->SuggestedUntimedSlotPos(), 10);
	}

void TestCalenDayInfo::testNeededUntimedSlotCount()
	{
		mInfo->iUntimedEvents.append(TCalenInstanceId());
		QCOMPARE(mInfo->iUntimedEvents.count(), mInfo->NeededUntimedSlotCount());
		
		mInfo->iTodoEvents.append(TCalenInstanceId());
		QCOMPARE(mInfo->iUntimedEvents.count() + 1, mInfo->NeededUntimedSlotCount());
	}

void TestCalenDayInfo::testFirstOccupiedSlot()
	{
		mInfo->iUntimedSlotCount = 0;
		QCOMPARE(mInfo->FirstOccupiedSlot(), -1);
		
		mInfo->iUntimedSlotCount = 1;
		mInfo->iFirstUntimedSlot = 8;
		QCOMPARE(mInfo->FirstOccupiedSlot(), 8);
		
		mInfo->iUntimedSlotCount = 0;
		CalenTimeRegion region;
		region.iStartSlot = 16;
		
		mInfo->iRegionList.append(region);
		QCOMPARE(mInfo->FirstOccupiedSlot(), 16);
	}

void TestCalenDayInfo::testLastOccupiedSlot()
	{
		mInfo->iUntimedSlotCount = 0;
		QCOMPARE(mInfo->LastOccupiedSlot(), -1);
		
		mInfo->iUntimedSlotCount = 1;
		mInfo->iFirstUntimedSlot = 8;
		QCOMPARE(mInfo->LastOccupiedSlot(), 8);
		
		CalenTimeRegion region;
		region.iEndSlot = 16;	
		mInfo->iRegionList.append(region);
		QCOMPARE(mInfo->LastOccupiedSlot(), 16);
	}

void TestCalenDayInfo::testEarliestEndSlot()
	{
		mInfo->iUntimedSlotCount = 10;
		mInfo->iEmptyUntimedSlots = 10;
		mInfo->iEarliestEndSlot = 2;
		
		QCOMPARE(mInfo->EarliestEndSlot(), -1);
		
		CalenTimeRegion region;
		region.iEndSlot = 16;	
		mInfo->iRegionList.append(region);
		QCOMPARE(mInfo->EarliestEndSlot(), 12);
		
		mInfo->iUntimedSlotCount = 15;
		mInfo->iEmptyUntimedSlots = 10;
		mInfo->iFirstUntimedSlot = 2;
		
		QCOMPARE(mInfo->EarliestEndSlot(), 13);
	}

void TestCalenDayInfo::testLastStartSlot()
	{
		
		mInfo->iUntimedSlotCount = 5; 
		mInfo->iEmptyUntimedSlots = 5;
		QCOMPARE(mInfo->LastStartSlot(), -1);
		
		mInfo->iUntimedSlotCount = 8; 
		mInfo->iEmptyUntimedSlots = 5;
		mInfo->iFirstUntimedSlot = 10;
		QCOMPARE(mInfo->LastStartSlot(), 17);
		
		CalenTimeRegion region;
		region.iEndSlot = 16;	
		mInfo->iRegionList.append(region);
		mInfo->iLastStartSlot = 5;
		mInfo->iUntimedSlotCount = 8; 
		QCOMPARE(mInfo->LastStartSlot(), 13);
	}

void TestCalenDayInfo::testSlotIndexForStartTime()
	{
		QDateTime dt;
		dt.setTime(QTime(10, 5, 0));
		mInfo->iSlotsInHour = CalenDayInfo::EOne;
		mInfo->iUntimedSlotCount = 0;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 10);
		
		qDebug() << dt.time().hour() << ":" << dt.time().minute();
		
		mInfo->iSlotsInHour = CalenDayInfo::ETwo;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 20);
		
		mInfo->iSlotsInHour = CalenDayInfo::EThree;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 30);
		
		mInfo->iSlotsInHour = CalenDayInfo::EFour;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 40);
		//---------
		
		dt.setTime(QTime(10, 20, 0));
		
		mInfo->iSlotsInHour = CalenDayInfo::EOne;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 10);
		
		mInfo->iSlotsInHour = CalenDayInfo::ETwo;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 20);
		
		mInfo->iSlotsInHour = CalenDayInfo::EThree;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 31);
		
		mInfo->iSlotsInHour = CalenDayInfo::EFour;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 41);
		
		//---------
		
		dt.setTime(QTime(10, 35, 0));
		
		mInfo->iSlotsInHour = CalenDayInfo::EOne;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 10);
		
		mInfo->iSlotsInHour = CalenDayInfo::ETwo;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 21);
		
		mInfo->iSlotsInHour = CalenDayInfo::EThree;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 31);
		
		mInfo->iSlotsInHour = CalenDayInfo::EFour;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 42);
		
		//---------
		dt.setTime(QTime(10, 45, 0));
		
		mInfo->iSlotsInHour = CalenDayInfo::EOne;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 10);
		
		mInfo->iSlotsInHour = CalenDayInfo::ETwo;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 21);
		
		mInfo->iSlotsInHour = CalenDayInfo::EThree;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 32);
		
		mInfo->iSlotsInHour = CalenDayInfo::EFour;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 43);
		
	}

void TestCalenDayInfo::testSlotIndexForEndTime()
	{

	}

void TestCalenDayInfo::testAlldayCount()
	{
		QCOMPARE(mInfo->AlldayCount(), 0);
		mInfo->iAlldayEvents.append(CalenTimedEventInfo());
		QCOMPARE(mInfo->AlldayCount(), 1);
	}

void TestCalenDayInfo::testTodoCount()
	{
		QCOMPARE(mInfo->TodoCount(), 0);
		mInfo->iTodoEvents.append(TCalenInstanceId());
		QCOMPARE(mInfo->TodoCount(), 1);
	}

/*!
 Test CalenTimeColumn::ContainsEvent() function
 1. Test ContainsEvent(), no events
 2. Test ContainsEvent(), events found
 */
void TestCalenDayInfo::testTimeColumnContainsEvent() 
{
    CalenTimeColumn column;
    
    //1)
    ulong id = 1;
    QDateTime dateTime = QDateTime::currentDateTime();
    bool instanceViewed = true;
    TCalenInstanceId instanceId(id, dateTime, instanceViewed);
    QCOMPARE(column.ContainsEvent(instanceId), false);
    
    //2)
    CalenTimedEventInfo info;
    info.iStartSlot = 5;
    info.iEndSlot = 10;
    info.iId = instanceId;
    column.AddEvent(info);
    QCOMPARE(column.ContainsEvent(instanceId), true);
}

/*!
 Test CalenTimeColumn::AddOffset() function
 1. Test addOffset, offset = 0
 2. Test addOffset, offset != 0, no events
 3. Test addOffset, offset != 0, events count > 0
 */
void TestCalenDayInfo::testTimeColumnAddOffset()
{
    const int start = 5;
    const int end = 10;
    CalenTimeColumn column;
    column.iStartSlot = start;
    column.iEndSlot = end;
    
    //1)
    int offset = 0;
    column.AddOffset(offset, 0);
    QCOMPARE(column.iStartSlot, start);
    QCOMPARE(column.iEndSlot, end);
    
    //2)
    offset = 5;
    column.AddOffset(offset, 0);
    QCOMPARE(column.iStartSlot, start + offset);
    QCOMPARE(column.iEndSlot, end + offset);
    
    //3)
    column.iStartSlot = start;
    column.iEndSlot = end;
    CalenTimedEventInfo eventInfo;
    eventInfo.iStartSlot = start;
    eventInfo.iEndSlot = end;
    column.iEventArray.append(eventInfo);
    column.AddOffset(offset, 0);
    QCOMPARE(column.iStartSlot, start + offset);
    QCOMPARE(column.iEndSlot, end + offset);
    QCOMPARE(column.iEventArray.at(0).iStartSlot, start + offset);
    QCOMPARE(column.iEventArray.at(0).iEndSlot, end + offset);
}

/*!
 Test CalenSlotInterval::AddOffset() function
 1. Test addOffset, pos < iStartSlot
 2. Test addOffset, pos > iStartSlot
 3. Test addOffset, pos > iEndSlot
 */
void TestCalenDayInfo::testSlotIntervalAddOffset()
{
    const int start = 5;
    const int end = 10;
    
    CalenSlotInterval interval;
    interval.iStartSlot = start;
    interval.iEndSlot = end;
    
    //1)
    int offset = 2;
    int pos = 0;
    interval.AddOffset(offset, pos);
    QCOMPARE(interval.iStartSlot, start + offset);
    QCOMPARE(interval.iEndSlot, end + offset);
    
    //2)
    pos = 6;
    interval.iStartSlot = start;
    interval.iEndSlot = end;
    interval.AddOffset(offset, pos);
    QCOMPARE(interval.iStartSlot, start);
    QCOMPARE(interval.iEndSlot, end + offset);
    
    //3)
    pos = 12;
    interval.iStartSlot = start;
    interval.iEndSlot = end;
    interval.AddOffset(offset, pos);
    QCOMPARE(interval.iStartSlot, start);
    QCOMPARE(interval.iEndSlot, end);
}

/*!
 Test CalenSlotInterval::Overlaps() function
 1. Test non overlapping interval
 2. Test overlapping interval
 3. Test overlapping interval
 */
void TestCalenDayInfo::testSlotIntervalOverlaps()
{
    CalenSlotInterval interval;
    interval.iStartSlot = 5;
    interval.iEndSlot = 10;
    
    //1)
    CalenSlotInterval testInterval;
    testInterval.iStartSlot = 15;
    testInterval.iEndSlot = 25;
    QCOMPARE(interval.Overlaps(testInterval), false);
    
    //2)
    testInterval.iStartSlot = 1;
    testInterval.iEndSlot = 8;
    QCOMPARE(interval.Overlaps(testInterval), true);
    
    //3)
    testInterval.iStartSlot = 9;
    testInterval.iEndSlot = 15;
    QCOMPARE(interval.Overlaps(testInterval), true);
}

/*!
 Test CalenSlotInterval::Union() function
 1. Test Union(), empty test interval
 2. TTest Union(), non empty test interval
 */
void TestCalenDayInfo::testSlotIntervalUnion()
{
    const int start = 5;
    const int end = 10;
    
    CalenSlotInterval interval;
    interval.iStartSlot = start;
    interval.iEndSlot = end;
    
    //1)
    CalenSlotInterval testInterval;
    testInterval.iStartSlot = start - 1;
    testInterval.iEndSlot = start - 2;
    interval.Union(testInterval);
    QCOMPARE(interval.iStartSlot, start);
    QCOMPARE(interval.iEndSlot, end);
    
    //2)
    testInterval.iStartSlot = start + 1;
    testInterval.iEndSlot = end + 1;
    interval.Union(testInterval);
    QCOMPARE(interval.iStartSlot, start);
    QCOMPARE(interval.iEndSlot, end + 1);
}

/*!
 Test CalenSlotInterval::Adjacent() function
 1. Test overlapping interval
 2. Test non overlapping interval, adjacent
 3. Test non overlapping interval, non adjacent
 */
void TestCalenDayInfo::testSlotIntervalAdjacent()
{
    const int start = 5;
    const int end = 10;
    
    CalenSlotInterval interval;
    interval.iStartSlot = start;
    interval.iEndSlot = end;
    
    //1)
    CalenSlotInterval testInterval;
    testInterval.iStartSlot = start + 1;
    testInterval.iEndSlot = end - 1;
    QCOMPARE(interval.Adjacent(testInterval), true);
    
    //2)
    testInterval.iStartSlot = start - 3;
    testInterval.iEndSlot = start;
    QCOMPARE(interval.Adjacent(testInterval), true);
    
    //3)
    testInterval.iStartSlot = start - 3;
    testInterval.iEndSlot = start - 1;
    QCOMPARE(interval.Adjacent(testInterval), false);
}

/*!
 Test CalenTimeRegion::AddOffset() function
 1. Test addOffset, offset = 0, intervals empty, columns empty
 2. Test addOffset, offset != 0, intervals empty, columns empty
 3. Test addOffset, offset != 0, intervals not empty, columns not empty
 */
void TestCalenDayInfo::testRegionAddOffset()
{
    CalenTimeRegion region;
    
    //1)
    int offset = 0;
    int pos = 0;
    region.AddOffset(offset, pos);
    QCOMPARE(region.iIntervals.count(), 0);
    QCOMPARE(region.iColumns.count(), 0);
    
    //2)
    offset = 2;
    region.AddOffset(offset, pos);
    QCOMPARE(region.iIntervals.count(), 0);
    QCOMPARE(region.iColumns.count(), 0);
    
    //3)
    CalenEventInterval interval;
    interval.iStartSlot = 1;
    interval.iEndSlot = 5;
    region.iIntervals.append(interval);
    
    CalenTimeColumn column;
    column.iStartSlot = 2;
    column.iEndSlot = 4;
    region.iColumns.append(column);
    
    region.AddOffset(offset, pos);
    
    QCOMPARE(region.iIntervals.at(0).iStartSlot, interval.iStartSlot + offset);
    QCOMPARE(region.iIntervals.at(0).iEndSlot, interval.iEndSlot + offset);
    
    QCOMPARE(region.iColumns.at(0).iStartSlot, column.iStartSlot + offset);
    QCOMPARE(region.iColumns.at(0).iEndSlot, column.iEndSlot + offset);
}

/*!
 Test CalenTimeRegion::Overlaps() function
 1. Test non overlapping
 2. Test overlapping
 */
void TestCalenDayInfo::testRegionOverlaps()
{
    CalenTimeRegion region;
    region.iStartSlot = 5;
    region.iEndSlot = 10;
    
    CalenTimeColumn column;
    region.iColumns.append(column);
    
    //1)
    CalenSlotInterval testInterval;
    testInterval.iStartSlot = 15;
    testInterval.iEndSlot = 20;
    QCOMPARE(region.Overlaps(testInterval), false);
    
    //2)
    testInterval.iStartSlot = 4;
    testInterval.iEndSlot = 12;
    QCOMPARE(region.Overlaps(testInterval), true);
}

/*!
 Test function for constructors
 1. Test if CalenDayInfo is not initialized
 2. Test if CalenDayInfo is correcty created
 */
void TestCalenDayInfo::testConstructors()
{
    //1)
    CalenDayInfo *testCalenDayInfo = 0;
    QVERIFY(!testCalenDayInfo);
    
    //2)
    testCalenDayInfo = new CalenDayInfo(CalenDayInfo::ETwo);
    QVERIFY(testCalenDayInfo);
}

/*!
 Test data for testIsAlldayEvent
 Data:
 1. Test if false is return when days are diferent
 Below days are the same
 2. Test when start and end time are not midnight
 3. Test when start is midnight and end not
 4. Test when start is not midnight and end is
 5. Test when start and end are midnight
 */
void TestCalenDayInfo::testIsAlldayEvent_data()
{
    QTest::addColumn<QDateTime>("testedValueStart");
    QTest::addColumn<QDateTime>("testedValueEnd");
    QTest::addColumn<bool>("testValue");

    QTest::newRow("Diferent days") 
                  << QDateTime(QDate(2010,9,14),QTime(12,20))
                  << QDateTime(QDate(2010,9,15),QTime(12,20))
                  << false;
    QTest::newRow("Diferent times, not midnight") 
                  << QDateTime(QDate(2010,9,14),QTime(12,20))
                  << QDateTime(QDate(2010,9,14),QTime(16,40))
                  << false;
    QTest::newRow("Diferent times, start is mignight") 
                  << QDateTime(QDate(2010,9,14),QTime(0,0))
                  << QDateTime(QDate(2010,9,14),QTime(12,20))
                  << false;
    QTest::newRow("Diferent times, end is mignight") 
                  << QDateTime(QDate(2010,9,14),QTime(12,20))
                  << QDateTime(QDate(2010,9,14),QTime(0,0))
                  << false;
    QTest::newRow("Both are midnight") 
                  << QDateTime(QDate(2010,9,14),QTime(0,0,0,0))
                  << QDateTime(QDate(2010,9,15),QTime(0,0,0,0))
                  << true;
}

/*!
  Test function for IsAllDayEvent, it use data from testIsAlldayEvent_data

 */
void TestCalenDayInfo::testIsAlldayEvent()
{
    //get data to test
    QFETCH(QDateTime, testedValueStart); 
    QFETCH(QDateTime, testedValueEnd); 
    QFETCH(bool, testValue);

    //make test
    QCOMPARE(mInfo->IsAlldayEvent(testedValueStart,testedValueEnd),testValue);
    
}

void TestCalenDayInfo::testGetLocation()
{
    
}

/*!
   Test function for getSelectedSlot
   1)Check when there is no region is selected
   2)Check when region is selected
 */
void TestCalenDayInfo::testGetSelectedSlot()
{
    int slot = 0;
    int region = 0;
    int columnIndex = 0;
    int columns = 0;
    //1)
    mInfo->iSelectedRegion = -1;
    mInfo->GetSelectedSlot(slot,region,columnIndex,columns);
    QCOMPARE(slot , mInfo->iSelectedSlot);
    QCOMPARE(region, KErrNotFound);
    QCOMPARE(columnIndex, mInfo->iSelectedColumn);
    QCOMPARE(columns, 0);
    
    //2)
    CalenTimeRegion timeRegion = CalenTimeRegion();
    timeRegion.iColumns << CalenTimeColumn();
    mInfo->iRegionList.append(CalenTimeRegion());
    mInfo->iRegionList.append(CalenTimeRegion());
    
    mInfo->iSelectedRegion = 1;
    mInfo->GetSelectedSlot(slot,region,columnIndex,columns);
    QCOMPARE(slot , mInfo->iSelectedSlot);
    QCOMPARE(region, mInfo->iSelectedRegion);
    QCOMPARE(columnIndex, mInfo->iSelectedColumn);
    QCOMPARE(columns, 0);
}

/*!
   Test function for FindRegion.
   1)Preapre region in day infor
   2)Prepare region inside above
   3)Find this inside region
 */
void TestCalenDayInfo::testFindRegion()
{
#ifndef __WINSCW__
    //1)
    CalenTimeRegion region;
    region.iStartSlot = 1;  
    region.iEndSlot = 16;  
    region.iColumns << CalenTimeColumn();
    mInfo->iRegionList.append(region);
    
    //2)
    CalenSlotInterval interval;
    interval.iStartSlot = 8;
    interval.iEndSlot = 12;
    
    //3)
    QVERIFY(mInfo->FindRegion(interval,1) == 0);
#endif /*__WINSCW__*/
}

/*!
   Test function for FindEvent
   1)Prepare events and select one to test
   2)Find selected event
 */
void TestCalenDayInfo::testFindEvent()
{
#ifndef __WINSCW__
    //1)
    CalenTimedEventInfo event;
    event.iStartSlot = 3;
    event.iEndSlot = 13;
    
    CalenTimeColumn column = CalenTimeColumn();
    column.AddEvent(event);
    
    CalenTimeRegion region;
    region.iStartSlot = 1;  
    region.iEndSlot = 16;  
    region.iColumns << column;
    mInfo->iRegionList.append(region);
    

    region.iStartSlot = 17;  
    region.iEndSlot = 20;  
    mInfo->iRegionList.append(region);
    
    //preapre data to search
    CalenSlotInterval interval;
    interval.iStartSlot = 8;
    interval.iEndSlot = 12;
    
    mInfo->iSelectedRegion = 0;
    mInfo->iSelectedColumn = 0;
    
    //2)
    QVERIFY(mInfo->FindEvent(interval,1) == 0);
#endif /*__WINSCW__*/
}

/*!
  Test function for IsAlldayEventSelected
  1)There is no all day event selected
  2)There is all day event selected
 */
void TestCalenDayInfo::testIsAlldayEventSelected()
{
    //1)
    QVERIFY(mInfo->IsAlldayEventSelected() == false);
    
    //2)
    mInfo->iSelectedAlldayEvent = 0;
    mInfo->iAlldayEvents.append(CalenTimedEventInfo());
    
    QVERIFY(mInfo->IsAlldayEventSelected() == true);
}

/*!
   Test function for AlldayEvent
   1)Prepare all day event at inxdex 0
   2)Check if good all day event is returned
 */
void TestCalenDayInfo::testAlldayEvent()
{
    CalenTimedEventInfo allDayEvent;
    TCalenInstanceId id;
    id = TCalenInstanceId::create(100, QDateTime(QDate(2010,9,21), QTime(12,12,12)),false);
    allDayEvent.iId = id;
    mInfo->iAlldayEvents.append(allDayEvent);
    
    QVERIFY(mInfo->AlldayEvent(0).iId == id);
}

QTEST_MAIN(TestCalenDayInfo);

#include "unittest_calendayinfo.moc"
