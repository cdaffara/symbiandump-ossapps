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
* Description:  This class is responsible for all category tables (Artist,
*                Album, Genre, Composer).
*
*
*/


#ifndef MPXDBGENRE_H
#define MPXDBGENRE_H

// INCLUDES
#include "mpxdbcategory.h"

// CLASS FORWARDS
class CMPXMedia;
class CMPXMediaArray;
class RSqlStatement;

// CLASS DECLARATION

/**
* Responsible for managing all music databases
*
* @lib MPXDbPlugin.lib
*/

class CMPXDbGenre :
    public CMPXDbCategory
    {
    public:

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbCategory instance.
        */
        static CMPXDbGenre* NewL(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbCategory instance on the cleanup stack.
        */
        static CMPXDbGenre* NewLC(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory);

        /**
        * Destructor
        */
        virtual ~CMPXDbGenre();

    private:

        /**
        * C++ constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        */
        CMPXDbGenre(CMPXDbManager& aDbManager, TMPXGeneralCategory aCategory);

        /**
        * Second phase constructor.
        */
        void ConstructL();

    private:

        /**
        * Column indexes in the category tables
        */
        enum TGenreColumns
            {
            EGenreUniqueId = KMPXTableDefaultIndex,
            EGenreName,
            EGenreSongCount,
            EGenreFieldCount
            };

    private:    // Data

    };
#endif // MPXDBGENRE_H

// End of File
