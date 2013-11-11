/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base for WAP adapters.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <commdb.h>
#include <cmmanagerext.h>
#include <cmconnectionmethodext.h>
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <wpwapadapterresource.rsg>
#include "CWPWAPItemBAse.h"
#include "CWPWAPAdapterBase.h"
#include "WPWAPDebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::CWPWAPAdapterBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPWAPAdapterBase::CWPWAPAdapterBase()
    : CWPAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::BaseConstructL
// 2nd phase base class constructor.
// -----------------------------------------------------------------------------
//
void CWPWAPAdapterBase::BaseConstructL()
    {
    }

// Destructor
CWPWAPAdapterBase::~CWPWAPAdapterBase()
    {
    iItems.ResetAndDestroy();
    iHiddenItems.ResetAndDestroy();
    delete iCommsDb;
    
    if ( iCmManager !=  NULL )
        {
        iCmManager->Close();
        delete iCmManager;
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::SummaryCount
// -----------------------------------------------------------------------------
//
TInt CWPWAPAdapterBase::ItemCount() const
    {
    return iItems.Count();
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::SummaryTitle
// -----------------------------------------------------------------------------
//
const TDesC16& CWPWAPAdapterBase::SummaryTitle(TInt aIndex) const
    {
    return iItems[aIndex]->Title();
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC& CWPWAPAdapterBase::SummaryText(TInt aIndex) const
    {
    return iItems[aIndex]->Name();
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::SaveL
// -----------------------------------------------------------------------------
//
void CWPWAPAdapterBase::SaveL(TInt aIndex)
    {
    iItems[aIndex]->SaveL();
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CWPWAPAdapterBase::CanSetAsDefault(TInt aIndex) const
    {
    return iItems[aIndex]->CanSetAsDefault();
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CWPWAPAdapterBase::SetAsDefaultL(TInt aIndex)
    {
    iItems[aIndex]->SetAsDefaultL();
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::DetailsL
// -----------------------------------------------------------------------------
//
TInt CWPWAPAdapterBase::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor*/ )
    {
    return KErrNotSupported;
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::VisitL
// -----------------------------------------------------------------------------
//
void CWPWAPAdapterBase::VisitL( CWPCharacteristic& aCharacteristic )
    {
    switch( aCharacteristic.Type() )
        {
        case KWPApplication:
            {
            aCharacteristic.AcceptL( *this );
            ApplicationL( aCharacteristic );
            iAppID.Set( KNullDesC );
            break;
            }

        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::VisitL
// -----------------------------------------------------------------------------
//
void CWPWAPAdapterBase::VisitL( CWPParameter& aParameter )
    {
    if( aParameter.ID() == EWPParameterAppID )
        {
        iAppID.Set( aParameter.Value() );
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::VisitL
// -----------------------------------------------------------------------------
//
void CWPWAPAdapterBase::VisitLinkL( CWPCharacteristic& /*aLink*/ )
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::ContextExtension
// -----------------------------------------------------------------------------
//
TInt CWPWAPAdapterBase::ContextExtension( MWPContextExtension*& aExtension )
    {
    aExtension = this;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::SaveDataL
// -----------------------------------------------------------------------------
//
const TDesC8& CWPWAPAdapterBase::SaveDataL( TInt aIndex ) const
    {
    return iItems[aIndex]->SaveData();
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::DeleteL
// -----------------------------------------------------------------------------
//
void CWPWAPAdapterBase::DeleteL( const TDesC8& aSaveData )
    {
    CreateDbL();

    TPckgBuf<MWPWAPItemBase::TWPWAPSaveItem> pckg;
    pckg.Copy( aSaveData );

    // Base class handles only access point deletion
    if( pckg().iSaveItemType == KWPWAPItemTypeAccesspoint )
        {
        DVA( "DeleteL: Deleting access point %08x", pckg().iUID );
        TRAPD( err, iCmManager->ConnectionMethodL( pckg().iUID).DeleteL());
        DVA( "DeleteL: Deleted, %d", err );
        
        switch( err )
            {
            case KErrNotFound:
                {
                // Item not existing is what we want, so this error is ok
                break;
                }
                
            case KErrLocked:
                {
                // Transform the code so that the caller can retry
                User::Leave( EWPCommsDBLocked );
                break;
                }
                
            default:
                {
                User::LeaveIfError( err );
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::Uid
// -----------------------------------------------------------------------------
//
TUint32 CWPWAPAdapterBase::Uid() const
    {
    return iDtor_ID_Key.iUid;
    }

// -----------------------------------------------------------------------------
// CWPWAPAdapterBase::CreateDbL
// -----------------------------------------------------------------------------
//
void CWPWAPAdapterBase::CreateDbL()
    {
    if( !iCommsDb )
        {
        iCommsDb = CCommsDatabase::NewL();
        }
    iCmManager = new RCmManagerExt;
    iCmManager->OpenL();

    
    }

//  End of File  
