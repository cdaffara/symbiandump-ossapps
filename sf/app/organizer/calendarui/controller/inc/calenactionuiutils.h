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
* Description:  Implements UI utils
*
*/


#ifndef CALENACTIONUIUTILS_H
#define CALENACTIONUIUTILS_H

// INCLUDES
#include <badesca.h>

// FORWARD DECLARATIONS
class AgendaEntry;
class AgendaUtil;
class TCalenInstanceId;
class QString;
class QDateTime;
// CLASS DEFINITIONS



/**
 * Implements UI utils found in the services API. We have to do this here
 * because both the controller and the services need these functions, but
 * the controller doesn't know about the services (and to link the two
 * would create a circular dependency).
 */
NONSHARABLE_CLASS( CalenActionUiUtils )
    {
    
    public:

        enum TRepeatQueryType
            {
            ESave = 1,
            EDelete,
            EEdit
            };
    public:  // New functions
                           
        /**
         * Find a instance with the given instance id and instance view
         * @param aId Target Instance Id
         * @param aInstanceView reference to InstanceView
         * @return Pointer to CCalInstance
         */
        static AgendaEntry findPossibleInstanceL( const TCalenInstanceId& id,
                                                     AgendaUtil* agendaUtil );
        
        /**
         * Mark to-do entry as completed or restore a completed to-do and save.
         * @param aEntryView CCalEntryView to save this instance to.
         * @param aToDoEntry CCalEntry reference. Entry type must be CCalEntry::ETodo.
         * @param aStatus ETrue, to set as completed. EFalse to restore completed to-do.
         * @panic Function panics if the type of aToDoEntry is different than CCalEntry::ETodo.
         **/
        static void setToDoCompleteStatus(AgendaUtil* agendaUtil,
                                            AgendaEntry& entry,
                                            const bool status );
        
    private: // Construction and destruction
        // Hiding constructor, because this is static utility class.
        //lint -e{1526}
        /**
         * Private C++ constructor
         */
        CalenActionUiUtils();

        // Hiding assignment, because this is static utility class.
        //lint -e{1526}
        /**
         * Private assignment operator
         */
        CalenActionUiUtils& operator=( const CalenActionUiUtils& );
    };

#endif // CALENACTIONUIUTILS_H

// End of file
