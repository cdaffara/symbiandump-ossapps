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
* Description:  This class is responsible for managing all of music collection
*                databases.
*
*/


#ifndef MPXCOLLECTIONDBMANAGER_H
#define MPXCOLLECTIONDBMANAGER_H

// INCLUDES

#include "mpxdbmanager.h"

// CLASS DECLARATION

/**
* Database manager specialization for the collection plugin.
*
* @lib MPXDbPlugin.lib
*/
class CMPXCollectionDbManager :
    public CMPXDbManager
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aFs file server session
        * @return The constructed object of CMPXDbManager
        */
        static CMPXCollectionDbManager* NewL(RFs& aFs);

        /**
        * Two-phased constructor.
        * @param aFs file server session
        * @return The constructed object of CMPXDbManager
        */
        static CMPXCollectionDbManager* NewLC(RFs& aFs);

        /**
        * Destructor
        */
        virtual ~CMPXCollectionDbManager();

    private:

        /**
        * C++ default constructor
        * @param aFs file server session
        */
        CMPXCollectionDbManager(RFs& aFs);

        /**
        * The second phase constructor ConstructL to safely construct things
        * that can leave
        */
        void ConstructL();
    };

#endif  // MPXCOLLECTIONDBMANAGER_H
