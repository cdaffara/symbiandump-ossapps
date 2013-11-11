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
* Description:   Logging utility for MC Photos
*
*/



#ifndef T_GLXLOGGER_H
#define T_GLXLOGGER_H

// CLASS HEADER
#include "glxlogger.h"

// EXTERNAL INCLUDES
#include <e32std.h>

namespace GlxLogger
    {
    /**
     * Logging utility, writes the given log to a shared chunk if the chunk
     * exists or to RDebug if not.
     * Note! due to performance reasons only 8 bit descriptor allowed
     * @param The descriptor containing the format string. 
     * @param The arguments to append to the format string (ellipsis notation)
     */
    IMPORT_C void WriteFormat( TRefByValue<const TDesC8> aFmt, ... );
    }

#endif // T_GLXLOGGER_H
