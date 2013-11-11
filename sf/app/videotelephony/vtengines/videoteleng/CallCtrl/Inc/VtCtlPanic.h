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
* Description:  Data types for call control
*
*/



#ifndef VTCTLPANIC_H
#define VTCTLPANIC_H

//  INCLUDES

#include <e32std.h>

// CONSTANTS

/**
* Panic text
*/
_LIT( KVtCtlPanicCategory, "VtCSCall" );

// DATA TYPES

/**
* Panic values
*/
enum TVtCtlPanic {
    //** Line subsession handle is null */
    EVtCtlPanicLineHandleNull,
    /** Call subsession handle is null */
    EVtCtlPanicCallHandleNull
    };

// FUNCTION PROTOTYPES

/**
* Call control panic.
* @param aPanic panic value
*/
void Panic( TVtCtlPanic aPanic );

#endif // VTCTLPANIC_H

// End of File
