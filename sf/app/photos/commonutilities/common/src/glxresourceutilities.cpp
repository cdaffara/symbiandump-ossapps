/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility for loading resource files
*
*/



//  CLASS HEADER
#include "glxresourceutilities.h"
    
//  EXTERNAL INCLUDES
#include <bautils.h>
#include <AknUtils.h>
#include <f32file.h>

//  INTERNAL INCLUDES

// ---------------------------------------------------------------------------
// GetResourceFilenameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxResourceUtilities::GetResourceFilenameL( TFileName& aResFile )
    {
	RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    CGlxResourceUtilities::GetResourceFilenameL( aResFile, fs );
    CleanupStack::PopAndDestroy( &fs );
    }

// ---------------------------------------------------------------------------
// GetResourceFilenameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxResourceUtilities::GetResourceFilenameL( TFileName& aResFile, RFs& aFs )
    {
    // don't use AknUtils CompleteWithAppPath  
    aResFile.Insert( 0, TDriveUnit( EDriveC).Name() );
	// try to locate the localised resource
    BaflUtils::NearestLanguageFile( aFs, aResFile );
    // if the localised resource is found, the file name is changed to 
    // the localised name (z:\apps\resources\xxx.r001)
    if( !BaflUtils::FileExists( aFs, aResFile ) )
        {
        // not found on c drive, try z
        aResFile.Replace( 0, KMaxDriveName, TDriveUnit( EDriveZ ).Name() );
    	// try to locate the localised resource again
        BaflUtils::NearestLanguageFile( aFs, aResFile );
        // if file was not found this time, there is no localised 
        // resource with the name
        if (!BaflUtils::FileExists( aFs, aResFile ) )
            {
            User::Leave( KErrNotFound );
            }
        }
    }


