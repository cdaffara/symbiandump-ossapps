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
 * Description: AgendaUtil unit test class.
 *
 */

// System includes
#include <QtTest/QtTest>
#include <QtGui>
#include <e32base.h>

// User includes
#include "agendautil.h"

class TestAgendaUtil : public QObject
{
	Q_OBJECT

public slots:
	void handleEntriesDeletion(int error);
	void handleEntryViewCreationCompleted(int status);

private slots:
	// Test case setup.
	void init();
	void cleanup();
	void initTestCase();
	void cleanupTestCase();

	// Test cases.
	void test_storeWithNewEntry();
	void test_deleteEntry();
	void test_deleteRepeatedEntry();
	void test_fetchById();
	void test_storeWithExistingEntry();
	void test_createEntryIdListForDay();
	void test_entryIds();
	void test_fetchAllEntries();
	void test_deleteEntries();
	void test_fetchEntriesInRange();
	void test_cloneEntry();
	void test_todoCompleted();
	void test_createExceptionalEntryWithSubjectChange();
	void test_createExceptionalEntryWithStartTimeChange();

private:
	int createMultipleEntries();
	void cloneNoteToTodo();
	void cloneNoteToMeeting();
	void cloneTodoToNote();
	void cloneTodoToMeeting();
	void cloneMeetingToNote();
	void cloneMeetingToTodo();
	void completeTodo();
	void unCompletedTodo();

private:
	AgendaUtil *mAgendaUtil;
	CActiveSchedulerWait* mWait;
};

/*!
	\class TestAgendaUtil

	This is unittest class for AgendaUtil apis.
 */

/*!
	This function is called to before every testcase.
 */
void TestAgendaUtil::init()
{
	/*QT_TRAP_THROWING(mAgendaUtil = new AgendaUtil);

	Q_ASSERT_X(mAgendaUtil, "TestAgendaUtil::test_addEntry", "No agendautil");

	// Connect to the required signals
	connect(
			mAgendaUtil, SIGNAL(entriesDeleted(int)), this,
			SLOT(handleEntriesDeletion(int)));

	mWait = NULL;*/
}

/*!
	This function is called after every testcase.
 */
void TestAgendaUtil::cleanup()
{

	//User::Exit(0);
    if (mWait && mWait->IsStarted()) {
                mWait->AsyncStop();
    }
    delete mWait;
    mWait = NULL;
}
/*!
	This function is called to init the testcase.
 */
void TestAgendaUtil::initTestCase()
{
	QT_TRAP_THROWING(mAgendaUtil = new AgendaUtil);

	connect(
			mAgendaUtil, SIGNAL(entryViewCreationCompleted(int)),
			this, SLOT(handleEntryViewCreationCompleted(int)));

	// Start the timer as the instanceview/entryview creation
	// operation is asynchronous
	QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
	}

	Q_ASSERT_X(mAgendaUtil, "TestAgendaUtil::test_addEntry", "No agendautil");

	// Connect to the required signals
	connect(
			mAgendaUtil, SIGNAL(entriesDeleted(int)), this,
			SLOT(handleEntriesDeletion(int)));

	mWait = NULL;
}

void TestAgendaUtil::cleanupTestCase()
{
	QCoreApplication::processEvents();
	if (mAgendaUtil) {
		delete mAgendaUtil;
	}
}
/*
	Tests the AgendaUtil::store.
 */
void TestAgendaUtil::test_storeWithNewEntry()
{
	// Test case setup.
	AgendaEntry entry;
	QDate dateSet(2010,12,1);
	QTime timeSet(10,10,10,10);
	QDateTime dateTimeSet(dateSet,timeSet);
	entry.setType(AgendaEntry::TypeEvent);
	entry.setSummary("Test case");
	entry.setStartAndEndTime(dateTimeSet, dateTimeSet);

	// Run the case.
	ulong id = mAgendaUtil->store(entry);

	// Verify.
	AgendaEntry newEntry = mAgendaUtil->fetchById(id);
	QDateTime dateTimeRetrieved = newEntry.startTime();
	QDate dateRetrieved = dateTimeRetrieved.date();
	QTime timeRetrieved = dateTimeRetrieved.time();
	QVERIFY(entry.summary() == newEntry.summary());
	QVERIFY(dateRetrieved == dateSet);
	QVERIFY(entry.type() == newEntry.type());
	// Cleanup.
	QVERIFY(mAgendaUtil->deleteEntry(id));
}

/*!
	Tests the api AgendaUtil::deleteEntry.
 */
void TestAgendaUtil::test_deleteEntry()
{
	// Test case setup.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeEvent);
	entry.setSummary("Test case");
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),
			QDateTime::currentDateTime());
	ulong id = mAgendaUtil->store(entry);

	// Run the case.
	QVERIFY(mAgendaUtil->deleteEntry(id));

	// Verify.
	QVERIFY(mAgendaUtil->fetchById(id).isNull());
}

/*!
	Test the api AgendaUtil::deleteRepeatedEntry.
 */
void TestAgendaUtil::test_deleteRepeatedEntry()
{
	// Test case setup.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeAppoinment);
	entry.setSummary("Test case");
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),
			QDateTime::currentDateTime());
	AgendaRepeatRule repeatRule;
	repeatRule.setType(AgendaRepeatRule::DailyRule);
	QDateTime ruleStartDateTime(QDate(2019, 11, 3));
	repeatRule.setRepeatRuleStart(ruleStartDateTime);
	QDateTime until(QDate(2019, 12, 3));
	repeatRule.setUntil(until);
	repeatRule.setInterval(1);
	entry.setRepeatRule(repeatRule);

	ulong id = mAgendaUtil->store(entry);
	AgendaEntry savedEntry = mAgendaUtil->fetchById(id);
	QVERIFY(!savedEntry.isNull());
	QVERIFY(savedEntry.isRepeating());

	// Run the case.
	mAgendaUtil->deleteRepeatedEntry(savedEntry, AgendaUtil::ThisAndAll);

	// Verify.
	QVERIFY(mAgendaUtil->fetchById(id).isNull());
}

/*!
    Test the api AgendaUtil::fetchById.
 */
void TestAgendaUtil::test_fetchById()
{
	// Test case setup.
	AgendaEntry entry;
	QDate dateSet(2010, 12, 1);
	QTime timeSet(10, 10, 10, 10);
	QDateTime dateTimeSet(dateSet, timeSet);
	entry.setType(AgendaEntry::TypeEvent);
	entry.setSummary("Test case");
	entry.setStartAndEndTime(dateTimeSet, dateTimeSet);

	// Run the case.
	ulong id = mAgendaUtil->store(entry);

	// Verify.
	AgendaEntry retrievedEntry = mAgendaUtil->fetchById(id);

	// Check if the fetched entry is same st the added entry
	QDateTime dateTimeRetrieved = retrievedEntry.startTime();
	QDate dateRetrieved = dateTimeRetrieved.date();
	QTime timeRetrieved = dateTimeRetrieved.time();
	QVERIFY(dateRetrieved == dateSet);
	QVERIFY(entry.type() == retrievedEntry.type());
	QVERIFY(entry.summary() == retrievedEntry.summary());
	mAgendaUtil->deleteEntry(id);

	// Fetch the same again and check if its null.
	AgendaEntry checkEntryIsNull = mAgendaUtil->fetchById(id);
	QVERIFY(checkEntryIsNull.isNull());
}

/*!
    Test the api AgendaUtil::store.
 */
void TestAgendaUtil::test_storeWithExistingEntry()
{
	// Test case setup.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeAppoinment);
	entry.setSummary("Test case");
	entry.setDescription("Entry to test the store API");
	entry.setLocation("Nokia");
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),
			QDateTime::currentDateTime());

	// Save the entry
	ulong id = mAgendaUtil->store(entry);

	// Fetch the same entry
	AgendaEntry savedEntry = mAgendaUtil->fetchById(id);
	QVERIFY(!savedEntry.isNull());
	savedEntry.setSummary("Fetced Entry");
	savedEntry.setLocation("India");
	savedEntry.setStartAndEndTime(
			QDateTime::currentDateTime().addDays(1),
			QDateTime::currentDateTime().addDays(1));

	// Update the entry
	ulong success = mAgendaUtil->store(savedEntry);
	QVERIFY(success);

	// Fetch the updated entry again
	AgendaEntry updatedEntry = mAgendaUtil->fetchById(id);

	// Verify
	QVERIFY(savedEntry.summary() == updatedEntry.summary());
	QVERIFY(savedEntry.type() == updatedEntry.type());

	// Cleanup.
	QVERIFY(mAgendaUtil->deleteEntry(id));
}

/*!
	Test the api AgendaUtil::createEntryIdListForDay.
 */
void TestAgendaUtil::test_createEntryIdListForDay()
{
	// Clean up all the entries.
	QDateTime startRange(QDate(1900, 01, 1), QTime(0, 0, 0, 0));
	QDateTime endRange(QDate(2100, 01, 01), QTime(23, 59, 59, 0));
	mAgendaUtil->deleteEntries(startRange, endRange);
	// Start the timer as the above operation is asynchronous
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}
	// Test for futur day.
	AgendaEntry futureEntry;
	// Create the day for which entries are needed
	QDateTime futureDay(QDate(2009, 05, 01), QTime(0, 0, 0, 0));
	futureEntry.setStartAndEndTime(futureDay, futureDay);
	futureEntry.setType(AgendaEntry::TypeEvent);
	futureEntry.setSummary("Test case");
	ulong id = mAgendaUtil->store(futureEntry);
	// Run the test case
	QList<AgendaEntry> entriesList;
	entriesList.append(mAgendaUtil->createEntryIdListForDay(futureDay));

	// Verify
	int num = entriesList.count();
	QVERIFY( 1 == entriesList.count());
	QString summary(entriesList.at(0).summary());
	QVERIFY(summary == "Test case");
	QVERIFY(entriesList.at(0).type() == AgendaEntry::TypeEvent);

	// Cleanup
	mAgendaUtil->deleteEntry(id);
}

/*!
	Test the api AgendaUtil::entryIds.
 */
void TestAgendaUtil::test_entryIds()
{
	// Clean up all the entries.
	QDateTime startRange(QDate(1900, 01, 1), QTime(0, 0, 0, 0));
	QDateTime endRange(QDate(2100, 01, 01), QTime(23, 59, 59, 0));
	mAgendaUtil->deleteEntries(startRange, endRange);

	qDebug("Request for deletion made.");

	// Start the timer as the above operation is asynchronous
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}

	qDebug("Waiting for confirmation.");

	QList<AgendaEntry> entriesRetrieved =
	        mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	int numOfEntriesRetrieved = entriesRetrieved.count();
	QList<ulong> checkList = mAgendaUtil->entryIds();
	int checkNum = checkList.count();

	// Fetch the entries and see if none are there
	QList<ulong> testIds;
	testIds.clear();
	int count = testIds.count();
	testIds = mAgendaUtil->entryIds();
	count = testIds.size();
	int numOfEntries = 0;

	// Appointment.
	AgendaEntry firstEntry;
	firstEntry.setType(AgendaEntry::TypeAppoinment);
	firstEntry.setSummary("first entry");
	QDateTime firstEntryStart(QDate(2020, 05, 01), QTime(9, 0, 0, 0));
	QDateTime firstEntryEnd(QDate(2020, 05, 01), QTime(10, 0, 0, 0));
	firstEntry.setStartAndEndTime(firstEntryStart, firstEntryEnd);
	ulong id = mAgendaUtil->store(firstEntry);
	numOfEntries++;

	// ToDo.
	AgendaEntry firstToDo;
	firstToDo.setType(AgendaEntry::TypeTodo);
	firstToDo.setSummary("first ToDo");
	QDateTime firstToDoStart(QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	QDateTime firstToDoEnd(QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	firstToDo.setStartAndEndTime(firstToDoStart, firstToDoEnd);
	id = mAgendaUtil->store(firstToDo);
	numOfEntries++;

	// Run the test case
	QList<ulong> ids;
	int num = ids.count();
	ids = mAgendaUtil->entryIds(AgendaUtil::IncludeAppointments);

	num = ids.count();
	QVERIFY(1 == num);
}
/*!
	Test the api AgendaUtil::fetchAllEntries.
 */
void TestAgendaUtil::test_fetchAllEntries()
{
	// Clean up all the entries.
	QDateTime startRange(QDate(1900, 01, 1), QTime(0, 0, 0, 0));
	QDateTime endRange(QDate(2100, 12, 30), QTime(23, 59, 59, 0));
	mAgendaUtil->deleteEntries(startRange, endRange);

	qDebug("Request for deletion made.");

	// Start the timer as the above operation is asynchronous
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}

	qDebug("Waiting for confirmation.");

	QList<AgendaEntry> entriesRetrieved =
	        mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	int numOfEntriesRetrieved = entriesRetrieved.count();
	QList<ulong> checkList = mAgendaUtil->entryIds();
	int checkNum = checkList.count();

	if (numOfEntriesRetrieved != 0 || checkNum != 0) {
		mAgendaUtil->deleteEntries(startRange, endRange);

		qDebug("Request for deletion made.");

		// Start the timer as the above operation is asynchronous
		if (!mWait) {
			QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
			if (!mWait->IsStarted()) {
				mWait->Start();
			}
		}

		qDebug("Waiting for confirmation.");
	}

	int numOfEntries = 0;

	// Appointment.
	AgendaEntry firstEntry;
	firstEntry.setType(AgendaEntry::TypeAppoinment);
	firstEntry.setSummary("first entry");
	QDateTime firstEntryStart(QDate(2020, 05, 01), QTime(9, 0, 0, 0));
	QDateTime firstEntryEnd(QDate(2020, 05, 01), QTime(10, 0, 0, 0));
	firstEntry.setStartAndEndTime(firstEntryStart, firstEntryEnd);
	ulong id = mAgendaUtil->store(firstEntry);
	numOfEntries++;

	// ToDo.
	AgendaEntry firstToDo;
	firstToDo.setType(AgendaEntry::TypeTodo);
	firstToDo.setSummary("first ToDo");
	QDateTime firstToDoStart(QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	QDateTime firstToDoEnd(QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	firstToDo.setStartAndEndTime(firstToDoStart, firstToDoEnd);
	id = mAgendaUtil->store(firstToDo);
	numOfEntries++;

	// Run the test case
	QList<AgendaEntry> entries;
	entries.clear();
	entries = mAgendaUtil->fetchAllEntries();

	// Verfiy
	numOfEntriesRetrieved = entries.count();
	QCOMPARE(numOfEntries,numOfEntriesRetrieved);
}

/*!
	Test the api AgendaUtil::deleteEntries.
 */
void TestAgendaUtil::test_deleteEntries()
{
	// The range required for the deletion.
	QDateTime startRange(QDate(1900, 01, 01), QTime(0, 0, 0, 0));

	QDateTime endRange(QDate(2100, 01, 01), QTime(23, 59, 59, 0));

	QList<AgendaEntry> entriesRetrieved =
	        mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	int numOfEntriesRetrieved = entriesRetrieved.count();
	QList<ulong> checkList = mAgendaUtil->entryIds();
	int checkNum = checkList.count();

	// Create the required entries.
	int numOfEntries = createMultipleEntries();

	entriesRetrieved = mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	numOfEntriesRetrieved = entriesRetrieved.count();
	checkList = mAgendaUtil->entryIds();
	checkNum = checkList.count();

	Q_ASSERT(numOfEntries);

	qDebug("Entries have been created");

	// Execute the case.
	mAgendaUtil->deleteEntries(startRange, endRange);

	qDebug("Request for deletion made.");

	// Start the timer as the above operation is asynchronous
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}

	qDebug("Waiting for confirmation.");
	entriesRetrieved = mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	numOfEntriesRetrieved = entriesRetrieved.count();
	checkList = mAgendaUtil->entryIds();
	checkNum = checkList.count();
	QVERIFY(0 == numOfEntriesRetrieved);
}

/*!
	Test the api AgendaUtil::fetchEntriesInRange.
 */
void TestAgendaUtil::test_fetchEntriesInRange()
{
	// Clean up all the entries.
	QDateTime startRange(QDate(1900, 01, 1), QTime(0, 0, 0, 0));
	QDateTime endRange(QDate(2100, 12, 30), QTime(23, 59, 59, 0));
	mAgendaUtil->deleteEntries(startRange,endRange);

	//qDebug("Request for deletion made.");

	// Start the timer as the above operation is asynchronous
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}

	//qDebug("Waiting for confirmation.");

	QList<AgendaEntry> entriesRetrieved =
			mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	int numOfEntriesRetrieved = entriesRetrieved.count();
	QList<ulong> checkList = mAgendaUtil->entryIds();
	int checkNum = checkList.count();

	// Appointment.
	AgendaEntry firstEntry;
	firstEntry.setType(AgendaEntry::TypeAppoinment);
	firstEntry.setSummary("first entry");
	QDateTime firstEntryStart(
			QDate(2020, 05, 01), QTime(9, 0, 0, 0));
	QDateTime firstEntryEnd(
			QDate(2020, 05, 01), QTime(10, 0, 0, 0));
	firstEntry.setStartAndEndTime(firstEntryStart, firstEntryEnd);
	ulong id = mAgendaUtil->store(firstEntry);

	// Create the range for which entries are needed
	QDateTime fromRange(
			QDate(2020, 01, 01), QTime(0, 0, 0, 0));
	QDateTime toRange(
			QDate(2023, 12, 31), QTime(23, 59, 0, 0));
	// Run the test case
	QList<AgendaEntry> entries = mAgendaUtil->fetchEntriesInRange(
			fromRange, toRange);

	// Verify
	int num = entries.count();
	QVERIFY( 1 == entries.count());

	//cleanup();

	//User::Exit(0);
}

/*!
	Test the api AgendaUtil::cloneEntry
 */
void TestAgendaUtil::test_cloneEntry()
{
	cloneNoteToTodo();
	cloneNoteToMeeting();
	cloneTodoToNote();
	cloneTodoToMeeting();
	cloneMeetingToNote();
	cloneMeetingToTodo();
}

/*
	Test the api AgendaUtil::setCompleted
 */
void TestAgendaUtil::test_todoCompleted()
{
	completeTodo();
	unCompletedTodo();
}

/*
	Test the api AgendaUtil::store for exceptional entry.
 */
void TestAgendaUtil::test_createExceptionalEntryWithSubjectChange()
{
	// Test case setup.
	
	// Clean up all the entries.
	QDateTime startRange(QDate(1900, 01, 1), QTime(0, 0, 0, 0));
	QDateTime endRange(QDate(2100, 12, 30), QTime(23, 59, 59, 0));
	mAgendaUtil->deleteEntries(startRange,endRange);
	
	// Start the timer as the above operation is asynchronous
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}

	// Create the repeating entry.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeAppoinment);
	entry.setSummary("Test case");
	QDateTime startTime(QDate(2010,1,1),QTime(8,0,0,0));
	QDateTime endTime(QDate(2010,1,1),QTime(9,0,0,0));
	
	entry.setStartAndEndTime(
			startTime,
			endTime);
	
	AgendaRepeatRule repeatRule;
	repeatRule.setType(AgendaRepeatRule::DailyRule);
	repeatRule.setRepeatRuleStart(startTime);
	QDateTime until(QDate(2010, 2, 3));
	repeatRule.setUntil(until);
	repeatRule.setInterval(1);
	entry.setRepeatRule(repeatRule);

	ulong id = mAgendaUtil->store(entry);
	
	// Get the instance and create the exceptional entry by changing the subject
	AgendaEntry savedEntry = mAgendaUtil->fetchById(id);
	QList<AgendaEntry> entriesRetrieved =
			mAgendaUtil->fetchEntriesInRange(
					QDateTime(QDate(2010, 1, 2), QTime(8, 0, 0, 0)),
					QDateTime(QDate(2010, 1, 2), QTime(9, 0, 0, 0)));
	
	int numOfEntriesRetrieved = entriesRetrieved.count();
	AgendaEntry modifiedEntry = entriesRetrieved.first();
	// Verify
	QVERIFY(id == modifiedEntry.id());
	
	modifiedEntry.setSummary("Test case Exceptional");
	
	// Run the test case
	ulong newId = mAgendaUtil->store(
			modifiedEntry, AgendaUtil::ThisOnly, modifiedEntry.startTime());
	
	// Verify.
	QVERIFY(id != newId);
	
	entriesRetrieved.clear();
	
	// Get the exceptional entry.
	AgendaEntry exceptionalEntry = mAgendaUtil->fetchById(newId);
	
	// Verify.
	QCOMPARE(exceptionalEntry.summary(), modifiedEntry.summary());
	QVERIFY(exceptionalEntry.recurrenceId() == modifiedEntry.startTime());
}

/*!
	Test the api AgendaUtil::store for exceptional entry.
 */
void TestAgendaUtil::test_createExceptionalEntryWithStartTimeChange()
{
	// Test case setup.
	
	// Clean up all the entries.
	QDateTime startRange(QDate(1900, 01, 1), QTime(0, 0, 0, 0));
	QDateTime endRange(QDate(2100, 12, 30), QTime(23, 59, 59, 0));
	mAgendaUtil->deleteEntries(startRange,endRange);
	
	// Start the timer as the above operation is asynchronous
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}

	QList<AgendaEntry> allEntries = mAgendaUtil->fetchAllEntries();
	int count = allEntries.count();
	
	// Create the repeating entry.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeAppoinment);
	entry.setSummary("Test case");
	QDateTime startTime(QDate(2010,2,1),QTime(8,0,0,0));
	QDateTime endTime(QDate(2010,2,1),QTime(9,0,0,0));
	
	entry.setStartAndEndTime(startTime, endTime);
	
	AgendaRepeatRule repeatRule;
	repeatRule.setType(AgendaRepeatRule::DailyRule);
	repeatRule.setRepeatRuleStart(startTime);
	QDateTime until(QDate(2010, 3, 3));
	repeatRule.setUntil(until);
	repeatRule.setInterval(1);
	entry.setRepeatRule(repeatRule);

	ulong id = mAgendaUtil->store(entry);
	
	// Get the instance and create the exceptional entry by changing the subject
	AgendaEntry savedEntry = mAgendaUtil->fetchById(id);
	QList<AgendaEntry> entriesRetrieved =
			mAgendaUtil->fetchEntriesInRange(
					QDateTime(QDate(2010, 2, 2), QTime(8, 0, 0, 0)),
					QDateTime(QDate(2010, 2, 2), QTime(9, 0, 0, 0)));
	
	int numOfEntriesRetrieved = entriesRetrieved.count();
	AgendaEntry modifiedEntry = entriesRetrieved.first();
	AgendaEntry originalEntry = modifiedEntry;
	// Verify
	QVERIFY(id == modifiedEntry.id());
	
	QDateTime modifiedStartTime(QDate(2010,2,1),QTime(8,0,0,0));
	QDateTime modifiedEndTime(QDate(2010,2,1),QTime(9,0,0,0));
	modifiedEntry.setStartAndEndTime(modifiedStartTime, modifiedStartTime);

	// Run the test case
	ulong newId = mAgendaUtil->store(
			modifiedEntry, AgendaUtil::ThisOnly, originalEntry.startTime());
	
	// Verify.
	QVERIFY(id != newId);
	
	entriesRetrieved.clear();
	
	// Get the exceptional entry.
	AgendaEntry exceptionalEntry = mAgendaUtil->fetchById(newId);
	
	// Verify.
	QVERIFY(exceptionalEntry.recurrenceId() == originalEntry.startTime());
}

/*!
	Slot to listen for the completion of deleting the entries asynchronously.
 */
void TestAgendaUtil::handleEntriesDeletion(int error)
{
	// Stop the wait timer.
	if (mWait && mWait->IsStarted()) {
		mWait->AsyncStop();
	}
	QVERIFY(!error);
}

/*!
	Slot to handle the entry view creation.
 */
void TestAgendaUtil::handleEntryViewCreationCompleted(int status)
{
	// Stop the wait timer.
	if (mWait && mWait->IsStarted()) {
		mWait->AsyncStop();
	}
	QVERIFY(!status);
}

/*!
	Private function to create entries needed for some of the test cases.
 */
int TestAgendaUtil::createMultipleEntries()
{
	// Start creating the entries.
	int numOfEntries = 0;

	// Appointments.
	AgendaEntry firstEntry;
	firstEntry.setType(AgendaEntry::TypeAppoinment);
	firstEntry.setSummary("first entry");
	QDateTime firstEntryStart(
			QDate(2020, 05, 01), QTime(9, 0, 0, 0));
	QDateTime firstEntryEnd(
			QDate(2020, 05, 01), QTime(10, 0, 0, 0));
	firstEntry.setStartAndEndTime(firstEntryStart, firstEntryEnd);
	ulong id = mAgendaUtil->store(firstEntry);
	numOfEntries++;

	// Events.
	AgendaEntry firstEvent;
	firstEvent.setType(AgendaEntry::TypeEvent);
	firstEvent.setSummary("first event");
	QDateTime firstEventStart(
			QDate(2023, 05, 01), QTime(10, 0, 0, 0));
	QDateTime firstEventEnd(
			QDate(2023, 05, 01), QTime(12, 0, 0, 0));
	firstEvent.setStartAndEndTime(firstEventStart, firstEventEnd);
	id = mAgendaUtil->store(firstEvent);
	numOfEntries++;

	// ToDos.
	AgendaEntry firstToDo;
	firstToDo.setType(AgendaEntry::TypeTodo);
	firstToDo.setSummary("first ToDo");
	QDateTime firstToDoStart(
			QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	QDateTime firstToDoEnd(
				QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	firstToDo.setStartAndEndTime(firstToDoStart, firstToDoEnd);
	id = mAgendaUtil->store(firstToDo);
	numOfEntries++;

	return numOfEntries;
}

/*!
	Test Cloning of Note to To-do
 */
void TestAgendaUtil::cloneNoteToTodo()
{
	// Create an agenda entry(TypeNote) and save it.
	AgendaEntry noteEntry(AgendaEntry::TypeNote);
	noteEntry.setDescription(QString("test cloning of note to todo"));

	// Clone the entry type TypeNote to TypeTodo
	long notSavedEntryId = mAgendaUtil->cloneEntry(
			noteEntry, AgendaEntry::TypeTodo);
	// Shouldn't clone the entry as AgendaEntry is not yet saved.
	QVERIFY(!notSavedEntryId);

	long noteId = mAgendaUtil->store(noteEntry);
	QVERIFY(noteId);

	// Fetch the saved note entry for cloning
	AgendaEntry fetchedNoteEntry = mAgendaUtil->fetchById(noteId);

	// Clone the entry type ENote to ETodo
	long clonedTodoEntryId = mAgendaUtil->cloneEntry(
			fetchedNoteEntry, AgendaEntry::TypeTodo);
	QVERIFY(clonedTodoEntryId);

	AgendaEntry clonedTodoEntry = mAgendaUtil->fetchById(clonedTodoEntryId);
	// Compare the attributes.
	QCOMPARE(AgendaEntry::TypeTodo,clonedTodoEntry.type());
	QCOMPARE(fetchedNoteEntry.description(),clonedTodoEntry.description());
}

/*!
	Test Cloning of Note to Meeting
 */

void TestAgendaUtil::cloneNoteToMeeting()
{
	// Create an agenda entry(TypeNote) and save it.
	AgendaEntry noteEntry(AgendaEntry::TypeNote);
	noteEntry.setDescription(QString("test cloning of note to meeting"));

	long noteId = mAgendaUtil->store(noteEntry);
	QVERIFY(noteId);

	// Fetch the saved note entry for cloning
	AgendaEntry fetchedNoteEntry = mAgendaUtil->fetchById(noteId);

	// Clone the entry type TypeNote to TypeTodo
	long clonedMeetingEntryId = mAgendaUtil->cloneEntry(
			fetchedNoteEntry, AgendaEntry::TypeAppoinment);
	QVERIFY(clonedMeetingEntryId);

	AgendaEntry clonedMeetingEntry = mAgendaUtil->fetchById(clonedMeetingEntryId);
	// Compare the attributes.
	QCOMPARE(AgendaEntry::TypeAppoinment,clonedMeetingEntry.type());
	QCOMPARE(fetchedNoteEntry.description(),clonedMeetingEntry.description());
}

/*!
	Test Cloning of To-do to Note
 */
void TestAgendaUtil::cloneTodoToNote()
{
	// Create an agenda entry(TypeTodo) and save it.
	AgendaEntry todoEntry(AgendaEntry::TypeTodo);
	todoEntry.setDescription(QString("test cloning of todo to note"));
	QDateTime dueDate(QDate(2011, 06, 01), QTime(10, 0, 0, 0));
	todoEntry.setStartAndEndTime(dueDate, dueDate);
	long todoId = mAgendaUtil->store(todoEntry);
	QVERIFY(todoId);

	// Fetch the saved note entry for cloning
	AgendaEntry fetchedTodoEntry = mAgendaUtil->fetchById(todoId);

	// Clone the entry type ETodo to EANote
	ulong clonedNoteId = mAgendaUtil->cloneEntry(
			fetchedTodoEntry, AgendaEntry::TypeNote);
	QVERIFY(clonedNoteId);
	AgendaEntry clonedNoteEntry =
			mAgendaUtil->fetchById(clonedNoteId);

	// Compare the attributes.
	QCOMPARE(AgendaEntry::TypeNote,clonedNoteEntry.type());
	QCOMPARE(fetchedTodoEntry.description(),clonedNoteEntry.description());
	QCOMPARE(fetchedTodoEntry.favourite(),clonedNoteEntry.favourite());
}

/*!
	Test Cloning of To-do to Meeting
 */
void TestAgendaUtil::cloneTodoToMeeting()
{
	// Create an agenda entry(TypeTodo) and save it.
	AgendaEntry todoEntry(AgendaEntry::TypeTodo);
	todoEntry.setSummary(QString("test cloning of todo to meeting"));
	QDateTime dueDate(QDate(2011, 06, 01), QTime(10, 0, 0, 0));
	todoEntry.setStartAndEndTime(dueDate, dueDate);
	long todoId = mAgendaUtil->store(todoEntry);
	QVERIFY(todoId);

	// Fetch the saved note entry for cloning
	AgendaEntry fetchedTodoEntry = mAgendaUtil->fetchById(todoId);

	// Clone the entry type TypeTodo to TypeAppointment
	ulong clonedAppointmentId = mAgendaUtil->cloneEntry(
			fetchedTodoEntry, AgendaEntry::TypeAppoinment);
	QVERIFY(clonedAppointmentId);
	AgendaEntry clonedAppointmentEntry =
			mAgendaUtil->fetchById(clonedAppointmentId);

	// Compare the attributes.
	QCOMPARE(AgendaEntry::TypeAppoinment,clonedAppointmentEntry.type());
	QCOMPARE(fetchedTodoEntry.summary(),clonedAppointmentEntry.summary());
	QCOMPARE(fetchedTodoEntry.startTime(),clonedAppointmentEntry.startTime());
	QCOMPARE(fetchedTodoEntry.endTime(),clonedAppointmentEntry.endTime());
}

/*!
	Test Cloning of Meeting to Note
 */
void TestAgendaUtil::cloneMeetingToNote()
{
	// Create an agenda entry(TypeAppointment) and save it.
	AgendaEntry appointment(AgendaEntry::TypeAppoinment);
	appointment.setSummary(QString("test cloning of meeting to note"));
	QDateTime startDate(QDate(2011, 06, 01), QTime(10, 0, 0, 0));
	QDateTime endDate(QDate(2011, 06, 01), QTime(12, 0, 0, 0));
	appointment.setStartAndEndTime(startDate, endDate);
	long appointmentId = mAgendaUtil->store(appointment);
	QVERIFY(appointmentId);

	// Fetch the saved note entry for cloning
	AgendaEntry fetchedAppointment = mAgendaUtil->fetchById(appointmentId);

	// Clone the entry type TypeAppointment to TypeNote
	ulong clonedNoteId = mAgendaUtil->cloneEntry(
			fetchedAppointment, AgendaEntry::TypeNote);
	QVERIFY(clonedNoteId);

	AgendaEntry clonedNote = mAgendaUtil->fetchById(clonedNoteId);
	// Compare the attributes.
	QCOMPARE(AgendaEntry::TypeNote,clonedNote.type());
	QCOMPARE(fetchedAppointment.description(),clonedNote.description());
	QCOMPARE(fetchedAppointment.favourite(),clonedNote.favourite());
}

/*!
	Test Cloning of Meeting to To-do
 */
void TestAgendaUtil::cloneMeetingToTodo()
{
	// Create an agenda entry(TypeAppointment) and save it.
	AgendaEntry appointment(AgendaEntry::TypeAppoinment);
	appointment.setSummary(QString("test cloning of meeting to todo"));
	QDateTime startDate(QDate(2011, 06, 01), QTime(10, 0, 0, 0));
	QDateTime endDate(QDate(2011, 06, 01), QTime(12, 0, 0, 0));
	appointment.setStartAndEndTime(startDate, endDate);
	long appointmentId = mAgendaUtil->store(appointment);
	QVERIFY(appointmentId);

	// Fetch the saved note entry for cloning
	AgendaEntry fetchedAppointment = mAgendaUtil->fetchById(appointmentId);

	// Clone the entry type EAppointment to TypeNote
	ulong clonedTodoId = mAgendaUtil->cloneEntry(
			fetchedAppointment, AgendaEntry::TypeTodo);
	QVERIFY(clonedTodoId);

	AgendaEntry clonedTodo = mAgendaUtil->fetchById(clonedTodoId);
	// Compare the attributes.
	QCOMPARE(AgendaEntry::TypeTodo,clonedTodo.type());
	QCOMPARE(fetchedAppointment.summary(),clonedTodo.summary());
	QCOMPARE(fetchedAppointment.startTime(),clonedTodo.startTime());
	QCOMPARE(fetchedAppointment.endTime(),clonedTodo.endTime());
}

/*!
	test making To-do as completed
 */
void TestAgendaUtil::completeTodo()
{
	// Create and Save a To-do event.
	AgendaEntry todoEntry(AgendaEntry::TypeTodo);
	todoEntry.setSummary(QString("Test making To-do event as Completed"));
	QDateTime dueDate(QDate(2011, 06, 01), QTime(0, 0, 0, 0));
	todoEntry.setStartAndEndTime(dueDate, dueDate);

	ulong todoId = mAgendaUtil->store(todoEntry);
	QVERIFY(todoId);

	// Complete the to-do.
	AgendaEntry fetchedTodoEntry = mAgendaUtil->fetchById(todoId);
	QDateTime completedTime(QDate(2011, 07, 01),QTime(0, 0, 0, 0));
	mAgendaUtil->setCompleted(fetchedTodoEntry, true, completedTime);

	// Compare the completed time.
	AgendaEntry completedTodo = mAgendaUtil->fetchById(todoId);
	QCOMPARE(completedTime,completedTodo.completedDateTime());
}

/*!
	test making To-do as uncompleted
 */
void TestAgendaUtil::unCompletedTodo()
{
	// Create and Save a To-do event.
	AgendaEntry todoEntry(AgendaEntry::TypeTodo);
	todoEntry.setSummary(QString("Test making To-do event as Completed"));
	QDateTime dueDate(QDate(2011, 06, 01), QTime(0, 0, 0, 0));
	todoEntry.setStartAndEndTime(dueDate, dueDate);

	ulong todoId = mAgendaUtil->store(todoEntry);
	QVERIFY(todoId);

	// Complete the to-do.
	AgendaEntry fetchedTodoEntry = mAgendaUtil->fetchById(todoId);
	QDateTime completedTime(QDate(2011, 07, 01),QTime(0, 0, 0, 0));
	mAgendaUtil->setCompleted(fetchedTodoEntry, true, completedTime);

	// Compare the completed time.
	AgendaEntry unCompletedTodo = mAgendaUtil->fetchById(todoId);
	// make to-do as uncompleted.
	mAgendaUtil->setCompleted(fetchedTodoEntry, false, completedTime);

	QCOMPARE(completedTime, unCompletedTodo.completedDateTime());
}

/*
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	TestAgendaUtil tc;

	char *pass[3];
	pass[0] = argv[0];
	pass[1] = "-o";
	pass[2] = "c:\\data\\TestAgendaUtil.txt";

	int res = QTest::qExec(&tc, 3, pass);
	return res;
}
*/

QTEST_MAIN ( TestAgendaUtil )

#include "unittest_agendautil.moc"

// End of file	--Don't remove this.
