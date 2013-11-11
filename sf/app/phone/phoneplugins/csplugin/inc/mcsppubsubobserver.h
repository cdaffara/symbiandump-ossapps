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
* Description:  Declarations for class MCSPPubSubObserver
*
*/



#ifndef MCSPPUBSUBOBSERVER_H
#define MCSPPUBSUBOBSERVER_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Observer interface for indicating P&S changes
*
*/
class MCSPPubSubObserver
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

#endif      // MCSPPUBSUBOBSERVER_H   

// End of File
