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
* Definition of NotesNoteEditor class.
*
*/

#ifndef NOTESNOTEEDITOR_H
#define NOTESNOTEEDITOR_H

// System includes
#include <QObject>
#include <QModelIndex>
#include <QPointer>

// User includes
#include <agendautil.h>
#include <agendaentry.h>

// Forward declarations
class HbView;
class HbTextEdit;
class HbAction;
class HbLabel;
class NotesEditorDocLoader;
class NotesEditorPrivate;
class CalenEditor;

class NotesNoteEditor : public QObject
{
	Q_OBJECT

public:
	NotesNoteEditor(NotesEditorPrivate *owner, QObject *parent = 0);
	virtual ~NotesNoteEditor();

private:
	void execute(AgendaEntry entry);
	QString getDescription();

private slots:
	void markNoteAsTodo();
	void saveNote();
	void deleteNote();
	void close();
	void updateNoteText();
	void checkOrientatioAndLoadSection(Qt::Orientation orientation);
	void markNoteAsFavourite();
	void handleDiscardChangesAction();
	void handleAddToCalendarAction();
	void handleEntrySaveFromCalendar();
	void handleCalendarEditorClosed();
	void handleNewNoteAction();
	void selectedAction(HbAction *action);

private:
	HbView *mEditor;

	QPointer<HbTextEdit> mTextEditor;
	QPointer<HbLabel> mViewHeading;
	QPointer<HbLabel> mFavouriteIcon;

	QPointer<HbAction> mMarkToDoAction;
	QPointer<HbAction> mAddToCalendarAction;
	QPointer<HbAction> mDeleteAction;
	QPointer<HbAction> mMarkFavouriteAction;
	QPointer<HbAction> mDiscardChangesAction;
	QPointer<HbAction> mNewNoteAction;
	HbAction *mDeleteNoteAction;
	HbAction *mCancelDeleteAction;
	ulong mEntryId;

	NotesEditorDocLoader *mDocLoader;
	NotesEditorPrivate* mOwner;

	QPointer<CalenEditor> mCalenEditor;

	bool mEntrySavedInCalendar;
	bool mDiscardChangesActionActive;
	bool mIgnoreFirstContentChange;
private:
	friend class NotesEditorPrivate;
};

#endif // NOTESNOTEEDITOR_H

// End of file	--Don't remove this.
