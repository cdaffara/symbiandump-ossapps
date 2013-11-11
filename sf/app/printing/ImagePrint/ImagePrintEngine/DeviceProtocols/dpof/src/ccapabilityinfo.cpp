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
* Description:  Contains the CCapabilityInfo class definition.
*
*/


#include "ccapabilityinfo.h"

//**********************************************************************
//		CCapabilityInfo
//**********************************************************************
void CCapabilityInfo::ConstructL(TResourceReader& aReader)
	{
	iPrinterClass = aReader.ReadInt32();
	iCapability.iCapabilityID = aReader.ReadInt32();
	iCapability.iType = (TPrintCapability::ECapType)aReader.ReadInt32();
	iCapability.iDefaultValue = aReader.ReadInt32();
	iCapability.iLow = aReader.ReadInt32();
	iCapability.iHigh = aReader.ReadInt32();

	TInt count = aReader.ReadInt16()/2;
	iCapability.iEnumCount = count;
	
	for( TInt i = 0; i < count ; i++ )
    	{	
		TInt value = aReader.ReadInt32();
		iCapability.iEnumCodes[i] = value;		
		value = aReader.ReadInt32();
        iLinks.Append(value);
    	}
	iLinkID = aReader.ReadInt32();
	iCurrentValue = 0;
	}


CCapabilityInfo* CCapabilityInfo::NewLC(TResourceReader& aReader)
	{
	CCapabilityInfo* self=new (ELeave) CCapabilityInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aReader);
	return self;
	}

CCapabilityInfo::~CCapabilityInfo()
	{
	iLinks.Close();		
	}

TInt CCapabilityInfo::SetCurrentValue( TInt aValue )
	// returns the link number from the resource file
	{
	if( aValue == iCurrentValue )
		return 0;

	iCurrentValue = aValue;
	if( iCapability.iType == TPrintCapability::Enum ) // links are only on Enums.
		{
		for( TInt i = 0 ; i < iCapability.iEnumCount; i++ )
			{
			if( iCapability.iEnumCodes[i] == aValue )
				{
				return iLinks[i];
				}
			}
		}
	return 0;
	}

TInt CCapabilityInfo::ValueLink( TInt aValue )
	{
	if( iCapability.iType == TPrintCapability::Enum ) // links are only on Enums.
		{
		for( TInt i = 0; i < iCapability.iEnumCount; i++ )
			{	
			if( iCapability.iEnumCodes[i] == aValue )
				{
				return iLinks[i];
				}
			}
		}
	return 0;
	}

CCapabilityInfo* CCapabilityInfo::CopyL() const
	{
	CCapabilityInfo *workCap = new (ELeave) CCapabilityInfo;
	CleanupStack::PushL(workCap);

	workCap->iPrinterClass = iPrinterClass;
	workCap->iCapability.iCapabilityID = iCapability.iCapabilityID;
	workCap->iCapability.iType = iCapability.iType;
	workCap->iCapability.iDefaultValue = iCapability.iDefaultValue;
	workCap->iCapability.iLow = iCapability.iLow;
	workCap->iCapability.iHigh = iCapability.iHigh;
	workCap->iCapability.iEnumCount = iCapability.iEnumCount;
	
	for( TInt i = 0; i < iCapability.iEnumCount; i++ )
    	{
    	workCap->iCapability.iEnumCodes[i] = iCapability.iEnumCodes[i];	
        workCap->iLinks.AppendL( iLinks[i] );
    	}
    	
	workCap->iLinkID = iLinkID;
	workCap->iCurrentValue = iCurrentValue;

	CleanupStack::Pop(workCap);
	return workCap;
	}


TInt CCapabilityInfo::CurrentValue() const
	{
	return iCurrentValue;
	}
	
TInt CCapabilityInfo::LinkID() const
	{
	return iLinkID; 
	}
	
TInt CCapabilityInfo::PrinterClass() const
	{
	return iPrinterClass; 
	}
	
TPrintCapability& CCapabilityInfo::Capability()
	{
	return iCapability; 
	}	

//  End of File
