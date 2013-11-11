/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface for indicating publish and subscribe 
*                changes
*
*/



#ifndef MVTENGSETTINGPSOBSERVER_H
#define MVTENGSETTINGPSOBSERVER_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Observer interface for indicating P&S changes
*
*  @lib videoteleng.lib
*  @since Series 60 2.8
*/
class MVtEngSettingPSObserver
    {
    public:  // New functions

        /**
        * Handler for changed event.
        * @param aUid uid of setting 
        * @param aKey id of setting
        * @param aStatus status of completed AO operation
        */
        virtual void HandleNotifyPSL( const TUid aUid, const TInt& aKey, 
            const TRequestStatus& aStatus ) = 0;
    };

#endif      // MVTENGSETTINGPSOBSERVER_H   

// End of File
