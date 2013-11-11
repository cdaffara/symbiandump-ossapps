/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Inline methods for CTeUtlRecoverySystem.
*
*/



// INCLUDE FILES
#include    "ctelerecoverysystem.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CleanupEarlyDestruct::PushL
// -----------------------------------------------------------------------------
//
template <class T>
inline void CleanupEarlyDestruct<T>::PushL( T& aRef )
    {
    CleanupStack::PushL( TCleanupItem( &Cleanup, &aRef ) );
    }

// -----------------------------------------------------------------------------
// CleanupEarlyDestruct::Cleanup
// -----------------------------------------------------------------------------
//
template <class T>
void CleanupEarlyDestruct<T>::Cleanup( TAny *aPtr )
    { 
    ( static_cast< T* >( aPtr ) )->EarlyDestruct();
    }

// -----------------------------------------------------------------------------
// CleanupEarlyDestructPushL
// -----------------------------------------------------------------------------
//
template <class T>
inline void CleanupEarlyDestructPushL( T& aRef )
    {
    CleanupEarlyDestruct<T>::PushL( aRef );
    }

//  End of File
