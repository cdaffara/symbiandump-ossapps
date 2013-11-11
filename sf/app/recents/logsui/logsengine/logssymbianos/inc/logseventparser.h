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

#ifndef LOGSEVENTPARSER_H
#define LOGSEVENTPARSER_H

//  INCLUDES
#include <e32std.h>
#include <logwrap.h>

// FORWARD DECLARATION
class CPhCltEmergencyCall;
class LogsEventData;
class LogsEventStrings;
class LogsEvent;

// CLASS DECLARATION

/**
 *
 */
class LogsEventParser
    {         
    public:
        
        /**
         * Parses the event and fills parsed data to given dest event.
         * Event state and view status are updated automatically at
         * this phase.
         * @param source, source event
         * @param dest, destination event
         * @param strings, list of strings used for comparison
         */
        static void parseL( const CLogEvent& source,
                            LogsEvent& dest,
                            const LogsEventStrings& strings );
        
    private:                               
        
        /**
         * Resolve event type based on available information.
         */
        static bool resolveEventType(LogsEvent& dest);
        
        static void eventTypeSpecificParsing(LogsEvent& dest);
        
        static bool isUnknownRemoteParty(LogsEvent& dest, const QString& remoteParty);
 
    private: // Testing related friend class definitions
        
        friend class UT_LogsEventParser;
    };

#endif  // LOGSEVENTPARSER_H

// End of File
