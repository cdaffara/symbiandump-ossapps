/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class handles the DRM functions for the playback plugin
*
*/


// Version : %version: 9 %


#include <caf/data.h>
#include <caf/caftypes.h>
#include <caf/content.h>
#include <Oma2Agent.h>

#include "mpxvideodrmhelper.h"
#include "mpxvideo_debug.h"

CMpxVideoDrmHelper::CMpxVideoDrmHelper()
{
}

CMpxVideoDrmHelper::~CMpxVideoDrmHelper()
{
    if ( iDrmUtility )
    {
        delete iDrmUtility;
        iDrmUtility = NULL;
    }
}


CMpxVideoDrmHelper* CMpxVideoDrmHelper::NewL()
{
    CMpxVideoDrmHelper* self = new( ELeave ) CMpxVideoDrmHelper();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

void CMpxVideoDrmHelper::ConstructL()
{
    iDrmUtility = DRM::CDrmUtility::NewL();
}

//  ------------------------------------------------------------------------------------------------
//    CMpxVideoDrmHelper::IsProtectedL()
//  ------------------------------------------------------------------------------------------------
//
TBool CMpxVideoDrmHelper::IsProtectedL( RFile& aFile )
{
    TBool drmProtected = EFalse;

    if ( aFile.SubSessionHandle() )
    {
        drmProtected = iDrmUtility->IsProtectedL( aFile );
    }

    MPX_DEBUG(_L("CMpxVideoDrmHelper::IsProtectedL(%d)"), drmProtected);

    return drmProtected;
}

//  ------------------------------------------------------------------------------------------------
//    CMpxVideoDrmHelper::GetDrmRightsStatus()
//  ------------------------------------------------------------------------------------------------
//
TInt CMpxVideoDrmHelper::GetDrmRightsStatus( RFile& aFile )
{
    MPX_ENTER_EXIT(_L("CMpxVideoDrmHelper::GetDrmRightsStatus()"));

    TInt drmError = KErrNone;
    TBool drmProtected = EFalse;

    MPX_TRAP( drmError, drmProtected = IsProtectedL(aFile ) );

    if ( drmProtected && (drmError == KErrNone) )
    {
        ContentAccess::CData* data = NULL;

        //
        //  Check if rights exist for clip
        //
        MPX_TRAP( drmError,
                  data = ContentAccess::CData::NewL( aFile,
                                                     ContentAccess::KDefaultContentObject,
                                                     ContentAccess::EPlay ) );

        if ( drmError == KErrNone )
        {
            drmError = data->EvaluateIntent( ContentAccess::EPlay );

            delete data;
        }
    }

    MPX_DEBUG(_L("CMpxVideoDrmHelper::GetDrmRightsStatus() drmError = %d"), drmError);

    return drmError;
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ------------------------------------------------------------------------------------------------
//    CMpxVideoDrmHelper::IsProtected64L()
//  ------------------------------------------------------------------------------------------------
//
TBool CMpxVideoDrmHelper::IsProtected64L( RFile64& aFile )
{
    TBool drmProtected = EFalse;

    if ( aFile.SubSessionHandle() )
    {
       drmProtected = iDrmUtility->IsProtectedL( aFile );
    }

    MPX_DEBUG(_L("CMpxVideoDrmHelper::IsProtected64L(%d)"), drmProtected);

    return drmProtected;
}

//  ------------------------------------------------------------------------------------------------
//    CMpxVideoDrmHelper::GetDrmRightsStatus64()
//  ------------------------------------------------------------------------------------------------
//
TInt CMpxVideoDrmHelper::GetDrmRightsStatus64( RFile64& aFile )
{
    MPX_ENTER_EXIT(_L("CMpxVideoDrmHelper::GetDrmRightsStatus64()"));

    TInt drmError = KErrNone;
    TBool drmProtected = EFalse;

    MPX_TRAP( drmError, drmProtected = IsProtected64L( aFile ) );

    if ( drmProtected && (drmError == KErrNone) )
    {
        ContentAccess::CData* data = NULL;

        //
        //  Check if rights exist for clip
        //
        MPX_TRAP( drmError,
                  data = ContentAccess::CData::NewL( aFile,
                                                     ContentAccess::KDefaultContentObject,
                                                     ContentAccess::EPlay ) );

        if ( drmError == KErrNone )
        {
            drmError = data->EvaluateIntent( ContentAccess::EPlay );

            delete data;
        }
    }

    MPX_DEBUG(_L("CMpxVideoDrmHelper::GetDrmRightsStatus64() drmError = %d"), drmError);

    return drmError;
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  EOF
