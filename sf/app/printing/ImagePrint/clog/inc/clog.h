/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CLOG_H__
#define __CLOG_H__

#include <e32base.h>
#include <e32std.h>

#include "logdef.h"

#ifdef ENABLE_LOGGING
#define LOG(s) Log::DoLog(_L(s))
#define LOG1(s, v) Log::DoLog( _L(s), v )
#define LOG2(s, v1, v2) Log::DoLog( _L(s), v1, v2 )

#define LOG8(s) Log::DoLog8(_L8(s))
#define LOG81(s, v) Log::DoLog8( _L8(s), v )
#define LOG82(s, v1, v2) Log::DoLog8( _L8(s), v1, v2 )

#else
#define LOG(s)
#define LOG1(s, v)
#define LOG2(s, v1, v2)
#define LOG8(s)
#define LOG81(s, v)
#define LOG82(s, v1, v2)
#endif // ENABLE_LOGGING

class Log
    {

    public:
    	
        static void DoLog( TRefByValue<const TDesC> aText, ... );

        static void DoLog8( TRefByValue<const TDesC8> aText, ... );
    };

#endif  // __CLOG_H__

// End of File
