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
* Description:  Handles MMS settings in provisioning.
*
*/



// INCLUDE FILES
#include "CWPWAPMMSAdapter.h"
#include <e32base.h>
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <wpwapadapterresource.rsg>
#include "MWPWAPItemBase.h"
#include "WPWAPUtil.h"
#include "WPWAPItemFactory.h"

// CONSTANTS
_LIT( KMMSAppID, "w4" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPWAPMMSAdapter::CWPWAPMMSAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPWAPMMSAdapter::CWPWAPMMSAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPWAPMMSAdapter::ConstructL()
    {
    BaseConstructL();

    iTitle = WPWAPUtil::ReadHBufCL( R_QTN_SM_MMS_ACCESSPOINTS );
    iDefaultName = WPWAPUtil::ReadHBufCL( R_QTN_SM_WAPAP_NAME );
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPWAPMMSAdapter* CWPWAPMMSAdapter::NewL()
    {
    CWPWAPMMSAdapter* self = new(ELeave) CWPWAPMMSAdapter; 
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CWPWAPMMSAdapter::~CWPWAPMMSAdapter()
    {
    delete iTitle;
    delete iDefaultName;
    }

// -----------------------------------------------------------------------------
// CWPWAPMMSAdapter::ApplicationL
// -----------------------------------------------------------------------------
//
void CWPWAPMMSAdapter::ApplicationL( CWPCharacteristic& aCharacteristic )
    {
    if( iItems.Count() == 0 && iAppID == KMMSAppID )
        {
        MWPWAPItemBase* item = WPWAPItemFactory::CreateMMSLC( *iTitle, *iDefaultName, 
            aCharacteristic, iCommsDb, iCmManager );
        if( item->ValidateL() )
            {
            item->AddL( iItems, iHiddenItems );
            CleanupStack::Pop(); // item
            }
        else
            {
            CleanupStack::PopAndDestroy(); // item
            }
        }
    }

//  End of File  
