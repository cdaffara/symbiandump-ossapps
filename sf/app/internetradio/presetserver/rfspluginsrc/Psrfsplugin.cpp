/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Preset Server RFS ECOM plugin implementation
*
*/


#include "psdebug.h"
#include "psrfsplugin.h"
#include <bautils.h>

_LIT( KPSDatabaseFileName, "pspresets.db" );    // File name of the preset database.
_LIT( KPSDatabasePath, "\\private\\10281cb5\\" );            // File name of the preset database.

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSRfsPlugin* CPSRfsPlugin::NewL( TAny* aInitParams )
    {
    CPSRfsPlugin* self = new ( ELeave ) CPSRfsPlugin( aInitParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CPSRfsPlugin::CPSRfsPlugin( TAny* /*aInitParams*/ )
    {
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CPSRfsPlugin::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CPSRfsPlugin::~CPSRfsPlugin()
    {
    // Base class handles the ECOM cleanup.
    }

// ---------------------------------------------------------------------------
// From base class CRFSPlugin
// ---------------------------------------------------------------------------
//
void CPSRfsPlugin::RestoreFactorySettingsL( const TRfsReason aType )
    {
    PSDEBUG(" *** CPSRfsPlugin::RestoreFactorySettingsL - Enter" );
    if ( aType == ENormalRfs )
        {
        PSDEBUG(" *** CPSRfsPlugin::RestoreFactorySettingsL - Deleting Preset Server files" );

        TFileName databaseFullName( KPSDatabaseFileName );
         
        RFs fsSession;
        User::LeaveIfError( fsSession.Connect() );
        CleanupClosePushL( fsSession );
        
        TInt err = ResolveDrive( fsSession, databaseFullName, KPSDatabasePath() );
        
        if ( err == KErrNone )
            {
            err = fsSession.Delete( databaseFullName );
            PSDEBUG3(" *** CPSRfsPlugin::RestoreFactorySettingsL - Deleted preset database (file = %S, err = %d)", &databaseFullName, err );
            }
        else
            {
            PSDEBUG2(" *** CPSRfsPlugin::RestoreFactorySettingsL - err = %d)", err );
            }
        CleanupStack::PopAndDestroy( &fsSession );
        }
    else
        {
        PSDEBUG2(" *** CPSRfsPlugin::RestoreFactorySettingsL - RFS type (%d) not supported", static_cast<TInt>( aType ) );
        }
    PSDEBUG(" *** CPSRfsPlugin::RestoreFactorySettingsL - Exit" );
    }

// ---------------------------------------------------------------------------
// From base class CRFSPlugin
// ---------------------------------------------------------------------------
//
void CPSRfsPlugin::GetScriptL( const TRfsReason /*aType*/, TDes& aPath )
    {
    aPath.Zero();
    }

// ---------------------------------------------------------------------------
// From base class CRFSPlugin
// ---------------------------------------------------------------------------
//
void CPSRfsPlugin::ExecuteCustomCommandL( const TRfsReason /*aType*/, TDesC& /*aCommand*/ )
    {
    }

// ---------------------------------------------------------------------------
// ResolveDrive
// ---------------------------------------------------------------------------
//
TInt CPSRfsPlugin::ResolveDrive( RFs& aFs, TFileName& aFileName, const TFileName& aPath )
    {
    PSDEBUG3( "*** CPSRfsPlugin::ResolveDrive( aFileName = %S, aPath = %s )", &aFileName, &aPath );

    TFindFile finder( aFs );
    TInt err = finder.FindByDir( aFileName, aPath );
    if ( err )
        {
        PSDEBUG3( "*** CPSRfsPlugin::ResolveDrive - File '%S%S' not found!", &aPath, &aFileName );
        return KErrNotFound;
        }
    
    aFileName.Copy( finder.File() );
    PSDEBUG2("*** CPSRfsPlugin::ResolveDrive( aFileName = %S )", &aFileName );
    return KErrNone;
    }
// ======== GLOBAL FUNCTIONS ========

