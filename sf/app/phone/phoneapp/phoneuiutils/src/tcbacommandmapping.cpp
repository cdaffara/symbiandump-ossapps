/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class holds mapping between CBA and custom commands.
*
*/


#include "tcbacommandmapping.h"

/** Uninitialized command identifier. */
const TInt KUninitializedCommmandId = -1;

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// TCbaCommandMapping::TCbaCommandMapping
// C++ default constructor for null object creation.
// -----------------------------------------------------------------------------
//
TCbaCommandMapping::TCbaCommandMapping()
    : 
    iCbaCommandId( KUninitializedCommmandId ), 
    iCustomCommandId( KUninitializedCommmandId ) 
    {}


// -----------------------------------------------------------------------------
// TCbaCommandMapping::TCbaCommandMapping
// Overloaded C++ constructor.
// -----------------------------------------------------------------------------
//
TCbaCommandMapping::TCbaCommandMapping( 
        TInt aCbaCommandId, TInt aCustomCommandId )
    : 
    iCbaCommandId( aCbaCommandId ), iCustomCommandId( aCustomCommandId ) 
    {}


// -----------------------------------------------------------------------------
// TCbaCommandMapping::CbaCommandId
// -----------------------------------------------------------------------------
//
TInt TCbaCommandMapping::CbaCommandId() const 
    { 
    return iCbaCommandId; 
    }


// -----------------------------------------------------------------------------
// TCbaCommandMapping::CustomCommandId
// -----------------------------------------------------------------------------
//
TInt TCbaCommandMapping::CustomCommandId() const 
    { 
    return iCustomCommandId; 
    }


// -----------------------------------------------------------------------------
// TCbaCommandMapping::operator==
// -----------------------------------------------------------------------------
//
TBool TCbaCommandMapping::operator==( 
        const TCbaCommandMapping& aMapping ) const
    {
    return ( iCbaCommandId == aMapping.CbaCommandId() &&
             iCustomCommandId == aMapping.CustomCommandId() );
    }

// end of file
