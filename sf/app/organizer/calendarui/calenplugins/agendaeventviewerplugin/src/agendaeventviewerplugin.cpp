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
* Description: Definition of AgendaEventViewerPlugin class
*
*/

// User Includes.
#include "agendaeventviewerplugin.h"
#include <agendaentry.h>

/*!
	\class AgendaEventViewerPlugin

	To view entry in agenda event viewer plugin.
	To delete this class object use deleteLater().
 */

/*!
	\fn void AgendaEventViewerPlugin::viewingCompleted(const QDate date = QDate())

	Signal is emitted when viewing of the agenda entry is complete
	Returns QDate for calendar application so it has proper context to set.
	After receiving this signal use deleteLater() to detele this class object.

 */

/*!
	\fn void AgendaEventViewerPlugin::editingStarted()

	Signal is emitted when editing of the agenda entry is start

 */

/*!
	\fn void AgendaEventViewerPlugin::editingCompleted()

	Signal is emitted when editing of the agenda entry is complete

 */

/*!
	\fn void AgendaEventViewerPlugin::deletingStarted()

	Signal is emitted when deleting of the agenda entry is start

 */

/*!
	\fn void AgendaEventViewerPlugin::deletingCompleted()

	Signal is emitted when deleting of the agenda entry is complete

 */

/*!
	Constructor.

	\param parent Pointer to QObject.
 */
AgendaEventViewerPlugin::AgendaEventViewerPlugin(QObject *parent)
:mEventViewer(NULL)
{
Q_UNUSED(parent)
}

/*!
	Destructor.
 */
AgendaEventViewerPlugin::~AgendaEventViewerPlugin()
{
}

/*!
	Launches the event viewer.Id is used for fetching
	the calendar entry information.

	\param id Local Uid of the calendar entry to be viewed
 */
void AgendaEventViewerPlugin::viewEvent(const ulong id,
                                        Actions action,
                                        AgendaUtil *agendaUtil)
{
	CreateAgendaEventViewer(agendaUtil);

	AgendaEventViewer::Actions eventviewerAction =
	        CreateAgendaEventViewerAction(action);
	if (mEventViewer) {
		mEventViewer->view(id, eventviewerAction);
	}
}

/*!
	Launches the event viewer.File handle of any vcs/ics file can be given as 
	input to view the calendar entry information.

	\param fileHandle reference to the file handle of vcs/ics file
 */
void AgendaEventViewerPlugin::viewEvent(const QFile &fileHandle,
                                        Actions action,
                                        AgendaUtil *agendaUtil)
{
	CreateAgendaEventViewer(agendaUtil);
	AgendaEventViewer::Actions eventviewerAction =
	        CreateAgendaEventViewerAction(action);
	if (mEventViewer) {
		mEventViewer->view(fileHandle, eventviewerAction);
	}
}

/*!
	Launches the event viewer.
	AgendaEntry can be given as input to view the calendar entry information.
	
	\param entry Object of calendar entry to be viewed
 */
void AgendaEventViewerPlugin::viewEvent(AgendaEntry entry,
                                        Actions action,
                                        AgendaUtil *agendaUtil)
{
	CreateAgendaEventViewer(agendaUtil);
	AgendaEventViewer::Actions eventviewerAction =
	        CreateAgendaEventViewerAction(action);
	if (mEventViewer) {
		mEventViewer->view(entry, eventviewerAction);
	}
}

/*!
	Create a Agenda Interface pointer
	\param agendaUtil Pointer of AgendaUtil
	\return Return Pointer of AgendaEventViewer
 */
void AgendaEventViewerPlugin::CreateAgendaEventViewer(
														AgendaUtil *agendaUtil)
{
	if (agendaUtil) {
		mEventViewer = new AgendaEventViewer(agendaUtil, this);
	} else {
		mEventViewer = new AgendaEventViewer(this);
	}

	if (mEventViewer) {
		connect(mEventViewer, SIGNAL(viewingCompleted(const QDate)), this,
		        SLOT(handleViewingCompleted(const QDate)));
		connect(mEventViewer, SIGNAL(editingStarted()), this,
		        SLOT(handleEditingStarted()));
		connect(mEventViewer, SIGNAL(editingCompleted()), this,
		        SLOT(handleEditingCompleted()));
		connect(mEventViewer, SIGNAL(deletingStarted()), this,
		        SLOT(handleDeletingStarted()));
		connect(mEventViewer, SIGNAL(deletingCompleted()), this,
		        SLOT(handleDeletingCompleted()));
	}
}

/*!
	Create AgendaEventViewer Action enum from EventViewerPluginInterface Action enum
	\param action EventViewerPluginInterface Action enum
	\return Return AgendaEventViewer Action enum
 */
AgendaEventViewer::Actions AgendaEventViewerPlugin::
								CreateAgendaEventViewerAction(Actions action)
{
	switch (action) {
		case EventViewerPluginInterface::ActionEdit:
			return AgendaEventViewer::ActionEdit;
		case EventViewerPluginInterface::ActionDelete:
			return AgendaEventViewer::ActionDelete;
		case EventViewerPluginInterface::ActionEditDelete:
			return AgendaEventViewer::ActionEditDelete;
		case EventViewerPluginInterface::ActionSave:
			return AgendaEventViewer::ActionSave;
	}
	
	return AgendaEventViewer::ActionNothing;
}

/*!
    Emits the signal viewing completed to the clients

    \param status true if viewing completed otherwise false.
 */
void AgendaEventViewerPlugin::handleViewingCompleted(const QDate date)
{
	emit viewingCompleted(date);

	// Cleanup viewer.
	if (mEventViewer) {
		mEventViewer->deleteLater();
	}

}

/*!
    Emits the signal editing started to the clients
 */
void AgendaEventViewerPlugin::handleEditingStarted()
{
	emit editingStarted();
}

/*!
    Emits the signal editing completed to the clients
 */
void AgendaEventViewerPlugin::handleEditingCompleted()
{
	emit editingCompleted();
}

/*!
    Emits the signal deleting started to the clients
 */
void AgendaEventViewerPlugin::handleDeletingStarted()
{
	emit deletingStarted();
}

/*!
    Emits the signal deleting completed to the clients
 */
void AgendaEventViewerPlugin::handleDeletingCompleted()
{
	emit deletingCompleted();
}

Q_EXPORT_PLUGIN2(agendaeventviewerplugin, AgendaEventViewerPlugin)

// End of file
