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

#ifndef LOGSREMOVEOBSERVER_H
#define LOGSREMOVEOBSERVER_H

// INCLUDES

// FORWARD DECLARATION

// CLASS DECLARATION

/**
 *  Reader observer interface
 */
class LogsRemoveObserver
    {

    public:

        /**
         * LogsRemove has completed removing.
         */
        virtual void removeCompleted() = 0;
        
        /**
         * Error occured while removing.
         * @param err
         */
        virtual void logsRemoveErrorOccured(int err) = 0;
        
    };

#endif      // LOGSREMOVEOBSERVER_H


// End of File
      

        
       
