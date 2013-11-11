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
* Description:   Allows change the screen size
*
*/


/**
 * @internal reviewed 24/08/2007 by D Holland
 */

//  CLASS HEADER
#include "glxresolutionmanager.h"
    
//  EXTERNAL INCLUDES

//  INTERNAL INCLUDES
#include "glxresolutionutility.h"
#include <glxlog.h>

// ---------------------------------------------------------------------------
// CGlxResolutionManager, inlined to save a few bytes of ROM
// ---------------------------------------------------------------------------
//
inline CGlxResolutionManager::CGlxResolutionManager()
    {
    }

// ---------------------------------------------------------------------------
// CGlxResolutionManager::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxResolutionManager* CGlxResolutionManager::NewL()
    {
    GLX_LOG_INFO("CGlxResolutionManager::NewL");
    CGlxResolutionManager* self = new ( ELeave ) CGlxResolutionManager();
    CleanupStack::PushL( self );
    self->iResUtility = CGlxResolutionUtility::InstanceL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CGlxResolutionManager
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxResolutionManager::~CGlxResolutionManager()
    {
    GLX_LOG_INFO("CGlxResolutionManager::~CGlxResolutionManager");
    if (iResUtility)
        {
        iResUtility->Close();
        iResUtility = NULL;
        }
    }

// ---------------------------------------------------------------------------
// SetScreenSize
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxResolutionManager::SetScreenSize( TSize aScreenSize )
    {
    GLX_LOG_INFO2(
        "CGlxResolutionManager::SetScreenSize (%d,%d)", 
        aScreenSize.iWidth, aScreenSize.iHeight );
    iResUtility->SetScreenSize( aScreenSize );	                       
    }


