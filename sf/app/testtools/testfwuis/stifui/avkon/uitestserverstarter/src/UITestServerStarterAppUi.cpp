/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This module contains the implementation of 
* CUITestServerStarterAppUi class member functions.
*
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>

#include <UITestServerStarter.rsg>

#include "UITestServerStarter.hrh"
#include "UITestServerStarterAppUi.h"
#include "UITestServerStarterAppView.h"

#include "TestServerThreadStarter.h"
#include <stifinternal/TestThreadContainerRunnerFactory.h>
#include "AknUiEnvProxy.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: ConstructL

     Description: ConstructL is second phase of two-phased constructor.

     ConstructL is second phase of two-phased constructor.

     Parameters: None
     
     Return Values: None.

     Errors/Exceptions: Leaves if one of called functions leave.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::ConstructL()
	{
	RDebug::Printf( "UITESTING: CUITestServerStarterAppUi::ConstructL - Begin" );

	// Initialise app UI with standard value.
	BaseConstructL( ENoAppResourceFile || CAknAppUi::EAknEnableSkin );

	CEikonEnv::Static()->DisableExitChecks( true );
	
	// Create view object
	iAppView = CUITestServerStarterAppView::NewL();
	
	AddViewL( iAppView );
	SetDefaultViewL( *iAppView );
		
	CAknUiEnvProxy* uiEnvProxy = CAknUiEnvProxy::NewL( this );
	
	CTestThreadContainerRunnerFactory* testThreadContainerRunnerFactory = 
		CTestThreadContainerRunnerFactory::NewL( RThread().Id(), CActiveScheduler::Current(), uiEnvProxy );
	
	RDebug::Printf( "UITESTING: CUITestServerStarterAppUi::ConstructL - Starting TestServer" );
	CTestServerThreadStarter* testServerThreadStarter = CTestServerThreadStarter::NewL( );
	testServerThreadStarter->RunTestServerThread( testThreadContainerRunnerFactory );
		
	RDebug::Printf( "UITESTING: CUITestServerStarterAppUi::ConstructL - Move to background" );
	
	TApaTask task( CCoeEnv::Static()->WsSession() );
	task.SetWgId( CCoeEnv::Static()->RootWin().Identifier() ); 
	task.SendToBackground();
	
	RDebug::Printf( "UITESTING: CUITestServerStarterAppUi::ConstructL - End" );
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: CUITestServerStarterAppUi

     Description: C++ default constructor can NOT contain any code, that might leave.

     C++ default constructor can NOT contain any code, that might leave.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterAppUi::CUITestServerStarterAppUi()
	{
	// No implementation required
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: ~CUITestServerStarterAppUi

     Description: C++ destructor.

     C++ destructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CUITestServerStarterAppUi::~CUITestServerStarterAppUi()
	{
	// No implementation required
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: HandleCommandL

     Description: Handles user commands.

     Derived from CCoeAppUi. Handles user commands.

     Parameters: TInt aCommand: in: User command code.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::HandleCommandL( TInt )
	{
	// No implementation required
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: HandleWsEventL

     Description: Handles window server event.

     Derived from CCoeAppUi. Handles window server event.

     Parameters: const TWsEvent& aEvent: in: Event.
     			 CCoeControl* aDestination: in: Destination control.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination )
	{	
	CAknAppUi::HandleWsEventL( aEvent, aDestination );

	if ( ( aEvent.Type() == EEventKey ) )
		{
		switch( iEventType )
			{
			case EPressKey:
				{
				if ( iEventStatus != NULL )
					{
					KeyPressHandled();					
					}
				}
				break;
			case ETypeText:
				{
				iTypeTextLength--;
				if ( ( iEventStatus != NULL ) && ( iTypeTextLength == 0 ) )
					{
					TextTypeHandled();
					}
				}
				break;
			default: 
			    // Do nothing
			    break;
			}
		}
	else if ( aEvent.Type() == EEventPointer )
		{
		if ( iEventType == EPointerEvent )
			{
			PointerEventHandled();
			}		
		}
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: PrepareToTypeText

     Description: Prepares AppUi to recive type text event

     Prepares AppUi to recive type text event

     Parameters: TInt aTextLength: in: Text length.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::PrepareToTypeText( TInt aTextLength )
	{
	iTypeTextLength = aTextLength;
	iEventType = ETypeText;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: PrepareToPressKey

     Description: Prepares AppUi to recive key press event

     Prepares AppUi to recive key press event

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::PrepareToPressKey()
	{
	iEventType = EPressKey;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: PrepareToPointerEvent

     Description: Prepares AppUi to recive pointer event

     Prepares AppUi to recive key press event

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::PrepareToPointerEvent()
	{
	iEventType = EPointerEvent;
	}


/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: KeyPressHandled

     Description: Sends notification that key press was handled.

     Notifies client that key press was handled.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::KeyPressHandled()
	{
	if ( ( iEventType == EPressKey ) && ( iEventStatus != NULL ) && ( iEventType == EPressKey ) )
		{
		iEventType = ENone;
		User::RequestComplete( iEventStatus, KErrNone );
		iEventStatus = NULL;
		}
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: TextTypeHandled

     Description: Sends notification that type text was handled.

     Notifies client that type text was handled.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::TextTypeHandled()
	{
	if ( ( iEventType == ETypeText ) && ( iEventStatus != NULL ) && ( iEventType == ETypeText ) )
		{
		iEventType = ENone;
		iTypeTextLength = 0;
		User::RequestComplete( iEventStatus, KErrNone );
		iEventStatus = NULL;
		}
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: PointerEventHandled

     Description: Sends notification that pointer event was handled.

     Notifies client that pointer event was handled.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::PointerEventHandled()
	{
	if ( ( iEventType == EPointerEvent ) && ( iEventStatus != NULL ) )
		{
		iEventType = ENone;
		User::RequestComplete( iEventStatus, KErrNone );
		iEventStatus = NULL;
		}
	}


/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: NotifyAboutHandledKeyPress

     Description: Requests notification of press key completion.

     Requests notification of press key completion.

     Parameters: TRequestStatus* aStatus: in: Request status pointer.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::NotifyAboutHandledKeyPress( TRequestStatus* aStatus )
	{
	iEventStatus = aStatus;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: NotifyAboutHandledTextType

     Description: Requests notification when text type event is handled.

     Requests notification when text type event is handled.

     Parameters: TRequestStatus* aStatus: in: Request status pointer.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::NotifyAboutHandledTextType( TRequestStatus* aStatus )
	{
	iEventStatus = aStatus;
	}

/*
-------------------------------------------------------------------------------

     Class: CUITestServerStarterAppUi

     Method: NotifyAboutHandledPointerEvent

     Description: Requests notification when pointer event is handled.

     Requests notification when pointer event is handled.

     Parameters: TRequestStatus* aStatus: in: Request status pointer.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUITestServerStarterAppUi::NotifyAboutHandledPointerEvent( TRequestStatus* aStatus )
	{
	iEventStatus = aStatus;
	}

// End of File
