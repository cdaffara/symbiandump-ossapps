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
* Description:  Contains the CPrinterCapabilities class definition.
*
*/


#include "cprintercapabilities.h"
#include "imageprint.h"

//**********************************************************************
//		CPrinterCapabilities
//**********************************************************************

CPrinterCapabilities::CPrinterCapabilities(CCapabilityArray* aSourceArray, TInt aPrinterClass)
	{
	iSourceArray = aSourceArray;
	iPrinterClass = aPrinterClass;
	}

CPrinterCapabilities::~CPrinterCapabilities()
	{
	iPrinterDataArray.ResetAndDestroy();
	iPrinterDataArray.Close();
	}

CPrinterCapabilities* CPrinterCapabilities::NewL(CCapabilityArray* aSourceArray, TInt aPrinterClass)
	{
	CPrinterCapabilities* self=new (ELeave) CPrinterCapabilities(aSourceArray,aPrinterClass);
	CleanupStack::PushL(self);
	self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

void CPrinterCapabilities::ConstructL()
	{    
	BuildCapabilityArrayL();
	}

void CPrinterCapabilities::BuildCapabilityArrayL()
	{
	CCapabilityInfo* capInfo;
	CCapabilityInfo* workCap;
	TInt count = iSourceArray->Count();
	
	for( TInt i = 0; i < count; i++ )
		{
		capInfo = (*iSourceArray)[i];
		if( capInfo->PrinterClass() == iPrinterClass )
			{
			workCap = capInfo->CopyL();
			CleanupStack::PushL( workCap );
			iPrinterDataArray.AppendL( workCap );
			CleanupStack::Pop(); // workCap
			TInt link = workCap->SetCurrentValue( workCap->Capability().iDefaultValue );
			if( link )
				{
				workCap = iSourceArray->LinkedCapability( link )->CopyL();
				CleanupStack::PushL( workCap );				
				iPrinterDataArray.AppendL( workCap );
				CleanupStack::Pop(); // workCap	
				}
			}
		}
	}

void CPrinterCapabilities::GetIDs( RArray<TInt>& aCapabilityIDs ) const
	{
	for( TInt nCnt = 0; nCnt < iPrinterDataArray.Count(); nCnt++ )
		{
		aCapabilityIDs.Append( iPrinterDataArray[nCnt]->Capability().iCapabilityID );
		}
	}

TInt CPrinterCapabilities::GetCapability( TInt aCapabilityID, TPrintCapability& aCapability ) const
	{
	TInt err( KErrInvalidData );
	for( TInt nCnt = 0; nCnt < iPrinterDataArray.Count() && err; nCnt++ )
		{
		if( iPrinterDataArray[nCnt]->Capability().iCapabilityID == aCapabilityID )
			{
			aCapability = iPrinterDataArray[nCnt]->Capability();
			err = KErrNone;
			}
		}
	return err;
	}

TInt CPrinterCapabilities::SetCurrentValueL( TInt aCapabilityID, TInt aValue )
	// returns any affected capability ID
	{		
	// locate the capability
	CCapabilityInfo* capInfo = NULL;
	for( TInt nCnt = 0; nCnt < iPrinterDataArray.Count() && !capInfo; nCnt++ )
		{
		if( iPrinterDataArray[nCnt]->Capability().iCapabilityID == aCapabilityID )
			{
			capInfo = iPrinterDataArray[nCnt];
			}
		}
		
	TInt affectedCapability( 0 );
			
	if( capInfo && aValue != capInfo->CurrentValue() )
		{
		// has changed
		TInt currentLink = capInfo->ValueLink( capInfo->CurrentValue() );
		TInt newLink = capInfo->SetCurrentValue( aValue );
		
		if( newLink != currentLink )
			{
			// Find and Delete the old link
			for( TInt nCnt = 0; nCnt < iPrinterDataArray.Count(); nCnt++ )
				{
				if( iPrinterDataArray[nCnt]->LinkID() == currentLink )
					{
					CCapabilityInfo *tmpCap;
					tmpCap = iPrinterDataArray[nCnt];
					iPrinterDataArray.Remove(nCnt);
					delete tmpCap;
					break;
					}
				}

			// add the new link
			if( newLink )
				{
				capInfo = iSourceArray->LinkedCapability( newLink );
				affectedCapability = capInfo->Capability().iCapabilityID;				
				CCapabilityInfo* tmpCap = capInfo->CopyL();
				CleanupStack::PushL( tmpCap );
				iPrinterDataArray.AppendL( tmpCap );
				CleanupStack::Pop(); // tmpCap
				}
			}			
		}
								
	return affectedCapability;
	}

TBool CPrinterCapabilities::GetCurrentValue( TInt aCapabilityID, TInt &aCapabilityValue ) const
	{
	TBool result( EFalse );
	
	for( TInt nCnt = 0; nCnt < iPrinterDataArray.Count() && !result; nCnt++ )
		{
		if( iPrinterDataArray[nCnt]->Capability().iCapabilityID == aCapabilityID )
			{
			aCapabilityValue = iPrinterDataArray[nCnt]->CurrentValue();
			result = ETrue;
			}
		}
			
	return result;
	}


TInt CPrinterCapabilities::Count() const
	{
	return iPrinterDataArray.Count();
	}

//  End of File
