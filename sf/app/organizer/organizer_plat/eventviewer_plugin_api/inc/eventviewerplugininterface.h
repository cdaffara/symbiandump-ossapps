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
* Description: Header file for EventViewer Plugin Interface
*
*/

#ifndef EVENTVIEWERPLUGININTERFACE_H
#define EVENTVIEWERPLUGININTERFACE_H

// System includes
#include <QObject>
#include <QtPlugin>
#include <QDate>
#include <qglobal.h>

// Forward declarations
class QFile;
class QString;
class AgendaUtil;
class AgendaEntry;

#define CALENDAR_EVENTVIEWER_PLUGIN_PATH QString("z:/resource/qt/plugins/calendar")
#define CALENDAR_EVENTVIEWER_PLUGIN_NAME QString("agendaeventviewerplugin.qtplugin")

class EventViewerPluginInterface : public QObject
{
	Q_OBJECT
	
public:
	enum Actions {
		ActionNothing,
		ActionEdit = 0x01,
		ActionDelete = 0x02,
		ActionEditDelete = ActionEdit | ActionDelete,
		ActionSave = 0x04
	};
	virtual ~EventViewerPluginInterface() {}


		
public:
	virtual void viewEvent(const ulong id, 
	                       Actions action = ActionNothing,
	                       AgendaUtil *agendaUtil = 0) = 0;
	virtual void viewEvent(const QFile &fileHandle, 
	                       Actions action = ActionNothing,
	                       AgendaUtil *agendaUtil = 0) = 0;
	virtual void viewEvent(AgendaEntry entry,
	                       Actions action = ActionNothing,
	                       AgendaUtil *agendaUtil = 0) = 0;

signals:
	void viewingCompleted(const QDate date = QDate());
	void editingStarted();
	void editingCompleted();
	void deletingStarted();
	void deletingCompleted();

};

Q_DECLARE_INTERFACE(EventViewerPluginInterface, 
        "org.nokia.calendar.EventViewerPluginInterface/1.0")

#endif // EVENTVIEWERPLUGININTERFACE_H

// End of file
