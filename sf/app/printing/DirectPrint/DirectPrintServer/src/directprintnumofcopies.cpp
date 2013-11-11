/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  
*
*/


#include "directprintnumofcopies.h"
#include "clog.h"

CDirectPrintNumOfCopies* CDirectPrintNumOfCopies::NewL()
    {
	CDirectPrintNumOfCopies* obj = new (ELeave) CDirectPrintNumOfCopies();
    CleanupStack::PushL( obj ) ;
    obj->ConstructL();
	CleanupStack::Pop(); // obj
    return obj;
    }

CDirectPrintNumOfCopies::~CDirectPrintNumOfCopies()
    {
    LOG("CDirectPrintNumOfCopies::~CDirectPrintNumOfCopies begin");
	iNumsOfCopies.Close();
	LOG("CDirectPrintNumOfCopies::~CDirectPrintNumOfCopies end");
    }

CDirectPrintNumOfCopies::CDirectPrintNumOfCopies() : iCount( 0 )
    {
    }

void CDirectPrintNumOfCopies::ConstructL()
    {
    }
    
void CDirectPrintNumOfCopies::Reset( TInt aCount )
    {
    LOG("CDirectPrintNumOfCopies::Reset begin");
    iCount = aCount;
    iNumsOfCopies.Reset();
    LOG("CDirectPrintNumOfCopies::Reset end");
    }    
    
void CDirectPrintNumOfCopies::AddNumOfCopyL( TInt aNumOfCopies, TBool& aFull )
	{
	LOG1("CDirectPrintNumOfCopies::AddNumOfCopyL aNumOfCopies: %d", aNumOfCopies);
	iNumsOfCopies.AppendL( aNumOfCopies );
	aFull = ( iNumsOfCopies.Count() == iCount ) ? ETrue : EFalse;
	LOG1("CDirectPrintNumOfCopies::AddNumOfCopyL end with aFull: %d", aFull);
	}

const RArray<TInt>& CDirectPrintNumOfCopies::NumsOfCopies() const
	{
	return iNumsOfCopies;
	}

//  End of File
