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
* Description: Definition of AgendaEventViewer class
*
*/

// User Includes.
#include "agendaeventviewer.h"
#include "agendaeventviewer_p.h"
#include "agendaeventview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "agendaeventviewerTraces.h"
#endif


/*!
	\class AgendaEventViewer

	To view entry in agenda event viewer.
	To delete this class object use deleteLater().
 */

/*!
	\fn void AgendaEventViewer::viewingCompleted(const QDate date = QDate())

	Signal is emitted when viewing of the agenda entry is complete
	Returns QDate for calendar application to set context.
	After receiving this signal use deleteLater() to detele this class object.

 */

/*!
	\fn void AgendaEventViewer::editingStarted()

	Signal is emitted when editing of the agenda entry is start

 */

/*!
	\fn void AgendaEventViewer::editingCompleted()

	Signal is emitted when editing of the agenda entry is complete

 */

/*!
	\fn void AgendaEventViewer::deletingStarted()

	Signal is emitted when deleting of the agenda entry is start

 */

/*!
	\fn void AgendaEventViewer::deletingCompleted()

	Signal is emitted when deleting of the agenda entry is complete

 */

/*!
	Constructor.

	\param parent Pointer to QObject.
 */
AgendaEventViewer::AgendaEventViewer(QObject *parent)
:QObject(parent), d_ptr(new AgendaEventViewerPrivate(0, this))
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEWER_AGENDAEVENTVIEWER_ENTRY );

    OstTraceFunctionExit0( AGENDAEVENTVIEWER_AGENDAEVENTVIEWER_EXIT );
}

/*!
	Constructor

	 \param agendaUtil Pointer to AgendaUtil.
	 \param parent Pointer to QObject.
 */
AgendaEventViewer::AgendaEventViewer(AgendaUtil *agendaUtil, QObject *parent)
: QObject(parent), d_ptr(new AgendaEventViewerPrivate(agendaUtil, this))
{
	OstTraceFunctionEntry0( DUP1_AGENDAEVENTVIEWER_AGENDAEVENTVIEWER_ENTRY );
	// Nothing
	OstTraceFunctionExit0( DUP1_AGENDAEVENTVIEWER_AGENDAEVENTVIEWER_EXIT );
}

/*!
	Destructor.
 */
AgendaEventViewer::~AgendaEventViewer()
{
	OstTraceFunctionEntry0( DUP2_AGENDAEVENTVIEWER_AGENDAEVENTVIEWER_ENTRY );
	if (d_ptr) {
		delete d_ptr;
	}
	OstTraceFunctionExit0( DUP2_AGENDAEVENTVIEWER_AGENDAEVENTVIEWER_EXIT );
}

/*!
	Launches the event viewer.Id is used for fetching
	the calendar entry information.

	\param id Local Uid of the calendar entry to be viewed
 */
void AgendaEventViewer::view(const ulong id, Actions action)
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEWER_VIEW_ENTRY );
	d_ptr->view(id, action);
	OstTraceFunctionExit0( AGENDAEVENTVIEWER_VIEW_EXIT );
}

/*!
	Launches the event viewer.File handle of any vcs/ics file can be given as 
	input to view the calendar entry information.

	\param fileHandle reference to the file handle of vcs/ics file
 */
void AgendaEventViewer::view(const QFile &fileHandle, Actions action)
{
	OstTraceFunctionEntry0( DUP1_AGENDAEVENTVIEWER_VIEW_ENTRY );
	d_ptr->view(fileHandle, action);
	OstTraceFunctionExit0( DUP1_AGENDAEVENTVIEWER_VIEW_EXIT );
}

/*!
	Launches the event viewer.
	AgendaEntry can be given as input to view the calendar entry information.
	
	\param entry Object of calendar entry to be viewed
 */
void AgendaEventViewer::view(AgendaEntry entry, Actions action)
{
	OstTraceFunctionEntry0( DUP2_AGENDAEVENTVIEWER_VIEW_ENTRY );
	d_ptr->view(entry, action);
	OstTraceFunctionExit0( DUP2_AGENDAEVENTVIEWER_VIEW_EXIT );
}

/*! 
    save the entry and close the editor
*/
void AgendaEventViewer::saveAndCloseEditor()
{
    d_ptr->saveAndCloseEditor();
}


/*! 
    close the aganda event view
*/
void AgendaEventViewer::closeAgendaEventView()
{
    d_ptr->closeAgendaEventView();   
}


// End of file
