/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  WPWAPUtil is a utility class for reading resource strings.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <barsc.h>
#include <bautils.h>
#include <WPAdapterUtil.h>
#include <CWPCharacteristic.h>
#include <uri16.h>                // TUriParser8
#include "WPWAPUtil.h"

// CONSTANTS
#if ( defined (__WINS__) || defined (__WINSCW) ) // this different on hw
    _LIT( KAdapterName, "WPWAPAdapterResource" );
#else
    _LIT( KAdapterName, "WPWAPAdapter" );
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WPWAPUtil::ReadHBufCL
// -----------------------------------------------------------------------------
//
HBufC* WPWAPUtil::ReadHBufCL( TInt aResourceId )
    {
    TFileName fileName;
    Dll::FileName( fileName );

    return WPAdapterUtil::ReadHBufCL( fileName, KAdapterName, aResourceId );
    }

// -----------------------------------------------------------------------------
// WPWAPUtil::CheckURL
// -----------------------------------------------------------------------------
//
TBool WPWAPUtil::CheckURI( const TDesC& aUrl )
    {
    return WPAdapterUtil::CheckURI( aUrl );
    }

//  End of File  
