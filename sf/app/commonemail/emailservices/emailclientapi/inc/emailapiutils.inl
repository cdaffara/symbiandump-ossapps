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
* Description: This file implements class CEmailMailbox.
*
*/

template<class T>
inline void CleanupResetAndDestroy<T>::PushL( T& aItem ) 
    { 
    TCleanupItem item( &CleanupResetAndDestroy::ResetAndDestroy, &aItem );
    CleanupStack::PushL( item );
    }

template<class T>
inline void CleanupResetAndDestroy<T>::ResetAndDestroy( TAny *aPtr )
    {
    reinterpret_cast<T*>( aPtr )->ResetAndDestroy();
    }

// End of file
