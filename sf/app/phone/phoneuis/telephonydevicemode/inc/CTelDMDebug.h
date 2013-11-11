/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file defines logging macros for TelephonyDeviceMode
*
*/



#ifndef CTELDMDEBUG_H
#define CTELDMDEBUG_H

#ifdef _DEBUG
// ===========================================================================
                  // RDebug logging for target HW
// ===========================================================================
#include <e32svr.h>

#define FLOG( a )   { RDebug::Print( a ); }
#define FTRACE( a ) { a; }

// Declare the FPrint function
//
inline void FPrint( const TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TInt tmpInt = VA_ARG( list, TInt );
    TInt tmpInt2 = VA_ARG( list, TInt );
    TInt tmpInt3 = VA_ARG( list, TInt );
    VA_END( list );
    RDebug::Print( aFmt, tmpInt, tmpInt2, tmpInt3 );
    }

// ===========================================================================
#else // // No loggings --> Reduced binary size
// ===========================================================================
#define FLOG( a )
#define FTRACE( a )

#endif // _DEBUG

#endif      // 
            
// End of File