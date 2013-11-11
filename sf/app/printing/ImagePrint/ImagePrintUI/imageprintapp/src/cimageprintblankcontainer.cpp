/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <imageprintapp.rsg>
#include <e32keys.h>
#include <aknViewAppUi.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsConstants.h>
#include <AknsListBoxBackgroundControlContext.h>

#include "cimageprintblankcontainer.h"
#include "imageprintapp.hrh"
#include "imageprintconsts.h"

CImagePrintBlankContainer* CImagePrintBlankContainer::NewL(
    TRect& aRect )
    {
    CImagePrintBlankContainer* self = NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }
    
CImagePrintBlankContainer* CImagePrintBlankContainer::NewLC( 
    TRect& aRect )
    {
    CImagePrintBlankContainer* self = new ( ELeave ) CImagePrintBlankContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// Default constructor
CImagePrintBlankContainer::CImagePrintBlankContainer()
	{
	}

// 2nd phase constructor
void CImagePrintBlankContainer::ConstructL( 
    const TRect& aRect  )
    {    
	CreateWindowL();
	SetRect( aRect );

    // Fetch the dimensions of the screen
	TScreenInfoV01 info;
	TPckgBuf<TScreenInfoV01> buf( info );
	UserSvr::ScreenInfo( buf );
	info = buf();
    TRect screenRect( TPoint( 0, 0 ), info.iScreenSize );    

    // Create skin control context for filling the background
    const TAknsItemID *id = &KAknsIIDNone;	
    iSkinControlContext = 
        CAknsListBoxBackgroundControlContext::NewL(
            KAknsIIDSkinBmpMainPaneUsual, screenRect, ETrue, *id, screenRect );
	
   	ActivateL();
    }

// Destructor
CImagePrintBlankContainer::~CImagePrintBlankContainer()
    {    
    delete iSkinControlContext;
	}

// Draw method
void CImagePrintBlankContainer::Draw( 
    const TRect& /*aRect*/ ) const
    {
	CWindowGc& gc = SystemGc();

	TRect rect = Rect();    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance(); 
    if ( AknsDrawUtils::HasBitmapBackground( skin, iSkinControlContext ) )
        {
        // Draw skin background        
        AknsDrawUtils::Background( skin, iSkinControlContext, NULL, gc, rect );
        }
    else
        {
        gc.Clear( rect );
        }
    }

void CImagePrintBlankContainer::HandleResourceChange( TInt aType )
    {
    if( aType == KEikDynamicLayoutVariantSwitch )
        {        
        CEikAppUi* appUi = static_cast<CEikAppUi*>( iEikonEnv->AppUi() );
        TRect rect = appUi->ClientRect();
        SetRect( rect );
        }
    }
   
//  End of File
