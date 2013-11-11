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
* Definition of NotesEditorPlugin class.
*
*/

// System includes.
#include <noteseditorinterface.h>
#include <AgendaUtil>
#include <AgendaEntry>

// User includes.
#include "noteseditorplugin.h"
#include "noteseditor.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "noteseditorpluginTraces.h"
#endif


/*!
	\class NotesEditorPlugin

	NotesEditorPlugin provides QT plugin implementation of NotesEditorInterface.
 */

/*!
	\fn void NotesEditorPlugin::editingCompleted(bool status = true)

	This signal is emitted when editing/creation of a note/to-do is completed.

	\param status This indicates whether the note/to-do was saved or not.
					This will be `true' if the note was saved by the user.
					false' otherwise.
 */


/*!
	Constructor.

	\param parent QObject pointer.
 */
NotesEditorPlugin::NotesEditorPlugin(QObject *parent)
{
	OstTraceFunctionEntry0( NOTESEDITORPLUGIN_NOTESEDITORPLUGIN_ENTRY );
	Q_UNUSED(parent)
	OstTraceFunctionExit0( NOTESEDITORPLUGIN_NOTESEDITORPLUGIN_EXIT );
}

/*!
	Destructor.
 */
NotesEditorPlugin::~NotesEditorPlugin()
{
	OstTraceFunctionEntry0( DUP1_NOTESEDITORPLUGIN_NOTESEDITORPLUGIN_ENTRY );
	OstTraceFunctionExit0( DUP1_NOTESEDITORPLUGIN_NOTESEDITORPLUGIN_EXIT );
}


/*!
	Shows the noteseditor. The argument acts as the description for the note.

	\param string reference to file string/buffer.
 */
void NotesEditorPlugin::edit(const QString &string, AgendaUtil *agendaUtil)
{
	OstTraceFunctionEntry0( NOTESEDITORPLUGIN_EDIT_ENTRY );
	if (!mNotesEditor) {
		createNotesEditor(agendaUtil);
	}

	mNotesEditor->edit(string);
	OstTraceFunctionExit0( NOTESEDITORPLUGIN_EDIT_EXIT );
}

/*!
	Shows the noteseditor by parsing a .vcs which could be of a To-do or a plain
	.txt file which will be edited as a note.

	\param handle reference to QFile handle.
 */
void NotesEditorPlugin::edit(const QFile &handle, AgendaUtil *agendaUtil)
{
	OstTraceFunctionEntry0( DUP1_NOTESEDITORPLUGIN_EDIT_ENTRY );
	if (!mNotesEditor) {
		createNotesEditor(agendaUtil);
	}

	mNotesEditor->edit(handle);
	OstTraceFunctionExit0( DUP1_NOTESEDITORPLUGIN_EDIT_EXIT );
}

/*!
	Shows the noteseditor, by parsing an AgendaEntry.

	\param entry An object of AgendaEntry.
 */
void NotesEditorPlugin::edit(AgendaEntry entry, AgendaUtil *agendaUtil)
{
	OstTraceFunctionEntry0( DUP2_NOTESEDITORPLUGIN_EDIT_ENTRY );
	if (!mNotesEditor) {
		createNotesEditor(agendaUtil);
	}

	mNotesEditor->edit(entry);
	OstTraceFunctionExit0( DUP2_NOTESEDITORPLUGIN_EDIT_EXIT );
}

/*!
	Shows the noteseditor, by fetching the note using the entry id provided.

	\param id entry id of the note.
 */
void NotesEditorPlugin::edit(ulong id, AgendaUtil *agendaUtil)
{
	OstTraceFunctionEntry0( DUP3_NOTESEDITORPLUGIN_EDIT_ENTRY );
	if (!mNotesEditor) {
		createNotesEditor(agendaUtil);
	}

	mNotesEditor->edit(id);
	OstTraceFunctionExit0( DUP3_NOTESEDITORPLUGIN_EDIT_EXIT );
}

/*!
	Creates a new note or to-do based on the create type.It lanuches the
	respective editors for editing

	\type type of the editor to be shown for creating new note and new to-do
 */
void NotesEditorPlugin::create( NotesEditorInterface::CreateType type,
							AgendaUtil *agendaUtil)
{
	OstTraceFunctionEntry0( NOTESEDITORPLUGIN_CREATE_ENTRY );
	if (!mNotesEditor) {
		createNotesEditor(agendaUtil);
	}

	mNotesEditor->create(static_cast<NotesEditor::CreateType>(type));
	OstTraceFunctionExit0( NOTESEDITORPLUGIN_CREATE_EXIT );
}

/*!
	Closes the notes editor

	\param type Type of the close required.
 */
ulong NotesEditorPlugin::close( NotesEditorInterface::CloseType type,
							AgendaUtil *agendaUtil)
{
	OstTraceFunctionEntry0( NOTESEDITORPLUGIN_CLOSE_ENTRY );
	if (!mNotesEditor) {
		createNotesEditor(agendaUtil);
	}

	return mNotesEditor->close(static_cast<NotesEditor::CloseType>(type));
}

/*!
	Creates notes Editor.
 */
void NotesEditorPlugin::createNotesEditor(AgendaUtil *agendaUtil)
{
	OstTraceFunctionEntry0( NOTESEDITORPLUGIN_CREATENOTESEDITOR_ENTRY );
	if (agendaUtil) {
		mNotesEditor = new NotesEditor(agendaUtil, this);
	} else {
		mNotesEditor = new NotesEditor(this);
	}

	if (mNotesEditor) {
		connect(
				mNotesEditor, SIGNAL(editingCompleted(bool)),
				this, SLOT(handleEditingCompleted(bool)));
	}
	OstTraceFunctionExit0( NOTESEDITORPLUGIN_CREATENOTESEDITOR_EXIT );
}

/*!
	Slot to handle editing completed
 */
void NotesEditorPlugin::handleEditingCompleted(bool status)
{
	OstTraceFunctionEntry0( NOTESEDITORPLUGIN_HANDLEEDITINGCOMPLETED_ENTRY );
	// Emits the signal.
	emit editingCompleted(status);

	// Cleanup.
	if (mNotesEditor) {
		mNotesEditor->deleteLater();
	}
	OstTraceFunctionExit0( NOTESEDITORPLUGIN_HANDLEEDITINGCOMPLETED_EXIT );
}

// Exports plugin class NotesEditorPlugin for the target
// plugin noteseditorplugin.
Q_EXPORT_PLUGIN2(noteseditorplugin, NotesEditorPlugin)

// End of file	--Don't remove this.
