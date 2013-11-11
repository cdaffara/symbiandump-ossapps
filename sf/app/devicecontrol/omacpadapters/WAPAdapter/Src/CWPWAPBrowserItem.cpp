/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  One browser setting item.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <msvapi.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <CWPAdapter.h>
#include <centralrepository.h>
#include <browseruisdkcrkeys.h>
#include "CWPWAPBrowserItem.h"
#include "WPWAPItemFactory.h"
#include "ProvisioningDebug.h"

// CONSTANTS
/// Delay to wait for WmlBrowser to catch up with changes in CommsDb. Microseconds.
const TInt KWmlDelay = 4*500000;
// Force browser to use User Defined AP
const TInt KUserDefined = 0;

// -----------------------------------------------------------------------------
// CWPWAPBrowserItem::CWPWAPBrowserItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPWAPBrowserItem::CWPWAPBrowserItem( TBool aFirst,
                                      const TDesC& aTitle,
                                      const TDesC& aBookmarkTitle,
                                      const TDesC& aDefaultName, 
                                      const TDesC& aDefaultBookmarkName,
                                      CWPCharacteristic& aCharacteristic, 
                                      CCommsDatabase*& aCommsDb, 
                                      RCmManagerExt*& aCmManager,
                                      RFavouritesDb aBookmarkDb 
                                    )
                                    : CWPWAPItemBase( aTitle,
                                                      aDefaultName, 
                                                      aCharacteristic, 
                                                      aCommsDb, 
                                                      aCmManager ), 
                                    iDefaultBookmarkName( aDefaultBookmarkName ),
                                    iBookmarkTitle( aBookmarkTitle ), 
                                    iDb( aBookmarkDb ),
                                    iFirst( aFirst )
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPWAPBrowserItem::ConstructL()
    {
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPWAPBrowserItem* CWPWAPBrowserItem::NewL( TBool aFirst,
                                            const TDesC& aTitle, 
                                            const TDesC& aBookmarkTitle,
                                            const TDesC& aDefaultName,  
                                            const TDesC& aDefaultBookmarkName,
                                            CWPCharacteristic& aCharacteristic, 
                                            CCommsDatabase*& aCommsDb, 
                                            RCmManagerExt*& aCmManager,
                                            RFavouritesDb aBookmarkDb
                                            )
    {
    CWPWAPBrowserItem* self = new(ELeave) CWPWAPBrowserItem( aFirst,
                                                             aTitle, 
                                                             aBookmarkTitle, 
                                                             aDefaultName, 
                                                             aDefaultBookmarkName,
                                                             aCharacteristic, 
                                                             aCommsDb, 
                                                             aCmManager, 
                                                             aBookmarkDb ); 
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPWAPBrowserItem::~CWPWAPBrowserItem()
    {
    iBookmarks.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserItem::SaveL
// -----------------------------------------------------------------------------
//
void CWPWAPBrowserItem::SaveL()
    {
    CreateDbL();

    TPckg<TUint32> uid( iUID );
    // Check if the iLink->Data() contains an item of the same size as uid. 
    // This is an extra check to ensure that iLink->Data() contains a packaged
    // TUin32.
    if( iLink && iLink->Data().Length() == uid.MaxLength() )
        {
        uid.Copy( iLink->Data() );
        }
    RCmConnectionMethodExt cm;
    cm = iCmManager->ConnectionMethodL( iUID );
    CleanupClosePushL( cm );
    if( iAddr )
        {
        // Update the access point startpage
        WriteHomePageL( cm );
        }
    CleanupStack::PopAndDestroy(); // cm
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserItem::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CWPWAPBrowserItem::CanSetAsDefault() const
    {
    return iUID != KWPWAPNoUID;
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserItem::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CWPWAPBrowserItem::SetAsDefaultL()
    {
    FLOG( _L( "[Provisioning] CWPWAPBrowserItem::SetAsDefaultL:" ) );
    
  /*  CreateDbL();
    TRAPD( err, iAPHandler->SetAsDefaultL( (TInt)iUID, EIspTypeWAPOnly) );
    if( err == KErrLocked )
        {
        err = EWPCommsDBLocked;
        }
    // This function need to leave at the end of the function
    // otherwise we dont set the default AP when db is locked
    //User::LeaveIfError( err );
    
    // There's some delay before the WmlBrowser can find the stored 
    // accesspoint from CommsDb so therefore we wait for a while before
    // telling the WmlBrowser that default accesspoint should be changed.
    // The value 500000 is chosen just by trying out.
    User::After( KWmlDelay );

    CRepository* repository = CRepository::NewLC( KCRUidBrowser );
    FTRACE(RDebug::Print(
                _L("[Provisioning] CWPWAPBrowserItem::SetAsDefaultL: AP:(%d)"),
                                                                  (TInt)iUID));
    
    // Force the browser to use the User Defined AP selection mode.
    User::LeaveIfError( repository->Set( KBrowserAccessPointSelectionMode,
                                         KUserDefined ) );
    // This sets the accesspoint default for browser.
    User::LeaveIfError( repository->Set( KBrowserDefaultAccessPoint,
                                         (TInt)iUID ) );

    //Set the HomePage to default
    if(iUID)
        {
        User::LeaveIfError( repository->Set( KBrowserNGHomepageType, EBrowserCenRepAccessPoint) );
        }

    CleanupStack::PopAndDestroy(); // repository
    FLOG( _L( "[Provisioning] CWPWAPBrowserItem::SetAsDefaultL: Set Done" ) );

    User::LeaveIfError( err );*/
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserItem::ValidateL
// -----------------------------------------------------------------------------
//
TBool CWPWAPBrowserItem::ValidateL()
    {
    return iLink != NULL;
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserItem::ResourceL
// -----------------------------------------------------------------------------
//
void CWPWAPBrowserItem::ResourceL( CWPCharacteristic& aResource )
    {
    MWPWAPItemBase* item = WPWAPItemFactory::CreateBookmarkLC( 
                                                          iBookmarkTitle, 
                                                          iDefaultBookmarkName,
                                                          aResource, 
                                                          iLink, 
                                                          iDb );

    if( item->CanSetAsDefault() && !iAddr )
        {
        iAddr = item->Address().AllocL();
        CleanupStack::PopAndDestroy(); // item
        }
    else
        {
        User::LeaveIfError( iBookmarks.Append( item ) );
        CleanupStack::Pop(); // item
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserItem::AddL
// -----------------------------------------------------------------------------
//
void CWPWAPBrowserItem::AddL( RPointerArray<MWPWAPItemBase>& aShownItems,
                              RPointerArray<MWPWAPItemBase>& aHiddenItems )
    {
    for( TInt i( iBookmarks.Count()-1 ); i >= 0 ; i-- )
        {
        MWPWAPItemBase* item = iBookmarks[i];

        if( item->ValidateL() )
            {
            // Transfer ownership of iBookmarks[i] to aItems
            iBookmarks[i]->AddL( aShownItems, aHiddenItems );
            }
        else
            {
            // invalid items get deleted
            delete item;
            }
        iBookmarks.Remove( i );
        }

    // Only first browser item is shown to the user.
    if( iFirst )
        {
        User::LeaveIfError( aShownItems.Append( this ) );
        }
    else
        {
        User::LeaveIfError( aHiddenItems.Append( this ) );
        }
    }

//  End of File  
