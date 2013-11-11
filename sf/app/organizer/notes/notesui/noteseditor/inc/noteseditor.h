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
* The header file for NotesEditor class.
*
*/

#ifndef NOTESEDITOR_H
#define NOTESEDITOR_H

// System includes
#include <QObject>
#include <qglobal.h>

// User includes
#include "noteseditordefines.h"

// Forward declarations
class QFile;
class QString;
class AgendaUtil;
class AgendaEntry;
class NotesEditorPrivate;

class NOTESEDITOR_EXPORT NotesEditor : public QObject
{
	Q_OBJECT

public:
	enum CreateType{
		CreateNote = 0,
		CreateTodo,
		CreateTypeUnKnown = -1
	};
	enum CloseType{
		CloseWithSave = 0,
		CloseWithoutSave,
		CloseTypeUnKnown = -1
	};

	explicit NotesEditor(QObject *parent = 0);
	explicit NotesEditor(AgendaUtil *agendaUtil, QObject *parent = 0);
	virtual ~NotesEditor();

public:
	void edit(const QString &string);
	void edit(const QFile &handle);
	void edit(AgendaEntry entry);
	void edit(ulong id);
	void create(CreateType type);
	ulong close(CloseType type);

signals:
	void editingCompleted(bool status = true);

private:
	NotesEditorPrivate *d_ptr;
	Q_DECLARE_PRIVATE_D(d_ptr, NotesEditor)
	Q_DISABLE_COPY(NotesEditor)
};

#endif // NOTESEDITOR_H

// End of file	--Don't remove this.
