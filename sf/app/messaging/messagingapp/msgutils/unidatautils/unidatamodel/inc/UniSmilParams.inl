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
*  Reads default parameters for composing SMIL from resource file   
*
*
*/


// -----------------------------------------------------------------------------
// TUniSmilComposeParam
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// TUniSmilComposeParam::TUniSmilComposeParam
// -----------------------------------------------------------------------------
inline TUniSmilComposeParam::TUniSmilComposeParam() :
    iRootLayoutCompose( 0 ),
    iDisplayWidth( 0 ),
    iDisplayHeight( 0 ),
    iComposeXmlns( EFalse ),
    iSlideDuration( 0 ),
    iTopPadding( 0 ),
    iRegionPadding( 0 ),
    iBottomPadding( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TUniSmilComposeParam::TUniSmilComposeParam
// -----------------------------------------------------------------------------
inline TUniSmilComposeParam::TUniSmilComposeParam( const TUniSmilComposeParam& aCopy ) :
    iRootLayoutCompose( aCopy.iRootLayoutCompose ),
    iDisplayWidth( aCopy.iDisplayWidth ),
    iDisplayHeight( aCopy.iDisplayHeight ),
    iComposeXmlns( aCopy.iComposeXmlns ),
    iSlideDuration( aCopy.iSlideDuration ),
    iTopPadding( aCopy.iTopPadding ),
    iRegionPadding( aCopy.iRegionPadding ),
    iBottomPadding( aCopy.iBottomPadding )
    {
    }


// -----------------------------------------------------------------------------
// TUniSmilRegionParam
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// TUniSmilRegionParam::TUniSmilRegionParam
// -----------------------------------------------------------------------------
inline TUniSmilRegionParam::TUniSmilRegionParam() :
    iTopPercent( 0 ),
    iLeftPercent( 0 ),
    iWidthPercent( 0 ),
    iHeightPercent( 0 ),
    iId( 0 ),
    iFit( 0 ),
    iMaxSize( 0, 0 ),
    iMinSize( 0, 0 ),
    iSize( 0, 0 ),
    iTopLeft( 0, 0 )
    {
    }

// -----------------------------------------------------------------------------
// TUniSmilRegionParam::TUniSmilRegionParam
// -----------------------------------------------------------------------------
inline TUniSmilRegionParam::TUniSmilRegionParam(const TUniSmilRegionParam& aCopy) :
    iTopPercent( aCopy.iTopPercent ),
    iLeftPercent( aCopy.iLeftPercent ),
    iWidthPercent( aCopy.iWidthPercent ),
    iHeightPercent( aCopy.iHeightPercent ),
    iMaxSize( aCopy.iMaxSize ),
    iMinSize( aCopy.iMinSize ),
    iSize( aCopy.iSize ),
    iTopLeft( aCopy.iTopLeft )
    {
    iId = aCopy.iId->Alloc();
    iFit = aCopy.iFit->Alloc();
    }

// -----------------------------------------------------------------------------
// TUniSmilRegionParam::~TUniSmilRegionParam
// -----------------------------------------------------------------------------
inline TUniSmilRegionParam::~TUniSmilRegionParam()
    {
    delete iId;
    delete iFit;
    }

// -----------------------------------------------------------------------------
// TUniSmilRegionParam::Width
// -----------------------------------------------------------------------------
inline TInt TUniSmilRegionParam::Width()
    {
    return iSize.iWidth;
    }

// -----------------------------------------------------------------------------
// TUniSmilRegionParam::Height
// -----------------------------------------------------------------------------
inline TInt TUniSmilRegionParam::Height()
    {
    return iSize.iHeight;
    }

// -----------------------------------------------------------------------------
// TUniSmilRegionParam::Top
// -----------------------------------------------------------------------------
inline TInt TUniSmilRegionParam::Top()
    {
    return iTopLeft.iY;
    }

// -----------------------------------------------------------------------------
// TUniSmilRegionParam::Left
// -----------------------------------------------------------------------------
inline TInt TUniSmilRegionParam::Left()
    {
    return iTopLeft.iX;
    }

// -----------------------------------------------------------------------------
// TUniSmilRegionParam::SetSize
// -----------------------------------------------------------------------------
inline void TUniSmilRegionParam::SetSize(const TSize& aSize)
    {
    iSize = aSize;
    }

// -----------------------------------------------------------------------------
// TUniSmilRegionParam::SetTopLeft
// -----------------------------------------------------------------------------
inline void TUniSmilRegionParam::SetTopLeft(const TPoint& aTl)
    {
    iTopLeft = aTl;
    }

// End of File
