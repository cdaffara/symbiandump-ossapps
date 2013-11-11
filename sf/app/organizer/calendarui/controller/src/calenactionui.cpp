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
* Description:  Calendar action ui
*
*/


// INCLUDES
#include "calendarui_debug.h"
#include "calenactionui.h"
#include "calencontroller.h"
#include "calendeleteui.h"
#include "caleneditui.h"
#include "calennotifier.h"
#include "calenservices.h"
#include "calensettingsview.h"
#include "calenviewmanager.h"
#include <hbmainwindow.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenactionuiTraces.h"
#endif

// ----------------------------------------------------------------------------
// CCalenActionUi::NewL
// 1st phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenActionUi* CCalenActionUi::NewL( CCalenController& aController )
    {
    OstTraceFunctionEntry0( CCALENACTIONUI_NEWL_ENTRY );
    
    CCalenActionUi* self = new( ELeave ) CCalenActionUi( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    OstTraceFunctionExit0( CCALENACTIONUI_NEWL_EXIT );
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::CCalenActionUi
// C++ default constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenActionUi::CCalenActionUi( CCalenController& aController )
    : iController( aController )
    {
    OstTraceFunctionEntry0( CCALENACTIONUI_CCALENACTIONUI_ENTRY );
    
    OstTraceFunctionExit0( CCALENACTIONUI_CCALENACTIONUI_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::ConstructL
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenActionUi::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENACTIONUI_CONSTRUCTL_ENTRY );
    
    OstTraceFunctionExit0( CCALENACTIONUI_CONSTRUCTL_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::~CCalenActionUi
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenActionUi::~CCalenActionUi()
    {
    OstTraceFunctionEntry0( DUP1_CCALENACTIONUI_CCALENACTIONUI_ENTRY );
    
    delete iDeleteUi;
    delete iEditUi;   

    OstTraceFunctionExit0( DUP1_CCALENACTIONUI_CCALENACTIONUI_EXIT );
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::GetCommandHandlerL
// Gets a command handler
// ----------------------------------------------------------------------------
//
MCalenCommandHandler* CCalenActionUi::GetCommandHandlerL( TInt aCommand )
    {
    OstTraceFunctionEntry0( CCALENACTIONUI_GETCOMMANDHANDLERL_ENTRY );
    
    MCalenCommandHandler* handler = NULL;

    if ( aCommand >= ECalenEditCommandBase
      && aCommand < ECalenDeleteCommandBase )
        {
        if( !iEditUi )
            {
            iEditUi = CCalenEditUi::NewL( iController );
            }
        handler = iEditUi;
        }
    else if ( aCommand >= ECalenDeleteCommandBase
      && aCommand < ECalenSettingsCommandBase )
        {
        if( !iDeleteUi )
            {
            iDeleteUi = CalenDeleteUi::NewL( iController );
            }
        handler = iDeleteUi;
        }
    else if ( aCommand >= ECalenSettingsCommandBase
      && aCommand < ECalenMapCommandBase )
        {
        handler = this;
        }
    else if ( aCommand >= ECalenMapCommandBase
      && aCommand < ECalenLastCommand )
        {
        }
        
    OstTraceFunctionExit0( CCALENACTIONUI_GETCOMMANDHANDLERL_EXIT );
    return handler;
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::HandleCommandL
// Handles key presses.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenActionUi::HandleCommandL( const TCalenCommand& aCommand )
    {
    OstTraceFunctionEntry0( CCALENACTIONUI_HANDLECOMMANDL_ENTRY );
    
    TBool continueCommand(EFalse);
    
    if(aCommand.Command()==ECalenShowSettings)
        {  
        iController.ViewManager().launchSettingsView();
        }
    
    OstTraceFunctionExit0( CCALENACTIONUI_HANDLECOMMANDL_EXIT );
    return continueCommand; 
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::saveEntry
// save the entry from editor
// ----------------------------------------------------------------------------
//
void CCalenActionUi::saveAndCloseEditor()
    {
    iEditUi->saveAndCloseEditor();
    }
// End of file
