/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of DM adapter test component
* 	This is part of omadmextensions/adapter test application.
*
*/






// INCLUDE FILES
#include <Stiftestinterface.h>
#include "mmsTest.h"
#include <stiflogger.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CmmsTest::CmmsTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
/*CmmsTest::CmmsTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }*/
CmmsTest::CmmsTest( CTestModuleIf& aTestModuleIf )
		: Cdmatest( aTestModuleIf, KAdapterUid )
    {
    }
// -----------------------------------------------------------------------------
// CmmsTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CmmsTest::ConstructL()
    {
/*    iLog = CStifLogger::NewL( KamtestLogPath, 
                          KamtestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );*///Commented the Part for mmsTest Dipak
    iLog = CStifLogger::NewL( KamtestLogPath, 
                          KamtestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile );
                          
    iLog->Log( _L( "Loading Adapter" ) );
    
	Cdmatest::ConstructL();                         	

    }

// -----------------------------------------------------------------------------
// CmmsTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CmmsTest* CmmsTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CmmsTest* self = new (ELeave) CmmsTest( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    
    }
    
// Destructor
CmmsTest::~CmmsTest()
    { 
    
    // Delete resources allocated from test methods
//    Delete();
    
    // Delete logger
//    delete iLog;  
    
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
    
    return ( CScriptBase* ) CmmsTest::NewL( aTestModuleIf );
        
    }


    
//  End of File
