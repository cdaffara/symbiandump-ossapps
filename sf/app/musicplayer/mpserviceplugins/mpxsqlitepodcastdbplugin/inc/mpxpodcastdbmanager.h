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
* Description:  This class is responsible for managing all of podcast collection
*                databases.
*
*/


#ifndef MPXPODCASTDBMANAGER_H
#define MPXPODCASTDBMANAGER_H

// INCLUDES
#include "mpxdbmanager.h"

// CLASS DECLARATION

/**
* Database manager specialization for the podcast plugin.
*
* @lib mpxpodcastdbplugin.lib
*/
class CMPXPodcastDbManager :
    public CMPXDbManager
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aFs file server session
        * @return New CMPXPodcastDbManager instance
        */
        static CMPXPodcastDbManager* NewL(RFs& aFs);

        /**
        * Two-phased constructor.
        * @param aFs file server session
        * @return New CMPXPodcastDbManager instance on the CS
        */
        static CMPXPodcastDbManager* NewLC(RFs& aFs);

        /**
        * Destructor
        */
        virtual ~CMPXPodcastDbManager();

    public: // New functions


    private:
        /**
        * C++ default constructor
        * @param aFs file server session
        */
        CMPXPodcastDbManager(RFs& aFs);

        /**
        * The second phase constructor ConstructL to safely construct things
        * that can leave
        */
        void ConstructL();
    };

#endif  // MPXPODCASTDBMANAGER_H
