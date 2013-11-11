/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include "cnumofcopies.h"
#include "clog.h"

CNumOfCopies* CNumOfCopies::NewL()
    {
	CNumOfCopies* obj = new (ELeave) CNumOfCopies();
    CleanupStack::PushL( obj ) ;
    obj->ConstructL();
	CleanupStack::Pop(); // obj
    return obj;
    }

CNumOfCopies::~CNumOfCopies()
    {
    LOG("CNumOfCopies::~CNumOfCopies begin");
	iNumsOfCopies.Close();
	LOG("CNumOfCopies::~CNumOfCopies end");
    }

CNumOfCopies::CNumOfCopies() : iCount( 0 )
    {
    }

void CNumOfCopies::ConstructL()
    {
    }
    
void CNumOfCopies::Reset( TInt aCount )
    {
    LOG("CNumOfCopies::Reset begin");
    iCount = aCount;
    iNumsOfCopies.Reset();
    LOG("CNumOfCopies::Reset end");
    }    
    
void CNumOfCopies::AddNumOfCopyL( TInt aNumOfCopies, TBool& aFull )
	{
	LOG1("CNumOfCopies::AddNumOfCopyL aNumOfCopies: %d", aNumOfCopies);
	iNumsOfCopies.AppendL( aNumOfCopies );
	aFull = ( iNumsOfCopies.Count() == iCount ) ? ETrue : EFalse;
	LOG1("CNumOfCopies::AddNumOfCopyL end with aFull: %d", aFull);
	}

const RArray<TInt>& CNumOfCopies::NumsOfCopies() const
	{
	return iNumsOfCopies;
	}

//  End of File
