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
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "emailTest.h"
#include <e32math.h>
#include <f32file.h>
#include <swinstapi.h>


//using namespace NApplicationManagement;//Dipak

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CemailTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CemailTest::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// CemailTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CemailTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "DDFStructure", CemailTest::DDFStructureL),
        ENTRY( "FetchNode", CemailTest::FetchNodeL ),
        ENTRY( "FetchLeaf", CemailTest::FetchLeafL ),
        ENTRY( "AddNode", CemailTest::AddNodeL ),
        ENTRY( "UpdateLeaf", CemailTest::UpdateLeafL ),
        ENTRY( "UpdateLeafData", CemailTest::UpdateLeafDataL ),
        ENTRY( "Execute", CemailTest::ExecuteLeafL ),
        ENTRY( "ExecuteData", CemailTest::ExecuteLeafDataL ),
        ENTRY( "DeleteNode", CemailTest::DeleteObjectL ),
        ENTRY( "StartAtomic", CemailTest::StartAtomicL),
	    ENTRY( "CommitAtomic", CemailTest::CommitAtomicL),
	    ENTRY( "RollbackAtomic", CemailTest::RollbackAtomicL),
	    ENTRY( "CompleteCommands", CemailTest::CompleteCommandsL),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

        
        
HBufC8 *CemailTest::GetNextStringLC ( CStifItemParser& aItem, const TDesC &aName )
	{
	TPtrC nodename;
	nodename.Set( KNullDesC );

    TInt i( aItem.GetNextString ( nodename ) );
    if ( i != KErrNone ) 
	    {
	    iLog->Log( _L( "ERROR Reading '%S' argument: 0x%X" ), &aName, i );
	    }
	else
		{
		iLog->Log( _L("%S: %S"), &aName, &nodename);
		}
	
	HBufC8 *buf = HBufC8::NewLC( nodename.Length() ) ;
	buf->Des().Copy( nodename );
	
	return buf;
	}
	
//  End of File
