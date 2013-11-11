/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <AknFontAccess.h>
#include <eikenv.h>
#include <glxtracer.h>
#include <glxlog.h>
#include "glxhdmicontainer.h"

const TInt KGroupNameLength = 32;
_LIT(KPhotosHdmi,"PhotosHdmi");

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
CGlxHdmiContainer* CGlxHdmiContainer::NewL(const TRect& aRect)
    {
    TRACER("CGlxHdmiContainer* CGlxHdmiContainer::NewL()");
    CGlxHdmiContainer* self = new (ELeave) CGlxHdmiContainer(aRect);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// destructor 
// -----------------------------------------------------------------------------
CGlxHdmiContainer::~CGlxHdmiContainer()
    {
    TRACER("CGlxHdmiContainer::~CGlxHdmiContainer()");
    iWsWindowGroup.Close();
    delete iWindowGc;
    delete iScreenDevice;
    }

// -----------------------------------------------------------------------------
// CTor 
// -----------------------------------------------------------------------------
CGlxHdmiContainer::CGlxHdmiContainer(const TRect& aRect):
    iRect(aRect)
    {
    TRACER("CGlxHdmiContainer::CGlxHdmiContainer()");
    // Implement nothing here
    }

// -----------------------------------------------------------------------------
// ConstructL 
// -----------------------------------------------------------------------------
void CGlxHdmiContainer::ConstructL()
    {
    TRACER("CGlxHdmiContainer::ConstructL()");
    CreateHdmiWindowL();
    ActivateL();
    }

// ----------------------------------------------------------
// GetWindow()
// ----------------------------------------------------------
//
RWindow* CGlxHdmiContainer::GetWindow()
    {
    TRACER("CGlxHdmiContainer::GetWindow()");
    return &iWsWindow;   
    }

// -----------------------------------------------------------------------------
// CreateHdmiWindowL 
// -----------------------------------------------------------------------------
void CGlxHdmiContainer::CreateHdmiWindowL()
    {
    TRACER ("CGlxHdmiContainer::CreateHdmiWindowL()");
    const TUint32 id = 12345678;
    
    // Create screen device and gc
    iSession = ControlEnv()->WsSession();
    iScreenDevice = new(ELeave) CWsScreenDevice(iSession);
    User::LeaveIfError(iScreenDevice->Construct(1));        // use Screen 1
    User::LeaveIfError(iScreenDevice->CreateContext(iWindowGc));
    
    // Create window group
    iWsWindowGroup = RWindowGroup(iSession);
    User::LeaveIfError(iWsWindowGroup.Construct(id, iScreenDevice));
    iWsWindowGroup.SetOrdinalPosition(0);
    TBuf<KGroupNameLength> winGroupName(KPhotosHdmi);
    iWsWindowGroup.SetName(winGroupName); 
    iWsWindowGroupID = iWsWindowGroup.Identifier();

    // Create window
    CreateWindowL(iWsWindowGroup);
    iWsWindow = Window();
    SetRect(TRect(iScreenDevice->SizeInPixels()));
    }

// -----------------------------------------------------------------------------
// ConstructL 
// -----------------------------------------------------------------------------
void CGlxHdmiContainer::Draw()
    {
    TRACER("CGlxHdmiContainer::Draw()");
    CWindowGc& gc = SystemGc();
    gc.DrawRect(iRect);
    }

// -----------------------------------------------------------------------------
// ConstructL 
// -----------------------------------------------------------------------------
void CGlxHdmiContainer::DoGenCallback()
    {
    TRACER("CGlxHdmiContainer::DoGenCallback()");
    DrawNow(); 
    }
