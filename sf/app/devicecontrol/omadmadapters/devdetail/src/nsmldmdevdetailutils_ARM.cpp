/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    DM DevDetail Adapter Source Code, ARM implementation
*
*/




// INCLUDES
#include <sysutil.h>
#include "nsmldmdevdetailadapter.h"
#include "nsmldebug.h"

#if defined (_ENABLE_MULTIROFS_SUPPORT)
#include <sysversioninfo.h>
#else
// CONSTANTS
_LIT8( KNSmlSwVersionSeparator, " " );
#endif

// ---------------------------------------------------------------------------
// CNSmlDmDevDetailAdapter::GetDevDetailDataL( )
// Fetches device specific data using EInfo interface.
// ---------------------------------------------------------------------------
void CNSmlDmDevDetailAdapter::GetDevDetailDataL( 
                                CBufBase& aObject, 
                                TNSmlDevDetailData aElement ) const
    {
    _DBG_FILE("CNSmlDmDevDetailAdapter::GetDevDetailDataL(): begin (MARM)");
    
    aObject.Reset();
    
    switch( aElement )
        {
        case ESwVersion:
            {
            // buffers for data fetched from sysutil
#if defined (_ENABLE_MULTIROFS_SUPPORT)

            TBuf16<KSysVersionInfoTextLength> temp;
			TBuf8<KSysVersionInfoTextLength> temp8;
			temp.Zero();
			temp8.Zero();
            SysVersionInfo::TVersionInfoType what = SysVersionInfo::EFWVersion;
            TInt error = SysVersionInfo::GetVersionInfo(what,temp);
			
			User::LeaveIfError(error);

			//Ascii copy which is required
			temp8.Copy(temp);
			aObject.InsertL(0,temp8);
			//Unicode copy if required
			//aObject.InsertL(0,temp.Ptr(),temp.Length());
#else
            HBufC* verBuf = HBufC::NewLC( KSysUtilVersionTextLength );
            HBufC8* verBuf8 = HBufC8::NewLC( KSysUtilVersionTextLength );
            
            TPtr ver = verBuf->Des();
            TPtr8 ver8 = verBuf8->Des();
            // fetch software version (sw.txt) from sysutil
        	SysUtil::GetSWVersion( ver );
        	ver8.Copy( ver );
        	
            // parse sw fields and append to aObject
        	TChar separator('\n');
        	TInt pos = ver8.Locate( separator );

            if ( pos == KErrNotFound )
                {
                User::Leave( KErrNotFound );
                }

            // insert version (1st field) and separator to beginning
        	aObject.InsertL( 0, ver8.Mid( 0, pos ) );
        	aObject.InsertL( 0, KNSmlSwVersionSeparator );
        	
        	// locate type designator (3rd field)
        	ver8 = ver8.Mid( pos + 1 );
        	pos = ver8.Locate( separator );
        	ver8 = ver8.Mid( pos + 1 );
        	pos = ver8.Locate( separator );
        	
        	if ( pos == KErrNotFound )
        	    {
                // type designator (the rest of the string, since there was no \n found) 
            	aObject.InsertL( 0, ver8 );
        	    }
        	else
        	    {
                // type designator to beginning
            	aObject.InsertL( 0, ver8.Mid( 0, pos ) );
        	    }
        	
        	// fetch variant (lang.txt) from sysutil
        	ver.Zero();
        	SysUtil::GetLangVersion( ver );
        	ver8.Copy( ver );
        	
            // insert separator and variant to the end
        	aObject.InsertL( aObject.Size(), KNSmlSwVersionSeparator );
        	aObject.InsertL( aObject.Size(), ver8 );
            
            CleanupStack::PopAndDestroy( verBuf8 );
            CleanupStack::PopAndDestroy( verBuf );
#endif //_ENABLE_MULTIROFS_SUPPORT
            break;
            }
        case EHwVersion:
            {
            break;
            }
        default:
            User::Panic( KNSmlDevDetailPanic, KErrArgument );
            break;
        }

    _DBG_FILE("CNSmlDmDevDetailAdapter::GetDevDetailDataL(): end (MARM)");
    }


