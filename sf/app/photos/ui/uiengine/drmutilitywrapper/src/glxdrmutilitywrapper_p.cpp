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
* Description: 
*
*/

#include"glxdrmutilitywrapper_p.h"
#include"glxdrmutility.h"

#include <QString>
#include <QDir>

GlxDRMUtilityWrapperPrivate::GlxDRMUtilityWrapperPrivate(GlxDRMUtilityWrapper* DRMUtilityWrapper)
    {
    iDRMUtilityWrapper = DRMUtilityWrapper;
    iDRMUtility = CGlxDRMUtility::InstanceL();
    }
/*
 * Destructor
 */
GlxDRMUtilityWrapperPrivate::~GlxDRMUtilityWrapperPrivate()
    {
    if(iDRMUtility)
        {
        iDRMUtility->Close();
        }
    }

/**
 * Check whether DRM rights are valid for specified item
 * is called before right is consumed and for all items (focused or unfocused).
 */
bool GlxDRMUtilityWrapperPrivate::ItemRightsValidityCheck(QString aUri, bool aCheckViewRights)
    {
    QString filepath(QDir::toNativeSeparators(aUri));
    TPtrC16 str(reinterpret_cast<const TUint16*> (filepath.utf16()));
    HBufC* uri = str.Alloc();
    TBool checkViewRight = aCheckViewRights?ETrue:EFalse;
    bool ret = iDRMUtility->ItemRightsValidityCheckL(*uri,checkViewRight);
    return ret;
    }

///**
// * Check whether DRM rights are valid for specified item
// * is called before right is consumed and for all items (focused or unfocused).
// */
//bool GlxDRMUtilityWrapperPrivate::ItemRightsValidityCheckL(RFile& aFileHandle, TBool aCheckViewRights)
//    {
//    
//    }
//
///**
// * Check whether DRM rights are valid for specified item
// * If the rights were just consumed, then allow to display
// * Otherwise, obtain current rights
// * is called after right is consumed and for only focused/displayed item.
// */
//bool GlxDRMUtilityWrapperPrivate::DisplayItemRightsCheckL(const TDesC& aUri, TBool aCheckViewRights)
//    {
//    
//    }
//
///**
// * Check whether DRM rights are valid for specified item
// * If the rights were just consumed, then allow to display
// * Otherwise, obtain current rights
// * is called after right is consumed and for only focused/displayed item.
// */
//bool GlxDRMUtilityWrapperPrivate::DisplayItemRightsCheckL(RFile& aFileHandle, TBool aCheckViewRights)
//    {
//    
//    }
//
/**
  * Consume rights for specified item
  * Caches item so that a client has right to display the item
  */
bool GlxDRMUtilityWrapperPrivate::ConsumeRights(QString aUri)
    {
    QString filepath(QDir::toNativeSeparators(aUri));
    TPtrC16 str(reinterpret_cast<const TUint16*> (filepath.utf16()));
    HBufC* uri = str.Alloc();
    bool ret = iDRMUtility->ConsumeRightsL(*uri);
    return ret;
    }
//
///**
//  * Consume rights for specified item
//  * Caches item so that a client has right to display the item
//  */
//bool GlxDRMUtilityWrapperPrivate::ConsumeRightsL(RFile& aFileHandle)
//    {
//    
//    }
//
///**
// * Clears Last Consumed Uri
// */
//void GlxDRMUtilityWrapperPrivate::ClearLastConsumedItemUriL()
//    {
//    
//    }
//
///**
// * Test whether a media item is OMA DRM 2.0 protected and has an associated
// * info URL.
// */
//bool GlxDRMUtilityWrapperPrivate::CanShowInfoOnlineL(const TDesC& aUri)
//    {
//    
//    }
//
///**
// * Open the associated info URL for a media item in the browser.
// */
//void GlxDRMUtilityWrapperPrivate::ShowInfoOnlineL(const TDesC& aUri)
//    {
//    
//    }
//
///**
// * Test whether a media item can be set as automated content.     *
// */
//bool GlxDRMUtilityWrapperPrivate::CanSetAsAutomatedL(const TDesC& aUri, TGlxDrmAutomatedType aType)
//    {
//    
//    }
//
///**
// * Set a media item as automated content.
// */
//void GlxDRMUtilityWrapperPrivate::SetAsAutomatedL(const TDesC& aUri, TGlxDrmAutomatedType aType)
//    {
//    
//    }
//
///**
// * Show DRM details for specified item.
// */
//void GlxDRMUtilityWrapperPrivate::ShowDRMDetailsPaneL(const TDesC& aUri)
//    {
//    
//    }
//
///**
// * Show DRM details for specified item.
// */
//void GlxDRMUtilityWrapperPrivate::ShowDRMDetailsPaneL(RFile& aFileHandle)
//    {
//    
//    }
//
///**
//  * Ask DRM manager if file is forward locked
//  */
//bool GlxDRMUtilityWrapperPrivate::IsForwardLockedL(const TDesC& aUri)
//    {
//    
//    }
//
/**
  * Show rights info
  */
void GlxDRMUtilityWrapperPrivate::ShowRightsInfo(QString aUri)
    {
    QString filepath(QDir::toNativeSeparators(aUri));
    TPtrC16 str(reinterpret_cast<const TUint16*> (filepath.utf16()));
    HBufC* uri = str.Alloc();
    iDRMUtility->ShowRightsInfoL(*uri);
    }

///**
//  * Show rights info
//  */
//void GlxDRMUtilityWrapperPrivate::ShowRightsInfoL(RFile& aFileHandle)
//    {
//    
//    }
//
///**
//  * Get size of thumbnail to be requested for DRM invalid item
//  */
//TSize GlxDRMUtilityWrapperPrivate::DRMThumbnailSize(TSize& aSize)
//    {
//    
//    }
