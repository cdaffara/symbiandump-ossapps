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
* Description:  Abstract class to observe query commands.
*
*/


#ifndef __MPHONEQUERYCOMMANDOBSERVER_H
#define __MPHONEQUERYCOMMANDOBSERVER_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
/**
*  Abstract class to observe query commands. 
*/
class MPhoneQueryCommandObserver
    {
    public:

        /**
         * Destructor.
         */
        virtual ~MPhoneQueryCommandObserver() {};

        /**
         * Sends command which was selected from a query for
         * command observer.
         * @param aCommandId A command id.
         */
        virtual void ProcessCommandL( TInt aCommandId  ) = 0;

    };

#endif // __MPHONEQUERYCOMMANDOBSERVER_H


// End of File
