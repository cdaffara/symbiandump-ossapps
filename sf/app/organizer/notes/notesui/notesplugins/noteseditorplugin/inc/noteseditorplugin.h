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
* The header file for NotesEditorPlugin class.
*
*/

#ifndef NOTESEDITORPLUGIN_H
#define NOTESEDITORPLUGIN_H

// System includes
#include <QObject>
#include <QPointer>
#include <noteseditorinterface.h>

#include "noteseditor.h"

// Forward declarations
class QFile;
class QString;
class AgendaUtil;
class AgendaEntry;

class NotesEditorPlugin: public NotesEditorInterface
{
	Q_OBJECT
	Q_INTERFACES(NotesEditorInterface)

public:
	NotesEditorPlugin(QObject *parent = 0);
	~NotesEditorPlugin();

public:
	void edit(const QString &string, AgendaUtil *agendaUtil=0);
	void edit(const QFile &handle, AgendaUtil *agendaUtil=0);
	void edit(AgendaEntry entry, AgendaUtil *agendaUtil=0);
	void edit(ulong id, AgendaUtil *agendaUtil=0);
	void create(CreateType type, AgendaUtil *agendaUtil=0);
	ulong close(CloseType type, AgendaUtil *agendaUtil=0);

private:
	void createNotesEditor(AgendaUtil *agendaUtil);

private slots:
	void handleEditingCompleted(bool status);

private:
	QPointer<NotesEditor> mNotesEditor;
};

#endif //NOTESEDITORPLUGIN_H

// End of file	--Don't remove this.
