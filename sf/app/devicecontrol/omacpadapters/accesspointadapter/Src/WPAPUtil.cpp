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
* Description:    Class offers common utility methods.
*
*/




// INCLUDE FILES

#include "WPAPUtil.h"			// Own header
#include <WPAdapterUtil.h>

// CONSTANTS
// Commented out by JN
//#if ( defined (__WINS__) || defined (__WINSCW) ) // this different on hw
    _LIT( KAdapterName, "WPAPAdapterResource" );
//#else
//    _LIT( KAdapterName, "WPAPAdapter" );
//#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WPAPUtil::ReadHBufCLC
// -----------------------------------------------------------------------------
//
HBufC* WPAPUtil::ReadHBufCL( TInt aResourceId )
	{
    TFileName fileName;
    Dll::FileName( fileName );

    return WPAdapterUtil::ReadHBufCL( fileName, KAdapterName, aResourceId );
	}

//  End of File
