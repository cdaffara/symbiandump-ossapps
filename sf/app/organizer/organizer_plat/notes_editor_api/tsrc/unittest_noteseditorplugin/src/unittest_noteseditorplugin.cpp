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
#include <NotesEditorInterface>
#include <AgendaUtil>
#include <AgendaEntry>

// User includes
#include "unittest_noteseditorplugin.h"


/*!
	\class TestNotesEditorPlugin

	Unit test clas for Notes Editor Plugin apis
 */


/*!
	Constructor
 */

TestNotesEditorPlugin::TestNotesEditorPlugin()
{
}

/*!
	Destructor
 */
TestNotesEditorPlugin::~TestNotesEditorPlugin()
{
}

/*!
	Initialise before calling the test case
 */
void TestNotesEditorPlugin::init()
{
}

/*!
	Do cleanup after each test case
 */
void TestNotesEditorPlugin::cleanup()
{
}

/*!
	Test the api NotesEditor::NotesEditor()
 */

void TestNotesEditorPlugin::testPluginLoadUnLoad()
{
	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create NotesEditor object
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());

	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	QVERIFY(interface);

	// Unload the plugin
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test the api NotesEditorInterface::edit(const QString &string)
 */
void TestNotesEditorPlugin::testEditingNoteWithText()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	QString noteText("Test editing of Note by providing text(QString)");
	// Call edit using the text.
	interface->edit(noteText);

	// Wait for Editor to launch.
	QTest::qWait(2000);

	// Call close on editor by saving the note
	ulong id = interface->close(NotesEditorInterface::CloseWithSave);
	QVERIFY(id);

	// Create agenda Utility.
	AgendaUtil *agendaUtil = new AgendaUtil();
	// Wait for completion of instance view/entryview creation.
	QTest::qWait(1);
	QVERIFY(agendaUtil);

	// Fetch the entry using the id.
	AgendaEntry entry = agendaUtil->fetchById(id);

	QString entryDescription = entry.description();
	int compareResult = noteText.compare(entryDescription);

	QVERIFY(!compareResult);

	// cleanup
	delete agendaUtil;

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test the api NotesEditor::edit(const QFile &handle)
 */
void TestNotesEditorPlugin::testEditingNoteWithFileHandle()
{
	// Nothing yet
}

/*!
	Test the api NotesEditor::edit(AgendaEntry entry)
 */
void TestNotesEditorPlugin::testEditingNoteWithAgendaEntry()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeNote);
	entry.setDescription(QString("Test Editing Note by providing Agenda Entry"));

	// Call edit on agenda entry
	interface->edit(entry);

	// Wait for Editor to launch.
	QTest::qWait(2000);

	ulong id = interface->close(NotesEditorInterface::CloseWithSave);

	AgendaUtil *agendaUtil = new AgendaUtil();
	// Wait for completion of instance view/entryview creation.
	QTest::qWait(1);
	QVERIFY(agendaUtil);

	// Compare the description after saving.
	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.description(), entry.description());

	// cleanup.
	delete agendaUtil;

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test the api NotesEditor::edit(ulong id)
 */
void TestNotesEditorPlugin::testEditingNoteWithId()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	AgendaUtil *agendaUtil = new AgendaUtil();
	// Wait for completion of instance view/entryview creation.
	QTest::qWait(1);

	QVERIFY(agendaUtil);

	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeNote);
	entry.setDescription(
			QString("Test Editing of Note by providing entry local Id"));

	ulong id = agendaUtil->store(entry);

	interface->edit(id);

	// Wait for Editor to launch.
	QTest::qWait(1000);

	ulong savedId = interface->close(NotesEditorInterface::CloseWithSave);

	// Compare the entry after saving. Using the old entry id since
	// text is not modified in the editor. Wait can be increased to allow user
	// to modify the text.
	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.description(), entry.description());

	// cleanup.
	delete agendaUtil;

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}


/*!
	Test the api NotesEditor::edit(const QFile &handle)
 */
void TestNotesEditorPlugin::testEditingTodoWithFileHandle()
{
	// Nothing yet.
}

/*!
	Test the api NotesEditor::edit(AgendaEntry entry)
 */
void TestNotesEditorPlugin::testEditingTodoWithAgendaEntry()
{
	HbMainWindow mainWindow;
	mainWindow.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	AgendaUtil *agendaUtil = new AgendaUtil();
	// Wait for completion of instance view/entryview creation.
	QTest::qWait(1);
	QVERIFY(agendaUtil);

	// Create a agenda entry with type to-do
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeTodo);
	entry.setSummary(QString("Test Editing of Todo by providing AgendaEntry"));
	entry.setDescription(QString("@QTest Framework."));
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),QDateTime::currentDateTime());
	entry.setStatus(AgendaEntry::TodoNeedsAction);
	entry.setPriority(2);

	ulong id = agendaUtil->store(entry);

	AgendaEntry storedEntry = agendaUtil->fetchById(id);

	// Call edit on agenda entry
	interface->edit(storedEntry);

	// Wait for Editor to launch.
	QTest::qWait(3000);

	ulong afterSaveId = interface->close(NotesEditorInterface::CloseWithSave);

	// Compare the entry after saving.
	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.id(),id);
	QCOMPARE(afterSave.type(),entry.type());
	QCOMPARE(afterSave.summary(), entry.summary());
	QCOMPARE(afterSave.description(), entry.description());

	// cleanup.
	delete agendaUtil;

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test the api NotesEditor::edit(ulong id)
 */
void TestNotesEditorPlugin::testEditingTodoWithId()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	AgendaUtil *agendaUtil = new AgendaUtil();
	// Wait for completion of instance view/entryview creation.
	QTest::qWait(1);
	QVERIFY(agendaUtil);

	// Create a agenda entry with type to-do
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeTodo);
	entry.setSummary(
			QString("Test Editing of Todo by providing entry local Id"));
	entry.setDescription(QString("@QTest Framework"));
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),QDateTime::currentDateTime());
	entry.setStatus(AgendaEntry::TodoNeedsAction);
	entry.setPriority(2);

	ulong id = agendaUtil->store(entry);
	AgendaEntry storedEntry = agendaUtil->fetchById(id);

	// Call edit on agenda entry
	interface->edit(storedEntry);

	// Wait for Editor to launch.
	QTest::qWait(3000);

	ulong afterSaveId = interface->close(NotesEditorInterface::CloseWithSave);

	// Compare the entry after saving.
	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.id(),id);
	QCOMPARE(afterSave.type(),entry.type());
	QCOMPARE(afterSave.summary(), entry.summary());
	QCOMPARE(afterSave.description(), entry.description());

	// cleanup.
	delete agendaUtil;

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test the api NotesEditor::create(CreateType type)
 */
void TestNotesEditorPlugin::testCreationOfNote()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	interface->create(NotesEditorInterface::CreateNote);

	// Wait for Editor to launch.
	QTest::qWait(3000);

	ulong afterSaveId = interface->close(NotesEditorInterface::CloseWithSave);

	// Note is not saved since description is empty
	// TODO use key press events here

	QVERIFY(!afterSaveId);

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test the api NotesEditor::create(CreateType type)
 */
void TestNotesEditorPlugin::testCreationofTodo()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	interface->create(NotesEditorInterface::CreateTodo);

	// TODO
	// Needs key interaction to edit the fields in to-do editor.

	// Wait for Editor to launch.
	QTest::qWait(3000);

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test the api NotesEditor::close(CloseType type);
 */

void TestNotesEditorPlugin::testClosingOfNotesEditorWithSave()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	QString noteText("Test Closing Of NotesEditor With Save");
	// Call edit using the text.
	interface->edit(noteText);

	// Wait for Editor to launch.
	QTest::qWait(1000);

	// Call close on editor by saving the note
	ulong id = interface->close(NotesEditorInterface::CloseWithSave);
	QVERIFY(id);

	// Create agenda Utility.
	AgendaUtil *agendaUtil = new AgendaUtil();
	// Wait for completion of instance view/entryview creation.
	QTest::qWait(1);
	QVERIFY(agendaUtil);

	// Fetch the entry using the id
	AgendaEntry entry = agendaUtil->fetchById(id);

	QCOMPARE(noteText,entry.description());

	// cleanup.
	delete agendaUtil;

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test the api NotesEditor::close(CloseType type);
 */
void TestNotesEditorPlugin::testClosingOfNotesEditorWithoutSave()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	QString noteText("Test Closing Of Notes Editor Without Save");
	// Call edit using the text.
	interface->edit(noteText);

	// Wait for Editor to launch.
	QTest::qWait(1000);

	// Call close on editor by saving the note
	ulong id = interface->close(NotesEditorInterface::CloseWithoutSave);
	QVERIFY(!id);

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test the api NotesEditor::close(CloseType type);
 */
void TestNotesEditorPlugin::testClosingOfTodoEditorWithSave()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	AgendaUtil *agendaUtil = new AgendaUtil();
	// Wait for completion of instance view/entryview creation.
	QTest::qWait(1);
	QVERIFY(agendaUtil);

	// Create a agenda entry with type to-do
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeTodo);
	entry.setSummary(
			QString("Test Closing Of Todo Editor With Save"));
	entry.setDescription(QString("@QTest Framework"));
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),QDateTime::currentDateTime());
	entry.setStatus(AgendaEntry::TodoNeedsAction);
	entry.setPriority(2);

	ulong id = agendaUtil->store(entry);

	AgendaEntry storedEntry = agendaUtil->fetchById(id);
	// Call edit on agenda entry
	interface->edit(storedEntry);

	// Wait for Editor to launch.
	QTest::qWait(1000);

	ulong afterSaveId = interface->close(NotesEditorInterface::CloseWithSave);

	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.id(),id);
	QCOMPARE(afterSave.type(),entry.type());
	QCOMPARE(afterSave.summary(), entry.summary());
	QCOMPARE(afterSave.description(), entry.description());

	// cleanup.
	delete agendaUtil;

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test the api NotesEditor::close(CloseType type);
 */
void TestNotesEditorPlugin::testClosingOfTodoEditorWithoutSave()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	AgendaUtil *agendaUtil = new AgendaUtil();
	// Wait for completion of instance view/entryview creation.
	QTest::qWait(1);
	QVERIFY(agendaUtil);

	// Create a agenda entry with type to-do
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeTodo);
	entry.setSummary(
			QString("Test Closing Of Todo Editor Without Save"));
	entry.setDescription(QString("@QTest framework"));
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(),QDateTime::currentDateTime());
	entry.setStatus(AgendaEntry::TodoNeedsAction);
	entry.setPriority(2);

	ulong id = agendaUtil->store(entry);

	AgendaEntry storedEntry = agendaUtil->fetchById(id);
	// Call edit on agenda entry
	interface->edit(storedEntry);

	// Wait for Editor to launch.
	QTest::qWait(3000);

	ulong afterSaveId = interface->close(
			NotesEditorInterface::CloseWithoutSave);
	QVERIFY(!afterSaveId);

	// cleanup.
	delete agendaUtil;

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test editing of note by owning agenda util.
 */
void TestNotesEditorPlugin::testAgendaUtilEditingNoteWithText()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	// Create agenda Utility.
	AgendaUtil *agendaUtil = new AgendaUtil();
	// Wait for completion of instance view/entryview creation.
	QTest::qWait(1);
	QVERIFY(agendaUtil);

	QString noteText("Test Editing of Note With Text by passing created "
			"agenda util to notes editor plugin");
	// Call edit using the text.
	interface->edit(noteText, agendaUtil);

	// Call close on editor by saving the note
	ulong id = interface->close(NotesEditorInterface::CloseWithSave);
	QVERIFY(id);

	// Fetch the entry using the id
	AgendaEntry entry = agendaUtil->fetchById(id);

	QString entryDescription = entry.description();
	int compareResult = noteText.compare(entryDescription);

	QVERIFY(!compareResult);

	// cleanup.
	delete agendaUtil;

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

/*!
	Test editing to-do entry by owning agenda util.
 */
void TestNotesEditorPlugin::testAgendaUtilEditingTodoWithId()
{
	HbMainWindow window;
	window.show();

	QDir dir(NOTES_EDITOR_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

	// Create plugin loader.
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);

	// Load the plugin.
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());
	NotesEditorInterface* interface =
			qobject_cast<NotesEditorInterface*>(plugin);

	AgendaUtil *agendaUtil = new AgendaUtil();
	// Wait for completion of instance view/entryview creation.
	QTest::qWait(1);

	QVERIFY(agendaUtil);

	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeNote);
	entry.setDescription(
			QString("Test Editing of Note by providing entry local Id"));

	ulong id = agendaUtil->store(entry);

	// Edits to-do entry by providing entry id and agendautil.
	interface->edit(id, agendaUtil);

	// Wait for Editor to launch.
	QTest::qWait(1000);

	ulong savedId = interface->close(NotesEditorInterface::CloseWithSave);

	// Compare the entry after saving. Using the old entry id since
	// text is not modified in the editor. Wait can be increased to allow user
	// to modify the text.
	AgendaEntry afterSave = agendaUtil->fetchById(id);
	QCOMPARE(afterSave.description(), entry.description());

	// cleanup.
	delete agendaUtil;

	// Unload the plugin.
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}
// End of file	--Don't remove this.
