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
* Description:   DRM utility implementation
*
*/



/**
 * @internal reviewed 03/07/2007 by Rowland Cook
 */

// INCLUDES

#include "glxdrmutility.h"

#include <glxpanic.h>

#include <DRMHelper.h>
#include <DRMCommon.h>

#include <caf/caf.h>
#include <caf/content.h>
#include <caf/data.h>
#include <caf/manager.h>
#include "glxtracer.h"

const TInt KGlxDRMThumbnailHeight = 120;
const TInt KGlxDRMThumbnailWidth = 90;

// ======== MEMBER FUNCTIONS ========

/**
 * CGlxDrmTls
 *
 * Global object stored in TLS. 
 * Owns the only instance of a CGlxDRMUtility
 */
struct CGlxDrmTls
    {
    CGlxDrmTls(CGlxDRMUtility* aUtility) 
        {
        iDrmUtility = aUtility;
        iRefCount = 0;
        }
        
    CGlxDRMUtility* iDrmUtility;
    TInt iRefCount;
    };

//============================================================================
// InstanceL
//============================================================================
EXPORT_C CGlxDRMUtility* CGlxDRMUtility::InstanceL()
    {
    TRACER("CGlxDRMUtility::InstanceL()");
    CGlxDrmTls* tls = reinterpret_cast<CGlxDrmTls*>(Dll::Tls());
    
    if ( !tls )
        {
        CGlxDRMUtility* drmutil = new(ELeave)CGlxDRMUtility();
        CleanupStack::PushL(drmutil);
        drmutil->ConstructL();
        
        tls = new (ELeave) CGlxDrmTls(drmutil);
        
        CleanupStack::Pop(drmutil);
        
        Dll::SetTls( reinterpret_cast<TAny*>(tls));
        }
        
    tls->iRefCount++;
    
    return tls->iDrmUtility;
    }
    
//============================================================================
// Close
//============================================================================ 
EXPORT_C void CGlxDRMUtility::Close()
    {
    TRACER("CGlxDRMUtility::Close()");
    CGlxDrmTls* tls = reinterpret_cast<CGlxDrmTls*>(Dll::Tls());
   
    if ( tls ) 
        {
        tls->iRefCount--;
        
        // Delete the tls pointer and list manager instance if this was the 
        // last reference
        if (tls->iRefCount == 0)
            {
            delete tls->iDrmUtility;
            delete tls;
            Dll::SetTls(NULL);
            }
        }
    }

//============================================================================
// Destructor
//============================================================================ 
CGlxDRMUtility::~CGlxDRMUtility()
    {
    TRACER("CGlxDRMUtility::~CGlxDRMUtility()");
    delete iCManager;
    delete iDrmHelper;
    delete iLastConsumedItemUri;
    }
    
//============================================================================
// ItemRightsValidityCheckL
// for checking DRM rights validity for item.
// is called before right is consumed and for all items (focused or unfocused).
//============================================================================
EXPORT_C TBool CGlxDRMUtility::ItemRightsValidityCheckL( const TDesC& aUri,
                                                TBool aCheckViewRights )
    {
    TRACER("CGlxDRMUtility::ItemRightsValidityCheckL()");
    // When checking current rights for a URI

    //Allow to Open if rights for a URI was just consumed (i.e. same as stored URI)
	if ( iLastConsumedItemUri->Length() > 0 )
		{
		if ( aUri.CompareF( *iLastConsumedItemUri ) == 0 )
			{
			return ETrue;
			}
		}

	//Else for uri of non-focused uri, just check validity rights
    TInt rightsValid = 0;
    TVirtualPathPtr path( aUri, KDefaultContentObject() );

    ContentAccess::TAttribute attr = 
        aCheckViewRights ? ContentAccess::ECanView : ContentAccess::ECanPlay;

    // rightsValid is not updated if an error occurs
    iCManager->GetAttribute( attr, rightsValid, path );
    
    return rightsValid;
    }

//============================================================================
// ItemRightsValidityCheckL
// for checking DRM rights validity for item.
// is called before right is consumed and for all items (focused or unfocused).
//============================================================================
EXPORT_C TBool CGlxDRMUtility::ItemRightsValidityCheckL( RFile& aFileHandle,
                                                TBool /*aCheckViewRights*/ )
    {
    TRACER("CGlxDRMUtility::ItemRightsValidityCheckL()");
    TBool rightsValid = EFalse;

    ContentAccess::CContent* content = ContentAccess::CContent::NewLC( aFileHandle );
    content->GetAttribute( ContentAccess::ECanView, rightsValid );
    CleanupStack::PopAndDestroy( content );
    return rightsValid;
    }

//============================================================================
// DisplayItemRightsCheckL
// is called after right is consumed and for only focused/displayed item.
//============================================================================
EXPORT_C TBool CGlxDRMUtility::DisplayItemRightsCheckL( const TDesC& aUri,
                                                    TBool aCheckViewRights )
    {
    TRACER("CGlxDRMUtility::DisplayItemRightsCheckL()");

    // Allow to display if rights for a URI was just consumed (i.e. same as stored URI)
    if ( iLastConsumedItemUri->Length() > 0 )
        {
        if ( aUri.CompareF( *iLastConsumedItemUri ) == 0 )
            {
            return ETrue;
            }
        }

    //Clear the stored uri since focus has changed
    ClearLastConsumedItemUriL();

    // Otherwise, check current rights for the URI of newly focused item
    return ItemRightsValidityCheckL( aUri, aCheckViewRights );
    }

//============================================================================
// DisplayItemRightsCheckL
// is called after right is consumed and for only focused/displayed item.
//============================================================================
EXPORT_C TBool CGlxDRMUtility::DisplayItemRightsCheckL( RFile& aFileHandle,
                                                    TBool aCheckViewRights )
    {
    TRACER("CGlxDRMUtility::DisplayItemRightsCheckL()");
    // Otherwise, check current rights for the URI of newly focused item
    return ItemRightsValidityCheckL( aFileHandle, aCheckViewRights );
    }


//============================================================================
// ConsumeRightsL
//============================================================================    
EXPORT_C TBool CGlxDRMUtility::ConsumeRightsL(const TDesC& aUri)
    {
    TRACER("CGlxDRMUtility::ConsumeRightsL()");
    TVirtualPathPtr path( aUri, KDefaultContentObject() );

    // Create a CData object to read the content
    // Tell the agent we are planning to display the content
    CData* data = CData::NewLC(path, ContentAccess::EView, EContentShareReadOnly);

    //When consuming rights for a URI, clear stored URI
    ClearLastConsumedItemUriL();

    // Execute the intent, tell the agent that we plan to display the content
    // It is at this point that any stateful rights will be decremented
    TInt err = data->ExecuteIntent(ContentAccess::EView);
    if ( err == KErrNone )
        {
        //Update stored URI
        iLastConsumedItemUri = iLastConsumedItemUri->ReAllocL( aUri.Length() );
        TPtr newPtr = iLastConsumedItemUri->Des();
        newPtr.Copy( aUri );
        }

    CleanupStack::PopAndDestroy(data);

    return (err == KErrNone);
    }


//============================================================================
// ConsumeRightsL
//============================================================================    
EXPORT_C TBool CGlxDRMUtility::ConsumeRightsL(RFile& aFileHandle)
    {
    TRACER("CGlxDRMUtility::ConsumeRightsL(RFile& aFileHandle)");
    CData* data = CData::NewLC( aFileHandle, KDefaultContentObject(), EPeek );
    TInt err = data->ExecuteIntent(ContentAccess::EView);
    CleanupStack::PopAndDestroy( data );
    return (err == KErrNone);
    }

//============================================================================
//Clears Last Consumed Uri
//============================================================================
EXPORT_C void CGlxDRMUtility::ClearLastConsumedItemUriL()
	{
	//clears the stored uri
	TPtr ptr = iLastConsumedItemUri->Des();
	ptr.Zero();
	iLastConsumedItemUri = iLastConsumedItemUri->ReAllocL( 0 );
	}

//============================================================================
// Test whether a media item is OMA DRM 2.0 protected and has an associated
// info URL.
//============================================================================
EXPORT_C TBool CGlxDRMUtility::CanShowInfoOnlineL(const TDesC& aUri)
    {
    TRACER("CGlxDRMUtility::CanShowInfoOnlineL()");
    TBool canShowInfoOnline = EFalse;

    HBufC8* urlBuf = NULL;
    canShowInfoOnline = iDrmHelper->HasInfoUrlL( const_cast<TDesC&>(aUri) , urlBuf);

    // discard buf we don't need it
    delete urlBuf;
    
    return canShowInfoOnline;
    }

//============================================================================
// Open the associated info URL for a media item in the browser.
//============================================================================
EXPORT_C void CGlxDRMUtility::ShowInfoOnlineL(const TDesC& aUri)
    {
    TRACER("CGlxDRMUtility::ShowInfoOnlineL()");
    iDrmHelper->OpenInfoUrlL( const_cast<TDesC&>(aUri) );
    }

//============================================================================
// Test whether a media item can be set as automated content.
//============================================================================
EXPORT_C TBool CGlxDRMUtility::CanSetAsAutomatedL(const TDesC& aUri, 
                                    TGlxDrmAutomatedType aType)
    {
    TRACER("CGlxDRMUtility::CanSetAsAutomatedL()");
    TBool canSetAutomated = EFalse;
    switch(aType)
        {
    case EGlxDrmAutomatedTypeWallpaper:
        {
        User::LeaveIfError(iDrmHelper->SetAutomatedType(CDRMHelper::EAutomatedTypeWallpaper));
        User::LeaveIfError(iDrmHelper->CanSetAutomated(aUri, canSetAutomated));
        break;
        }
    default:
        break;
        };
    return canSetAutomated;
    }

//============================================================================
// SetAsAutomatedL
//============================================================================
EXPORT_C void CGlxDRMUtility::SetAsAutomatedL(const TDesC& aUri, 
                                    TGlxDrmAutomatedType aType)
    {
    TRACER("CGlxDRMUtility::SetAsAutomatedL()");
     switch(aType)
        {
    case EGlxDrmAutomatedTypeWallpaper:
        {
        TInt error = iDrmHelper->SetAutomatedType(CDRMHelper::EAutomatedTypeWallpaper);
        if(KErrNone==error)
            {
            error= iDrmHelper->SetAutomatedPassive(aUri);
            if(KErrNone!=error)
                {
                iDrmHelper->HandleErrorL(error, aUri);
                }
            }
        
        break;
        }
    default:
        break;
        };
    }

//============================================================================
// ShowDRMDetailsPane
//============================================================================  
EXPORT_C void CGlxDRMUtility::ShowDRMDetailsPaneL( const TDesC& aUri )
    {
    TRACER("CGlxDRMUtility::ShowDRMDetailsPaneL()");
    TRAPD( err, iDrmHelper->LaunchDetailsViewEmbeddedL( aUri ) );
    // if no rights ask user to re-activate
    if( err == KErrCANoRights )
        {
        HBufC* buf = aUri.AllocLC();
        iDrmHelper->ActivateContentL( *buf );        
        CleanupStack::PopAndDestroy( buf );
        }

    }

//============================================================================
// ShowDRMDetailsPane
//============================================================================  
EXPORT_C void CGlxDRMUtility::ShowDRMDetailsPaneL(RFile& aFileHandle)
    {
    TRACER("CGlxDRMUtility::ShowDRMDetailsPaneL()");
    TRAPD( err, iDrmHelper->LaunchDetailsViewEmbeddedL( aFileHandle ) );
    // if no rights ask user to re-activate
    if( err == KErrCANoRights )
        {
        //need to check if we need to handle.
        }

    }

//============================================================================
// IsForwardLockedL
//============================================================================  
EXPORT_C TBool CGlxDRMUtility::IsForwardLockedL(const TDesC& aUri)
    {
    TRACER("CGlxDRMUtility::IsForwardLockedL()");
    TBool forwardLocked = EFalse;
    TVirtualPathPtr path( aUri, KDefaultContentObject() );

    // forwardLocked is not updated if an error occurs
    iCManager->GetAttribute( ContentAccess::EIsForwardable, forwardLocked, path );

    return forwardLocked;
    }
    
//============================================================================
// ShowRightsInfoL
//============================================================================    
EXPORT_C void CGlxDRMUtility::ShowRightsInfoL(const TDesC& aUri)    
    {
    TRACER("CGlxDRMUtility::ShowRightsInfoL()");
    iDrmHelper->CheckRightsAmountL( aUri );
    }

//============================================================================
// ShowRightsInfoL
//============================================================================    
EXPORT_C void CGlxDRMUtility::ShowRightsInfoL(RFile& aFileHandle)    
    {
    TRACER("CGlxDRMUtility::ShowRightsInfoL(aFileHandle)");
    iDrmHelper->CheckRightsAmountL( aFileHandle );
    }

//============================================================================
// Return size for DRM thumbnail request.
//============================================================================
EXPORT_C TSize CGlxDRMUtility::DRMThumbnailSize(TSize& aSize)
    {
    TRACER("CGlxDRMUtility::DRMThumbnailSize()");
    TSize thumbnailSize(KGlxDRMThumbnailWidth, KGlxDRMThumbnailHeight);
    
    if((aSize.iWidth*aSize.iHeight)/4 < 
            KGlxDRMThumbnailWidth * KGlxDRMThumbnailHeight)
        {
        thumbnailSize.iWidth = aSize.iWidth/2;
        thumbnailSize.iHeight = aSize.iHeight/2;
        }
    
    return thumbnailSize;
    }
    
/**
 * C++ default constructor.
 */
CGlxDRMUtility::CGlxDRMUtility()
    {
    TRACER("CGlxDRMUtility::CGlxDRMUtility()");
    }

/**
 * By default Symbian 2nd phase constructor is private.
 */
void CGlxDRMUtility::ConstructL()
    {
    TRACER("CGlxDRMUtility::ConstructL()");
    iCManager = ContentAccess::CManager::NewL();
    iDrmHelper = CDRMHelper::NewL();
    iLastConsumedItemUri = HBufC::NewL(0);
    }


// End of File
