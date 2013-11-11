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
* Description: Header file for AgendaEventViewer plugin
*
*/

#ifndef AGENDAEVENTVIEWERPLUGIN_H
#define AGENDAEVENTVIEWERPLUGIN_H

// System includes
#include <QObject>
#include <eventviewerplugininterface.h>
#include "agendaeventviewer.h"


// Forward declarations
class QFile;
class QString;
class AgendaUtil;
class AgendaEntry;


class AgendaEventViewerPlugin: public EventViewerPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(EventViewerPluginInterface)
	
public:
	AgendaEventViewerPlugin(QObject *parent = 0);
	~AgendaEventViewerPlugin();
	
public:
	
	void viewEvent(const ulong id, Actions action = ActionNothing, AgendaUtil *agendaUtil = 0);
	void viewEvent(const QFile &fileHandle, 
	               Actions action = ActionNothing, AgendaUtil 
	               *agendaUtil = 0);
	void viewEvent(AgendaEntry entry, 
	               Actions action = ActionNothing, 
	               AgendaUtil *agendaUtil = 0);
	
private:
	
	void CreateAgendaEventViewer(AgendaUtil *agendaUtil);
	AgendaEventViewer::Actions CreateAgendaEventViewerAction(Actions action);
	
private slots:

	void handleViewingCompleted(const QDate date);
	void handleEditingStarted();
	void handleEditingCompleted();
	void handleDeletingStarted();
	void handleDeletingCompleted();

private:

	AgendaEventViewer *mEventViewer;
};

#endif // AGENDAEVENTVIEWERPLUGIN_H

// End of file
