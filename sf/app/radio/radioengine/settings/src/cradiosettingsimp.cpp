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
* Description:
*
*/

// System includes
#include <bautils.h>
#include <coemain.h>
#include <ConeResLoader.h>
#include <f32file.h>
#include <data_caging_path_literals.hrh>

// User includes
#include "cradiosettingsimp.h"
#include "cradioapplicationsettings.h"
#include "cradioenginesettings.h"
//#include "radioengineutils.h"
#include "cradioenginelogger.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSettingsImp* CRadioSettingsImp::NewL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioSettingsImp* self = new (ELeave) CRadioSettingsImp;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioSettingsImp::ConstructL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iFsSession = new ( ELeave ) RFs;
    User::LeaveIfError( iFsSession->Connect() );

    // Constructs the implementors of the interfaces.
    iApplicationSettings = CRadioApplicationSettings::NewL();
    iEngineSettings = CRadioEngineSettings::NewL( *this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSettingsImp::CRadioSettingsImp()
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSettingsImp::~CRadioSettingsImp()
    {
    LEVEL3( LOG_METHOD_AUTO );
    delete iEngineSettings;
    delete iApplicationSettings;
    if ( iFsSession )
        {
        iFsSession->Close();
        }
    delete iFsSession;
    }

// ---------------------------------------------------------------------------
// Determines if region is allowed.
// ---------------------------------------------------------------------------
//
TBool CRadioSettingsImp::IsRegionAllowed( TRadioRegion aRegionId ) const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iEngineSettings->IsRegionAllowed( aRegionId );
    }

// ---------------------------------------------------------------------------
// Returns the application settings interface.
// ---------------------------------------------------------------------------
//
MRadioApplicationSettings& CRadioSettingsImp::ApplicationSettings() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iApplicationSettings;
    }

// ---------------------------------------------------------------------------
// Returns the radio settings interface.
// ---------------------------------------------------------------------------
//
MRadioEngineSettings& CRadioSettingsImp::EngineSettings() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iEngineSettings;
    }

// ---------------------------------------------------------------------------
// Returns the radio settings setter interface.
// ---------------------------------------------------------------------------
//
MRadioSettingsSetter& CRadioSettingsImp::RadioSetter() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iEngineSettings;
    }

// ---------------------------------------------------------------------------
// Static version of ResolveDriveL.
// ---------------------------------------------------------------------------
//
void CRadioSettingsImp::ResolveDriveL( TFileName& aFileName, const TDesC& aPath )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aFileName = %S, aPath = %S", &aFileName, &aPath ) );

    TFileName fileName;
    TFileName baseResource;
    TFindFile finder( *iFsSession );
    TLanguage language( ELangNone );

    _LIT( resourceFileExt, ".rsc" );
    _LIT( resourceFileWildExt, ".r*" );

    TParsePtrC parse( aFileName );
    TBool isResourceFile = ( parse.Ext() == resourceFileExt() );

    TInt err = KErrUnknown;
    if  ( isResourceFile )
        {
        CDir* entries = NULL;
        fileName.Copy( parse.Name() );
        fileName.Append( resourceFileWildExt() );
        err = finder.FindWildByDir( fileName, aPath, entries );
        delete entries;
        }
    else
        {
        // TFindFile applies search order that is from
        // drive Y to A, then Z
        err = finder.FindByDir( aFileName, aPath );
        }

    LEVEL3( LOG_FORMAT( "err = %d", err ) );
    TBool found = EFalse;
    if ( !isResourceFile && ( KErrNone == err ) )
        {
        found = ETrue;
        aFileName.Zero();
        aFileName.Append( finder.File() );
        }

    while ( !found && ( KErrNone == err ) && isResourceFile )
        {
        // Found file
        fileName.Zero();
        TParsePtrC foundPath( finder.File() );
        fileName.Copy( foundPath.DriveAndPath() );
        fileName.Append( aFileName );
        BaflUtils::NearestLanguageFile( *iFsSession, fileName, language );
        if ( language != ELangNone && BaflUtils::FileExists( *iFsSession, fileName ) )
            {
            found = ETrue;
            aFileName.Zero();
            aFileName.Copy( fileName );
            }
        else
            {
            if ( language == ELangNone &&
                 !baseResource.Compare( KNullDesC ) &&
                 BaflUtils::FileExists( *iFsSession, fileName ) )
                {
                baseResource.Copy( fileName );
                }
            CDir* entries = NULL;
            err = finder.FindWild( entries );
            delete entries;
            }
        }

    if ( !found && baseResource.Compare( KNullDesC ) )
        {
        // If we found *.rsc then better to use that than nothing
        if ( BaflUtils::FileExists( *iFsSession, baseResource ) )
            {
            found = ETrue;
            aFileName.Zero();
            aFileName.Append( baseResource );
            }
        }

    if ( !found )
        {
        LEVEL3( LOG_FORMAT( "File %S not found ( err = %d )!", &aFileName, err ) );
        if ( !err )
            {
            err = KErrNotFound;
            }
        User::Leave( err );
        }

    LEVEL3( LOG_FORMAT( "aFileName = %S", &aFileName ) );
    }

// ---------------------------------------------------------------------------
// Returns the file server session
// ---------------------------------------------------------------------------
//
RFs& CRadioSettingsImp::FsSession()
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iFsSession;
    }
