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
#include "mpxdbcomposer.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbComposer* CMPXDbComposer::NewL(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbComposer::NewL");

    CMPXDbComposer* self = CMPXDbComposer::NewLC(aDbManager, aCategory);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXDbComposer* CMPXDbComposer::NewLC(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("CMPXDbComposer::NewLC");

    CMPXDbComposer* self = new (ELeave) CMPXDbComposer(aDbManager, aCategory);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CMPXDbComposer::~CMPXDbComposer()
    {
    MPX_FUNC("CMPXDbComposer::~CMPXDbComposer");
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CMPXDbComposer::CMPXDbComposer(
    CMPXDbManager& aDbManager,
    TMPXGeneralCategory aCategory) :
    CMPXDbCategory(aDbManager, aCategory)
    {
    MPX_FUNC("CMPXDbComposer::CMPXDbComposer");
    }

// ----------------------------------------------------------------------------
// Second phase constructor.
// ----------------------------------------------------------------------------
//
void CMPXDbComposer::ConstructL()
    {
    MPX_FUNC("CMPXDbComposer::ConstructL");

    BaseConstructL();
    }

// End of File
