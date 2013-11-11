/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AppView implementation, only displays two labels changed
*                from CHtiAdminAppUi class
*
*/


#include <coemain.h>
#include <AknUtils.h>
#include <HtiAdmin.rsg>

#include "HtiAdminAppView.h"
#include "HtiAdminApplication.h" //logging

// Standard construction sequence
CHtiAdminAppView* CHtiAdminAppView::NewL(const TRect& aRect)
    {
    CHtiAdminAppView* self = CHtiAdminAppView::NewLC(aRect);
    CleanupStack::Pop(self);
    return self;
    }

CHtiAdminAppView* CHtiAdminAppView::NewLC(const TRect& aRect)
    {
    CHtiAdminAppView* self = new (ELeave) CHtiAdminAppView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

CHtiAdminAppView::CHtiAdminAppView()
    {
    // no implementation required
    }

CHtiAdminAppView::~CHtiAdminAppView()
    {
    delete iHtiCaption;
    delete iHtiStatus;
    delete iHtiVersionCaption;
    delete iHtiVersion;
    delete iHtiAutoStartCaption;
    delete iHtiAutoStartStatus;
    delete iHtiSelectedCommCaption;
    delete iHtiSelectedComm;
    delete iHtiCommDetails;
    }

void CHtiAdminAppView::ConstructL(const TRect& aRect)
    {
    //HTI_LOG_TEXT("CHtiAdminAppView::ConstructL");

    iHtiCaption = HBufC::NewL( KMaxDisplayText );
    iHtiStatus = HBufC::NewL( KMaxDisplayText );
    iHtiVersionCaption = HBufC::NewL( KMaxDisplayText );
    iHtiVersion = HBufC::NewL( KMaxDisplayText );
    iHtiAutoStartCaption = HBufC::NewL( KMaxDisplayText );
    iHtiAutoStartStatus = HBufC::NewL( KMaxDisplayText );
    iHtiSelectedCommCaption = HBufC::NewL( KMaxDisplayText );
    iHtiSelectedComm = HBufC::NewL( KMaxDisplayText );
    iHtiCommDetails = HBufC::NewL( KMaxDisplayText );

    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect(aRect);

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// Draw this application's view to the screen
void CHtiAdminAppView::Draw(const TRect& /*aRect*/) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect rect = Rect();

    // Clears the screen
    gc.Clear(rect);

    //draw text
    const CFont* f = AknLayoutUtils::FontFromId( EAknLogicalFontSecondaryFont );
    gc.UseFont( f );

    TInt leftMargin = f->MaxNormalCharWidthInPixels() / 4;

    TPoint dP( leftMargin, f->FontLineGap() );
    gc.DrawText( *iHtiCaption, dP );
    dP.iX += f->TextWidthInPixels( *iHtiCaption );
    gc.DrawText( *iHtiStatus, dP );

    dP.iX = leftMargin;
    dP.iY += f->FontLineGap() * 1.5;
    gc.DrawText( *iHtiVersionCaption, dP );
    dP.iX += f->TextWidthInPixels( *iHtiVersionCaption );
    gc.DrawText( *iHtiVersion, dP );

    dP.iX = leftMargin;
    dP.iY += f->FontLineGap() * 1.5;
    gc.DrawText( *iHtiAutoStartCaption, dP );
    dP.iX += f->TextWidthInPixels( *iHtiAutoStartCaption );
    gc.DrawText( *iHtiAutoStartStatus, dP );

    dP.iX = leftMargin;
    dP.iY += f->FontLineGap() * 1.5;
    gc.DrawText( *iHtiSelectedCommCaption, dP );
    dP.iX += f->TextWidthInPixels( *iHtiSelectedCommCaption );
    gc.DrawText( *iHtiSelectedComm, dP );
    dP.iX = leftMargin;
    dP.iY += f->FontLineGap();
    gc.DrawText( *iHtiCommDetails, dP );
    }

void CHtiAdminAppView::SetCaption( const TDesC& aText )
    {
    iHtiCaption->Des().Copy( aText );
    DrawDeferred();
    }

void CHtiAdminAppView::SetStatus( const TDesC& aText )
    {
    iHtiStatus->Des().Copy( aText );
    DrawDeferred();
    }

void CHtiAdminAppView::SetVersionCaption( const TDesC& aText )
    {
    iHtiVersionCaption->Des().Copy( aText );
    DrawDeferred();
    }

void CHtiAdminAppView::SetVersion( const TDesC& aText )
    {
    iHtiVersion->Des().Copy( aText );
    DrawDeferred();
    }

void CHtiAdminAppView::SetAutoStartCaption( const TDesC& aText )
    {
    iHtiAutoStartCaption->Des().Copy( aText );
    DrawDeferred();
    }

void CHtiAdminAppView::SetAutoStartStatus( const TDesC& aText )
    {
    iHtiAutoStartStatus->Des().Copy( aText );
    DrawDeferred();
    }

void CHtiAdminAppView::SetSelectedCommCaption( const TDesC& aText )
    {
    iHtiSelectedCommCaption->Des().Copy( aText );
    DrawDeferred();
    }

void CHtiAdminAppView::SetSelectedComm( const TDesC& aText )
    {
    iHtiSelectedComm->Des().Copy( aText );
    DrawDeferred();
    }

void CHtiAdminAppView::SetCommDetails( const TDesC& aText )
    {
    iHtiCommDetails->Des().Copy( aText );
    DrawDeferred();
    }

// End of file
