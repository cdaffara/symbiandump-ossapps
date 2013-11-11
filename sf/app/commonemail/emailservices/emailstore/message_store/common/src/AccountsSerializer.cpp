/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Account serializer implementation.
*
*/



// ========
// INCLUDES
// ========

#include "AccountsSerializer.h"
#include <s32mem.h>

// ==============================
// Method: CAccountsSerializer
// ==============================
CAccountsSerializer::CAccountsSerializer() : iCount(0)
	{
	}

// ==============================
// Method: ~CAccountsSerializer
// ==============================
CAccountsSerializer::~CAccountsSerializer()
	{
	iOwnerIdArray.Close();
	iNameBufArray.ResetAndDestroy();
	}
        
// ==============================
// Method: AddAccountL
// ==============================
void CAccountsSerializer::AddAccountL( TInt32 aOwnerId, const TDesC& aName )
	{
	HBufC* nameCopy = aName.AllocL();
	iNameBufArray.AppendL( nameCopy );
	iOwnerIdArray.AppendL( aOwnerId );
	iCount++;
	}
        
// ==============================
// Method: Serialize
// ==============================
void CAccountsSerializer::SerializeL( TDes8& aAccountsBuf )
	{
    RDesWriteStream outStream( aAccountsBuf );
    
    CleanupClosePushL(outStream); // +outStream

    outStream.WriteInt16L( iCount );
    
    for ( int i = 0 ; i < iCount ; i++ )
    	{
    	outStream.WriteInt32L( iOwnerIdArray[i] );
    	HBufC* name = iNameBufArray[i];
	    //write the Length
        TPtr namePtr = name->Des();
        
        TInt length = namePtr.Length();
	    //write the Length
        outStream.WriteUint16L( length );
        
        if ( length > 0 )
        	{
			//write the descriptor
	        outStream.WriteL( namePtr );
    		}
    	}
   
    outStream.CommitL();

	CleanupStack::PopAndDestroy(&outStream); // -outStream
	}
        
void CAccountsSerializer::DeserializeL( const TDesC8& aAccountBuf )
	{
    RDesReadStream inStream;
    CleanupClosePushL( inStream );               //+inStream

    inStream.Open( aAccountBuf );
    
    iOwnerIdArray.Reset();
    iNameBufArray.ResetAndDestroy();
    
    iCount = inStream.ReadInt16L();
    
    for ( TInt i = 0 ; i < iCount ; i++ )
		{
		TInt32 ownerId = inStream.ReadInt32L();
		
		TInt length = inStream.ReadUint16L();
		
		HBufC* name = HBufC::NewLC( length );   //+buf
		
		if ( length > 0 )
			{
		        TPtr namePtr = name->Des();
		        inStream.ReadL( namePtr, length );
			}
		
		iOwnerIdArray.AppendL( ownerId );
		iNameBufArray.AppendL( name );
		
		CleanupStack::Pop( name );                //-buf
		}
    
    CleanupStack::PopAndDestroy( &inStream );  //-inStream
	}
