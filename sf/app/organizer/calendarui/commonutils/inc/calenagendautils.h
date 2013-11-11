/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Static utility functions. 
 *                - date utils to help comparisions and calculations witg
 *                  dates and times. 
 *
*/


#ifndef CALENAGENDAUTILS_H
#define CALENAGENDAUTILS_H

//  INCLUDES

#include <agendaentry.h>
#include <qglobal.h> // Q_DECL_EXPORT macro

#ifdef  CALENDATEUTILS_DLL
#define CALENAGENDAUTIL_EXPORT Q_DECL_EXPORT
#else
#define CALENAGENDAUTIL_EXPORT Q_DECL_IMPORT
#endif

// FORWARD DECLARATIONS
class CCalInstance;
class CCalEntryView;
class CCalInstanceView;

// CLASS DECLARATION
NONSHARABLE_CLASS( CALENAGENDAUTIL_EXPORT CalenAgendaUtils )
    {
    public:
        
        /**
         * Checks that if entry ends at starting midnight of the day,
         * but has started earlier.
         * Such entries are not shown in day that they end, because end time 
         * is just marking the end of previous day. 
         * @returns ETrue, if entry ends at starting midnight of day, but starts before that
         *          EFalse, otherwise
         **/
        static bool endsAtStartOfDay( AgendaEntry& entry,
                                                        const QDateTime& day );
        
        /**
         * Checks if the given entry is an all-day
         * Need to call this API only for entry types other than EEvent 
         * @param entry Entry for which all-day attribute is being queried
         * @returns True, If entry is an all-day else False
         */
        static bool CalenAgendaUtils::isAlldayEvent( const AgendaEntry& entry );

    };

#endif // CALENAGENDAUTILS_H


// End of File
