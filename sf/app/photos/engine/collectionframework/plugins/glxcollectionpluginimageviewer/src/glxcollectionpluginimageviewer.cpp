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
* Description:    This class browses file system
*
*/




// INCLUDE FILES
#include <e32cmn.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxcmn.h>
#include <mpxcollectionmessagedefs.h>
#include <glxcollectionmessagedefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <glxpluginimageviewer.rsg>
#include <StringLoader.h>
#include <glxtracer.h>

#include "glxcollectionpluginimageviewer.h"
#include "glxcollectionpluginimageviewer.hrh"
#include <glxmediageneraldefs.h>
#include <glxmediacollectioninternaldefs.h>
#include <glxcollectiongeneraldefs.h>

#include <glxpanic.h>
#include <mglxdatasource.h>
#include <glxrequest.h>
#include <glxidlistrequest.h>
#include <glxfilterfactory.h>
#include <glxlog.h>

// ============================ LOCAL FUNCTIONS ==============================

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginImageViewer* CGlxCollectionPluginImageViewer::NewL(TAny* aObs)
    {
    CGlxCollectionPluginImageViewer* self = 
        new (ELeave) CGlxCollectionPluginImageViewer(
            static_cast<MMPXCollectionPluginObserver*>(aObs));
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginImageViewer::~CGlxCollectionPluginImageViewer()
    {
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginImageViewer::CGlxCollectionPluginImageViewer(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginImageViewer::ConstructL()
    {
    iDataSource = MGlxDataSource::OpenDataSourceL(KGlxDefaultDataSourceUid, *this);
    }

void CGlxCollectionPluginImageViewer::CpiAttributeAdditionalAttributes(
        const TMPXAttribute& /*aCpiAttribute*/, 
        RArray<TMPXAttribute>& /*aAttributeArray*/)
    {
    // Nothing to be shared apart from the ones that are commonly loaded.
    }

void CGlxCollectionPluginImageViewer::HandleCpiAttributeResponseL(
        CMPXMedia* aResponse, TArray<TMPXAttribute> aCpiAttributes, 
        TArray<TGlxMediaId> aMediaIds)
    {
    TRACER("CGlxCollectionPluginImageViewer::HandleCpiAttributeResponseL");
    const TInt mediaIdCount = aMediaIds.Count();
    
    switch (mediaIdCount)
        {
    case 0:
        User::Leave(KErrNotSupported);
        break;
    case 1:
        HandleCpiAttributeResponseL(aResponse, aCpiAttributes, aMediaIds[0]);
        break;
    default:
            {
            // We have an array of CMPXMedia items
            
            if (TGlxMediaId(KGlxCollectionRootId) == aMediaIds[0])
                {
                User::Leave(KErrNotSupported);
                }
                
            CMPXMediaArray* mediaArray = aResponse->ValueCObjectL<CMPXMediaArray>(KMPXMediaArrayContents);
            CleanupStack::PushL(mediaArray);

            const TInt arrayCount = mediaArray->Count();
            
            // Sanity check
            if (arrayCount != mediaIdCount)
                {
                User::Leave(KErrArgument);
                }
            
            for (TInt index = 0; index < arrayCount; index++)
                {
                HandleCpiAttributeResponseL((*mediaArray)[index], aCpiAttributes, aMediaIds[index]);
                }

            aResponse->SetCObjectValueL(KMPXMediaArrayContents, mediaArray);
            CleanupStack::PopAndDestroy(mediaArray);
            }
        break;
        }
    }

void CGlxCollectionPluginImageViewer::HandleCpiAttributeResponseL(
        CMPXMedia* /*aResponse*/, TArray<TMPXAttribute> aCpiAttributes,
        TGlxMediaId /*aMediaId*/)
    {
    const TInt attribCount = aCpiAttributes.Count();

    for (TInt index = 0; index < attribCount ; index++)
        {
        const TMPXAttribute attr = aCpiAttributes[index];
        
        if (attr == KGlxMediaCollectionPluginSpecificSubTitle)
            {
            User::Leave(KErrNotSupported);
            }
        else if (attr == KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle)
            {
            User::Leave(KErrNotSupported);
            }
        else if (attr == KGlxMediaCollectionPluginSpecificNewMediaItemTitle)
            {
            User::Leave(KErrNotSupported);
            }
        else if (attr == KGlxMediaCollectionPluginSpecificDefaultMediaTitle)
            {
            User::Leave(KErrNotSupported);
            }
        else if (attr == KMPXMediaGeneralTitle)
            {
            User::Leave(KErrNotSupported);
            }
        }
    }

TBool CGlxCollectionPluginImageViewer::IsUpdateMessageIgnored(CMPXMessage& /*aMessage*/)
    {
    TBool ignore = EFalse;
    return ignore;
    }

TGlxFilterProperties CGlxCollectionPluginImageViewer::DefaultFilter(TInt /*aLevel*/)
    {
    TGlxFilterProperties filterProperties;

    filterProperties.iSortDirection = EGlxFilterSortDirectionAscending;
    filterProperties.iMinCount = 1;
    filterProperties.iItemType = EGlxFilterImage;

    return filterProperties;
    }
