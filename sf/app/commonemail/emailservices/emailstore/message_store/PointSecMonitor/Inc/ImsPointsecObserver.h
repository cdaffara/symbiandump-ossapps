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
* Description:  PointSec monitor observer interface.
*
*/

#ifndef __IMS_POINT_SEC_OBSERVER_H__
#define __IMS_POINT_SEC_OBSERVER_H__

#include <e32base.h>

// CLASS DECLARATION
class MImsPointsecObserver
    {
    public:
        virtual void PointSecLockStarted() = 0;
        virtual void PointSecLockEnded() = 0;
    };


#endif // __IMS_POINT_SEC_OBSERVER_H__
