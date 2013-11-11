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
#include "mmsTest.h"
#include <e32math.h>
#include <f32file.h>
#include <swinstapi.h>


//using namespace NApplicationManagement;//Dipak

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CmmsTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CmmsTest::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// CmmsTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CmmsTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "DDFStructure", CmmsTest::DDFStructureL),
        ENTRY( "FetchNode", CmmsTest::FetchNodeL ),
        ENTRY( "FetchLeaf", CmmsTest::FetchLeafL ),
        ENTRY( "AddNode", CmmsTest::AddNodeL ),
        ENTRY( "UpdateLeaf", CmmsTest::UpdateLeafL ),
        ENTRY( "UpdateLeafData", CmmsTest::UpdateLeafDataL ),
        ENTRY( "Execute", CmmsTest::ExecuteLeafL ),
        ENTRY( "ExecuteData", CmmsTest::ExecuteLeafDataL ),
        ENTRY( "DeleteNode", CmmsTest::DeleteObjectL ),
        ENTRY( "StartAtomic", CmmsTest::StartAtomicL),
	    ENTRY( "CommitAtomic", CmmsTest::CommitAtomicL),
	    ENTRY( "RollbackAtomic", CmmsTest::RollbackAtomicL),
	    ENTRY( "CompleteCommands", CmmsTest::CompleteCommandsL),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

        
        
HBufC8 *CmmsTest::GetNextStringLC ( CStifItemParser& aItem, const TDesC &aName )
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
