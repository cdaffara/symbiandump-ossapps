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

#ifndef LOGSREADEROBSERVER_H
#define LOGSREADEROBSERVER_H

// INCLUDES
#include <QList>

// FORWARD DECLARATION
class LogsEvent;

// CLASS DECLARATION

/**
 *  Reader observer interface
 */
class LogsReaderObserver
    {

    public:

        /**
         * Reader has completed reading.
         */
        virtual void readCompleted() = 0;
        
        /**
         * Error occured while reading.
         * @param err
         */
        virtual void errorOccurred(int err) = 0;
        
        /**
         * Temporary error occured while reading.
         * It should be possible to continue reading at some point.
         * @param err
         */
        virtual void temporaryErrorOccurred(int err) = 0;
        
        /**
         * Reader has completed modifying the event.
         */
        virtual void eventModifyingCompleted() = 0;
        
        /**
         * Duplicate reading has completed
         * @param duplicates, list of read duplicates, onwership is transferred
         */
        virtual void duplicatesReadingCompleted(QList<LogsEvent*> duplicates) = 0;
    };

#endif      // LOGSREADER_H


// End of File
      

        
       
