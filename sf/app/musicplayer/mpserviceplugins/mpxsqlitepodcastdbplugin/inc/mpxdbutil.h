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
#include <mpxpodcastdefs.h> // Podcast specific

// FORWARD DECLARATIONS
class CMPXMedia;
class CMPXMediaArray;
class RSqlStatement;

// CLASS DECLARATION

/**
* This static class contains utilities functions used by Podcast Collection
*
* @lib mpxpodcastdbplugin.lib
*/
class MPXDbUtil
    {
    public:
        /**
        * Create a media object with title, podcast type, podcast category
        * and ID then append it to the media array
        * @param aArray media array
        * @param aTitle title
        * @param aType attribute type
        * @param aCat attribute category
        * @param aPodcastType attribute podcast type
        * @param aPodcastCat attribute podcast category
        * @param aId unique ID
        * @param aNonPermissibleActions bitmask specifying the non-permissible
        *       actions for the entry
        */
        static void AppendMediaL(CMPXMediaArray& aArray, const TDesC& aTitle,
            TMPXGeneralType aType, TMPXGeneralCategory aCat,
            TMPXPodcastType aPodcastType, TMPXPodcastCategory aPodcastCat,
            TMPXItemId aId = 0, TInt aNonPermissibleActions = 0);

        /**
        * Returns the Podcast field name for a specified category
        * @param aCategory identifies the category
        * @return corresponding field name
        */
        static TPtrC PodcastFieldNameForCategoryL(TMPXGeneralCategory aCategory);

        /**
        * Returns today's date (no hour, minutes, seconds)
        * @return date value
        */
        static TTime Today();

        /**
        * Returns this month' start date (no hour, minutes, seconds)
        * @return date value
        */
        static TTime MonthStart();

        /**
        * Returns the index of the current day in week (0-6)
        * @return index value
        */
        static TInt DayNoInWeek();

        /**
        * Returns the table name for a specified category
        * @param aCategory identifies the category
        * @return corresponding table name
        */
        static TPtrC TableNameForCategoryL(TMPXGeneralCategory aCategory);
    };

#endif // MPXDBUTIL_H

// End of File

