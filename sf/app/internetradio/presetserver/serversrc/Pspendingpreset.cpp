/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Represents a pending preset that has not yet been committed
*
*/


#include "pspendingpreset.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSPendingPreset* CPSPendingPreset::NewL( TInt aId )
    {
    CPSPendingPreset* self = new ( ELeave ) CPSPendingPreset( aId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CPSPendingPreset::CPSPendingPreset( TInt aId )
    : iId( aId )
    {
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CPSPendingPreset::ConstructL()
    {
    iName.CreateL( KNullDesC );
    iData.CreateL( KNullDesC8 );
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CPSPendingPreset::~CPSPendingPreset()
    {
    iName.Close();
    iData.Close();
    }

// ---------------------------------------------------------------------------
// Returns the ID of the preset.
// ---------------------------------------------------------------------------
//
TInt CPSPendingPreset::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
// Sets the index of the preset.
// ---------------------------------------------------------------------------
//
void CPSPendingPreset::SetIndex( TInt aIndex )
    {
    iIndex = aIndex;
    }

// ---------------------------------------------------------------------------
// Returns the index of the preset.
// ---------------------------------------------------------------------------
//
TInt CPSPendingPreset::Index() const
    {
    return iIndex;
    }

// ---------------------------------------------------------------------------
// Sets the name of the preset.
// ---------------------------------------------------------------------------
//
void CPSPendingPreset::SetNameL( const TDesC& aName )
    {
    iName.Close();
    iName.CreateL( aName );
    }

// ---------------------------------------------------------------------------
// Returns the name of the preset.
// ---------------------------------------------------------------------------
//
const TDesC& CPSPendingPreset::Name() const
    {
    return iName;
    }

// ---------------------------------------------------------------------------
// Sets the data of the preset.
// ---------------------------------------------------------------------------
//
void CPSPendingPreset::SetDataL( const TDesC8& aData )
    {
    iData.Close();
    iData.CreateL( aData );
    }

// ---------------------------------------------------------------------------
// Returns the data of the preset.
// ---------------------------------------------------------------------------
//
const TDesC8& CPSPendingPreset::Data() const
    {
    return iData;
    }

// ======== GLOBAL FUNCTIONS ========
