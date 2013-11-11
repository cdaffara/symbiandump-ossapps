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
* Description: Declaration of the class TestNotesEditorPlugin.
*
*/

#ifndef UNITTEST_NOTESEDITORPLUGIN_H
#define UNITTEST_NOTESEDITORPLUGIN_H

// System includes
#include <QObject>

class TestNotesEditorPlugin : public QObject
{
	Q_OBJECT

public:
	TestNotesEditorPlugin();
	virtual ~TestNotesEditorPlugin();

private slots:
	// Test case setup.
	void init();
	void cleanup();

	// Test cases go here.
	void testPluginLoadUnLoad();
	void testEditingNoteWithText();
	void testEditingNoteWithFileHandle();
	void testEditingNoteWithAgendaEntry();
	void testEditingNoteWithId();
	void testEditingTodoWithFileHandle();
	void testEditingTodoWithAgendaEntry();
	void testEditingTodoWithId();
	void testCreationOfNote();
	void testCreationofTodo();
	void testClosingOfNotesEditorWithSave();
	void testClosingOfNotesEditorWithoutSave();
	void testClosingOfTodoEditorWithSave();
	void testClosingOfTodoEditorWithoutSave();
	void testAgendaUtilEditingNoteWithText();
	void testAgendaUtilEditingTodoWithId();
};

#endif // UNITTEST_NOTESEDITORPLUGIN_H

// End of file	--Don't remove this.
