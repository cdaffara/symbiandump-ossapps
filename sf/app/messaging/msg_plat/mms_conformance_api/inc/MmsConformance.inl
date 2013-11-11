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
* Description: MmsConformance.inl*
*/


    
// -----------------------------------------------------------------------------
// CMmsConformance::ConformanceVersion
// ACCESSOR
// -----------------------------------------------------------------------------
inline TUint8 CMmsConformance::ConformanceVersion() const
    {
    return iMmsVersion;
    }

// -----------------------------------------------------------------------------
// CMmsConformance::MaxSendSize
// ACCESSOR
// -----------------------------------------------------------------------------
inline TUint32 CMmsConformance::MaxSendSize() const
    {
    return iMaxSendSize;
    }

// -----------------------------------------------------------------------------
// CMmsConformance::SetMaxSendSize
// MUTATOR
// -----------------------------------------------------------------------------
inline void CMmsConformance::SetMaxSendSize( TUint32 aMaxSize )
    {
    iMaxSendSize = aMaxSize;
    }


// -----------------------------------------------------------------------------
// CMmsConformance::CreationModeUserChangeable
// ACCESSOR
// -----------------------------------------------------------------------------
inline TBool CMmsConformance::CreationModeUserChangeable() const
    {
    return ( iResolverFlags & ECreationModeUserChangeable );
    }

// -----------------------------------------------------------------------------
// CMmsConformance::SetCreationMode
// ACCESSOR
// -----------------------------------------------------------------------------
inline TBool CMmsConformance::CreationMode() const
    {
    return !( iResolverFlags & ECreationModeRestricted );
    }

// -----------------------------------------------------------------------------
// CMmsConformance::SetCreationMode
// MUTATOR
// -----------------------------------------------------------------------------
inline void CMmsConformance::SetCreationMode( TBool aFree )
    {
    if ( aFree )
        {
        iResolverFlags &= ~ECreationModeRestricted;
        }
    else
        {
        iResolverFlags |= ECreationModeRestricted;
        }
    }

// -----------------------------------------------------------------------------
// CMmsConformance::SetImageSizeSetting
// MUTATOR
// -----------------------------------------------------------------------------
inline void CMmsConformance::SetImageSizeSetting( TSize aImageSize )
    {
    iSettingsImageSize = aImageSize;
    }

// -----------------------------------------------------------------------------
// CMmsConformance::LargerThanImageSizeSetting
// -----------------------------------------------------------------------------
//
inline TBool CMmsConformance::LargerThanImageSizeSetting( TSize aImageSize )
    {
    return ( aImageSize.iWidth > iSettingsImageSize.iWidth ||
        aImageSize.iHeight > iSettingsImageSize.iHeight );
    }

// -----------------------------------------------------------------------------
// CMmsConformance::SkipCharacterSetCheck
// -----------------------------------------------------------------------------
//
inline void CMmsConformance::CheckCharacterSet( TBool aCheck )
    {
    if ( aCheck )
        {
        iResolverFlags |= ECheckCharacterSet;
        }
    else
        {
        iResolverFlags &= ~ECheckCharacterSet;
        }
    }

// -----------------------------------------------------------------------------
// TMmsConformance::TMmsConformance
// -----------------------------------------------------------------------------
//
inline TMmsConformance::TMmsConformance() :
    iConfClass( EMmsClassUnclassified ),
    iConfStatus( EMmsConfOk ),
    iCanAdapt( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// TMmsConformance::TMmsConformance
// -----------------------------------------------------------------------------
//
inline TMmsConformance::TMmsConformance(
            TMmsConfClass aConfClass,
            TUint32 aConfStatus,
            TBool aCanAdapt ) :
    iConfClass( aConfClass ),
    iConfStatus( aConfStatus ),
    iCanAdapt( aCanAdapt )
    {
    }

// -----------------------------------------------------------------------------
// TMmsConformance::TMmsConformance
// -----------------------------------------------------------------------------
//
inline TMmsConformance::TMmsConformance( const TMmsConformance& aConformance ) :
    iConfClass( aConformance.iConfClass ),
    iConfStatus( aConformance.iConfStatus ),
    iCanAdapt( aConformance.iCanAdapt )
    {
    }

