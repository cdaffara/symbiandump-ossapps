/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class MPXDbUtil which contains utilities functions
*
*/


#ifndef MPXDBUTIL_H
#define MPXDBUTIL_H

// INCLUDES
#include <mpxmediageneraldefs.h>
#include "mpxcollectiondbstd.h"

// FORWARD DECLARATIONS
class CMPXMedia;
class CMPXMediaArray;

// CLASS DECLARATION

/**
* This static class contains utilities functions used by Music Collection
*
* @lib MPXDbPlugin.lib
*/
class MPXDbUtil
    {
    public:
        /**
        * Filter out duplicated Ids in the array
        * @param aIds array containing Ids to be filtered
        * @param aFilteredIds filtered Ids without duplicates, generated from
        *        the input array
        */
        static void FilterDuplicatedIdsL(const TArray<TUint32>& aIds,
            RArray<TUint32>& aFilteredIds);

        /**
        * Returns the table name for a specified category
        * @param aCategory identifies the category
        * @return corresponding table name
        */
        static TPtrC TableNameForCategoryL(TMPXGeneralCategory aCategory);

        /**
        * Returns the music field name for a specified category
        * @param aCategory identifies the category
        * @return corresponding field name
        */
        static TPtrC MusicFieldNameForCategoryL(TMPXGeneralCategory aCategory);
    };

#endif // MPXDBUTIL_H

// End of File

