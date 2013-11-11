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


// INCLUDE FILES
#include <mpxlog.h>
#include "mpxpodcastcollectiondbdef.h"
#include "mpxpodcastdbmanager.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXPodcastDbManager* CMPXPodcastDbManager::NewL(
    RFs& aFs)
    {
    MPX_FUNC("CMPXPodcastDbManager::NewL");

    CMPXPodcastDbManager* self = CMPXPodcastDbManager::NewLC(aFs);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXPodcastDbManager* CMPXPodcastDbManager::NewLC(
    RFs& aFs)
    {
    MPX_FUNC("CMPXPodcastDbManager::NewLC");

    CMPXPodcastDbManager* self = new (ELeave) CMPXPodcastDbManager(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXPodcastDbManager::~CMPXPodcastDbManager()
    {
    MPX_FUNC("CMPXPodcastDbManager::~CMPXPodcastDbManager");
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXPodcastDbManager::CMPXPodcastDbManager(
    RFs& aFs) :
    CMPXDbManager(aFs)
    {
    MPX_FUNC("CMPXPodcastDbManager::CMPXPodcastDbManager");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXPodcastDbManager::ConstructL()
    {
    MPX_FUNC("CMPXPodcastDbManager::ConstructL");
    CMPXDbManager::ConstructL(TFileName(KMCDbFile));
    }

// End of File
