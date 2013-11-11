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


// INCLUDE FILES
#include    <qglobal.h>
#include    <e32cmn.h>  
#include    <qcontactmanager.h>
#include    <CProfileChangeNotifyHandler.h>
#include    <MProfileChangeObserver.h>
#include    <featmgr.h>     // Feature Manager
#include    <QString>
#include    "cphcntfactoryimpl2.h"
#include    "cphcntmatcherimpl2.h"
#include    "qtphonelog.h"


static const QString SYMBIAN_CONTACTS_BACKEND  = "symbian";


// -----------------------------------------------------------------------------
// CPhCntFactoryImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhCntFactoryImpl2* CPhCntFactoryImpl2::NewL()
    {
    PHONE_DEBUG("CPhCntFactoryImpl2::NewL - enter");
    CPhCntFactoryImpl2* self = new (ELeave) CPhCntFactoryImpl2;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    PHONE_DEBUG("CPhCntFactoryImpl2::cntfinder2 is ready. lets rock.");
    return self;
    }

// ---------------------------------------------------------------------------
// CPhCntFactoryImpl2::~CPhCntFactoryImpl2
// ---------------------------------------------------------------------------
//
CPhCntFactoryImpl2::~CPhCntFactoryImpl2()
    {

    FeatureManager::UnInitializeLib();
    }



// ---------------------------------------------------------------------------
// CPhCntFactoryImpl2::CreateContactMatcherL
// 
//  Create contact matcher
// ---------------------------------------------------------------------------
//
CPhCntMatcher* CPhCntFactoryImpl2::CreateContactMatcherL()
    {
    QContactManager* contactMgr = NULL;
    QT_TRYCATCH_LEAVING(contactMgr = new QContactManager(SYMBIAN_CONTACTS_BACKEND));
    CPhCntMatcher*matcher = CPhCntMatcherImpl2::NewL(contactMgr);
    return matcher;
    }


// ---------------------------------------------------------------------------
// CPhCntFactoryImpl2::CreateProfileEngineNotifyHandlerL
// ---------------------------------------------------------------------------
//
CBase* CPhCntFactoryImpl2::CreateProfileEngineNotifyHandlerL( 
        MProfileChangeObserver* aObserver )
    {
    return CProfileChangeNotifyHandler::NewL( aObserver );
    }


// ---------------------------------------------------------------------------
// CPhCntFactoryImpl2::CreateContactIdL
// ---------------------------------------------------------------------------
//
/*
CPhCntContactId* CPhCntFactoryImpl2::CreateContactIdL()
    {
    return NULL;
    }
*/   

// ---------------------------------------------------------------------------
// CPhCntFactoryImpl2::CreateContactIdL
// ---------------------------------------------------------------------------
//    
CPhCntContactId* CPhCntFactoryImpl2::CreateContactIdL( 
    TContactItemId /* aContactId*/)
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPhCntFactoryImpl2::CreateContactIdL
// ---------------------------------------------------------------------------
//    
CPhCntContactId* CPhCntFactoryImpl2::CreateContactIdL( 
    const TDesC8& /*aContactId*/ )
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CPhCntFactoryImpl2::CPhCntFactoryImpl2
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCntFactoryImpl2::CPhCntFactoryImpl2()
    {
    }

// -----------------------------------------------------------------------------
// CPhCntFactoryImpl2::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhCntFactoryImpl2::ConstructL()
    {
    FeatureManager::InitializeLibL();
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------------------------
// CreateCntFactoryL
// 
//  Creates instance of contact factory.
// ---------------------------------------------------------------------------
//
EXPORT_C CPhCntFactory* CreateCntFactoryL()
    {
    return CPhCntFactoryImpl2::NewL();
    }

// End of File
