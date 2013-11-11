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

// Version : %version: 7 %


#include "mpxvideodrmhelper.h"
#include "mpxvideo_debug.h"

_LIT( KDrmRightsFile, "c:\\vhpp_drmrights.txt" );

CMpxVideoDrmHelper::CMpxVideoDrmHelper()
{
}

CMpxVideoDrmHelper::~CMpxVideoDrmHelper()
{
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
}

//  ------------------------------------------------------------------------------------------------
//    CMpxVideoDrmHelper::IsProtectedL()
//  ------------------------------------------------------------------------------------------------
//
TBool CMpxVideoDrmHelper::IsProtectedL( RFile& aFile )
{
    TBool drmProtected = EFalse;

    RFs fs;
    RFile file;

    fs.Connect();

    TInt openError = file.Open( fs, KDrmRightsFile, EFileRead | EFileShareAny );

    if ( openError == KErrNone )
    {
        drmProtected = ETrue;
        file.Close();
    }

    fs.Close();

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

    MPX_DEBUG(_L("CMpxVideoDrmHelper::GetDrmRightsStatus() drmError = %d"), drmError);

    RFs fs;
    RFile file;

    fs.Connect();

    TInt openError = file.Open( fs, KDrmRightsFile, EFileRead | EFileShareAny );

    if ( openError == KErrNone )
    {
        HBufC8* fBuf = HBufC8::NewL( 16 );
        TPtr8 fileBuf = fBuf->Des();

        file.Read( fileBuf, 16 );

        TLex8 number( fileBuf );
        number.Val( drmError );

        file.Close();
    }

    fs.Close();

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

    RFs fs;
    RFile64 file;

    fs.Connect();

    TInt openError = file.Open( fs, KDrmRightsFile, EFileRead | EFileShareAny );

    if ( openError == KErrNone )
    {
        drmProtected = ETrue;
        file.Close();
    }

    fs.Close();

    return drmProtected;
}

//  ------------------------------------------------------------------------------------------------
//    CMpxVideoDrmHelper::GetDrmRightsStatus64()
//  ------------------------------------------------------------------------------------------------
//
TInt CMpxVideoDrmHelper::GetDrmRightsStatus64( RFile64& aFile )
{
    MPX_ENTER_EXIT(_L("CMpxVideoDrmHelper::GetDrmRightsStatus()"));

    TInt drmError = KErrNone;

    MPX_DEBUG(_L("CMpxVideoDrmHelper::GetDrmRightsStatus() drmError = %d"), drmError);

    RFs fs;
    RFile64 file;

    fs.Connect();

    TInt openError = file.Open( fs, KDrmRightsFile, EFileRead | EFileShareAny );

    if ( openError == KErrNone )
    {
        HBufC8* fBuf = HBufC8::NewL( 16 );
        TPtr8 fileBuf = fBuf->Des();

        file.Read( fileBuf, 16 );

        TLex8 number( fileBuf );
        number.Val( drmError );

        file.Close();
    }

    fs.Close();

    return drmError;
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  EOF
