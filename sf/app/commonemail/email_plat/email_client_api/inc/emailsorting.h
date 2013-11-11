/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Email Client sort criteria definition
*
*/

#ifndef EMAILSORTING
#define EMAILSORTING

#include <e32base.h>

namespace EmailInterface {

/**
* Sort criteria used in searching messages with MEmailMessageSearchAsync 
* declared in memailmessagesearch.h.
*/
class TEmailSortCriteria
{
public:
    /**
    * Fields for sort criteria
    */
    enum TSortField {
        EDontCare,
        EByDate,
        EBySender,
        EByRecipient,
        EBySubject,
        EByPriority,
        EByFlagStatus,
        EByUnread,
        EBySize,
        EByAttachment,
    };
    
    // is ascending sort order
    TBool iAscending;
    
    // sort by field
    TSortField iField; 

    inline TEmailSortCriteria() : iAscending( ETrue ), iField( EDontCare ) {}
};

typedef RArray<TEmailSortCriteria> RSortCriteriaArray;

} // EmailInterface

#endif // EMAILSORTING
