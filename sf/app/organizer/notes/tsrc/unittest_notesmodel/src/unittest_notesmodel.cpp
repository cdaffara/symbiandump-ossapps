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
* NotesModel unit-test class.
*
*/

// System includes
#include <QtTest/QtTest>
#include <QtGui>
#include <QPointer>
#include <e32base.h>

// User includes
#include <agendautil.h>
#include "notesmodel.h"

class TestNotesModel : public QObject
{
	Q_OBJECT

public slots:
	void handleEntriesDeletion(int var);

private slots:
	// Test case setup.
	void initTestCase();
	void cleanupTestCase();

	// Test cases.
	void test_construction();
	void test_sourceModelAddNote();
	void test_sourceModelDeleteNote();
	void test_sourceModelUpdateNote();

private:
	QPointer<NotesModel> mNotesModel;
	QPointer<AgendaUtil> mAgendaUtil;

	QDateTime mStart;
	QDateTime mEnd;

	CActiveSchedulerWait *mWait;
};

/*!
	\class TestNotesModel

	This is unittest class for NotesModel apis.
 */

/*!
	Slot called when all the entries have been deleted.
 */
void TestNotesModel::handleEntriesDeletion(int var)
{
	Q_UNUSED(var)

	if (mWait && mWait->IsStarted()) {
		mWait->AsyncStop();
	}
}

/*!
	This function is called to init the testcase.
	Called for every test case execution.
 */
void TestNotesModel::initTestCase()
{
	QT_TRAP_THROWING(mAgendaUtil = new AgendaUtil(this););
	QVERIFY(mAgendaUtil != 0);

	// Connect to the required signals
	connect(
			mAgendaUtil, SIGNAL(entriesDeleted(int)),
			this, SLOT(handleEntriesDeletion(int)));

	mWait = 0;

	mStart.setDate(QDate(1900, 1, 1));
	mStart.setTime(QTime(0, 0, 0));
	mEnd.setDate(QDate(2100, 30, 12));
	mEnd.setTime(QTime(23, 59, 0));
}

/*!
	This function is called when the testcase execution is
	completed, for cleaning up.
 */
void TestNotesModel::cleanupTestCase()
{
	if (mAgendaUtil) {
		delete mAgendaUtil;
	}
	if (mNotesModel) {
		delete mNotesModel;
	}
	if (mWait && mWait->IsStarted()) {
		mWait->AsyncStop();
	}
	if (mWait) {
		delete mWait;
	}
}

/*!
	This test functions tests the construction of NotesModel object.
 */
void TestNotesModel::test_construction()
{
	mNotesModel = new NotesModel(mAgendaUtil, this);
	QVERIFY(mNotesModel);

	if (mNotesModel) {
		delete mNotesModel;
		QVERIFY(!mNotesModel);
	}
}

/*!
	This test function tests if the source model is updated when a note is
	added.
 */
void TestNotesModel::test_sourceModelAddNote()
{
	mAgendaUtil->deleteEntries(mStart, mEnd, AgendaUtil::IncludeAll);
	if (!mWait) {
		mWait =  new CActiveSchedulerWait;
	}
	if (!mWait->IsStarted()) {
		mWait->Start();
	}
	mNotesModel = new NotesModel(mAgendaUtil, this);
	QVERIFY(mNotesModel);

	QAbstractItemModel *model = 0;
	model = mNotesModel->sourceModel();
	QVERIFY(model);
	QVERIFY(!model->rowCount());

	// Now save a note.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeNote);
	entry.setDescription("Hello");
	entry.setLastModifiedDateTime(
			QDateTime(QDate::currentDate(), QTime::currentTime()));
	QVERIFY(mAgendaUtil->store(entry));
	
	// Now check the count in the model.
	startTimer(1000);
	QVERIFY(model->rowCount());
	QVERIFY(model->rowCount() == 1);
}

/*!
	This test function tests if the source model is updated when a note is
	deleted.
 */
void TestNotesModel::test_sourceModelDeleteNote()
{
	mAgendaUtil->deleteEntries(mStart, mEnd, AgendaUtil::IncludeAll);
	if (!mWait) {
		mWait =  new CActiveSchedulerWait;
	}
	if (!mWait->IsStarted()) {
		mWait->Start();
	}
	mNotesModel = new NotesModel(mAgendaUtil, this);
	QVERIFY(mNotesModel);

	QAbstractItemModel *model = 0;
	model = mNotesModel->sourceModel();
	QVERIFY(model);
	QVERIFY(!model->rowCount());

	// Now save a note.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeNote);
	entry.setDescription("Hello");
	entry.setLastModifiedDateTime(
			QDateTime(QDate::currentDate(), QTime::currentTime()));
	ulong id;
	QVERIFY(id = mAgendaUtil->store(entry));

	// Now check the count in the model.
	startTimer(1000);
	QVERIFY(model->rowCount());
	QVERIFY(model->rowCount() == 1);

	// Verify if the model has the updated content.
	startTimer(1000);
	QModelIndex index = model->index(0, 0);
	QVERIFY(index.isValid());
	QStringList list = index.data(Qt::DisplayRole).value<QStringList>();
	QCOMPARE(entry.description(), list.at(0));

	// Delete the note saved above.
	QVERIFY(mAgendaUtil->deleteEntry(id));
	// Now check the count in the model.
	startTimer(1000);
	QVERIFY(!model->rowCount());
}

/*!
	This test function tests if the source model is updated when a note is
	modified.
 */
void TestNotesModel::test_sourceModelUpdateNote()
{
	mAgendaUtil->deleteEntries(mStart, mEnd, AgendaUtil::IncludeAll);
	if (!mWait) {
		mWait =  new CActiveSchedulerWait;
	}
	if (!mWait->IsStarted()) {
		mWait->Start();
	}
	mNotesModel = new NotesModel(mAgendaUtil, this);
	QVERIFY(mNotesModel);

	QAbstractItemModel *model = 0;
	model = mNotesModel->sourceModel();
	QVERIFY(model);
	QVERIFY(!model->rowCount());

	// Now save a note.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeNote);
	entry.setDescription("Hello");
	entry.setLastModifiedDateTime(
			QDateTime(QDate::currentDate(), QTime::currentTime()));
	ulong id;
	QVERIFY(id = mAgendaUtil->store(entry));
	// Now check the count in the model.
	startTimer(1000);
	QVERIFY(model->rowCount());
	QVERIFY(model->rowCount() == 1);

	// Now update the content on of the note created above.
	entry = mAgendaUtil->fetchById(id);
	QVERIFY(!entry.isNull());
	entry.setDescription("Hello world");
	QVERIFY(mAgendaUtil->store(entry));

	// Verify if the model has the updated content.
	startTimer(1000);
	QModelIndex index = model->index(0, 0);
	QVERIFY(index.isValid());
	QStringList list = index.data(Qt::DisplayRole).value<QStringList>();
	QCOMPARE(entry.description(), list.at(0));

	// Delete the note saved above.
	QVERIFY(mAgendaUtil->deleteEntry(id));
	// Now check the count in the model.
	startTimer(1000);
	QVERIFY(!model->rowCount());
}

QTEST_MAIN(TestNotesModel)
#include "unittest_notesmodel.moc"

// End of file	--Don't remove this.
