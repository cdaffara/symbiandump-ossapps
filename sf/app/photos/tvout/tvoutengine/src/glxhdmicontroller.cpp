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

#include <w32std.h>
#include <glxtracer.h>
#include <glxlog.h>

#include "glxhdmicontainer.h"
#include "glxhdmisurfaceupdater.h"

#include "glxhdmicontroller.h"

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
EXPORT_C CGlxHdmiController* CGlxHdmiController::NewL(TRect aRect)
    {
    TRACER("CGlxHdmiController* CGlxHdmiController::NewL()");
    CGlxHdmiController* self = new (ELeave) CGlxHdmiController(aRect);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// destructor 
// -----------------------------------------------------------------------------
EXPORT_C CGlxHdmiController::~CGlxHdmiController()
    {
    TRACER("CGlxHdmiController::~CGlxHdmiController()");
    DestroySurfaceUpdater();
    DestroyContainer();
    delete iStoredImagePath;
    iStoredImagePath = NULL;
    if (iFsBitmap)
        {
        delete iFsBitmap;
        iFsBitmap = NULL;
        }
    }

// -----------------------------------------------------------------------------
// Setting an Image Path 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::SetImageL(const TDesC& aImageFile, CFbsBitmap* aFsBitmap,
                                             TBool aStore)
    {
    TRACER("CGlxHdmiController::SetImageL()");
    if (aFsBitmap == NULL || !aImageFile.Length())
        {
        GLX_LOG_INFO("CGlxHdmiController::SetImageL() - NULL Uri");
        return;
        }
    if (aStore)
        {
        iIsImageSupported = ETrue;
        StoreImageInfoL( aImageFile, aFsBitmap );
        }
    GLX_LOG_INFO("CGlxHdmiController::SetImageL() - 2");
    // do not close the surface , use the same surface instead.
    // Call a function to pass imagefile
    if (!iHdmiContainer)
        {            
        CreateHdmiContainerL(); 
        }            
    if (!iSurfaceUpdater)
        {
        // This case would come when surface updater is not created at the first instance and also
        // it satisfies the 720p condition                
        CreateSurfaceUpdaterL(aImageFile);
        }
    else
        {
        GLX_LOG_INFO("CGlxHdmiController::SetImageL() - 3");
        iSurfaceUpdater->UpdateNewImageL(aImageFile, aFsBitmap);
        }
    iHdmiContainer->DrawNow();
    }

// -----------------------------------------------------------------------------
// ItemNotSupported 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::ItemNotSupported()
    {
    TRACER("CGlxHdmiController::IsVideo()");
    iIsImageSupported = EFalse;
    DestroySurfaceUpdater();
    }


// -----------------------------------------------------------------------------
// ActivateZoom 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::ActivateZoom(TBool aAutoZoomOut)
    {
    TRACER("CGlxHdmiController::ActivateZoom()");
    if (iSurfaceUpdater)
        {
        iSurfaceUpdater->ActivateZoom(aAutoZoomOut);
        }
    }

// -----------------------------------------------------------------------------
// DeactivateZoom 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::DeactivateZoom()
    {
    TRACER("CGlxHdmiController::DeactivateZoom()");
    if (iSurfaceUpdater)
        {
        iSurfaceUpdater->DeactivateZoom();
        }
    }

// -----------------------------------------------------------------------------
// ShiftToCloningMode 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::ShiftToCloningMode()
    {
    TRACER("CGlxHdmiController::ShiftToCloningMode()");
    // Shift to cloning only if HDMI is connected and surfaceupdater available.
    if (iSurfaceUpdater)
        {
        iSurfaceUpdater->ShiftToCloningMode();
        }
    }

// -----------------------------------------------------------------------------
// ShiftToPostingMode 
// -----------------------------------------------------------------------------
EXPORT_C void CGlxHdmiController::ShiftToPostingMode()
    {
    TRACER("CGlxHdmiController::ShiftToPostingMode()");
    if (!iSurfaceUpdater)
        {
        GLX_LOG_INFO("CGlxHdmiController::ShiftToPostingMode() - 1");
        // This case would come when HDMI connected, TvOut /headphones being connected
        // and then it shows a popup of "microphone connected" 
        // thus Background - Foreground when headphones connected during HDMI connected
        if (iFsBitmap == NULL || !iStoredImagePath->Length())
            {
            GLX_LOG_INFO("CGlxHdmiController::ShiftToPostingMode() - NULL Uri");
            return;
            }
        SetImageL(iStoredImagePath->Des(), iFsBitmap, EFalse);
        }
    else
        {
        GLX_LOG_INFO("CGlxHdmiController::ShiftToPostingMode() - 2");
        iSurfaceUpdater->ShiftToPostingMode();
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CGlxHdmiController::CGlxHdmiController(TRect aRect):
            iFsBitmap(NULL),
            iStoredImagePath(NULL),
            iRect(aRect)
    {
    TRACER("CGlxHdmiController::CGlxHdmiController()");
    // Implement nothing here
    }

// -----------------------------------------------------------------------------
// ConstructL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::ConstructL()
    {
    TRACER("CGlxHdmiController::ConstructL()");
    }

// -----------------------------------------------------------------------------
// DestroyContainer 
// -----------------------------------------------------------------------------
void CGlxHdmiController::DestroyContainer()
    {
    TRACER("CGlxHdmiController::DestroyContainer()");
    if (iHdmiContainer)
        {
        GLX_LOG_INFO("CGlxHdmiController::DestroyHdmi() - deleting iHdmiContainer 1");
        delete iHdmiContainer;
        iHdmiContainer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// DestroySurfaceUpdater 
// -----------------------------------------------------------------------------
void CGlxHdmiController::DestroySurfaceUpdater()
    {
    TRACER("CGlxHdmiController::DestroySurfaceUpdater()");
    if (iSurfaceUpdater)
        {
        delete iSurfaceUpdater;
        iSurfaceUpdater = NULL;
        }    
    }

// -----------------------------------------------------------------------------
// CreateHdmiContainerL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::CreateHdmiContainerL()
    {
    TRACER("CGlxHdmiController::CreateHdmiContainer()");
    iHdmiContainer = CGlxHdmiContainer::NewL(iRect);
    }

// -----------------------------------------------------------------------------
// CreateSurfaceUpdaterL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::CreateSurfaceUpdaterL(const TDesC& aImageFile)
    {
    TRACER("CGlxHdmiController::CreateSurfaceUpdater()");
    iSurfaceUpdater = CGlxHdmiSurfaceUpdater::NewL(iHdmiContainer->GetWindow(), 
                                                aImageFile, iFsBitmap,iHdmiContainer);
    iHdmiContainer->DrawNow();
    }

// -----------------------------------------------------------------------------
// StoreImageInfoL 
// -----------------------------------------------------------------------------
void CGlxHdmiController::StoreImageInfoL(const TDesC& aImageFile, CFbsBitmap* aFsBitmap)
    {
    TRACER("CGlxHdmiController::StoreImageInfoL()");
    if(iStoredImagePath)
        {
        delete iStoredImagePath;
        iStoredImagePath = NULL;
        }
    if (iFsBitmap)
        {
        delete iFsBitmap;
        iFsBitmap = NULL;
        }
    iStoredImagePath = aImageFile.AllocL();
    iFsBitmap = new (ELeave) CFbsBitmap;
    iFsBitmap->Duplicate(aFsBitmap->Handle());
    }

