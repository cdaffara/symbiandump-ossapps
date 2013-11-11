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
* Description:  Responsible for interation with the category tables:
*                Artist, Album, Genre and Composer
*
*/


// INCLUDE FILES
#include <mpxlog.h>
#include "mpxdbgenre.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbGenre* CMPXDbGenre::NewL(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbGenre::NewL");

    CMPXDbGenre* self = CMPXDbGenre::NewLC(aDbManager, aCategory);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbGenre* CMPXDbGenre::NewLC(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbGenre::NewLC");

    CMPXDbGenre* self = new (ELeave) CMPXDbGenre(aDbManager, aCategory);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbGenre::~CMPXDbGenre()
    {
    MPX_FUNC("CMPXDbGenre::~CMPXDbGenre");
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbGenre::CMPXDbGenre(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory) :
    CMPXDbCategory(aDbManager, aCategory)
    {
    MPX_FUNC("CMPXDbGenre::CMPXDbGenre");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbGenre::ConstructL()
    {
    MPX_FUNC("CMPXDbGenre::ConstructL");

    BaseConstructL();
    }

// End of File
