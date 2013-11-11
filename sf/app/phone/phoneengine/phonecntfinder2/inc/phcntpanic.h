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

#ifndef PHCNTPANIC
#define PHCNTPANIC

#include <e32std.h>

enum TPhCntPanic 
    {
    EPhCntOperationAlreadyOngoing,
    EPhCntServiceRequestActiveAlready
    };

/**
 * Panics the current thread with panic code.
 * @since S60 v3.1
 * @param aPanicCode Panic code used with panic.
 */
void PhCntPanic( TPhCntPanic aPanicCode );


#endif // PHCNTPANIC
