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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/



// INCLUDE FILES
#include<BADESCA.H>
#include <Stiftestinterface.h>
#include "TestCntDataStoreExtn.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::CTestCntDataStoreExtn
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTestCntDataStoreExtn::CTestCntDataStoreExtn( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestCntDataStoreExtn::ConstructL()
    {
    iLog = CStifLogger::NewL( KTestCntDataStoreExtnLogPath, 
            KTestCntDataStoreExtnLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    iCntDataStoreExtension = CNsmlContactsDataStoreExtension::NewL( _L("symbian") );
    iEntryArray = new(ELeave) CArrayFixFlat<TUid>(4);
    }

// -----------------------------------------------------------------------------
// CTestCntDataStoreExtn::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestCntDataStoreExtn* CTestCntDataStoreExtn::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CTestCntDataStoreExtn* self = new (ELeave) CTestCntDataStoreExtn( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }

// Destructor
CTestCntDataStoreExtn::~CTestCntDataStoreExtn()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 
    
    if( iEntryArray )
        {
        delete iEntryArray;
        iEntryArray = NULL;
        }
    
    if(iCntDataStoreExtension)
        {
        delete iCntDataStoreExtension;
        iCntDataStoreExtension = NULL;
        }

   }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CTestCntDataStoreExtn::NewL( aTestModuleIf );

    }


//  End of File
