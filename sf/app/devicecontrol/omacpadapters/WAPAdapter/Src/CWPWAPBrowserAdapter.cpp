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
* Description:  Handles browser settings in provisioning.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <favouritesdb.h>
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <wpwapadapterresource.rsg>
#include "CWPWAPBrowserAdapter.h"
#include "MWPWAPItemBase.h"
#include "WPWAPItemFactory.h"
#include "WPWAPUtil.h"
#include "WPWAPDebug.h"

// CONSTANTS
_LIT( KBrowserAppID, "w2" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPWAPBrowserAdapter::CWPWAPBrowserAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPWAPBrowserAdapter::CWPWAPBrowserAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPWAPBrowserAdapter::ConstructL()
    {
    BaseConstructL();

    iTitle = WPWAPUtil::ReadHBufCL( R_QTN_OP_HEAD_BROWSER );
    iBookmarkTitle = WPWAPUtil::ReadHBufCL( R_QTN_SM_BOOKMARKS );
    iDefaultName = WPWAPUtil::ReadHBufCL( R_QTN_SM_WAPAP_NAME );
    iDefaultBookmarkName = WPWAPUtil::ReadHBufCL( R_QTN_SM_BOOKMARK_NAME );
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPWAPBrowserAdapter* CWPWAPBrowserAdapter::NewL()
    {
    CWPWAPBrowserAdapter* self = new(ELeave) CWPWAPBrowserAdapter; 
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CWPWAPBrowserAdapter::~CWPWAPBrowserAdapter()
    {
    delete iTitle;
    delete iBookmarkTitle;
    delete iDefaultName;
    delete iDefaultBookmarkName;
    iFavouritesDb.Close();
    iSession.Close();
    }


// -----------------------------------------------------------------------------
// CWPWAPBrowserAdapter::ApplicationL
// -----------------------------------------------------------------------------
//
void CWPWAPBrowserAdapter::ApplicationL( CWPCharacteristic& aCharacteristic )
    {
    if( iAppID == KBrowserAppID )
        {
        MWPWAPItemBase* item = WPWAPItemFactory::CreateBrowserLC( !iProcessed, *iTitle, 
            *iBookmarkTitle, *iDefaultName, *iDefaultBookmarkName, 
            aCharacteristic, iCommsDb, iCmManager, iFavouritesDb );

        if( item->ValidateL() )
            {
            iProcessed = ETrue;
            item->AddL( iItems, iHiddenItems );
            CleanupStack::Pop(); // item
            }
        else
            {
            CleanupStack::PopAndDestroy(); // item
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserAdapter::DeleteL
// -----------------------------------------------------------------------------
//
void CWPWAPBrowserAdapter::DeleteL( const TDesC8& aSaveData )
    {
    CreateDbL();

    TPckgBuf<MWPWAPItemBase::TWPWAPSaveItem> item;
    item.Copy( aSaveData );

    switch( item().iSaveItemType )
        {
        case KWPWAPItemTypeAccesspoint:
            {
            // Access point deletion is handled by the base class
            CWPWAPAdapterBase::DeleteL( aSaveData );
            break;
            }

        case KWPWAPItemTypeBookmark:
            {
            DVA( "DeleteL: Deleting bookmark %08x", item().iUID );
            TInt err = iFavouritesDb.Begin( ETrue );
            iFavouritesDb.CleanupRollbackPushL();  // push a rollback
            
            DVA( "DeleteL: Transaction, %d", err );
            if( err == KErrNone )
                {
                err = iFavouritesDb.Delete( item().iUID );
                DVA( "DeleteL: Deleted, %d", err );
                iFavouritesDb.Commit();
                DEBUG( "DeleteL: Committed" );
                }
            CleanupStack::Pop();    // pop the rollback

            switch( err )
                {
                case KErrNotFound:
                    {
                    // Item not existing is what we want, so this error is ok
                    break;
                    }
                    
                case KErrLocked:
                    {
                    User::Leave( EWPBookmarksLocked );
                    break;
                    }
                    
                default:
                    {
                    User::LeaveIfError( err );
                    break;
                    }
                }
            break;
            }

        default:
            {
            // Don't know how to delete an item of this type. Better
            // just ignore it.
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPBrowserAdapter::CreateDbL
// -----------------------------------------------------------------------------
//
void CWPWAPBrowserAdapter::CreateDbL()
    {
    User::LeaveIfError( iSession.Connect() );
    User::LeaveIfError( iFavouritesDb.Open( iSession, KBrowserBookmarks ) );

    CWPWAPAdapterBase::CreateDbL();
    }

//  End of File  
