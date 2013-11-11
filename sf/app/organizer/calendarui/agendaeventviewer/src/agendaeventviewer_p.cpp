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
* Description: Definition of AgendaEventViewerPrivate class
*
*/

// System includes.
#include <QFile>
#include <QDir>

// User includes.
#include <agendautil.h>
#include <agendaentry.h>
#include "agendaeventviewer_p.h"
#include "agendaeventviewer.h"
#include "agendaeventview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "agendaeventviewer_pTraces.h"
#endif


/*!
	\class AgendaEventViewerPrivate

	Private class implementation of AgendaEventViewer.It is responsible for
	launching the agenda event viewer.
 */


/*!
	Constructor

	\param agendaUtil Pointer to AgendaUtil.
	\param parent Pointer to QObject.
 */
AgendaEventViewerPrivate::AgendaEventViewerPrivate(
		AgendaUtil *agendaUtil, QObject *parent)
: QObject(parent),mAction(AgendaEventViewer::ActionNothing),mShowEventViewById(false),mShowEventViewByFileHandle(false),mFileName(NULL)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEWERPRIVATE_AGENDAEVENTVIEWERPRIVATE_ENTRY );

	// Get the q-pointer.from parent
	q_ptr = static_cast<AgendaEventViewer *> (parent);

	// Check for agendaUtil sent by client. If null agendaUtil will be
	// created and owned by AgendaEventViewerPrivate
	if (!agendaUtil) {
		mAgendaUtil = new AgendaUtil(parent);
		mViewerOwnsAgendaUtil = true;
	} else {
		mAgendaUtil = agendaUtil;
		mViewerOwnsAgendaUtil = false;
	}

	// Register for the entry change signal when the same entry is updated
	// by other application.This is to handle the db conflict changes.
	connect(
			mAgendaUtil, SIGNAL(entriesChanged(QList<ulong> )),
			this, SLOT(handleEntriesChanged(QList<ulong> )));
	
	// Register for the calenInstance view creation sucessfully
	connect(
	        mAgendaUtil, SIGNAL(entryViewCreationCompleted(int)),
	        this, SLOT(viewCreationCompleted(int)));

	OstTraceFunctionExit0( AGENDAEVENTVIEWERPRIVATE_AGENDAEVENTVIEWERPRIVATE_EXIT );
}

/*!
	Destructor.
 */
AgendaEventViewerPrivate::~AgendaEventViewerPrivate()
{
    OstTraceFunctionEntry0( DUP1_AGENDAEVENTVIEWERPRIVATE_AGENDAEVENTVIEWERPRIVATE_ENTRY );

	if (mViewerOwnsAgendaUtil) {
		delete mAgendaUtil;
		mAgendaUtil = 0;
	}

	OstTraceFunctionExit0( DUP1_AGENDAEVENTVIEWERPRIVATE_AGENDAEVENTVIEWERPRIVATE_EXIT );
}

/*!
	Launches the event viewer.Id is used for fetching
	the calendar entry information.

	\param id Local Uid of the calendar entry to be viewed
 */
void AgendaEventViewerPrivate::view(const ulong id, 
                                    AgendaEventViewer::Actions action)
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEWERPRIVATE_VIEW_ENTRY );
	
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	
	if (entry.isNull()) {
        // save the entries , to show the entry once instances are created
        // if entry is null exit ,later  call back comes in viewCreationCompleted 
	    mAction = action;
        mId =id;
        //to avoid view creation multiple times
        mShowEventViewById = true;
		OstTraceFunctionExit0( AGENDAEVENTVIEWERPRIVATE_VIEW_EXIT );
		return;
	}
	// Construct the agenda event view
	mAgendaEventView = new AgendaEventView(this);
	mAgendaEventView->execute(entry, action);

	OstTraceFunctionExit0( DUP1_AGENDAEVENTVIEWERPRIVATE_VIEW_EXIT );
}

/*!
	Launches the event viewer.File handle of any vcs/ics
	file can be given as input to view the calendar entry information.

	\param fileHandle reference to the file handle of vcs/ics file
 */
void AgendaEventViewerPrivate::view(const QFile &fileHandle, 
                                    AgendaEventViewer::Actions action)
{
    OstTraceFunctionEntry0( DUP1_AGENDAEVENTVIEWERPRIVATE_VIEW_ENTRY );

	// Using calendar importer read the filehandle and generate agenda entry
	QString filePath = fileHandle.fileName();
	QString nativeFilePath = QDir::toNativeSeparators(filePath);
	AgendaEntry entry;
	mAgendaUtil->importvCalendar(nativeFilePath, entry);
	if (!entry.isNull()) {
		mAgendaEventView = new AgendaEventView(this);
		mAgendaEventView->execute(entry, action);
	} else {
        //store the file name
        mFileName = filePath ;
        mAction = action ;
        mShowEventViewByFileHandle = true;
        q_ptr->viewingCompleted(QDateTime::currentDateTime().date());
	}
	OstTraceFunctionExit0( DUP2_AGENDAEVENTVIEWERPRIVATE_VIEW_EXIT );
}

/*!
	Launches the event viewer.AgendaEntry can be given as input to view the
	calendar entry information
 */
void AgendaEventViewerPrivate::view(AgendaEntry entry, 
                                    AgendaEventViewer::Actions action)
{
    OstTraceFunctionEntry0( DUP2_AGENDAEVENTVIEWERPRIVATE_VIEW_ENTRY );

	if (entry.isNull()) {
			OstTraceFunctionExit0( DUP3_AGENDAEVENTVIEWERPRIVATE_VIEW_EXIT );
			return;
		}
	// Construct the agenda event view
	mAgendaEventView = new AgendaEventView(this);
	mAgendaEventView->execute(entry, action);

	OstTraceFunctionExit0( DUP4_AGENDAEVENTVIEWERPRIVATE_VIEW_EXIT );
}

/*!
	Emits the signal viewing completed to the clients

	\param status true if viewing completed otherwise false.
 */
void AgendaEventViewerPrivate::viewingCompleted(const QDate date)
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEWERPRIVATE_VIEWINGCOMPLETED_ENTRY );

	emit q_ptr->viewingCompleted(date);

	// Cleanup viewer.
	if (mAgendaEventView) {
		mAgendaEventView->deleteLater();
	}

	OstTraceFunctionExit0( AGENDAEVENTVIEWERPRIVATE_VIEWINGCOMPLETED_EXIT );
}

/*!
	Emits the signal editing started to the clients
 */
void AgendaEventViewerPrivate::editingStarted()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEWERPRIVATE_EDITINGSTARTED_ENTRY );

	emit q_ptr->editingStarted();

	OstTraceFunctionExit0( AGENDAEVENTVIEWERPRIVATE_EDITINGSTARTED_EXIT );
}

/*!
	Emits the signal editing completed to the clients
 */
void AgendaEventViewerPrivate::editingCompleted()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEWERPRIVATE_EDITINGCOMPLETED_ENTRY );

	emit q_ptr->editingCompleted();

	OstTraceFunctionExit0( AGENDAEVENTVIEWERPRIVATE_EDITINGCOMPLETED_EXIT );
}

/*!
	Emits the signal deleting started to the clients
 */
void AgendaEventViewerPrivate::deletingStarted()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEWERPRIVATE_DELETINGSTARTED_ENTRY );

	emit q_ptr->deletingStarted();

	OstTraceFunctionExit0( AGENDAEVENTVIEWERPRIVATE_DELETINGSTARTED_EXIT );
}

/*!
	Emits the signal deleting completed to the clients
 */
void AgendaEventViewerPrivate::deletingCompleted()
{
    OstTraceFunctionEntry0( AGENDAEVENTVIEWERPRIVATE_DELETINGCOMPLETED_ENTRY );

	emit q_ptr->deletingCompleted();

	OstTraceFunctionExit0( AGENDAEVENTVIEWERPRIVATE_DELETINGCOMPLETED_EXIT );
}


/*!
     calls when instances of calenInstanceview and 
     entryInstanceview is created successfully
 */
void AgendaEventViewerPrivate::viewCreationCompleted(int error)
    {
    OstTraceFunctionEntry0( AGENDAEVENTVIEWERPRIVATE_VIEWCREATIONCOMPLETED_ENTRY );
    
    if((KErrNone == error))
        {
        AgendaEntry entry;
        if (mShowEventViewById)
            {
            entry = mAgendaUtil->fetchById(mId);
            }
        else if(mShowEventViewByFileHandle)
            {
            QString nativeFilePath = QDir::toNativeSeparators(mFileName);
            mAgendaUtil->importvCalendar(nativeFilePath, entry);
            }
        //if entry is there , then show the view
        if (!entry.isNull()) 
            {
            mAgendaEventView = new AgendaEventView(this);
            mAgendaEventView->execute(entry, mAction);
            }
        }      
    //reset the variables
    mId = 0;
    mFileName.clear();
    mShowEventViewById = false;
    mShowEventViewByFileHandle = false;
    mAction = AgendaEventViewer::ActionNothing;
	OstTraceFunctionExit0( AGENDAEVENTVIEWERPRIVATE_VIEWCREATIONCOMPLETED_EXIT );
	}

// ----------------------------------------------------------------------------
// AgendaEventViewerPrivate::saveAndCloseEditor
// save the entry and close the editor
// ----------------------------------------------------------------------------
//
void AgendaEventViewerPrivate::saveAndCloseEditor()
{
    mAgendaEventView->saveAndCloseEditor();
}


// ----------------------------------------------------------------------------
// AgendaEventViewerPrivate::closeAgendaEventView
// close the agenda event view 
// ----------------------------------------------------------------------------
//
void AgendaEventViewerPrivate::closeAgendaEventView()
{
    mAgendaEventView->closeAgendaEventView(); 
}
// End of file
