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
#include "emailTest.h"
#include <stiflogger.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CemailTest::CemailTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
/*CemailTest::CemailTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }*/
CemailTest::CemailTest( CTestModuleIf& aTestModuleIf )
		: Cdmatest( aTestModuleIf, KAdapterUid )
    {
    }
// -----------------------------------------------------------------------------
// CemailTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CemailTest::ConstructL()
    {
/*    iLog = CStifLogger::NewL( KamtestLogPath, 
                          KamtestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );*///Commented the Part for emailTest Dipak
    iLog = CStifLogger::NewL( KamtestLogPath, 
                          KamtestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile );
                          
    iLog->Log( _L( "Loading Adapter" ) );
    
	Cdmatest::ConstructL();                         	

    }

// -----------------------------------------------------------------------------
// CemailTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CemailTest* CemailTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CemailTest* self = new (ELeave) CemailTest( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    
    }
    
// Destructor
CemailTest::~CemailTest()
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
    
    return ( CScriptBase* ) CemailTest::NewL( aTestModuleIf );
        
    }


    
//  End of File
