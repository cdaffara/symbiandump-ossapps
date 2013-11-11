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
* Description:  Flag container class inline methods.
*
*/


// -----------------------------------------------------------------------------
// TLcVtFlagContainer::TLcVtFlagContainer
// -----------------------------------------------------------------------------
//
TLcVtFlagContainer::TLcVtFlagContainer()
    {
    Clear();
    }

// -----------------------------------------------------------------------------
// TLcVtFlagContainer::Clear
// -----------------------------------------------------------------------------
//
void TLcVtFlagContainer::Clear()
    {
    iFlags = 0;
    }

// -----------------------------------------------------------------------------
// TLcVtFlagContainer::IsFlag
// -----------------------------------------------------------------------------
//
TBool TLcVtFlagContainer::IsFlag( TUint32 aFlags ) const
    {
    return ( iFlags & aFlags );
    }

// -----------------------------------------------------------------------------
// TLcVtFlagContainer::SetFlag
// -----------------------------------------------------------------------------
//
void TLcVtFlagContainer::SetFlag( TUint32 aFlags )
    {
    iFlags |= aFlags;
    }

// -----------------------------------------------------------------------------
// TLcVtFlagContainer::ClearFlag
// -----------------------------------------------------------------------------
//
void TLcVtFlagContainer::ClearFlag( TUint32 aFlags )
    {
    iFlags &= ~aFlags;
    }

// -----------------------------------------------------------------------------
// TLcVtFlagContainer::Value
// -----------------------------------------------------------------------------
//
TUint32 TLcVtFlagContainer::Value() const
    {
    return iFlags;
    }

// -----------------------------------------------------------------------------
// TLcVtFlagContainer::SetValue
// -----------------------------------------------------------------------------
//
void TLcVtFlagContainer::SetValue( TUint32 aValue )
    {
    iFlags = aValue;
    }

// -----------------------------------------------------------------------------
// TLcVtUpdateableFlagContainer::Update
// -----------------------------------------------------------------------------
//
void TLcVtUpdateableFlagContainer::Update()
    {
    Clear();
    UpdateFlags();
    }
