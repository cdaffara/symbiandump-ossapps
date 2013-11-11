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
* Description:  Factory method to create collection utility object
*
*/

#include <e32base.h>
#include <mpxcollectionpath.h>

#include "mpxcollectionutility.h"

// ============================== MEMBER FUNCTIONS =================================================

// -------------------------------------------------------------------------------------------------
// Create the collection utility object by mode
// -------------------------------------------------------------------------------------------------
//
MMPXCollectionUtility* MMPXCollectionUtility::NewL( MMPXCollectionObserver* aObs )
{
    return CMPXCollectionUtility::NewL( aObs );
}

// -------------------------------------------------------------------------------------------------
// Two phase constructor
// -------------------------------------------------------------------------------------------------
//
CMPXCollectionUtility* CMPXCollectionUtility::NewL( MMPXCollectionObserver* aObs )
{
    CMPXCollectionUtility* p = new ( ELeave ) CMPXCollectionUtility( aObs );
    CleanupStack::PushL( p );
    p->ConstructL();
    CleanupStack::Pop( p );
    return p;
}

// -------------------------------------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------------------------------------
//
CMPXCollectionUtility::~CMPXCollectionUtility()
{
}

// -------------------------------------------------------------------------------------------------
// C++ constructor
// -------------------------------------------------------------------------------------------------
//
CMPXCollectionUtility::CMPXCollectionUtility( MMPXCollectionObserver* /*aObs*/ )
{
}

// -------------------------------------------------------------------------------------------------
// 2nd phase constructor
// -------------------------------------------------------------------------------------------------
//
void CMPXCollectionUtility::ConstructL()
{
}

// -------------------------------------------------------------------------------------------------
// Return reference to collection
// -------------------------------------------------------------------------------------------------
//
MMPXCollection& CMPXCollectionUtility::Collection()
{
    return *this;
}

// -------------------------------------------------------------------------------------------------
// Destroy this object
// -------------------------------------------------------------------------------------------------
//
void CMPXCollectionUtility::Close()
{
    delete this;
}

// -------------------------------------------------------------------------------------------------
// The UID identifying this collection
// -------------------------------------------------------------------------------------------------
//
TUid CMPXCollectionUtility::UidL() const
{
    return iUid;
}

// -------------------------------------------------------------------------------------------------
// CMPXCollectionUtility::PathL
// -------------------------------------------------------------------------------------------------
//
CMPXCollectionPath* CMPXCollectionUtility::PathL()
{
    return NULL;
}

// End of file
