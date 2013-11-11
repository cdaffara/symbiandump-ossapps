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
* Description:  Contains the CCapabilityArray class definition.
*
*/


#include "ccapabilityarray.h"

//**********************************************************************
//		CCapabilityArray
//**********************************************************************
CCapabilityArray* CCapabilityArray::NewL(TResourceReader& aReader)
	{
    CCapabilityArray* self=new (ELeave) CCapabilityArray;
	CleanupStack::PushL(self);
	self->ConstructL(aReader);
    CleanupStack::Pop(self);
    return self;
	}


CCapabilityArray::~CCapabilityArray()
	{		
	iDataArray.ResetAndDestroy();
	iDataArray.Close();
	}

void CCapabilityArray::ConstructL( TResourceReader& aReader )
	{    
	AddDataL( aReader );	    
	}

void CCapabilityArray::AddDataL(TResourceReader& aReader)
	{        
    TInt count = aReader.ReadInt16();    
    for( TInt i = 0; i < count; i++ )
    	{
        CCapabilityInfo* info = CCapabilityInfo::NewLC( aReader );
        iDataArray.AppendL( info );
        CleanupStack::Pop( info );
    	}
	}
	
CCapabilityInfo* CCapabilityArray::operator[]( TInt aIndex ) const
	{
	return iDataArray[aIndex];
	}

CCapabilityInfo* CCapabilityArray::LinkedCapability( TInt aLink ) const
	{
	CCapabilityInfo* capInfo = NULL;
	for( TInt i = 0; i < iDataArray.Count() && !capInfo; i++ )
		{
		if( iDataArray[i]->LinkID() == aLink )
			{
			capInfo = iDataArray[i];	
			}
		}
		
	return capInfo;
	}

TInt CCapabilityArray::Count() const
	{
	return iDataArray.Count();
	}

//  End of File
