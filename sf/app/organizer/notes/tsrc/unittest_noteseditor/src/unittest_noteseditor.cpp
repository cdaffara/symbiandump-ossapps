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

// System include
#include <QtTest/QtTest>
#include <HbMainWindow>
#include <AgendaUtil>
#include <AgendaEntry>

// User includes
#include "unittest_noteseditor.h"
#include "noteseditor.h"


/*!
	\class TestNotesEditor

	Unit test clas for Notes Editor apis
 */


/*!
	Constructor
 */

TestNotesEditor::TestNotesEditor()
{

}

/*!
	Destructor
 */
TestNotesEditor::~TestNotesEditor()
{
}

/*!
	Initialise before calling the test case
 */
void TestNotesEditor::init()
{
}

/*!
	Do cleanup after each test case
 */
void TestNotesEditor::cleanup()
{
}

/*!
	Test the api NotesEditor::NotesEditor()
 */

void TestNotesEditor::testConstructionWithoutAgendaUtil()
{
	// Create NotesEditor object
	NotesEditor *notesEditor = new NotesEditor(this);
	QVERIFY(notesEditor!=0);

	// Cleanup
	delete notesEditor;
}

/*!
	Test the api NotesEditor::NotesEditor()
 */
void TestNotesEditor::testConstructionWithAgendaUtil()
{
	// Create AgendaUtil object.
	AgendaUtil *agendaUtil = new AgendaUtil();
	QVERIFY(agendaUtil);

	// Create NotesEditor object.
	NotesEditor *notesEditor = new NotesEditor(agendaUtil);

	QVERIFY(notesEditor!=0);

	// Cleanup.
	delete notesEditor;
	delete agendaUtil;
}

/*!
	Test the api NotesEditor::~NotesEditor
 */
void TestNotesEditor::testDestruction()
{
	// Create NotesEditor object.
	QPointer<NotesEditor> notesEditor = new NotesEditor();
	delete notesEditor;

	QVERIFY(!notesEditor);

	// Create AgendaUtil object.
	QPointer<AgendaUtil> agendaUtil = new AgendaUtil();
	QPointer<NotesEditor> notesEditorWithAgendaUtil =
			new NotesEditor(agendaUtil);

	delete notesEditorWithAgendaUtil;
	delete agendaUtil;

	QVERIFY(!notesEditorWithAgendaUtil);
}

/*!
	Test the api NotesEditor::edit(const QString &string)
 */
void TestNotesEditor::testEditingNoteWithText()
{
	HbMainWindow window;
	window.show();
	// Create NotesEditor object.
	NotesEditor *notesEditor = new NotesEditor(this);
	QVERIFY(notesEditor);

	QString noteText("This is test note");
	// Call edit using the text.
	notesEditor->edit(QString("This is test note"));

	QTest::qWait(1000);

	// Call close on editor by saving the note
	ulong id = notesEditor->close(NotesEditor::CloseWithSave);
	QVERIFY(id);

	// Create agenda Utility.
	AgendaUtil *agendaUtil = new AgendaUtil();
	QVERIFY(agendaUtil);

	// Fetch the entry using the id
	AgendaEntry entry = agendaUtil->fetchById(id);

	int compareResult = noteText.compare(entry.description());
	QVERIFY(!compareResult);

	delete agendaUtil;
	delete notesEditor;
}

/*!
	Test the api NotesEditor::edit(const QFile &handle)
 */
void TestNotesEditor::testEditingNoteWithFileHandle()
{
	// Nothing yet
}

/*!
	Test the api NotesEditor::edit(AgendaEntry entry)
 */
void TestNotesEditor::testEditingNoteWithAgendaEntry()
{
	HbMainWindow window;
	window.show();

	NotesEditor *notesEditor = new NotesEditor();
	QVERIFY(notesEditor);

	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeNote);
	entry.setDescription(QString("A simple note"));

	// Call edit on agenda entry
	notesEditor->edit(entry);

	QTest::qWait(1000);

	ulong id = notesEditor->close(NotesEditor::CloseWithSave);

	AgendaUtil *agendaUtil = new AgendaUtil();
	QVERIFY(agendaUtil);

	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.description(), entry.description());

	delete agendaUtil;
	delete notesEditor;
}

/*!
	Test the api NotesEditor::edit(ulong id)
 */
void TestNotesEditor::testEditingNoteWithId()
{
	HbMainWindow window;
	window.show();

	NotesEditor *notesEditor = new NotesEditor();
	QVERIFY(notesEditor);

	AgendaUtil *agendaUtil = new AgendaUtil();
	QVERIFY(agendaUtil);

	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeNote);
	entry.setDescription("A simple note");

	ulong id = agendaUtil->store(entry);

	notesEditor->edit(id);

	QTest::qWait(1000);

	ulong savedId = notesEditor->close(NotesEditor::CloseWithSave);

	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.description(), entry.description());

	delete agendaUtil;
	delete notesEditor;

}


/*!
	Test the api NotesEditor::edit(const QFile &handle)
 */
void TestNotesEditor::testEditingTodoWithFileHandle()
{
	// Nothing yet.
}

/*!
	Test the api NotesEditor::edit(AgendaEntry entry)
 */
void TestNotesEditor::testEditingTodoWithAgendaEntry()
{
	HbMainWindow window;
	window.show();

	NotesEditor *notesEditor = new NotesEditor();
	QVERIFY(notesEditor);

	AgendaUtil *agendaUtil = new AgendaUtil();
	QVERIFY(agendaUtil);

	// Create a agenda entry with type to-do
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeTodo);
	entry.setSummary(QString("Buy Books"));
	entry.setDescription(QString("@Landmarks"));
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),QDateTime::currentDateTime());
	entry.setStatus(AgendaEntry::TodoNeedsAction);
	entry.setPriority(2);
	
	ulong id = agendaUtil->store(entry);

	// Call edit on agenda entry
	notesEditor->edit(entry);

	QTest::qWait(3000);

	ulong afterSaveId = notesEditor->close(NotesEditor::CloseWithSave);

	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.id(),id);
	QCOMPARE(afterSave.type(),entry.type());
	QCOMPARE(afterSave.summary(), entry.summary());
	QCOMPARE(afterSave.description(), entry.description());

	delete agendaUtil;
	delete notesEditor;
}

/*!
	Test the api NotesEditor::edit(ulong id)
 */
void TestNotesEditor::testEditingTodoWithId()
{
	HbMainWindow window;
	window.show();

	NotesEditor *notesEditor = new NotesEditor();
	QVERIFY(notesEditor);

	AgendaUtil *agendaUtil = new AgendaUtil();
	QVERIFY(agendaUtil);

	// Create a agenda entry with type to-do
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeTodo);
	entry.setSummary(QString("Buy Books"));
	entry.setDescription(QString("@Landmarks"));
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),QDateTime::currentDateTime());
	entry.setStatus(AgendaEntry::TodoNeedsAction);
	entry.setPriority(2);

	ulong id = agendaUtil->store(entry);

	// Call edit using the id
	notesEditor->edit(id);

	QTest::qWait(3000);

	ulong afterSaveId = notesEditor->close(NotesEditor::CloseWithSave);

	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.id(),id);
	QCOMPARE(afterSave.type(),entry.type());
	QCOMPARE(afterSave.summary(), entry.summary());
	QCOMPARE(afterSave.description(), entry.description());

	delete agendaUtil;
	delete notesEditor;

}

/*!
	Test the api NotesEditor::create(CreateType type)
 */
void TestNotesEditor::testCreationOfNote()
{
	HbMainWindow window;
	window.show();

	NotesEditor *notesEditor = new NotesEditor();
	QVERIFY(notesEditor);

	notesEditor->create(NotesEditor::CreateNote);

	QTest::qWait(3000);

	ulong afterSaveId = notesEditor->close(NotesEditor::CloseWithSave);

	// Note is not saved since description is empty
	// TODO use key press events here

	QVERIFY(!afterSaveId);

	delete notesEditor;
}

/*!
	Test the api NotesEditor::create(CreateType type)
 */
void TestNotesEditor::testCreationofTodo()
{
	HbMainWindow window;
	window.show();

	NotesEditor *notesEditor = new NotesEditor();
	QVERIFY(notesEditor);

	notesEditor->create(NotesEditor::CreateTodo);

	// TODO
	// Needs key interaction to edit the fields in to-do editor.

	QTest::qWait(3000);

	delete notesEditor;
}

/*!
	Test the api NotesEditor::close(CloseType type);
 */

void TestNotesEditor::testClosingOfNotesEditorWithSave()
{
	HbMainWindow window;
	window.show();
	// Create NotesEditor object.
	NotesEditor *notesEditor = new NotesEditor(this);
	QVERIFY(notesEditor);

	QString noteText("This is test note");
	// Call edit using the text.
	notesEditor->edit(QString("This is test note"));

	QTest::qWait(1000);

	// Call close on editor by saving the note
	ulong id = notesEditor->close(NotesEditor::CloseWithSave);
	QVERIFY(id);

	// Create agenda Utility.
	AgendaUtil *agendaUtil = new AgendaUtil();
	QVERIFY(agendaUtil);

	// Fetch the entry using the id
	AgendaEntry entry = agendaUtil->fetchById(id);

	QCOMPARE(noteText,entry.description());

	delete agendaUtil;
	delete notesEditor;
}

/*!
	Test the api NotesEditor::close(CloseType type);
 */
void TestNotesEditor::testClosingOfNotesEditorWithoutSave()
{
	HbMainWindow window;
	window.show();
	// Create NotesEditor object.
	NotesEditor *notesEditor = new NotesEditor(this);
	QVERIFY(notesEditor);

	QString noteText("This is test note");
	// Call edit using the text.
	notesEditor->edit(QString("This is test note"));

	QTest::qWait(1000);

	// Call close on editor by saving the note
	ulong id = notesEditor->close(NotesEditor::CloseWithoutSave);
	QVERIFY(!id);

	delete notesEditor;
}

/*!
	Test the api NotesEditor::close(CloseType type);
 */
void TestNotesEditor::testClosingOfTodoEditorWithSave()
{
	HbMainWindow window;
	window.show();

	NotesEditor *notesEditor = new NotesEditor();
	QVERIFY(notesEditor);

	AgendaUtil *agendaUtil = new AgendaUtil();
	QVERIFY(agendaUtil);

	// Create a agenda entry with type to-do
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeTodo);
	entry.setSummary(QString("Buy Books"));
	entry.setDescription(QString("@Landmarks"));
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),QDateTime::currentDateTime());
	entry.setStatus(AgendaEntry::TodoNeedsAction);
	entry.setPriority(2);
	
	ulong id = agendaUtil->store(entry);

	// Call edit on agenda entry
	notesEditor->edit(entry);

	QTest::qWait(3000);

	ulong afterSaveId = notesEditor->close(NotesEditor::CloseWithSave);

	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.id(),id);
	QCOMPARE(afterSave.type(),entry.type());
	QCOMPARE(afterSave.summary(), entry.summary());
	QCOMPARE(afterSave.description(), entry.description());

	delete agendaUtil;
	delete notesEditor;
}

/*!
	Test the api NotesEditor::close(CloseType type);
 */
void TestNotesEditor::testClosingOfTodoEditorWithoutSave()
{
	HbMainWindow window;
	window.show();

	NotesEditor *notesEditor = new NotesEditor();
	QVERIFY(notesEditor);

	AgendaUtil *agendaUtil = new AgendaUtil();
	QVERIFY(agendaUtil);

	// Create a agenda entry with type to-do
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeTodo);
	entry.setSummary(QString("Buy Books"));
	entry.setDescription(QString("@Landmarks"));
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),QDateTime::currentDateTime());
	entry.setStatus(AgendaEntry::TodoNeedsAction);
	entry.setPriority(2);
			
	ulong id = agendaUtil->store(entry);

	// Call edit on agenda entry
	notesEditor->edit(entry);

	QTest::qWait(3000);

	ulong afterSaveId = notesEditor->close(NotesEditor::CloseWithoutSave);
	QVERIFY(!afterSaveId);

	delete agendaUtil;
	delete notesEditor;
}
// End of file	--Don't remove this.
