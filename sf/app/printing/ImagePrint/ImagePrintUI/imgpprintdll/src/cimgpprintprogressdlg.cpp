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


#include <eikcapc.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsConstants.h>
#include <AknsListBoxBackgroundControlContext.h>
#include <AknDef.h>

#include "cimgpprintprogressdlg.h"
#include "imageprintapp.hrh"
#include "imageprintconsts.h"
#include "clog.h"

const TInt KBrandingBitmapPosPortY ( 50 );
const TInt KBrandingBitmapPosLandY ( 50 );

//  CONSTRUCTION
CIMGPPrintProgressDlg::CIMGPPrintProgressDlg(
    CEikDialog** aSelfPtr,
    TBool aVisibilityDelayOff )
      : CAknProgressDialog( aSelfPtr, aVisibilityDelayOff )
    {
    }

//  Destructor
CIMGPPrintProgressDlg::~CIMGPPrintProgressDlg()
    {
    }

// Positions the UI controls correctly
void CIMGPPrintProgressDlg::PostLayoutDynInitL()
    {
    LOG("CIMGPPrintProgressDlg::PostLayoutDynInitL");
	TSize screenSize = CCoeEnv::Static()->ScreenDevice()->SizeInPixels();
	TInt bitmapPosY;
	if( screenSize.iWidth == KScreenWidthPortrait )
		{
		bitmapPosY = KBrandingBitmapPosPortY;
		}
	else
		{
		bitmapPosY = KBrandingBitmapPosLandY;
		}
    CAknProgressDialog::PostLayoutDynInitL();
    CEikCaptionedControl* capControl = GetFirstLineOnFirstPageOrNull();
    if( capControl )
        {
        capControl->SetPosition( TPoint( 0, 0 ) ) ;
        }
    Line( EBrandingBitmap )->SetPosition( TPoint( 70, 20 ) );
    Control( EBrandingBitmap )->SetPosition( TPoint( 0, bitmapPosY ) );
    Line( EBrandingBitmap )->SetRect( TRect( TPoint( 70, bitmapPosY ), TPoint( 180, bitmapPosY+40 ) ) );
    }

// Overloaded method to enable the bitmap to appear to dialog
CEikCaptionedControl* CIMGPPrintProgressDlg::GetFirstLineOnFirstPageOrNull()
    {
    return Line( EPrintWaitNote );
    }

void CIMGPPrintProgressDlg::HandleResourceChange( TInt aType )
    {
    LOG1("CIMGPPrintProgressDlg::HandleResourceChange: %d", aType);
    //if (aType == KEikDynamicLayoutVariantSwitch)
    //    {
        CAknNoteDialog::HandleResourceChange( aType );
        TRAP_IGNORE ( PostLayoutDynInitL() );
    //    }
    }
    
TKeyResponse CIMGPPrintProgressDlg::OfferKeyEventL( const TKeyEvent &/*aKeyEvent*/, TEventCode /*aType*/ )
    {
    return EKeyWasConsumed;   
    }

//  End of File
