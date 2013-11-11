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


#ifndef MPXDBCOMPOSER_H
#define MPXDBCOMPOSER_H

// INCLUDES
#include "mpxdbcategory.h"

// CLASS FORWARDS

// CLASS DECLARATION

/**
* Responsible for managing all music databases
*
* @lib MPXDbPlugin.lib
*/

class CMPXDbComposer :
    public CMPXDbCategory
    {
    public:

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbCategory instance.
        */
        static CMPXDbComposer* NewL(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory);

        /**
        * Two-phased constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        * @return New CMPXDbCategory instance on the cleanup stack.
        */
        static CMPXDbComposer* NewLC(CMPXDbManager& aDbManager,
            TMPXGeneralCategory aCategory);

        /**
        * Destructor
        */
        virtual ~CMPXDbComposer();

    private:

        /**
        * C++ constructor.
        * @param aDbManager database manager to use for database interactions
        * @param aCategory identifies the category
        */
        CMPXDbComposer(CMPXDbManager& aDbManager, TMPXGeneralCategory aCategory);

        /**
        * Second phase constructor.
        */
        void ConstructL();

    private:

        /**
        * Column indexes in the category tables
        */
        enum TComposerColumns
            {
            EComposerUniqueId = KMPXTableDefaultIndex,
            EComposerName,
            EComposerSongCount,
            EAlbumFieldCount
            };

    private:    // Data

    };
#endif // MPXDBCOMPOSER_H

// End of File
