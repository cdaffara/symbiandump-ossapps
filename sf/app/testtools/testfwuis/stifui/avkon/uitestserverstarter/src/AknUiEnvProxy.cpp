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
* CAknUiEnvProxy class member functions.
*
*/

// INCLUDE FILES
#include "AknUiEnvProxy.h"
#include "UITestServerStarterAppUi.h"
#include "EventUtil.h"

#include <coeaui.h>
#include <eikenv.h>
#include <apgtask.h>
#include <w32std.h>

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

     Class: CAknUiEnvProxy

     Method: NewL

     Description: NewL is first phase of two-phased constructor.

     NewL is first phase of two-phased constructor.

     Parameters: CUITestServerStarterAppUi* aAppUi: in: Pointer to CUITestServerStarterAppUi.
     
     Return Values: Pointer to new CAknUiEnvProxy object.

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CAknUiEnvProxy* CAknUiEnvProxy::NewL( CUITestServerStarterAppUi* aAppUi )
	{
	CAknUiEnvProxy* self = new(ELeave)CAknUiEnvProxy();
	CleanupStack::PushL( self );
	self->ConstructL( aAppUi );
	CleanupStack::Pop( self );
	return self;
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: ConstructL

     Description: ConstructL is second phase of two-phased constructor.

     Performs construction of CAknUiEnvProxy object.

     Parameters: CUITestServerStarterAppUi* aAppUi: in: Pointer to CUITestServerStarterAppUi.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CAknUiEnvProxy::ConstructL( CUITestServerStarterAppUi* aAppUi )
	{	
	iAppUi = aAppUi;
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: BringToForeground

     Description: Brings UI component container to foreground.

     Brings UI component container to foreground. 

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CAknUiEnvProxy::BringToForeground()
	{
	TApaTask task( CCoeEnv::Static()->WsSession() );
	task.SetWgId( CCoeEnv::Static()->RootWin().Identifier() );
	task.BringToForeground();	
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: SendToBackground

     Description: Sends UI component container to background.

     Sends UI component container to background.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CAknUiEnvProxy::SendToBackground(  )
	{
	TApaTask task( CCoeEnv::Static()->WsSession() );
	task.SetWgId( CCoeEnv::Static()->RootWin().Identifier() ); 
	task.SendToBackground();		
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: PressKeyL

     Description: Sends local key press event to UI component.

     Sends key press event to UI component. UI component must be first added to 
     AppUi stack to be able to recive this key event. 

     Parameters: TRequestStatus* aStatus: out: Pointer to request status used to notify that
     										   key event was recived.
     			 TUint aKeyCode: in: Key code.
     			 TInt aKeyScanCode: in: Key scan code.
     			 TUint aKeyModifiers: in: Key modifiers
     			 TInt aKeyRepeats: in: Key press repeats count.
     
     Return Values: None.

     Errors/Exceptions: Leaves if some error occurs during key event sending.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CAknUiEnvProxy::PressKeyL( TRequestStatus* aStatus, TUint aKeyCode, TInt aKeyScanCode, 
							   TUint aKeyModifiers, TInt aKeyRepeats )
	{
	// Fill key event structure
	TWsEvent wsEvent;
	wsEvent.SetType(EEventKey);
	TKeyEvent* keyEvent = wsEvent.Key();
	keyEvent->iCode = aKeyCode;
	keyEvent->iScanCode = aKeyScanCode;
	keyEvent->iModifiers = aKeyModifiers;
	keyEvent->iRepeats = aKeyRepeats;
		
	// Send info to AppUi that we are going to send key event
	iAppUi->PrepareToPressKey();
	
	RWsSession& wsSession = CCoeEnv::Static()->WsSession();
	TInt wgId = CCoeEnv::Static()->RootWin().Identifier();	
	// Send key event
	TInt ret = wsSession.SendEventToWindowGroup( wgId, wsEvent );
	User::LeaveIfError( ret );
	
	wsSession.Flush();
	
	// Request notification when key press is handled
	*aStatus = KRequestPending;
	iAppUi->NotifyAboutHandledKeyPress( aStatus );
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: PressKeyL

     Description: Sends global key press event to UI component.

     Sends key press event to UI component which is currently focuused. 

     Parameters: TUint aKeyCode: in: Key code.
     			 TInt aKeyScanCode: in: Key scan code.
     			 TUint aKeyModifiers: in: Key modifiers
     			 TInt aKeyRepeats: in: Key press repeats count.
     
     Return Values: None.

     Errors/Exceptions: Leaves if some error occurs during key event sending.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CAknUiEnvProxy::PressKeyL( TUint aKeyCode, TInt aKeyScanCode, 
							   TUint aKeyModifiers, TInt aKeyRepeats )
	{	
	// Fill key event structure
	TWsEvent wsEvent;
	wsEvent.SetType(EEventKey);
	TKeyEvent* keyEvent = wsEvent.Key();
	keyEvent->iCode = aKeyCode;
	keyEvent->iScanCode = aKeyScanCode;
	keyEvent->iModifiers = aKeyModifiers;
	keyEvent->iRepeats = aKeyRepeats;
		
	RWsSession& wsSession = CCoeEnv::Static()->WsSession();	
	TInt wgId = wsSession.GetFocusWindowGroup();	
	TInt ret = wsSession.SendEventToWindowGroup( wgId, wsEvent );	
	User::LeaveIfError( ret );
	wsSession.Flush();
	}


/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: TypeTextL

     Description: Sends text to UI component.

     Sends text to UI component. UI component must be first added to 
     AppUi stack to be able to recive this key event. 

     Parameters: TRequestStatus* aStatus: out: Pointer to request status used to notify that
     										   key event was recived.
    			 TPtrC aText: in: Text which will be send to UI component. 
     
     Return Values: None.

     Errors/Exceptions: Leaves if some error occurs during key event sending.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CAknUiEnvProxy::TypeTextL( TRequestStatus* aStatus, const TDesC& aText )
	{
	TLex textParser( aText );

	// Send info to AppUi that we are going to send text
	iAppUi->PrepareToTypeText( aText.Length() );
	
	TInt wgId = CCoeEnv::Static()->RootWin().Identifier();
	RWsSession& wsSession = CCoeEnv::Static()->WsSession();

	TChar character = 0;
	while ( ( character = textParser.Get() ) != 0 )
		{
		// Fill key event structure
		TWsEvent wsEvent;
		wsEvent.SetType(EEventKey);
		TKeyEvent* keyEvent = wsEvent.Key();
		keyEvent->iCode = character;
		keyEvent->iScanCode = 0;
		keyEvent->iModifiers = 0;
		keyEvent->iRepeats = 0;
		
		// Send single character from text to UI component
		TInt ret = wsSession.SendEventToWindowGroup( wgId, wsEvent );
		User::LeaveIfError( ret );
		wsSession.Flush();
		}
		
	// Request notification when send text is recived
	*aStatus = KRequestPending;
	iAppUi->NotifyAboutHandledTextType( aStatus );	
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: TypeTextL

     Description: Sends global text to UI component.

     Sends global text to UI component. UI component must be focused to receive that event. 

     Parameters: TPtrC aText: in: Text which will be send to UI component. 
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CAknUiEnvProxy::TypeTextL( const TDesC& aText )
	{
	TLex textParser( aText );

	RWsSession& wsSession = CCoeEnv::Static()->WsSession();
	TInt wgId = wsSession.GetFocusWindowGroup();

	TChar character = 0;	
	while ( ( character = textParser.Get() ) != 0 )
		{
		// Fill key event structure
		TWsEvent wsEvent;
		wsEvent.SetType(EEventKey);
		TKeyEvent* keyEvent = wsEvent.Key();
		keyEvent->iCode = character;
		keyEvent->iScanCode = 0;
		keyEvent->iModifiers = 0;
		keyEvent->iRepeats = 0;
		
		// Send single character from text to UI component
		TInt ret = wsSession.SendEventToWindowGroup( wgId, wsEvent );			
		User::LeaveIfError( ret );
		wsSession.Flush();
		}
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: SendPointerEventL

     Description: Send pointer event

     Send pointer event.

     Parameters: TUint aType: in: KEvent type.
     			 const TPoint& aPosition: in: Position.
     			      
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CAknUiEnvProxy::SendPointerEventL( TUint aType, const TPoint& aPosition )
	{
	RWsSession& wsSession = CCoeEnv::Static()->WsSession();
	TRawEvent pointerEvent;

	if ( aType < TEventUtil::EButton1 ) {
		pointerEvent.Set( ( TRawEvent::TType )aType, aPosition.iX, aPosition.iY );
		wsSession.SimulateRawEvent( pointerEvent );
		} 
	else if ( ( aType >= TEventUtil::EButton1 ) && ( aType <= TEventUtil::EButton3 ) )
		{
		TRawEvent pointerEventDown;
		TRawEvent pointerEventUp;
		
		switch ( aType )
			{
			case TEventUtil::EButton1:
				{
				pointerEventDown.Set( TRawEvent::EButton1Down, aPosition.iX, aPosition.iY );
				pointerEventUp.Set( TRawEvent::EButton1Up, aPosition.iX, aPosition.iY );
				}
				break;
			case TEventUtil::EButton2:
				{
				pointerEventDown.Set( TRawEvent::EButton2Down, aPosition.iX, aPosition.iY );
				pointerEventUp.Set( TRawEvent::EButton2Up, aPosition.iX, aPosition.iY );
				}
				break;
			case TEventUtil::EButton3:
				{
				pointerEventDown.Set( TRawEvent::EButton3Down, aPosition.iX, aPosition.iY );
				pointerEventUp.Set( TRawEvent::EButton3Up, aPosition.iX, aPosition.iY );
				}
				break;
			default:
				User::Leave( KErrArgument );
			}
		wsSession.SimulateRawEvent( pointerEventDown );
		wsSession.Flush();
		wsSession.SimulateRawEvent( pointerEventUp );
		wsSession.Flush();
		}
	else
		{
		User::Leave( KErrArgument );
		}	
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: SendPointerEventL

     Description: Send pointer event

     Send pointer event.

     Parameters: TRequestStatus* aStatus: in: Pointer to request status used to notify that
     										  pointer event was recived.
     			 TUint aType: in: KEvent type.
     			 const TPoint& aPosition: in: Position.
     			      
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CAknUiEnvProxy::SendPointerEventL( TRequestStatus* aStatus, TUint aType, const TPoint& aPosition )
	{
	RWsSession& wsSession = CCoeEnv::Static()->WsSession();

	if ( aType < TEventUtil::EButton1 ) {
		TRawEvent pointerEvent;
		pointerEvent.Set( ( TRawEvent::TType )aType, aPosition.iX, aPosition.iY );
		// Send info to AppUi that we are going to send key event
		iAppUi->PrepareToPointerEvent();
		wsSession.SimulateRawEvent( pointerEvent );
		// Request notification when key press is handled
		*aStatus = KRequestPending;
		iAppUi->NotifyAboutHandledPointerEvent( aStatus );	
		} 
	else if ( ( aType >= TEventUtil::EButton1 ) && ( aType <= TEventUtil::EButton3 ) )
		{
		TRawEvent pointerEventDown;
		TRawEvent pointerEventUp;
		switch ( aType )
			{
			case TEventUtil::EButton1:
				{
				pointerEventDown.Set( TRawEvent::EButton1Down, aPosition.iX, aPosition.iY );
				pointerEventUp.Set( TRawEvent::EButton1Up, aPosition.iX, aPosition.iY );
				}
				break;
			case TEventUtil::EButton2:
				{
				pointerEventDown.Set( TRawEvent::EButton2Down, aPosition.iX, aPosition.iY );
				pointerEventUp.Set( TRawEvent::EButton2Up, aPosition.iX, aPosition.iY );
				}
				break;
			case TEventUtil::EButton3:
				{
				pointerEventDown.Set( TRawEvent::EButton3Down, aPosition.iX, aPosition.iY );
				pointerEventUp.Set( TRawEvent::EButton3Up, aPosition.iX, aPosition.iY );
				}
				break;
			default:
				User::Leave( KErrArgument );
			}
		
			CCoeEnv::Static()->RootWin().SimulatePointerEvent( pointerEventDown );
			wsSession.Flush();
			
			// Send info to AppUi that we are going to send key event
			iAppUi->PrepareToPointerEvent();
	
			CCoeEnv::Static()->RootWin().SimulatePointerEvent( pointerEventUp );
			wsSession.Flush();
	
			// Request notification when key press is handled
			*aStatus = KRequestPending;
			iAppUi->NotifyAboutHandledPointerEvent( aStatus );	
		} 
	else
		{
		User::Leave( KErrArgument );
		}
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: ParseKeyCode

     Description: Parses key code.

     Parses key code.

     Parameters: TDesC& aKeyCodeName: in: Key code name.
     			 TUint& aKeyCode: out: Parsed key code.
     
     Return Values: KErrNone if no error occures during parsing.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CAknUiEnvProxy::ParseKeyCode( const TDesC& aKeyCodeName, TUint& aKeyCode ) const
	{
	return TEventUtil::ParseKeyCode( aKeyCodeName, aKeyCode );
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: ParseKeyScanCode

     Description: Parses key scan code.

     Parses key scan code.

     Parameters: TDesC& aKeyScanCodeName: in: Key scan code name.
     			 TUint& aKeyScanCode: out: Parsed key scan code.
     
     Return Values: KErrNone if no error occures during parsing.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CAknUiEnvProxy::ParseKeyScanCode( const TDesC& aKeyScanCodeName, TInt& aKeyScanCode ) const
	{
	return TEventUtil::ParseKeyScanCode( aKeyScanCodeName, aKeyScanCode );
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: ParseModifier

     Description: Parses key modifier.

     Parses key modifier.

     Parameters: TDesC& aModifierName: in: Key modifier.
     			 TUint& aModifier: out: Parsed key modifier.
     
     Return Values: KErrNone if no error occures during parsing.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CAknUiEnvProxy::ParseModifier( const TDesC& aModifierName, TUint& aModifier ) const
	{
	return TEventUtil::ParseModifier( aModifierName, aModifier );
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: ParsePointerEventType

     Description: Parses pointer event type.

     Parses key modifier.

     Parameters: const TDesC& aPointerEventTypeName: in: Pointer event type.
     			 TUint& aModifier: out: Parsed pointer event type.
     
     Return Values: KErrNone if no error occures during parsing.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CAknUiEnvProxy::ParsePointerEventType( const TDesC& aPointerEventTypeName, TUint& aPointerEventType ) const
	{
	return TEventUtil::ParsePointerEventType( aPointerEventTypeName, aPointerEventType );
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: CAknUiEnvProxy

     Description: C++ constructor.

     C++ constructor.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CAknUiEnvProxy::CAknUiEnvProxy()
	{	
	}

/*
-------------------------------------------------------------------------------

     Class: CAknUiEnvProxy

     Method: ~CAknUiEnvProxy

     Description: C++ destructor.

     C++ destructor.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CAknUiEnvProxy::~CAknUiEnvProxy()
	{	
	}

// End of File
