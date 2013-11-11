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
#include "syncmldm12Test.h"
#include <e32math.h>
#include <f32file.h>
#include <swinstapi.h>


//using namespace NApplicationManagement;//Dipak

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Csyncmldm12Test::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Csyncmldm12Test::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// Csyncmldm12Test::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Csyncmldm12Test::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "DDFStructure", Csyncmldm12Test::DDFStructureL),
        ENTRY( "FetchNode", Csyncmldm12Test::FetchNodeL ),
        ENTRY( "FetchLeaf", Csyncmldm12Test::FetchLeafL ),
        ENTRY( "AddNode", Csyncmldm12Test::AddNodeL ),
        ENTRY( "UpdateLeaf", Csyncmldm12Test::UpdateLeafL ),
        ENTRY( "UpdateLeafData", Csyncmldm12Test::UpdateLeafDataL ),
        ENTRY( "Execute", Csyncmldm12Test::ExecuteLeafL ),
        ENTRY( "ExecuteData", Csyncmldm12Test::ExecuteLeafDataL ),
        ENTRY( "DeleteNode", Csyncmldm12Test::DeleteObjectL ),
        ENTRY( "StartAtomic", Csyncmldm12Test::StartAtomicL),
	    ENTRY( "CommitAtomic", Csyncmldm12Test::CommitAtomicL),
	    ENTRY( "RollbackAtomic", Csyncmldm12Test::RollbackAtomicL),
	    ENTRY( "CompleteCommands", Csyncmldm12Test::CompleteCommandsL),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

        
        
HBufC8 *Csyncmldm12Test::GetNextStringLC ( CStifItemParser& aItem, const TDesC &aName )
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
