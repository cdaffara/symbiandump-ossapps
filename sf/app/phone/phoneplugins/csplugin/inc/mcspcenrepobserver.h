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
* Description:  Declarations for class MCSPCenRepObserver
*
*/



#ifndef MCSPCENREPOBSERVER_H
#define MCSPCENREPOBSERVER_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Observer interface for indicating P&S changes
*
*/
class MCSPCenRepObserver
    {
    public:  // New functions

        /**
        * Handler for changed event.
        * @param aUid uid of setting 
        * @param aKey the changed key
        * @param aVal value
        */
        virtual void HandleNotifyCenRepL( const TUid aUid, 
                                          const TUint32 aKey, 
                                          TInt aVal) = 0;
    };

#endif      // MCSPCENREPOBSERVER_H   

// End of File
