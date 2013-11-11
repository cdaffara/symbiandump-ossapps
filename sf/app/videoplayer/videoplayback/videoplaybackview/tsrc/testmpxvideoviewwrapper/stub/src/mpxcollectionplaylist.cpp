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
* Description:  implementation of collection playlist
*
*/

#include "mpxcollectionplaylist.h"


// -------------------------------------------------------------------------------------------------
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXCollectionPlaylist* CMPXCollectionPlaylist::NewL()
{
    CMPXCollectionPlaylist* p = new ( ELeave ) CMPXCollectionPlaylist();
    CleanupStack::PushL( p );
    p->ConstructL();
    CleanupStack::Pop( p );
    return p;
}

// -------------------------------------------------------------------------------------------------
// Default constructor
// -------------------------------------------------------------------------------------------------
//
CMPXCollectionPlaylist::CMPXCollectionPlaylist()
{
}

// -------------------------------------------------------------------------------------------------
// 2nd phase constructor.
// -------------------------------------------------------------------------------------------------
//
void CMPXCollectionPlaylist::ConstructL()
{
}

// -------------------------------------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------------------------------------
//
CMPXCollectionPlaylist::~CMPXCollectionPlaylist()
{
}

// -------------------------------------------------------------------------------------------------
// Returns the number of items in the same container
// -------------------------------------------------------------------------------------------------
//
TInt CMPXCollectionPlaylist::Count() const
{
    return iCount;
}

// End of file
