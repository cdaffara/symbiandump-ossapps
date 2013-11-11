/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the License "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors: 
 *
 * Description: CalenEditor unit test class.
 *
 */

// System includes.
#include <QtGui>
#include <QtTest/QtTest>
#include <QDateTime.h>
#include <QObject>
#include <QPoint>
#include <QString>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hblineedit.h>
#include <hbcombobox.h>

// User includes.
#include "caleneditor.h"
#include "caleneditor_p.h"
#include "agendaentry.h"
#include "agendautil.h"

/*!
    \class TestCalenEditor
    
    This object has functions to test the public apis in the class
    CalenEditor.
 */
class TestCalenEditor : public QObject
{
	Q_OBJECT

public slots:
	void handleEntrySaved(ulong id);
	
private slots:
	// Test case setup.
	void initTestCase();
	void cleanupTestCase();
	
	// Test cases.
	void testNonRepeatingEntry();
	void testRepeatingEntry();
	void testDiscardChanges();
	void testExistingEntry();
	
private:
	AgendaEntry mEntry;
	AgendaUtil *mAgendaUtil;
	CalenEditor *mCalenEditor;
	CalenEditorPrivate *mCalenEditorPrivate_ptr;
	QDateTime mNewEntryStartDateTime;
	bool mRepeatingEntry;
	bool mExistingEntry;
	bool mDoneEdited;
	ulong mId;
};

/*!
	This function will be called before first testfunction.
 */
void TestCalenEditor::initTestCase()
{
	mAgendaUtil= new AgendaUtil(this);
	TRAPD(mError, mCalenEditor = new CalenEditor(mAgendaUtil, this));
	mRepeatingEntry = false;
	mExistingEntry = false;
	mDoneEdited = false;
	mId = 0;
	connect(mAgendaUtil, SIGNAL(entryAdded(ulong)), this, SLOT(handleEntrySaved(ulong)));
	connect(mAgendaUtil, SIGNAL(entryUpdated(ulong)), this, SLOT(handleEntrySaved(ulong)));
	Q_UNUSED(mError);
	Q_ASSERT(!mError);
	Q_ASSERT(mCalenEditor);
}

/*!
	This function will be called after last testfunction.
 */
void TestCalenEditor::cleanupTestCase()
{
	delete mCalenEditor;
	delete mAgendaUtil;
}

/*!
	This function will create a new non-repeating entry.
 */
void TestCalenEditor::testNonRepeatingEntry()
{
	HbMainWindow window;
	window.show();

	QDate newEntryStartDate(2010,05, 05 );
	QTime newEntryStartTime(10 , 10, 10);
	mNewEntryStartDateTime.setDate(newEntryStartDate);
	mNewEntryStartDateTime.setTime(newEntryStartTime);
	
	// Create a new entry.
	mCalenEditor->create(mNewEntryStartDateTime,0,CalenEditor::TypeAppointment);
	
	// Add subject.
	QString subject = "checksubject";
	mCalenEditor->d_ptr->mSubjectLineEdit->setText(subject);
	
	// Add location.
	QString location = "checklocation";
	mCalenEditor->d_ptr->mLocationLineEdit->setText(location);
	
	// Description item.
	mCalenEditor->d_ptr->mDescriptionItemAdded = false;
	mCalenEditor->d_ptr->populateDescriptionItem();
	QString description = "checkdescription";
	mCalenEditor->d_ptr->mDescriptionLineEdit->setText(description);
	
	// Save the entry.
	mCalenEditor->d_ptr->saveAndCloseEditor();
	
	// Fetch the saved entry.
	mEntry = mAgendaUtil->fetchById(mId);
	QDateTime newEntryEndDateTime;
	
	// Get the expected end time.
	newEntryEndDateTime = mNewEntryStartDateTime.addSecs(60 * 60);
	QVERIFY(mEntry.summary() == "checksubject");
	QVERIFY(mEntry.startTime().date() == mNewEntryStartDateTime.date());
	QVERIFY(mEntry.startTime().time().hour() == mNewEntryStartDateTime.time().hour());
	QVERIFY(mEntry.startTime().time().minute() == mNewEntryStartDateTime.time().minute());
	QVERIFY(mEntry.endTime().date() == newEntryEndDateTime.date());
	QVERIFY(mEntry.endTime().time().hour() == newEntryEndDateTime.time().hour());
	QVERIFY(mEntry.endTime().time().minute() == newEntryEndDateTime.time().minute());
	QVERIFY(mEntry.location() == "checklocation");
	QVERIFY(mEntry.description() == "checkdescription");
	QVERIFY(mEntry.repeatRule().type() == AgendaRepeatRule::InvalidRule);
}

/*!
	This function will create a new repeating entry.
 */
void TestCalenEditor::testRepeatingEntry()
{
	mRepeatingEntry = true;
	HbMainWindow window;
	window.show();

	QDate newEntryStartDate(2010,05, 05 );
	QTime newEntryStartTime(10 , 10, 10);
	mNewEntryStartDateTime.setDate(newEntryStartDate);
	mNewEntryStartDateTime.setTime(newEntryStartTime);
	
	// Create a new entry.
	mCalenEditor->create(mNewEntryStartDateTime,0,CalenEditor::TypeAppointment);
	
	// Add subject.
	QString subject = "checksubject";
	mCalenEditor->d_ptr->mSubjectLineEdit->setText(subject);
	
	// Change repeat type to monthly rule.
	mCalenEditor->d_ptr->mRepeatComboBox->setCurrentIndex(4);
	
	// Change reminder item.
	mCalenEditor->d_ptr->mReminderComboBox->setCurrentIndex(3);
	
	// Save the entry.
	mCalenEditor->d_ptr->saveAndCloseEditor();
	
	// Fetch the saved entry.
	mEntry = mAgendaUtil->fetchById(mId);                                 
	QVERIFY(mEntry.summary() == "checksubject");
	QVERIFY(mEntry.repeatRule().type() == AgendaRepeatRule::MonthlyRule);						
	QVERIFY(mEntry.alarm().timeOffset() == 15);	
}

/*!
	This function will test discard changes.
 */
void TestCalenEditor::testDiscardChanges()
{
 	HbMainWindow window;
	window.show();
	QDate newEntryStartDate(2010,05, 05 );
	QTime newEntryStartTime(10 , 10, 10);
	mNewEntryStartDateTime.setDate(newEntryStartDate);
	mNewEntryStartDateTime.setTime(newEntryStartTime);
	// Create a new entry.
	mCalenEditor->create(mNewEntryStartDateTime,0,CalenEditor::TypeAppointment);

	// Add subject.
	QString subject = "checksubject";
	mCalenEditor->d_ptr->mSubjectLineEdit->setText(subject);

	// Add location.
	QString location = "checklocation";
	mCalenEditor->d_ptr->mLocationLineEdit->setText(location);

	// Make it non-repeating.
	mCalenEditor->d_ptr->mRepeatComboBox->setCurrentIndex(1);

	// Description item.
	mCalenEditor->d_ptr->mDescriptionItemAdded = false;
	mCalenEditor->d_ptr->populateDescriptionItem();
	QString description = "checkdescription";
	mCalenEditor->d_ptr->mDescriptionLineEdit->setText(description);
	
	// Reminder item.
	mCalenEditor->d_ptr->mReminderComboBox->setCurrentIndex(2);
	
	// Discard all the changes.
	mCalenEditor->d_ptr->discardChanges();
	
	// Verify if all the changes have been discarded.
	QVERIFY(mCalenEditor->d_ptr->mSubjectLineEdit->text() == NULL);
	QVERIFY(mCalenEditor->d_ptr->mLocationLineEdit->text() == NULL);
	QVERIFY(mCalenEditor->d_ptr->mDescriptionLineEdit->text() == NULL);
	QVERIFY(mCalenEditor->d_ptr->mRepeatComboBox->currentIndex() == 0);
	QVERIFY(mCalenEditor->d_ptr->mReminderComboBox->currentIndex() == 3);
}

/*!
	This function will edit and existing entry.
 */
void TestCalenEditor::testExistingEntry()
{
	mExistingEntry = true;
	
	HbMainWindow window;
	window.show();

	QDate newEntryStartDate(2010,05, 05 );
	QTime newEntryStartTime(10 , 10, 10);
	mNewEntryStartDateTime.setDate(newEntryStartDate);
	mNewEntryStartDateTime.setTime(newEntryStartTime);
	// Create a new entry.
	mCalenEditor->create(mNewEntryStartDateTime,0,CalenEditor::TypeAppointment);

	// Add subject.
	QString subject = "checksubject";
	mCalenEditor->d_ptr->mSubjectLineEdit->setText(subject);

	// Add location.
	QString location = "checklocation";
	mCalenEditor->d_ptr->mLocationLineEdit->setText(location);

	// Make it non-repeating.
	mCalenEditor->d_ptr->mRepeatComboBox->setCurrentIndex(0);

	// Description item.
	mCalenEditor->d_ptr->mDescriptionItemAdded = false;
	mCalenEditor->d_ptr->populateDescriptionItem();
	QString description = "checkdescription";
	mCalenEditor->d_ptr->mDescriptionLineEdit->setText(description);

	// Save the entry.
	mCalenEditor->d_ptr->saveAndCloseEditor();
	
	// Get the saved entry.
	mEntry = mAgendaUtil->fetchById(mId);
	mCalenEditor->edit(mEntry, 0);
	
	// Change subject.
	mCalenEditor->d_ptr->mSubjectLineEdit->setText("");
	mCalenEditor->d_ptr->mSubjectLineEdit->setText("changedsubject");

	// Change repeat type.
	mCalenEditor->d_ptr->mRepeatComboBox->setCurrentIndex(1);
	mCalenEditor->d_ptr->handleRepeatIndexChanged(1);

	// Chenage location.
	mCalenEditor->d_ptr->mLocationLineEdit->setText("changedlocation");

	// Save the changes.
	mDoneEdited = true;
	mCalenEditor->d_ptr->saveAndCloseEditor();

	// Get the entry edited and verify the changes.
	mEntry = mAgendaUtil->fetchById(mId);
	QVERIFY(mEntry.summary() == "changedsubject");
	QVERIFY(mEntry.location() == "changedlocation");
	QVERIFY(mEntry.repeatRule().type() == AgendaRepeatRule::DailyRule);
}

/*!
	This is a slot called after saving an entry which will store the id returned.
 */
void TestCalenEditor::handleEntrySaved(ulong id)
{
	mId = id;
}

QTEST_MAIN(TestCalenEditor)
#include "unittest_caleneditor.moc"

// End of file.
    

	
