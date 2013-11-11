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

#ifndef UNITTEST_NOTESEDITOR_H
#define UNITTEST_NOTESEDITOR_H

// System includes
#include <QObject>

// Forward declarations.
class NotesEditor;

class TestNotesEditor : public QObject
{
	Q_OBJECT

public:
	TestNotesEditor();
	virtual ~TestNotesEditor();

private slots:
	// Test case setup.
	void init();
	void cleanup();

	// Test cases go here.
	void testConstructionWithoutAgendaUtil();
	void testConstructionWithAgendaUtil();
	void testDestruction();
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

private:
	NotesEditor *mEditor;
};

#endif // UNITTEST_NOTESEDITOR_H

// End of file	--Don't remove this.
