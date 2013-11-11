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
* Description:   MC Photos settings model
*
*/



// INCLUDE FILES

#include "glxsettingsmodel.h"

#include <centralrepository.h>
#include <featmgr.h>

#include <glxgallery.hrh>
#include <glxtracer.h>
#include "glxsingletonstore.h"

// ---------------------------------------------------------------------------
// InstanceL
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxSettingsModel* CGlxSettingsModel::InstanceL()
    {
    return CGlxSingletonStore::InstanceL(&NewL);
    }

// ---------------------------------------------------------------------------
// Close
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxSettingsModel::Close()
    {
    CGlxSingletonStore::Close(this);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxSettingsModel::~CGlxSettingsModel()
    {
    if ( iFeatureManagerInitialised )
        {
        FeatureManager::UnInitializeLib();
        }

    delete iRepository;
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CGlxSettingsModel* CGlxSettingsModel::NewL()
    {
    CGlxSettingsModel* self = new (ELeave) CGlxSettingsModel();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CGlxSettingsModel::CGlxSettingsModel()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CGlxSettingsModel::ConstructL()
    {
    TRACER( "CGlxSettingsModel::ConstructL()" );
    iRepository = CRepository::NewL( TUid::Uid(KGlxGalleryApplicationUid) );

    FeatureManager::InitializeLibL();
    iFeatureManagerInitialised = ETrue;
    }

// ---------------------------------------------------------------------------
// SupportedOrientations
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxSettingsModel::TSupportedOrientations
                                    CGlxSettingsModel::SupportedOrientations()
    {
    TInt value = ELandscapeAndPortrait;
    iRepository->Get( KGlxDefaultOrientation, value );

    return static_cast<TSupportedOrientations>( value );
    }

// ---------------------------------------------------------------------------
// IsTextEntryPortrait
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CGlxSettingsModel::IsTextEntryPortrait()
    {
    // Orientation changes to portrait for text entry in monoblock products
    return FeatureManager::FeatureSupported( KFeatureIdKeypadNoSlider );
    }

// ---------------------------------------------------------------------------
// ShowRotateInToolbar
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CGlxSettingsModel::ShowRotateInToolbar()
    {
    // View in landscape/portrait toolbar option shown only in monoblock products
    return FeatureManager::FeatureSupported( KFeatureIdKeypadNoSlider );
    }

// ---------------------------------------------------------------------------
// ShowSlideshowInToolbar
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CGlxSettingsModel::ShowSlideshowInToolbar()
    {
    // Slideshow toolbar option shown only in slide products
    return !FeatureManager::FeatureSupported( KFeatureIdKeypadNoSlider );
    }
