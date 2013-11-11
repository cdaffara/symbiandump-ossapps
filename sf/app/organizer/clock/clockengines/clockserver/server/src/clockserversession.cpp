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
* Description:   The source file of the CClkSrvSession class.
*
*/

// System includes

// User includes
#include "clockserver.h"
#include "clockserversession.h"
#include "clockservercmds.h"
#include "clockserverimpl.h"
#include "clock_debug.h"
#include "clocktimesourceinterface.hrh"

// Constants
const TInt KZerothArgument( 0 );
const TInt KFirstArgument( 1 );

// ---------------------------------------------------------
// CClkSrvSession::CClkSrvSession
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkSrvSession::CClkSrvSession( CClkSrvImpl* aClkSrvImpl ) : CSession2(), iClkSrvImpl( aClkSrvImpl )
	{
	__PRINTS( "CClkSrvSession::CClkSrvSession - Entry" );
	
	// No implementation yet
	
	__PRINTS( "CClkSrvSession::CClkSrvSession - Exit" );
	}
	
// ---------------------------------------------------------
// CClkSrvSession::~CClkSrvSession
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkSrvSession::~CClkSrvSession()
	{
	__PRINTS( "CClkSrvSession::~CClkSrvSession - Entry" );
	
	if( NULL != iSessionLink.iNext )
        {
        iSessionLink.Deque();
        }

	__PRINTS( "CClkSrvSession::~CClkSrvSession - Exit" );
	
	}
	
// ---------------------------------------------------------
// CClkSrvSession::ServiceL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkSrvSession::ServiceL( const RMessage2& aMessage )
	{
	__PRINTS( "CClkSrvSession::ServiceL - Entry" );
	
	TInt returnValue( KErrNone );
	
	iMessageComplete = ETrue;
	
	// Do the actual service
	TRAPD( error, returnValue = DoServiceL( aMessage ) );
    
    if( iMessageComplete || ( KErrNone > error ) )
    	{
    	// Set the message as completed.
    	aMessage.Complete( ( error == KErrNone ) ? returnValue : error );
    	}
        
    __PRINTS( "CClkSrvSession::ServiceL - Exit" );
	}
	
// ---------------------------------------------------------
// CClkSrvSession::DoServiceL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvSession::DoServiceL( const RMessage2& aMessage )
	{
	__PRINTS( "CClkSrvSession::DoServiceL - Entry" );
	
	TInt returnValue( KErrNone );
	
	// Switch on the type of service request.
	switch( aMessage.Function() )
        {
        case EClkSrvActivateProtocol:
        	{
        	returnValue = DoActivateProtocolL( aMessage );
        	}
            break;
            
        case EClkSrvIsProtocolActive:
        	{
			returnValue = DoCheckProtocolActiveL( aMessage );
        	}
            break;
            
        case EClkSrvDeactivateProtocol:
        	{
        	returnValue = DoDeActivateProtocolL( aMessage );
        	}
            break;
            
        case EClkSrvGetProtocolInfo:
        	{
        	returnValue = DoGetProtocolInfoL( aMessage );
        	}
        	break;
        	
        case EClkSrvGetCurrentMcc:
        	{
        	returnValue = DoGetCurrentMccL( aMessage );
        	}
        	break;
        	
        case EClkSrvGetCurrentTimeZoneId:
        	{
        	returnValue = DoGetCurrentTimeZoneIdL( aMessage );
        	}
        	break;
        	
        case EClkSrvNotifyOnChange:
        	{
        	DoNotifyOnChange( aMessage );
        	}
        	break;
        	
        case EClkSrvCancelNotifyOnChange:
            {
        	NotifyAboutChange( EComponentNone, KErrNone, KErrCancel );
        	}
        	break;
        	
        case EClkSrvIsAutoTimeUpdadeOn:
            {
            returnValue = DoCheckAutoTimeUpdateOn( aMessage );
            }
            break;
        	
		default:
			{
			returnValue = KErrNotSupported;
			}         
			break;
        }
    
	__PRINTS( "CClkSrvSession::DoServiceL - Exit" );
	
	return returnValue;
	}

// ---------------------------------------------------------
// CClkSrvSession::LinkOffset()
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
TInt CClkSrvSession::LinkOffset()
	{
	__PRINTS( "CClkSrvSession::LinkOffset - Entry" );
	
	__PRINTS( "CClkSrvSession::LinkOffset - Exit" );
	
	return _FOFF( CClkSrvSession, iSessionLink );
	}
	
// ---------------------------------------------------------
// CClkSrvSession::DoActivateProtocol()
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
TInt CClkSrvSession::DoActivateProtocolL( const RMessage2& /*aMessage*/ )
	{
	__PRINTS( "CClkSrvSession::DoActivateProtocolL - Entry" );
	
	// Activate all the protocols.
	TInt returnVal = iClkSrvImpl->ActivateAllProtocolsL();
	
	__PRINTS( "CClkSrvSession::DoActivateProtocolL - Exit" );
	
	return returnVal;
	}

// ---------------------------------------------------------
// CClkSrvSession::DoCheckProtocolActive()
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
TInt CClkSrvSession::DoCheckProtocolActiveL( const RMessage2& aMessage )
	{
	__PRINTS( "CClkSrvSession::DoCheckProtocolActiveL - Entry" );
	
	TBool protocolActive( EFalse );
	
	// Check if protocol is active.
	TInt returnVal = iClkSrvImpl->IsProtocolActive( protocolActive, aMessage.Int0() );
	
	if( KErrNone == returnVal )
		{
		TPckgBuf< TBool > packageBuf( protocolActive );
		
		// Return the value.
		aMessage.WriteL( KFirstArgument, packageBuf, NULL );
		}
	
	__PRINTS( "CClkSrvSession::DoCheckProtocolActiveL - Exit" );
	
	return returnVal;
	}

// ---------------------------------------------------------
// CClkSrvSession::DoDeActivateProtocol()
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
TInt CClkSrvSession::DoDeActivateProtocolL( const RMessage2& /*aMessage*/ )
	{
	__PRINTS( "CClkSrvSession::DoDeActivateProtocol - Entry" );
	
	// Deactivate all the protocols.
	TInt returnVal = iClkSrvImpl->DeActivateAllProtocolsL();
	
	__PRINTS( "CClkSrvSession::DoDeActivateProtocol - Exit" );
	
	return returnVal;
	}

// ---------------------------------------------------------
// CClkSrvSession::DoGetProtocolInfoL()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvSession::DoGetProtocolInfoL( const RMessage2& aMessage )
	{
	__PRINTS( "CClkSrvSession::DoGetProtocolInfoL - Entry" );
	
	STimeAttributes timeInformation;
		
	// Ask the implementation for the time information.
	TInt returnVal = iClkSrvImpl->GetProtocolInformationL( timeInformation );

	if( KErrNone == returnVal )
		{
		TPckg< STimeAttributes > packageBuf( timeInformation );

		// Write the result.
		aMessage.WriteL( KFirstArgument, packageBuf, NULL );
		}
	
	__PRINTS( "CClkSrvSession::DoGetProtocolInfoL - Exit" );
	
	return returnVal;
	}

// ---------------------------------------------------------
// CClkSrvSession::DoGetCurrentMccL()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvSession::DoGetCurrentMccL( const RMessage2& aMessage )
	{
	__PRINTS( "CClkSrvSession::DoGetCurrentMccL - Entry" );
	
	TInt currentMcc( 0 ); 
	
	// Ask the implementation for the mcc.
	TInt returnVal = iClkSrvImpl->GetCurrentMccL( currentMcc );
	
	if( KErrNone == returnVal )
		{
		TPckg< TInt > packageBuf( currentMcc );
		
		// Write the result
		aMessage.WriteL( KZerothArgument, packageBuf, NULL );
		}
	
	__PRINTS( "CClkSrvSession::DoGetCurrentMccL - Exit" );
	
	return returnVal;
	}

// ---------------------------------------------------------
// CClkSrvSession::DoGetCurrentMccL()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkSrvSession::DoGetCurrentTimeZoneIdL( const RMessage2& aMessage )
	{
	__PRINTS( "CClkSrvSession::DoGetCurrentTimeZoneIdL - Entry" );
	
	TInt currentTimeZoneId( -1 );
	
	TInt returnVal = iClkSrvImpl->GetCurrentTimeZoneIdL( currentTimeZoneId );
	
	__PRINT( "currentTimeZoneId in DoGetCurrentTimeZoneIdL : %d", currentTimeZoneId );
	
	if( KErrNone == returnVal )
		{
		TPckg< TInt > packageBuf( currentTimeZoneId );
		
		// Write the result
		aMessage.WriteL( KZerothArgument, packageBuf, NULL );
		}
	
	__PRINTS( "CClkSrvSession::DoGetCurrentTimeZoneIdL - Exit" );
	
	return returnVal;
	}

// ---------------------------------------------------------
// CClkSrvSession::DoNotifyOnChange()
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClkSrvSession::DoNotifyOnChange( const RMessage2& aMessage )
	{
	__PRINTS( "CClkSrvSession::DoNotifyOnChange - Entry" );
	
	// First store the message for notification later.
	SetNotifyMessagePtr( aMessage );
	
	if( FirstNotifyRequest() )
        {
        NotifyAboutChange( EComponentNone, 0, KErrNone );
        }
        
    iMessageComplete = EFalse;
	
	__PRINTS( "CClkSrvSession::DoNotifyOnChange - Exit" );
	}

// ---------------------------------------------------------
// CClkSrvSession::DoCheckAutoTimeUpdateOn
// rest of the details are commented in the header
// ---------------------------------------------------------
//  
TInt CClkSrvSession::DoCheckAutoTimeUpdateOn( const RMessage2& aMessage )
    {
    __PRINTS( "CClkSrvSession::DoCheckAutoTimeUpdateOn - Entry" );
        
    TBool autoTimeUpdateOn( EFalse );

    // Check if any protocol is active.
    TInt returnVal = iClkSrvImpl->IsAutoTimeUpdateOn( autoTimeUpdateOn );

    if( KErrNone == returnVal )
        {
        TPckgBuf< TBool > packageBuf( autoTimeUpdateOn );

        // Return the value.
        aMessage.WriteL( KZerothArgument, packageBuf, NULL );
        }

    __PRINTS( "CClkSrvSession::DoCheckAutoTimeUpdateOn - Exit" );

    return returnVal;
    }
	
// ---------------------------------------------------------
// CClkSrvSession::SetNotifyMessagePtr()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkSrvSession::SetNotifyMessagePtr( const RMessage2& aMessage )
    {
    __PRINTS( "CClkSrvSession::SetNotifyMessagePtr - Entry" );
    
    // __ASSERT_ALWAYS( NotifyPending() == EFalse, PanicClient( aMessage, ENitzMdlNotifyPending ) );
    
    // Store the message for notifying later.
    iNotifyChangeMsg = aMessage;
    
    // Reset the flags.
    iMsgPendingFlags = 0;
    // Change the state to msg pending.
    iMsgPendingFlags |= ENotifyMsgPending;
    
    __PRINTS( "CClkSrvSession::SetNotifyMessagePtr - Exit" );
    }	
	
// ---------------------------------------------------------
// CClkSrvSession::NotifyAboutChange()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkSrvSession::NotifyAboutChange( TInt32 aWhatChanged, TInt32 aWhoChanged, TInt aNotification )
    {
    __PRINTS( "CClkSrvSession::NotifyAboutChange - Entry" );
    
    if( NotifyPending() )
        {
        __PRINTS( "CClkSrvSession::NotifyAboutChange - Notification was pending" );
        
        // Reset the flags.
        iMsgPendingFlags &= ~( ENotifyMsgPending );
        
        TPckg< TInt32 > whatChangedPkg( aWhatChanged );
        TPckg< TInt32 > whoChangedPkg( aWhoChanged );
        
        // Write the result
        TRAP_IGNORE( iNotifyChangeMsg.WriteL( KZerothArgument, whatChangedPkg ) );
        
        __PRINTS( "CClkSrvSession::NotifyAboutChange - 0th WriteL didn't leave" );
        
        TRAP_IGNORE( iNotifyChangeMsg.WriteL( KFirstArgument, whoChangedPkg ) );
        
        __PRINTS( "CClkSrvSession::NotifyAboutChange - 1st WriteL didn't leave" );
        
        // Mark the message as complete.
        iNotifyChangeMsg.Complete( aNotification );
        }
    
    __PRINTS( "CClkSrvSession::NotifyAboutChange - Notification was not pending" );
    
    __PRINTS( "CClkSrvSession::NotifyAboutChange - Exit" );
    }

// ---------------------------------------------------------
// CClkSrvSession::FirstNotifyRequest()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClkSrvSession::FirstNotifyRequest() const
    {
    __PRINTS( "CClkSrvSession::FirstNotifyRequest - Entry" );
    
    __PRINTS( "CClkSrvSession::FirstNotifyRequest - Exit" );
    
    return ( iMsgPendingFlags & EFirstChangeMsg );
    }

// ---------------------------------------------------------
// CClkSrvSession::FirstNotifyRequest()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClkSrvSession::NotifyPending() const
    {
    __PRINTS( "CClkSrvSession::NotifyPending - Entry" );
    
    __PRINT( "Notification Pending?: %d", iMsgPendingFlags & ENotifyMsgPending );
    
    __PRINTS( "CClkSrvSession::NotifyPending - Exit" );
    
    return ( iMsgPendingFlags & ENotifyMsgPending );
    }
    	
// End of file
