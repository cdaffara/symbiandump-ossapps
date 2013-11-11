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
* Description: 
*       CUniSmilModel, Storage for Smil presentation data.
*
*
*/


// ---------------------------------------------------------
// CUniSmilModel::Layout
//
// Accessor
// ---------------------------------------------------------
//
inline TUniLayout CUniSmilModel::Layout() const
    {
    return iSlideLayout;    
    }

// ---------------------------------------------------------
// CUniSmilModel::SlideCount
// ---------------------------------------------------------
//
inline TInt CUniSmilModel::SlideCount() const
    {
#ifdef _DEBUG_TRACES_
    qCritical() << " CUniSmilModel SlideCount start";
#endif
    return iSlideArray->Count();
    }

// ---------------------------------------------------------
// CUniSmilModel::SetScreenSize
// ---------------------------------------------------------
//
inline void CUniSmilModel::SetScreenSize( const TSize& aScreenSize )
    {
    iScreenSize = aScreenSize;
    }

// ---------------------------------------------------------
// CUniSmilModel::MaxSlideCount
// ---------------------------------------------------------
//
inline TInt CUniSmilModel::MaxSlideCount() const
    {
    return iMaxSlideCount;
    }
        
// ---------------------------------------------------------
// CUniSmilModel::SetMaxSlideCount
// ---------------------------------------------------------
//
inline void CUniSmilModel::SetMaxSlideCount( TInt aSlideCount )
    {
    iMaxSlideCount = aSlideCount;
    }

// ---------------------------------------------------------
// CUniSmilModel::MaxSlideCountReached
// ---------------------------------------------------------
//
inline TBool CUniSmilModel::MaxSlideCountReached() const
    {
    if ( iMaxSlideCount > 0 &&
         iSlideArray->Count() >= iMaxSlideCount )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
