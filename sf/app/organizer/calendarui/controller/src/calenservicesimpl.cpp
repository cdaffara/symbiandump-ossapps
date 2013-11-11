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
* Description:  Implementation of Calendar services
*
*/

// System includes
#include <agendautil.h>
#include <hbwidget.h>

#include "calendarui_debug.h"
#include "calenservicesimpl.h"
#include "calencontroller.h"
#include "calenviewmanager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenservicesimplTraces.h"
#endif



// ----------------------------------------------------------------------------
// CalenServicesImpl::NewL
// Two phased constructor.
// Taking the command range start and end points
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenServicesImpl* CalenServicesImpl::NewL( TInt aCommandRangeStart,
                                                                  TInt aCommandRangeEnd )
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_NEWL_ENTRY );
    CalenServicesImpl* self = new( ELeave ) CalenServicesImpl(
                                       aCommandRangeStart, aCommandRangeEnd );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CALENSERVICESIMPL_NEWL_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::NewL
// 1st phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenServicesImpl* CalenServicesImpl::NewL()
    {
    OstTraceFunctionEntry0( DUP1_CALENSERVICESIMPL_NEWL_ENTRY );
    
    CalenServicesImpl* self = new( ELeave ) CalenServicesImpl( 0, 0 );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( DUP1_CALENSERVICESIMPL_NEWL_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::CalenServicesImpl
// C++ constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenServicesImpl::CalenServicesImpl( TInt aCommandRangeStart,
                                                        TInt aCommandRangeEnd )
    : iCommandRangeStart( aCommandRangeStart ),
      iCommandRangeEnd( aCommandRangeEnd )
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_CALENSERVICESIMPL_ENTRY );
    
    OstTraceFunctionExit0( CALENSERVICESIMPL_CALENSERVICESIMPL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::ConstructL
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::ConstructL()
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_CONSTRUCTL_ENTRY );
    
    iController = CCalenController::InstanceL();

    OstTraceFunctionExit0( CALENSERVICESIMPL_CONSTRUCTL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::Release
// Performs cleanup of this object.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::Release()
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_RELEASE_ENTRY );

    if( iController )
        {
        iController->Release();
        }  
    
    delete this;

    OstTraceFunctionExit0( CALENSERVICESIMPL_RELEASE_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::SessionL
// Retrieve the calendar session currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
HbMainWindow& CalenServicesImpl::MainWindow()
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_MAINWINDOW_ENTRY );
    
    ASSERT( iController ); // Make sure ConstructL has been called.
    
    OstTraceFunctionExit0( CALENSERVICESIMPL_MAINWINDOW_EXIT );
    return iController->MainWindow();
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::RegisterForNotificationsL
// Register for notification of Calendar event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            TCalenNotification aNotification )
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_REGISTERFORNOTIFICATIONSL_ENTRY );
    
    ASSERT( iController ); // Make sure ConstructL has been called.
    iController->RegisterForNotificationsL( aHandler, aNotification );

    OstTraceFunctionExit0( CALENSERVICESIMPL_REGISTERFORNOTIFICATIONSL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::RegisterForNotificationsL
// Register for array notifications of Calendar events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            RArray<TCalenNotification>& aNotifications )
    {
    OstTraceFunctionEntry0( DUP1_CALENSERVICESIMPL_REGISTERFORNOTIFICATIONSL_ENTRY );
    
    ASSERT( iController ); // Make sure ConstructL has been called.
    iController->RegisterForNotificationsL( aHandler, aNotifications );

    OstTraceFunctionExit0( DUP1_CALENSERVICESIMPL_REGISTERFORNOTIFICATIONSL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::CancelNotifications
// Cancel notifications of Calendar events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::CancelNotifications( MCalenNotificationHandler* aHandler )
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_CANCELNOTIFICATIONS_ENTRY );
    
    ASSERT( iController ); // Make sure ConstructL has been called.
    iController->CancelNotifications( aHandler );

    OstTraceFunctionExit0( CALENSERVICESIMPL_CANCELNOTIFICATIONS_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::IssueCommandL
// Issue a command to be handled by Calendar or a customization
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CalenServicesImpl::IssueCommandL( TInt aCommand )
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_ISSUECOMMANDL_ENTRY );
    
    ASSERT( iController ); // Make sure ConstructL has been called.
    
    OstTraceFunctionExit0( CALENSERVICESIMPL_ISSUECOMMANDL_EXIT );
    return iController->IssueCommandL( aCommand );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::IssueNotificationL
// Issue a notification to Calendar, which will be broadcast synchronously
// to all registered notification handlers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::IssueNotificationL( TCalenNotification aNotification )
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_ISSUENOTIFICATIONL_ENTRY );
    
    ASSERT( iController ); // Make sure ConstructL has been called.
    iController->BroadcastNotification( aNotification );

    OstTraceFunctionExit0( CALENSERVICESIMPL_ISSUENOTIFICATIONL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::RequestActivationL
// Request activation of a specific view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::RequestActivationL( const TVwsViewId& /*aViewId*/ )
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_REQUESTACTIVATIONL_ENTRY );
    //ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
   // iController->RequestActivationL( aViewId );
    
    OstTraceFunctionExit0( CALENSERVICESIMPL_REQUESTACTIVATIONL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::ActivationNotificationL
// Notify Calendar that a specific view has been activated
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::ActivationNotificationL( const TVwsViewId& /*aViewId*/ )
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_ACTIVATIONNOTIFICATIONL_ENTRY );
    
    //ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
    OstTraceFunctionExit0( CALENSERVICESIMPL_ACTIVATIONNOTIFICATIONL_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::Context
// Returns the context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenContext& CalenServicesImpl::Context()
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_CONTEXT_ENTRY );
    
    ASSERT( iController ); // Make sure ConstructL has been called.
    
    OstTraceFunctionExit0( CALENSERVICESIMPL_CONTEXT_EXIT );
    return iController->context();
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::Infobar
// Descriptor passed to plugins to get customised info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
HbWidget* CalenServicesImpl::Infobar( )
    {    
    OstTraceFunctionEntry0( CALENSERVICESIMPL_INFOBAR_ENTRY );
    
    OstTraceFunctionExit0( CALENSERVICESIMPL_INFOBAR_EXIT );
    return iController->Infobar();
    }
// ----------------------------------------------------------------------------
// CCalenServicesImpl::InfobarTextL
// @returns info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
QString* CalenServicesImpl::InfobarTextL()
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_INFOBARTEXTL_ENTRY );
    
    OstTraceFunctionExit0( CALENSERVICESIMPL_INFOBARTEXTL_EXIT );
    return iController->InfobarTextL();
    }
// ----------------------------------------------------------------------------
// CalenServicesImpl::GetCommandRange
// Returns the assigned command range
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::GetCommandRange( TInt& aCommandRangeStart, 
                                                              TInt& aCommandRangeEnd ) const
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_GETCOMMANDRANGE_ENTRY );
    
    aCommandRangeStart = iCommandRangeStart;
    aCommandRangeEnd = iCommandRangeEnd;

    OstTraceFunctionExit0( CALENSERVICESIMPL_GETCOMMANDRANGE_EXIT );
    }
// ----------------------------------------------------------------------------
// CalenServicesImpl::OfferMenu
// Offers the menu to plugins for customisation
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::OfferMenu(HbMenu* aHbMenu )
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_OFFERMENU_ENTRY );
    
    iController->OfferMenu(aHbMenu);

    OstTraceFunctionExit0( CALENSERVICESIMPL_OFFERMENU_EXIT );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::agendaInterface
// returns the interface to the agenda database
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
AgendaUtil* CalenServicesImpl::agendaInterface()
    {
    OstTraceFunctionEntry0( CALENSERVICESIMPL_AGENDAINTERFACE_ENTRY );
    
    OstTraceFunctionExit0( CALENSERVICESIMPL_AGENDAINTERFACE_EXIT );
    return iController->agendaInterface();
    
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::getFirstView
// returns the first view with which calendar has been launched
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
int CalenServicesImpl::getFirstView()
{
    OstTraceFunctionEntry0( CALENSERVICESIMPL_GETFIRSTVIEW_ENTRY );
    
    OstTraceFunctionExit0( CALENSERVICESIMPL_GETFIRSTVIEW_EXIT );
	return iController->getFirstView();
}

// End of file
