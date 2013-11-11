/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
*
*/


// Protection against nested includes
#ifndef __CLOG_H__
#define __CLOG_H__

// System includes
#include <e32base.h>
#include <e32std.h>

#include "logdef.h"

#ifdef ENABLE_LOGGING
#define LOG(s) Log::DoLog(_L(s))
#define LOG1(s, v) Log::DoLog( _L(s), v )
#define LOG2(s, v1, v2) Log::DoLog( _L(s), v1, v2 )
#define LOG3(s, v1, v2, v3) Log::DoLog( _L(s), v1, v2, v3 )

#define LOG8(s) Log::DoLog8(_L8(s))
#define LOG81(s, v) Log::DoLog8( _L8(s), v )
#define LOG82(s, v1, v2) Log::DoLog8( _L8(s), v1, v2 )
#define LOG83(s, v1, v2, v3) Log::DoLog8( _L8(s), v1, v2, v3 )

#else
#define LOG(s)
#define LOG1(s, v)
#define LOG2(s, v1, v2)
#define LOG3(s, v1, v2, v3)

#define LOG8(s)
#define LOG81(s, v)
#define LOG82(s, v1, v2)
#define LOG83(s, v1, v2, v3)
#endif // ENABLE_LOGGING

class Log
    {

    public:
    	
        static void DoLog( TRefByValue<const TDesC> aText, ... );

        static void DoLog8( TRefByValue<const TDesC8> aText, ... );
    };

#endif  // __CLOG_H__

// End of File
