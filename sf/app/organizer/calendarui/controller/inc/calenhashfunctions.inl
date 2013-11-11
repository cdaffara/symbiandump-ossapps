/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar controller
*
*/

#ifndef CALENHASHFUNCTIONS_INL_
#define CALENHASHFUNCTIONS_INL_

#include "calennotificationhandler.h"

// ----------------------------------------------------------------------------
// Global function HashCalenNotificationFunction
// The hash function used to hash the objects of type T. 
// ----------------------------------------------------------------------------
//
inline  TUint32 HashCalenNotificationFunction(const TCalenNotification& aNotification)
    {
    return aNotification;
    }

// ----------------------------------------------------------------------------
// Global function HashCalenNotificationIdentityRelation
// The identity relation used to determine if two objects of type T should be considered identical. 
// ----------------------------------------------------------------------------
//
inline  TBool HashCalenNotificationIdentityRelation(const TCalenNotification& aNotification1,
                                                            const TCalenNotification& aNotification2)
    {
    return (aNotification1 == aNotification2);
    }

#endif /*CALENFUNCTIONS_INL_*/

