/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ConML parser/generator
*
*/


#ifndef __SCONXMLWORKSPACE_H__
#define __SCONXMLWORKSPACE_H__

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <e32base.h>

// -----------------------------------------------------------------------------
// CXMLWorkspace - workspace for WBXML / XML -documents
// -----------------------------------------------------------------------------
class CXMLWorkspace : public CBase
	{
public:
	~CXMLWorkspace();
	static CXMLWorkspace* NewL();

	// transaction handling
	/**
        * Function to Begin transaction
        * @param none
        * @return none
        */
	void BeginTransaction();
	
	/**
        * Commits the transactions done
        * @param none
        * @return none
        */
	void CommitAll();
	
	/**
        *  Commits the latest transacion done
        * @param none
        * @return KErrNone or an error code
        */
	TInt Commit();
	
	/**
        *  Deletes the latest transacion done
        * @param none
        * @return KErrNone or an error code
        */
	TInt Rollback();

        /**
        *  Resets the buffer
        * @param none
        * @return KErrNone or an error code
        */
	void Reset();
	
	/**
        *  Writes to the buffer
        * @param none
        * @return none
        */
	void WriteL( const TDesC8& aData );
	void WriteL( const TUint8 aData );
	
	/**
        * Returns the buffer
        * @param none
        * @return the buffer
        */
	TPtrC8 Buffer();
        
        /**
        *  Returns the free space left in the buffer
        * @param none
        * @return free space of the buffer
        */
	TInt FreeSize();
	
	/**
        *  Returns the maximum size of the buffer
        * @param none
        * @return Buffer maximum size
        */
	TInt MaxSize();
	
	/**
        *  Returns the size of the buffer space used
        * @param none
        * @return Size of the buffer used
        */
	TInt Size();

protected:
	void ConstructL();

private:
	void Delete( TInt aPos, TInt aLength );
	CXMLWorkspace();
private:
	CBufFlat* iBuffer;
	RArray<TUint>* iTransactions;
	};

#endif // __SCONXMLWORKSPACE_H__
	
