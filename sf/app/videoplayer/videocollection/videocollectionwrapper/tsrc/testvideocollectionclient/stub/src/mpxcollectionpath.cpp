/**
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: stub CMPXCollectionPath for testing CVideoCollectionClient
* 
*/

#include "stub/inc/mpxcollectionpath.h"

int gPatheLevels = 3;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CMPXCollectionPath* CMPXCollectionPath::NewL()
{
    CMPXCollectionPath *path = new CMPXCollectionPath();
    return path;
}

// -----------------------------------------------------------------------------
// CMPXCollectionPath
// -----------------------------------------------------------------------------
//
CMPXCollectionPath::CMPXCollectionPath()
{
    
}

// -----------------------------------------------------------------------------
// ~CMPXCollectionPath
// -----------------------------------------------------------------------------
//
CMPXCollectionPath::~CMPXCollectionPath()
{
    
}

// -----------------------------------------------------------------------------
// Levels
// -----------------------------------------------------------------------------
//
TInt CMPXCollectionPath::Levels() const
{
    return gPatheLevels;
}

// -----------------------------------------------------------------------------
// AppendL
// -----------------------------------------------------------------------------
//
void CMPXCollectionPath::AppendL(const TArray<TMPXItemId>& /*aIds*/)
{

}

// -----------------------------------------------------------------------------
// AppendL
// -----------------------------------------------------------------------------
//
void CMPXCollectionPath::AppendL(const TMPXItemId& /*aId*/)
{
    
}
// -----------------------------------------------------------------------------
// SelectL
// -----------------------------------------------------------------------------
//
void CMPXCollectionPath::SelectL(const TMPXItemId& /*aId*/)
{
    
}

void CMPXCollectionPath::setLevel(int level)
{
	gPatheLevels = level;
}
