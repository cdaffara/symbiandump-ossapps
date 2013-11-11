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
#include "syncmldsTest.h"
#include <stiflogger.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CsyncmldsTest::CsyncmldsTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
/*CsyncmldsTest::CsyncmldsTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }*/
CsyncmldsTest::CsyncmldsTest( CTestModuleIf& aTestModuleIf )
		: Cdmatest( aTestModuleIf, KAdapterUid )
    {
    }
// -----------------------------------------------------------------------------
// CsyncmldsTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CsyncmldsTest::ConstructL()
    {
/*    iLog = CStifLogger::NewL( KamtestLogPath, 
                          KamtestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );*///Commented the Part for syncmldsTest Dipak
    iLog = CStifLogger::NewL( KamtestLogPath, 
                          KamtestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile );
                          
    iLog->Log( _L( "Loading Adapter" ) );
    
	Cdmatest::ConstructL();                         	

    }

// -----------------------------------------------------------------------------
// CsyncmldsTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CsyncmldsTest* CsyncmldsTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CsyncmldsTest* self = new (ELeave) CsyncmldsTest( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    
    }
    
// Destructor
CsyncmldsTest::~CsyncmldsTest()
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
    
    return ( CScriptBase* ) CsyncmldsTest::NewL( aTestModuleIf );
        
    }


    
//  End of File
