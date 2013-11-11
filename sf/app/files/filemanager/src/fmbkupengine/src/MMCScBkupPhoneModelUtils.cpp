/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: MMCScBkupPhoneModelUtils implementation
*
*
*/

#include "MMCScBkupPhoneModelUtils.h"

// User includes
#include "MMCScBkupArchiveFlags.h"
#include "MMCScBkupArchiveUtils.h"

// System includes
#include <sysutil.h>



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// MMCScBkupPhoneModelUtils::CurrentPhoneModelLC()
// 
// 
// ---------------------------------------------------------------------------
HBufC8* MMCScBkupPhoneModelUtils::CurrentPhoneModelLC()
    {
    HBufC8* ret = HBufC8::NewLC( MaximumPhoneModelIdentifierLength() );
    //
    TBuf<KSysUtilVersionTextLength> version;
    const TInt thirdLine(3);
    const TInt err = SysUtil::GetSWVersion( version);
    
    if  ( err == KErrNone )
        {
        // Assume syntax Vxx.xx\ndd-mm-yy\nNHL-vvv\n(c)
        // Extract model version from third line
        for(TInt i(0); i < thirdLine; i++)
            {
            TInt cutPos = version.Locate( '\n' );

            if( cutPos == KErrNotFound )
                {
                break;
                }
            else if(i < ( thirdLine - 1 ) )
                {
                version.Delete( 0, cutPos + 1 );
                }
            else
                {
                cutPos = version.Locate( '\n' );
                if( cutPos != KErrNotFound )
                    {
                    version.Delete( cutPos, (version.Length() - cutPos) );
                    }
                ret->Des().Copy( version );
                }
            }
        }

    // Implicit requirement - should always be the case though
    __ASSERT_ALWAYS( ret->Length() <= 255, User::Invariant());
    return ret;
    }


// ---------------------------------------------------------------------------
// MMCScBkupPhoneModelUtils::MaximumPhoneModelIdentifierLength()
// 
// 
// ---------------------------------------------------------------------------
TInt MMCScBkupPhoneModelUtils::MaximumPhoneModelIdentifierLength()
    {
    return KSysUtilVersionTextLength;
    }


// ---------------------------------------------------------------------------
// MMCScBkupPhoneModelUtils::ArchiveRestorePermissableL()
// 
// 
// ---------------------------------------------------------------------------
TBool MMCScBkupPhoneModelUtils::ArchiveRestorePermissableL( const TDesC8& aPhoneModelData, TBitFlags aArchiveFlags, const TVersion& aArchiveVersion )
    {
    TBool restoreOkay = EFalse;
    
    // Check version first
    restoreOkay = ( aArchiveVersion.iMajor >= KMMCScBkupArchiveFileFormatFirstRestorableMajorVersion );
    
    // Check flags next
    if  ( restoreOkay )
        {
        restoreOkay = ( aArchiveFlags.Value() & EMMCScBkupArchiveFlagsContentValid );
       
        // Only check model if the flags were alright
        if  ( restoreOkay )
            {
            HBufC8* currentData = CurrentPhoneModelLC();
            restoreOkay = ( aPhoneModelData.Compare( *currentData ) == 0 );
            CleanupStack::PopAndDestroy( currentData );
            }
        }

    // Done
    return restoreOkay;
    }




