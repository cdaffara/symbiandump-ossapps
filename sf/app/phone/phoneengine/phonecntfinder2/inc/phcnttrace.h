/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef PHCNTTRACE_H
#define PHCNTTRACE_H

#include <e32std.h>

//#define PHCNT_TRACE

#ifdef PHCNT_TRACE

#pragma message("DEBUG PRINTS ON")

#define PRINTF( format, data ) RDebug::Print( _L(format), data );
#define PRINT( format ) RDebug::RawPrint( _L(format) );

#else

#define PRINTF( format, data )
#define PRINT( format ) 

#endif // PHCNT_TRACE

#endif // PHCNTTRACE_H
