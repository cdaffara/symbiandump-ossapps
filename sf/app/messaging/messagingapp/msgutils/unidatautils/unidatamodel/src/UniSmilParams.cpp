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
*       Reads default parameters for composing SMIL from resource file
*
*
*/


// INCLUDE FILES
#include <e32math.h>

#include <barsread.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

#include <unidatamodel.rsg>

#include "UniSmil.hrh"
#include "UniSmilParams.h"
#include "UniModelConst.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CUniSmilParams::CUniSmilParams( const RFs& aSession ) : iSession( aSession )
    {
    }

// Symbian OS default constructor can leave.
void CUniSmilParams::ConstructL()
    {
    iComposeParams = new( ELeave ) TUniSmilComposeParam();
    iImageRegionParams = new( ELeave ) TUniSmilRegionParam();
    iTextRegionParams = new( ELeave ) TUniSmilRegionParam();
    ReadResourcesL();
    }

// Two-phased constructor.
EXPORT_C CUniSmilParams* CUniSmilParams::NewL( const RFs& aSession )
    {
    CUniSmilParams* self = new ( ELeave ) CUniSmilParams( aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Two-phased constructor.
EXPORT_C CUniSmilParams* CUniSmilParams::NewLC( const RFs& aSession )
    {
    CUniSmilParams* self = new (ELeave) CUniSmilParams( aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CUniSmilParams::~CUniSmilParams()
    {
    delete iImageRegionParams;
    delete iTextRegionParams;
    delete iComposeParams;
    }


// ---------------------------------------------------------
// CUniSmilParams::ReadResourcesL()
//
// ---------------------------------------------------------
//
void CUniSmilParams::ReadResourcesL()
    {
    RResourceFile resFile;
    OpenResourceFileL( resFile );

    ReadComposeParamResourcesL( resFile );
    ReadImageRegionResourcesL( resFile );
    ReadTextRegionResourcesL( resFile );

    resFile.Close();
    }

// ---------------------------------------------------------
// ReadComposeParamResourcesL()
//
// ---------------------------------------------------------
//
void CUniSmilParams::ReadComposeParamResourcesL(
    const RResourceFile& aResFile )
    {
    TResourceReader reader;
    HBufC8* buf = aResFile.AllocReadLC( R_COMPOSE_PARAMS );
    reader.SetBuffer( buf );

    iComposeParams->iRootLayoutCompose = reader.ReadInt16();
    iComposeParams->iDisplayWidth      = reader.ReadInt16();
    iComposeParams->iDisplayHeight     = reader.ReadInt16();
    iComposeParams->iComposeXmlns      = reader.ReadInt16();
    iComposeParams->iTopPadding        = reader.ReadInt16();
    iComposeParams->iRegionPadding     = reader.ReadInt16();
    iComposeParams->iBottomPadding     = reader.ReadInt16();

    CleanupStack::PopAndDestroy( buf );

    buf = aResFile.AllocReadLC( R_SLIDE_PARAMS );
    reader.SetBuffer( buf );

    iComposeParams->iSlideDuration = reader.ReadInt16();

    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------
// CUniSmilParams::ReadImageRegionResourcesL
//
// ---------------------------------------------------------
//
void CUniSmilParams::ReadImageRegionResourcesL(
    const RResourceFile& aResFile )
    {
    TResourceReader reader;
    HBufC8* buf = aResFile.AllocReadLC( R_IMAGE_REGION_ATTRIBUTES );
    reader.SetBuffer( buf );

    TPtrC ptr;

    // Id
    ptr.Set( reader.ReadTPtrC() );
    iImageRegionParams->iId = HBufC::NewL( ptr.Length() );
    iImageRegionParams->iId->Des().Copy( ptr );

    // Fit
    ptr.Set( reader.ReadTPtrC() );
    iImageRegionParams->iFit = HBufC::NewL( ptr.Length() );
    iImageRegionParams->iFit->Des().Copy( ptr );

    // Max/Min width/height
    iImageRegionParams->iMaxSize.iHeight = reader.ReadInt16();   //r_max_height
    iImageRegionParams->iMinSize.iHeight = reader.ReadInt16();   //r_min_height
    iImageRegionParams->iMaxSize.iWidth = reader.ReadInt16();   //r_max_width
    iImageRegionParams->iMinSize.iWidth = reader.ReadInt16();   //r_min_width

    iImageRegionParams->iTopLeft.iY = reader.ReadInt16();         //r_top = 0;
    iImageRegionParams->iTopLeft.iX = reader.ReadInt16();         //r_left = 0;

    // Default Default ;)
    iImageRegionParams->iSize = iImageRegionParams->iMaxSize;

    CleanupStack::PopAndDestroy( buf );

    // Make some sanity checks
    if ( iImageRegionParams->iMaxSize.iHeight < iImageRegionParams->iMinSize.iHeight ||
         iImageRegionParams->iMaxSize.iWidth  < iImageRegionParams->iMinSize.iWidth )
        {
        User::Leave( KErrArgument );
        }

    }


// ---------------------------------------------------------
// CUniSmilParams::ReadTextRegionResourcesL()
//
// ---------------------------------------------------------
//
void CUniSmilParams::ReadTextRegionResourcesL(
    const RResourceFile& aResFile )
    {
    TResourceReader reader;
    HBufC8* buf = aResFile.AllocReadLC( R_TEXT_REGION_ATTRIBUTES );
    reader.SetBuffer( buf );

    TPtrC ptr;

    // Id
    ptr.Set( reader.ReadTPtrC() );
    iTextRegionParams->iId = HBufC::NewL( ptr.Length() );
    iTextRegionParams->iId->Des().Copy( ptr );

    // Fit
    ptr.Set( reader.ReadTPtrC() );
    iTextRegionParams->iFit = HBufC::NewL( ptr.Length() );
    iTextRegionParams->iFit->Des().Copy( ptr );

    // Max/Min width/height
    //r_max_height
    iTextRegionParams->iMaxSize.iHeight = reader.ReadInt16();
    //r_min_height
    iTextRegionParams->iMinSize.iHeight = reader.ReadInt16();
    //r_max_width
    iTextRegionParams->iMaxSize.iWidth = reader.ReadInt16();
    //r_min_width
    iTextRegionParams->iMinSize.iWidth = reader.ReadInt16();

    //r_top = 0;
    iTextRegionParams->iTopLeft.iY = reader.ReadInt16();
    //r_left = 0;
    iTextRegionParams->iTopLeft.iX = reader.ReadInt16();

    // Default Default ;)
    iTextRegionParams->iSize = iTextRegionParams->iMaxSize;

    CleanupStack::PopAndDestroy( buf );

    // Make some sanity checks
    if ( iTextRegionParams->iMaxSize.iHeight < iTextRegionParams->iMinSize.iHeight ||
         iTextRegionParams->iMaxSize.iWidth  < iTextRegionParams->iMinSize.iWidth )
        {
        User::Leave( KErrArgument );
        }

    }

// ---------------------------------------------------------
// CUniSmilParams::OpenResourceFileL()
//
// ---------------------------------------------------------
//
void CUniSmilParams::OpenResourceFileL( RResourceFile& aResFile )
    {
    TFileName fileName;
    TParse parse;
    parse.Set( KUniDataModelResourceFilename, &KDC_RESOURCE_FILES_DIR, NULL );
    fileName = parse.FullName();
    BaflUtils::NearestLanguageFile( iSession, fileName );
    aResFile.OpenL( const_cast<RFs&>( iSession ), fileName );
    aResFile.ConfirmSignatureL( KDefResFileSignature );
    }

//  End of File

