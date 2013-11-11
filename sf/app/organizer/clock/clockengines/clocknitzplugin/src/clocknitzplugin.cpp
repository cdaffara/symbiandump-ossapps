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
* Description:   The source file of the CClockNitzPlugin class.
*
*/

// User includes
#include "clocknitzplugin.h"
#include "clocknitzpluginimpl.h"
#include "clockpluginobserver.h"
#include "clock_debug.h"

// ---------------------------------------------------------
// CClockNitzPlugin::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockNitzPlugin* CClockNitzPlugin::NewL()
	{
	__PRINTS( "CClockNitzPlugin::NewL - Entry" );
    
    CClockNitzPlugin* self = new ( ELeave ) CClockNitzPlugin;
	CleanupStack::PushL( self );
	
	self->ConstructL();
	
	CleanupStack::Pop();
	
	__PRINTS( "CClockNitzPlugin::NewL - Exit" );
	
	return self;
	}
	
// ---------------------------------------------------------
// CClockNitzPlugin::~CClockNitzPlugin
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockNitzPlugin::~CClockNitzPlugin()
	{
	__PRINTS( "CClockNitzPlugin::~CClockNitzPlugin - Entry" );
	
	if( iNitzPluginImpl )
		{
		delete iNitzPluginImpl;
		iNitzPluginImpl = NULL;
		}
	
	__PRINTS( "CClockNitzPlugin::~CClockNitzPlugin - Exit" );
	}
	
// ---------------------------------------------------------
// CClockNitzPlugin::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzPlugin::ConstructL()
	{
	__PRINTS( "CClockNitzPlugin::ConstructL - Entry" );
	
	if( !iNitzPluginImpl )
		{
		iNitzPluginImpl = CClockNitzPluginImpl::NewL( this );
		}
	
	__PRINTS( "CClockNitzPlugin::ConstructL - Exit" );
	}
	
// ---------------------------------------------------------
// CClockNitzPlugin::GetTimeInformationL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzPlugin::GetTimeInformationL( TInterfaceAttribute aInterfaceAttribute,
                                          	STimeAttributes* aTimeAttributes )
	{
	__PRINTS( "CClockNitzPlugin::GetTimeInformationL - Entry" );
	
	__PRINT( "TInterfaceAttribute: %d", aInterfaceAttribute );
	
	if( iNitzPluginImpl )
	    {
    	// Get the time information from the plugin implementation
    	iNitzPluginImpl->GetTimeInformationL( aInterfaceAttribute, aTimeAttributes );
	    }
	
	__PRINTS( "CClockNitzPlugin::GetTimeInformationL - Exit" );
	}
 

// ---------------------------------------------------------
// CClockNitzPlugin::NotifyOnTimeChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockNitzPlugin::NotifyOnTimeChange( TInterfaceAttribute aInterfaceAttribute )
	{
	__PRINTS( "CClockNitzPlugin::NotifyOnTimeChange - Entry" );
	
	__PRINT( "TInterfaceAttribute: %d", aInterfaceAttribute );
	
	if( iNitzPluginImpl )
	    {
	    // Subscribe with the NITZ listener for notifications
	    iNitzPluginImpl->NotifyOnNITZInfoChange( aInterfaceAttribute );
	    }
	
	__PRINTS( "CClockNitzPlugin::NotifyOnTimeChange - Exit" );
	}

// ---------------------------------------------------------
// CClockNitzPlugin::GetPluginUid
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
TInt CClockNitzPlugin::GetPluginUid()
	{
	__PRINTS( "CClockNitzPlugin::GetPluginUid - Entry" );
	
	__PRINT( "Plugin id: %x", KNitzPluginId );
	
	__PRINTS( "CClockNitzPlugin::GetPluginUid - Exit" );
	
	return KNitzPluginId;
	}

// ---------------------------------------------------------
// CClockNitzPlugin::NotifyNITZInfoChangeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClockNitzPlugin::NotifyNITZInfoChangeL()
	{
	__PRINTS( "CClockNitzPlugin::NotifyNITZInfoChangeL - Entry" );
	
	if( iObserver )
		{
		// Notify the observer that the time has been changed
		iObserver->NotifyTimeChangeL( *this );
		}
	
	__PRINTS( "CClockNitzPlugin::NotifyNITZInfoChangeL - Exit" );
	}

// End of file
