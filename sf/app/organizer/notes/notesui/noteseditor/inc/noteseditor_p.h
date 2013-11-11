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
* The header file for NotesEditorPrivate class.
*
*/

#ifndef NOTESEDITOR_P_H
#define NOTESEDITOR_P_H

// System includes
#include <qglobal.h>
#include <QObject>
#include <QPointer>

// User includes
#include <agendaentry.h>
#include "noteseditor.h"

// Forward declarations
class QFile;
class QString;
class HbTranslator;
class HbView;
class AgendaUtil;
class NotesEditor;
class NotesNoteEditor;
class NotesTodoEditor;

class NotesEditorPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(NotesEditor)

public:
	explicit NotesEditorPrivate(AgendaUtil *agendaUtil,QObject *parent);
	virtual ~NotesEditorPrivate();

private:
	void edit(const QString &string);
	void edit(const QFile &handle);
	void edit(AgendaEntry entry);
	void edit(ulong id);
	void create(NotesEditor::CreateType type);
	ulong close(NotesEditor::CloseType type);
	QString dateFormatString();
	QString timeFormatString();

private slots:
	void handleEntriesChanged(QList<ulong> ids);
	void markNoteAsTodo();
	void updateNoteText();
	void deleteNote();
	bool saveNote();
	bool saveTodo();
	void editingCompleted(bool status);
	void viewCreationCompleted(int error);

private:
	bool isNoteEdited();
	bool isTodoEdited();
	void showNotification(QString text);

private:
	NotesEditor *q_ptr;

	QPointer<NotesNoteEditor> mNoteEditor;
	QPointer<NotesTodoEditor> mTodoEditor;
	QPointer<AgendaUtil> mAgendaUtil;

	AgendaEntry mOriginalNote;
	AgendaEntry mModifiedNote;

	bool mNewEntry;
	bool mOwnsAgendaUtil;
	bool mSaveEntry;

	HbTranslator *mTranslator;

	ulong mNoteId;
private:
	friend class NotesTodoEditor;
	friend class NotesNoteEditor;
	friend class NotesTodoEditorCustomItem;
};

#endif // NOTESEDITOR_P_H

// End of file	--Don't remove this.
