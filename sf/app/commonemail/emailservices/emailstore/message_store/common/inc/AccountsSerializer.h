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
* Description:  Account serializer.
*
*/



#ifndef __ACCOUNTS_SERIALIZER_H__
#define __ACCOUNTS_SERIALIZER_H__

// ========
// INCLUDES
// ========

#include <e32std.h>
#include <e32base.h>

// ==============================
// CLASS: TPropertiesDeserializer
// ==============================
class CAccountsSerializer : public CBase
    {
    public:
    
		CAccountsSerializer();
		
		~CAccountsSerializer();
		
		void AddAccountL( TInt32 aOwnerId, const TDesC& aNameBuf );
    
	    void SerializeL( TDes8& aAccountsBuf );
	    
	    void DeserializeL( const TDesC8& aAccountBuf );
	    
	    inline TInt Count() { return iCount; }
	    
	    //NOTE: parallel arrays - NOT Good.  When have time, change to use objects
	    inline RArray<TInt32>& OwnerIds() { return iOwnerIdArray; }
	    
	    inline RPointerArray<HBufC>& NamesArray() { return iNameBufArray; };
	        
	private:
	
		TInt 				  iCount;
		RArray<TInt32>        iOwnerIdArray;
		RPointerArray<HBufC>  iNameBufArray;

    }; // end class TPropertiesDeserializer
    
#endif  //__ACCOUNTS_SERIALIZER_H__
