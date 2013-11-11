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
* Description: This is the common includes file for the notes editor.
*
*/

#ifndef NOTESEDITORCOMMON_H
#define NOTESEDITORCOMMON_H

#define NOTES_EDITOR_VIEW_DOCML				":/docml/notesnoteeditor.docml"
#define NOTES_EDITOR_VIEW_OBJECT			"notesEditorView"
#define NOTES_EDITOR_TEXT_EDIT				"textEdit"
#define NOTES_EDITOR_ADD_TO_CALENDAR		"addToCalendarAction"
#define NOTES_EDITOR_MARKTODO_ACTION		"makeAsTodoAction"
#define NOTES_EDITOR_VIEW_HEADING			"editorHeading"
#define NOTES_EDITOR_DELETE_NOTE			"deleteAction"
#define NOTES_EDITOR_MODIFICATION_LABEL		"modificationDate"
#define NOTES_EDITOR_FAVOURITE_ICON			"favoriteIcon"
#define NOTES_EDITOR_MARK_FAVOURITE_ACTION	"makeFavoriteAction"
#define NOTES_EDITOR_DISCARD_CHANGES_ACTION "discardChangesAction"
#define NOTES_EDITOR_NEWNOTE_ACTION			"newNoteAction"
#define TODO_EDITOR_DOCML					":/docml/todoeditor.docml"
#define TODO_EDITOR_VIEW					"todoEditorview"
#define TODO_EDITOR_SUB_HEADING				"subHeading"
#define TODO_EDITOR_DATA_FORM				"todoEditorDataForm"

enum TodoEditorCustomOffset {
	DueDateItemOffset = 1,
	AlarmDateItemOffset,
	ItemOffsetUnKnown = -1
};

enum TodoPriority {
	HighPriority = 1,
	NormalPriority ,
	LowPriority,
	PriorityUnKnown = -1
};
#endif // NOTESEDITORCOMMON_H

// End of file	--Don't remove this.
