/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Controller class for editing related UI behavior
 *
*/

// System includes.
#include <QDateTime>

// User includes.
#include "calendarui_debug.h"
#include "caleneditui.h"
#include "calencontroller.h"
#include "hb_calencommands.hrh"
#include "calenservicesimpl.h"
#include <agendaentry.h>
#include <caleneditor.h>
#include "calendateutils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenedituiTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenEditUi::NewL
// First stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenEditUi* CCalenEditUi::NewL( CCalenController& aController )
    {
    OstTraceFunctionEntry0( CCALENEDITUI_NEWL_ENTRY );
    
    CCalenEditUi* self = new(ELeave) CCalenEditUi( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCALENEDITUI_NEWL_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::~CCalenEditUi
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenEditUi::~CCalenEditUi()
    {
    OstTraceFunctionEntry0( CCALENEDITUI_CCALENEDITUI_ENTRY );
    
    iController.CancelNotifications(this);
    
    OstTraceFunctionExit0( CCALENEDITUI_CCALENEDITUI_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::CCalenEditUi
// Constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenEditUi::CCalenEditUi( CCalenController& aController )
    : iController( aController )
    {
    OstTraceFunctionEntry0( DUP1_CCALENEDITUI_CCALENEDITUI_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CCALENEDITUI_CCALENEDITUI_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::ConstructL
// Second stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENEDITUI_CONSTRUCTL_ENTRY );
    
    iController.RegisterForNotificationsL( this, ECalenNotifyDialogClosed );
    
    OstTraceFunctionExit0( CCALENEDITUI_CONSTRUCTL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::HandleActionUiCommandL
// Handles action ui commands
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenEditUi::HandleCommandL( const TCalenCommand& aCommand )
    {
    OstTraceFunctionEntry0( CCALENEDITUI_HANDLECOMMANDL_ENTRY );

    TBool continueCommand(EFalse);
    TInt command = aCommand.Command();
    MCalenContext& context = iController.Services().Context();
    AgendaEntry editedEntry;
    QDateTime newEntryDateTime = iController.context().focusDateAndTime();
   
    // Check if it is not on same day and set the default time and date accordingly.
    bool isSameDay = CalenDateUtils::isOnToday(newEntryDateTime);
    if (!isSameDay) {
    	newEntryDateTime = CalenDateUtils::defaultTime(newEntryDateTime);
    }
    switch ( command )
        {
        case ECalenNewMeeting:
			iEditor= new CalenEditor(iController.Services().agendaInterface());
			iEditor->create(newEntryDateTime, false, CalenEditor::TypeAppointment );
		    connect(iEditor, SIGNAL(entrySaved()), this, SLOT(handleEntrySaved()));
		    connect(iEditor, SIGNAL(dialogClosed()), this, SLOT(handleDialogClosed()));
            break;
        case ECalenEditCurrentEntry:            
            editedEntry = iController.Services().agendaInterface()->fetchById(
                                    context.instanceId().mEntryLocalUid );
            // For repeating entry, we need to update proper start time here
            // from the context
            if(!editedEntry.isNull()) {
            	if (editedEntry.isRepeating()) {
            		// Get the instancde time frm the context
            		QDateTime instanceTime = context.focusDateAndTime();
            		editedEntry.setStartAndEndTime(instanceTime, editedEntry.endTime());
            	}
            	iEditor= new CalenEditor(iController.Services().agendaInterface());
            	iEditor->edit(editedEntry, false);
            	connect(iEditor, SIGNAL(entrySaved()), this, SLOT(handleEntrySaved()));
            	connect(iEditor, SIGNAL(dialogClosed()), this, SLOT(handleDialogClosed()));
            }
            break;
        default:
            // Controller decided this class was the place to handle this
            // command
            break;
        }
    
    OstTraceFunctionExit0( CCALENEDITUI_HANDLECOMMANDL_EXIT );
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::handleEntrySaved
// Issues the entry saved notification after receiving signal from editor. 
// ----------------------------------------------------------------------------
//
void CCalenEditUi::handleEntrySaved()
{
    OstTraceFunctionEntry0( CCALENEDITUI_HANDLEENTRYSAVED_ENTRY );
    
	iController.Services().IssueNotificationL(ECalenNotifyEntrySaved);
	
	OstTraceFunctionExit0( CCALENEDITUI_HANDLEENTRYSAVED_EXIT );
}

// ----------------------------------------------------------------------------
// CCalenEditUi::handleEntrySaved
// Issues the dialog closed notification after receiving signal from editor. 
// ----------------------------------------------------------------------------
//
void CCalenEditUi::handleDialogClosed()
{
    OstTraceFunctionEntry0( CCALENEDITUI_HANDLEDIALOGCLOSED_ENTRY );
    
	iController.Services().IssueNotificationL(ECalenNotifyDialogClosed);
	
	OstTraceFunctionExit0( CCALENEDITUI_HANDLEDIALOGCLOSED_EXIT );
}

// ----------------------------------------------------------------------------
// CCalenEditUi::HandleNotification
// Handle notifications
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::HandleNotification(const TCalenNotification aNotification )
    {
    OstTraceFunctionEntry0( CCALENEDITUI_HANDLENOTIFICATION_ENTRY );
    
    if( aNotification == ECalenNotifyDialogClosed )
		{
    	iEditor->deleteLater();
		}
    
    OstTraceFunctionExit0( CCALENEDITUI_HANDLENOTIFICATION_EXIT );
    }
// ----------------------------------------------------------------------------
// CCalenEditUi::saveEntry
// save the entry from editor
// ----------------------------------------------------------------------------
//
void CCalenEditUi::saveAndCloseEditor()
    {
    iEditor->saveAndCloseEditor();
    }
// End of file
