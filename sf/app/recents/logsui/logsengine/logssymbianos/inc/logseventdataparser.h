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

#ifndef LOGSEVENTDATAPARSER_H
#define LOGSEVENTDATAPARSER_H

//  INCLUDES
#include <QObject>

// FORWARD DECLARATION
class LogsEventData;
class CLogEvent;
class TDesC8;

// CLASS DECLARATION

/**
 * 
 */
class LogsEventDataParser
    {
    public:  

        /**
         * Parses event data from given data
         * @param source
         * @param dest
         * @return 0 if parsed succesfully
         **/
        static int parse( const CLogEvent& source, LogsEventData& dest );
        
    private:
        
        static bool checkNonTaggedData( const TDesC8 &data, LogsEventData& dest );
        static void checkTaggedData( const TDesC8 &data, LogsEventData& dest );        
        static void setMsgPartsNumber( const TDesC8 &data, LogsEventData& dest );

    private: // Testing related friend class definitions
        
        friend class UT_LogsEventDataParser;
        
    };

#endif      // LOGSEVENTDATAPARSER_H


// End of File
