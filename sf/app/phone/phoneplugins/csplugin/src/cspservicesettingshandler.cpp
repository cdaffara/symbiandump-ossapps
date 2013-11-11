/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains the implementation of class CSPServiceSettingsHandler
*
*/


// CLASS HEADER
#include "cspservicesettingshandler.h"

// EXTERNAL INCLUDES
#include    <spsettings.h>
#include    <spentry.h>

// INTERNAL INCLUDES
#include "csplogger.h"


_LIT(KLineNameVoice, "CS");



CSPServiceSettingsHandler* CSPServiceSettingsHandler::NewL( )
    {
    CSPServiceSettingsHandler* self 
            = new ( ELeave ) CSPServiceSettingsHandler( );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor of the object.
// ---------------------------------------------------------------------------
//
CSPServiceSettingsHandler::~CSPServiceSettingsHandler( )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPServiceSettingsHandler::~CSPServiceSettingsHandler()" );
    }

// ---------------------------------------------------------------------------
// Finds service ID by name.
// ---------------------------------------------------------------------------
//
void CSPServiceSettingsHandler::FindByNameL( const TDesC& aName, 
                                            TUint32& aServiceId )
    {
    CSPLOGSTRING(CSPINT, "CSPServiceSettingsHandler::FindByNameL()" );
    CSPLOGSTRING2(CSPINT, "CSPServiceSettingsHandler::FindByNameL() Name=%S", &aName );
    CSPSettings* settingsApi = CSPSettings::NewLC();
    RIdArray idArray;
    CleanupClosePushL( idArray );
    TInt err = settingsApi->FindServiceIdsL( idArray );
    
    TBool found = EFalse;
    if ( KErrNone == err )
        {
        TInt idCount = idArray.Count();
        if( idCount > 0 )
            {   
            
            for ( TInt i = 0; !found  && i < idCount; i++ )
                {
                CSPEntry* entry = CSPEntry::NewLC();
                TInt ferr = settingsApi->FindEntryL( idArray[i], *entry );            
                
                if( ferr == KErrNone 
                    && aName.Compare( entry->GetServiceName() ) == 0 )
                    {
                    aServiceId = idArray[i];
                    found = ETrue;
                    }
                CleanupStack::PopAndDestroy( entry );
                }            
            }
        else
            {
            err = KErrNotFound;
            }
        }
    
    if ( !found )
        {
        err = KErrNotFound;
        }
    
    CleanupStack::PopAndDestroy( &idArray );
    CleanupStack::PopAndDestroy( settingsApi );
    CSPLOGSTRING3(CSPINT, "CSProvider::FindByNameL() err = %d serv_id = %u", 
                            err, aServiceId );
    User::LeaveIfError( err );
    }
    

    
TInt CSPServiceSettingsHandler::ReadCSServiceId( TUint32& aServiceId )
    {
    TRAPD( err, FindByNameL( KLineNameVoice, aServiceId ) );

    if ( err )
        {        
        CSPLOGSTRING2(CSPINT, 
            "CSPServiceSettingsHandler::FindByNameL() leave %d", 
            err );
        }    
    return err;
    }

// ---------------------------------------------------------------------------
// Constructs the requester.
// ---------------------------------------------------------------------------
//
CSPServiceSettingsHandler::CSPServiceSettingsHandler( )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPServiceSettingsHandler::CSPServiceSettingsHandler()" );
    }

// ---------------------------------------------------------------------------
// Constructs the requester in the second phase.
// ---------------------------------------------------------------------------
//
void CSPServiceSettingsHandler::ConstructL( ) 
    {
    }


