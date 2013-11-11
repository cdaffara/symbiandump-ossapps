/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Settings inline methods
*
*/


//Constants
const TInt8 KVTEngMaxVolume = 10;
const TInt8 KVTEngMinVolume = 0;


// -----------------------------------------------------------------------------
// CVtEngSettings::GetMinVolume
// Return device min volume.
// 
// -----------------------------------------------------------------------------
//
inline TInt CVtEngSettings::MinVolume() const
    { return KVTEngMinVolume; }

// -----------------------------------------------------------------------------
// CVtEngSettings::GetMaxVolume
// Return event manager.
// 
// -----------------------------------------------------------------------------
//
inline TInt CVtEngSettings::MaxVolume() const
    { return KVTEngMaxVolume; }

// -----------------------------------------------------------------------------
// CVtEngSettings::ValidVolume
// Checks if the volume is within allowed limits
// 
// -----------------------------------------------------------------------------
//
inline TInt CVtEngSettings::ValidVolume( const TInt aVolume ) const
    {
    if ( aVolume < KVTEngMinVolume )
        {
        return KVTEngMinVolume;
        }
    else if ( aVolume > KVTEngMaxVolume )
        {
        return KVTEngMaxVolume;
        }
    else
        {
        return aVolume;
        }
    }

// End of File
