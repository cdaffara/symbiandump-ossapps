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
* Description: 
*
*/

// System includes
#include <QFile>
#include <QCoreApplication>
#include <agendautil.h>

// User includes
#include "calenviewerservice.h"
#include "agendaeventviewer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenviewerserviceTraces.h"
#endif


// ----------------------------------------------------------------------------
// CalenViewerService::CalenViewerService
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
CalenViewerService::CalenViewerService(QObject* parent) : 
	XQServiceProvider(QLatin1String("calenviewerservice.com.nokia.symbian.IFileView"), parent),
	mFileName(0),
	mViewer(NULL),
	mEntryViewReady(false),
	mWaitingForEntryViewReady(false),
	mAsyncReqIndex(0)
{
    OstTraceFunctionEntry0( CALENVIEWERSERVICE_CALENVIEWERSERVICE_ENTRY );
    
    // Construct the agenda util object
    mAgendaUtil = new AgendaUtil();
    connect(mAgendaUtil, SIGNAL(entryViewCreationCompleted(int)), this, SLOT(entryViewReady(int)));
    connect(this, SIGNAL(clientDisconnected()), this, SLOT(handleClientDisconnected()));
    
	publishAll();
	
	OstTraceFunctionExit0( CALENVIEWERSERVICE_CALENVIEWERSERVICE_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewerService::~CalenViewerService
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
CalenViewerService::~CalenViewerService()
{
    OstTraceFunctionEntry0( DUP1_CALENVIEWERSERVICE_CALENVIEWERSERVICE_ENTRY );
    
    // Cleanup
    if (mAgendaUtil) {
        delete mAgendaUtil;
        mAgendaUtil = NULL;
    }
    if (mViewer) {
        mViewer->deleteLater();
        mViewer = NULL;
    }
    
    OstTraceFunctionExit0( DUP1_CALENVIEWERSERVICE_CALENVIEWERSERVICE_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewerService::view
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
bool CalenViewerService::view(QString file)
{	
    OstTraceFunctionEntry0( CALENVIEWERSERVICE_VIEW_ENTRY );
    
    // Set the request as asynchronous
    mAsyncReqIndex = setCurrentRequestAsync();
    
    // Store the file name
	mFileName = file;
	
	if (mEntryViewReady) {
	    // View the entry only after the entry view
	    // is completely instantiated
	    viewEntry();
	} else {
	    OstTrace0( TRACE_NORMAL, DUP1_CALENVIEWERSERVICE_VIEW, "Entry view not ready" );
	    // Mark the flag to indicate that after the
	    // entry view is ready, the file must be opened
	    mWaitingForEntryViewReady = true;
	}
	
	OstTraceFunctionExit0( CALENVIEWERSERVICE_VIEW_EXIT );
	
	return true;
}

// ----------------------------------------------------------------------------
// CalenViewerService::view
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
bool CalenViewerService::view(XQSharableFile sf)
{	
    OstTraceFunctionEntry0( DUP1_CALENVIEWERSERVICE_VIEW_ENTRY );
    
    Q_UNUSED(sf);
    
    OstTrace0( TRACE_NORMAL, DUP2_CALENVIEWERSERVICE_VIEW, "Functionality not yet present" );
    
    // Set the request as asynchronous
    mAsyncReqIndex = setCurrentRequestAsync();
    
	// TODO: Convert RFile to QFile handle and view
	// Support for this has to come from Qt
	complete(false);
	
	OstTraceFunctionExit0( DUP1_CALENVIEWERSERVICE_VIEW_EXIT );
	
	return true;
}

// ----------------------------------------------------------------------------
// CalenViewerService::entryViewReady
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenViewerService::entryViewReady(int error)
{
    OstTraceFunctionEntry0( CALENVIEWERSERVICE_ENTRYVIEWREADY_ENTRY );
    
    Q_UNUSED(error);
    
    mEntryViewReady = true;

    // Now open the file for viewing
    if (mWaitingForEntryViewReady) {
        // If control comes here, it means that
        // the request to view the file has come before
        // the entry view is ready. So view the entry now
        viewEntry();
    }
    
    OstTraceFunctionExit0( CALENVIEWERSERVICE_ENTRYVIEWREADY_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewerService::handleClientDisconnected
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenViewerService::handleClientDisconnected()
{
    OstTraceFunctionEntry0( CALENVIEWERSERVICE_HANDLECLIENTDISCONNECTED_ENTRY );
    
    // Just exit the application
    qApp->quit();
    
    OstTraceFunctionExit0( CALENVIEWERSERVICE_HANDLECLIENTDISCONNECTED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewerService::viewEntry
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenViewerService::viewEntry()
{
    OstTraceFunctionEntry0( CALENVIEWERSERVICE_VIEWENTRY_ENTRY );
    
    if (mFileName.isEmpty()) {
        // Invalid file name or viewer is already being viewed
        OstTrace0( TRACE_NORMAL, CALENVIEWERSERVICE_VIEWENTRY, "Filename is empty" );
        
        // Just exit the application
        qApp->quit();

        return;
    }
    QFile fileHandle(mFileName);

    if (!mViewer) {
        mViewer = new AgendaEventViewer(mAgendaUtil, this);
        connect(mViewer, SIGNAL(viewingCompleted()), qApp, SLOT(quit()));
    }
    
    // TODO : No indication if the parsing of vcal/ical is successful
    mViewer->view(fileHandle, AgendaEventViewer::ActionSave);
    complete(true);
    
    OstTraceFunctionExit0( DUP1_CALENVIEWERSERVICE_VIEWENTRY_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewerService::complete
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenViewerService::complete(bool ok)
{
    OstTraceFunctionEntry0( CALENVIEWERSERVICE_COMPLETE_ENTRY );
    
    // TODO : Pass return value based on whether parsing
    // of vcal or ical is successful or not
    completeRequest(mAsyncReqIndex, ok);
    
    OstTraceFunctionExit0( CALENVIEWERSERVICE_COMPLETE_EXIT );
}

// End of file	--Don't remove this.

