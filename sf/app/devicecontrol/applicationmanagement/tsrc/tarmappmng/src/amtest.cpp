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
* Description: Implementation of applicationmanagement components
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "amtest.h"
#include <stiflogger.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Camtest::Camtest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
Camtest::Camtest( CTestModuleIf& aTestModuleIf )
		: Cdmatest( aTestModuleIf, KAdapterUid )
    {
    }
// -----------------------------------------------------------------------------
// Camtest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void Camtest::ConstructL()
    {
    iLog = CStifLogger::NewL( KamtestLogPath, 
                          KamtestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile );
                          
    iLog->Log( _L( "Loading Adapter" ) );
    
	Cdmatest::ConstructL();                         	

    }

// -----------------------------------------------------------------------------
// Camtest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Camtest* Camtest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    Camtest* self = new (ELeave) Camtest( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    
    }
    
// Destructor
Camtest::~Camtest()
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
    
    return ( CScriptBase* ) Camtest::NewL( aTestModuleIf );
        
    }


    
//  End of File
