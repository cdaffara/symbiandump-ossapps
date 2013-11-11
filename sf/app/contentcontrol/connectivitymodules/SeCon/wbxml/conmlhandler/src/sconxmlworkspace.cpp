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


// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "sconxmlworkspace.h"

// -----------------------------------------------------------------------------
// CXMLWorkspace
// -----------------------------------------------------------------------------
CXMLWorkspace::CXMLWorkspace()
	{
	}
	
// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
CXMLWorkspace* CXMLWorkspace::NewL()
	{
	CXMLWorkspace* self = new (ELeave) CXMLWorkspace();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
void CXMLWorkspace::ConstructL()
	{
	iBuffer = CBufFlat::NewL(32);
	iTransactions = new (ELeave) RArray<TUint>();
	}

// -----------------------------------------------------------------------------
// ~CXMLWorkspace
// -----------------------------------------------------------------------------
CXMLWorkspace::~CXMLWorkspace()
	{
	delete iBuffer;
	if( iTransactions )
		{
		iTransactions->Reset();
		}
	delete iTransactions;
	}

// -----------------------------------------------------------------------------
// BeginTransaction
// -----------------------------------------------------------------------------
void CXMLWorkspace::BeginTransaction()
	{
	iTransactions->Append(Size());
	}
	
// -----------------------------------------------------------------------------
// Commit
// -----------------------------------------------------------------------------
TInt CXMLWorkspace::Commit()
	{
	if( iTransactions->Count() == 0 )
		{
		return KErrGeneral;
		}
	iTransactions->Remove(iTransactions->Count() - 1);
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CommitAll
// -----------------------------------------------------------------------------
void CXMLWorkspace::CommitAll()
	{
	iTransactions->Reset();
	}

// -----------------------------------------------------------------------------
// Rollback
// -----------------------------------------------------------------------------
TInt CXMLWorkspace::Rollback()
	{
	if( iTransactions->Count() == 0 )
		{
		return KErrGeneral;
		}
	TUint begin = iTransactions->operator[](iTransactions->Count() - 1);
	Delete(begin, Size() - begin);
	iTransactions->Remove(iTransactions->Count() - 1);
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// Reset
// -----------------------------------------------------------------------------
void CXMLWorkspace::Reset()
	{
	iBuffer->Reset();
	}

// -----------------------------------------------------------------------------
// WriteL
// -----------------------------------------------------------------------------
void CXMLWorkspace::WriteL( const TDesC8& aData )
	{
	TRAPD(err, iBuffer->InsertL(iBuffer->Size(), aData));
	if( err != KErrNone )
		{
		User::Leave(KErrTooBig);
		}
	}

// -----------------------------------------------------------------------------
// WriteL
// -----------------------------------------------------------------------------
void CXMLWorkspace::WriteL( const TUint8 aData )
	{
	WriteL(TPtrC8(&aData, 1));
	}

// -----------------------------------------------------------------------------
// Buffer
// -----------------------------------------------------------------------------
TPtrC8 CXMLWorkspace::Buffer()
	{
	return iBuffer->Ptr(0);
	}

// -----------------------------------------------------------------------------
// FreeSize
// -----------------------------------------------------------------------------
TInt CXMLWorkspace::FreeSize()
	{
	return (KMaxTInt32 - Size());
	}

// -----------------------------------------------------------------------------
// MaxSize
// -----------------------------------------------------------------------------
TInt CXMLWorkspace::MaxSize()
	{
	return KMaxTInt32;
	}

// -----------------------------------------------------------------------------
// Size
// -----------------------------------------------------------------------------
TInt CXMLWorkspace::Size()
	{
	return iBuffer->Size();
	}

// -----------------------------------------------------------------------------
// Delete
// -----------------------------------------------------------------------------
void CXMLWorkspace::Delete( TInt aPos, TInt aLength )
	{
	iBuffer->Delete(aPos, aLength);
	}
