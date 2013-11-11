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
* The header file for Notes Editor Plugin Interface.
*
*/

#ifndef NOTESEDITORINTERFACE_H
#define NOTESEDITORINTERFACE_H

// System includes
#include <QObject>
#include <QtPlugin>
#include <qglobal.h>

// Forward declarations
class QFile;
class QString;
class AgendaUtil;
class AgendaEntry;

#define NOTES_EDITOR_PLUGIN_PATH QString("z:/resource/qt/plugins/notes")
#define NOTES_EDITOR_PLUGIN_NAME QString("noteseditorplugin.qtplugin")

class NotesEditorInterface : public QObject
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

	virtual ~NotesEditorInterface(){}

public:
	virtual void edit(const QString &string, AgendaUtil *agendaUtil=0) = 0;
	virtual void edit(const QFile &handle, AgendaUtil *agendaUtil=0) = 0;
	virtual void edit(AgendaEntry entry, AgendaUtil *agendaUtil=0) = 0;
	virtual void edit(ulong id, AgendaUtil *agendaUtil=0) = 0;
	virtual void create(CreateType type, AgendaUtil *agendaUtil=0) = 0;
	virtual ulong close(CloseType type, AgendaUtil *agendaUtil=0) = 0;

signals:
	void editingCompleted(bool status = true);
};

Q_DECLARE_INTERFACE(NotesEditorInterface,
						"org.nokia.notes.NotesEditorInterface/1.0")

#endif // NOTESEDITORINTERFACE_H

// End of file	--Don't remove this.
