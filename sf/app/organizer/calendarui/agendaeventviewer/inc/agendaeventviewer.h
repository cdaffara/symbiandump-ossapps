/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Header file for AgendaEventViewer class
*
*/

#ifndef AGENDAEVENTVIEWER_H
#define AGENDAEVENTVIEWER_H

// System includes
#include <QObject>
#include <qglobal.h>
#include <QDate>

// Forward declarations
class QFile;
class QString;
class AgendaUtil;
class AgendaEntry;
class AgendaEventViewerPrivate;


#ifdef AGENDAEVENTVIEWER_LIB
#	define AGENDAEVENTVIEWER_EXPORT Q_DECL_EXPORT
#else
#	define AGENDAEVENTVIEWER_EXPORT Q_DECL_IMPORT
#endif

class AgendaEventViewer: public QObject
{
	Q_OBJECT
	Q_ENUMS(Actions)

public:
	enum Actions {
		ActionNothing,
		ActionEdit = 0x01,
		ActionDelete = 0x02,
		ActionEditDelete = ActionEdit | ActionDelete,
		ActionSave = 0x04
	};
	AGENDAEVENTVIEWER_EXPORT explicit AgendaEventViewer(QObject *parent = 0);
	AGENDAEVENTVIEWER_EXPORT explicit AgendaEventViewer(AgendaUtil *agendaUtil, 
	                                                    QObject *parent = 0);
	AGENDAEVENTVIEWER_EXPORT virtual ~AgendaEventViewer();

public:
	AGENDAEVENTVIEWER_EXPORT void view(const ulong id, 
	                                   Actions action = ActionNothing);
	AGENDAEVENTVIEWER_EXPORT void view(const QFile &fileHandle, 
	                                   Actions action = ActionNothing);
	AGENDAEVENTVIEWER_EXPORT void view(AgendaEntry entry, 
	                                   Actions action = ActionNothing);

	AGENDAEVENTVIEWER_EXPORT void saveAndCloseEditor();
	
	AGENDAEVENTVIEWER_EXPORT void closeAgendaEventView();

signals:
	void viewingCompleted(const QDate date = QDate());
	void editingStarted();
	void editingCompleted();
	void deletingStarted();
	void deletingCompleted();

private:
	AgendaEventViewerPrivate *d_ptr;
	Q_DECLARE_PRIVATE_D(d_ptr, AgendaEventViewer)
	Q_DISABLE_COPY(AgendaEventViewer)
};

#endif // AGENDAEVENTVIEWER_H

// End of file
