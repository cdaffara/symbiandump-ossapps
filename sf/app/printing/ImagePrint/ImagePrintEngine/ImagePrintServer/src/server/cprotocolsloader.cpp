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
* Description:  Contains the CProtocolsLoader class definition.
*
*/


#include <ecom/ecom.h>

#include "cprotocolsloader.h"
#include "mprotprintingdevice.h"
#include "clog.h"
#include "imageprint.h"

namespace
	{
	const TUid KImagePrintEcomUid = { 0x10208A1E };
	}

CProtocolsLoader* CProtocolsLoader::NewL()
	{		
	LOG("[CProtocolsLoader::NewL]\t Begin");
	CProtocolsLoader* self = new (ELeave) CProtocolsLoader();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	LOG("[CProtocolsLoader::NewL]\t End");
	return self;	
	}


CProtocolsLoader::~CProtocolsLoader()
	{
	LOG("[CProtocolsLoader::~CProtocolsLoader]\t Begin");
	iProtocols.ResetAndDestroy(); 
	LOG("[CProtocolsLoader::~CProtocolsLoader]\t End");
	}


TInt CProtocolsLoader::GetNumberOfProtocolsAvailable() const
	{
	LOG("[CProtocolsLoader::GetNumberOfProtocolsAvailable]\t Begin");	
	LOG1("[CProtocolsLoader::GetNumberOfProtocolsAvailable]\t return: %d", iAvailableProtocols);
	LOG("[CProtocolsLoader::GetNumberOfProtocolsAvailable]\t End");
	return iAvailableProtocols;
	}


MProtPrintingDevice* CProtocolsLoader::GetNextProtocol()
	{
	LOG("[CProtocolsLoader::GetNextProtocol]\t Begin");
	if (iAvailableProtocols <= 0)
		return NULL;

	if (iProtIndex == iAvailableProtocols)
		iProtIndex = 0;

	MProtPrintingDevice* temp = iProtocols[iProtIndex];
	iProtIndex++;
	LOG("[CProtocolsLoader::GetNextProtocol]\t End");
	return temp;
	}


MProtPrintingDevice* CProtocolsLoader::GetProtocolAt(TInt aIndex)
	{
	LOG("[CProtocolsLoader::GetProtocolAt]\t Begin");
	if( iAvailableProtocols <= 0 ) return NULL;

	MProtPrintingDevice* temp = NULL;

	if (aIndex < 0)
		temp = (MProtPrintingDevice*) iProtocols[0];
	else
		if (aIndex >= iAvailableProtocols)
			temp = (MProtPrintingDevice*) iProtocols[iAvailableProtocols - 1];
		else
			temp = (MProtPrintingDevice*) iProtocols[aIndex];

	LOG("[CProtocolsLoader::GetProtocolAt]\t End");
	return temp;
	
	}


void CProtocolsLoader::Reset()
	{
	LOG("[CProtocolsLoader::Reset]\t Begin");
	iProtIndex = 0;
	LOG("[CProtocolsLoader::Reset]\t End");
	}


TInt CProtocolsLoader::GetProtocolIndex() const
	{
	LOG("[CProtocolsLoader::GetProtocolIndex]\t Begin");

	TInt retVal = iProtIndex - 1;
	// The next check is probably unnecessary since GetNextProtocol() should only leave iProtIndex with
	// a value between 1 and iAvailableProtocols
	if (retVal < 0)
	{
		retVal = iProtocols.Count() - 1;
	}
	LOG1("[CProtocolsLoader::GetProtocolIndex] return: %d", retVal);
	LOG("[CProtocolsLoader::GetProtocolIndex]\t End");
	return retVal;
	}


TUint CProtocolsLoader::SupportedProtocols() const
	{
	LOG("[CProtocolsLoader::SupportedProtocols]\t Begin");
	LOG1("[CProtocolsLoader::SupportedProtocols] return: %d", iSupportedProtocols);
	LOG("[CProtocolsLoader::SupportedProtocols]\t End");
	return iSupportedProtocols;
	}


CProtocolsLoader::CProtocolsLoader()
	{
	LOG("[CProtocolsLoader::CProtocolsLoader]\t Begin");
	iAvailableProtocols = 0;
	iProtIndex = 0;
	iSupportedProtocols = 0;
	LOG("[CProtocolsLoader::CProtocolsLoader]\t End");
	}

void CProtocolsLoader::ConstructL()
	{
	LOG("[CProtocolsLoader::ConstructL]\t Begin");
	LoadL();
	LOG("[CProtocolsLoader::ConstructL]\t End");
	}

void CProtocolsLoader::LoadL()
	{
	LOG("[CProtocolsLoader::LoadL]\t Begin");	
	RImplInfoPtrArray infoArray;		
	REComSession::ListImplementationsL( KImagePrintEcomUid, infoArray );	
	CleanupClosePushL( infoArray );		
	MProtPrintingDevice* device = NULL;	
	LOG1("CProtocolsLoader::LoadL infoArray.Count(): %d", infoArray.Count());		
	for( TInt i = 0; i < infoArray.Count(); i++ )
		{		
		LOG1("CProtocolsLoader::LoadL i: %d", i);
		TPtrC8 dataType = infoArray[i]->DataType();
		TEComResolverParams resolverParams;
		resolverParams.SetDataType( dataType );
		resolverParams.SetWildcardMatch( ETrue );		
		TAny* prot = REComSession::CreateImplementationL( KImagePrintEcomUid,
				_FOFF( MProtPrintingDevice, iDtor_ID_Key ), NULL, resolverParams );
		device = static_cast<MProtPrintingDevice*>(prot);						
		TCleanupItem clItem( CleanupProt, device );
        CleanupStack::PushL( clItem );				
		device->ConstructL( KNullDesC );
		LOG1("CProtocolsLoader::LoadL device->SupportedProtocols(): %d", device->SupportedProtocols());			
		iProtocols.AppendL( device );
		iSupportedProtocols |= device->SupportedProtocols();														
		CleanupStack::Pop(); // device
		device = NULL;		
		}

	infoArray.ResetAndDestroy();
	CleanupStack::PopAndDestroy(); // infoArray		
	
	iAvailableProtocols = iProtocols.Count();
	LOG("[CProtocolsLoader::LoadL]\t End");
	}
				
void CProtocolsLoader::CleanupProt( TAny* aData )
    {
    LOG("[CProtocolsLoader::CleanupProt]\t Begin");
    MProtPrintingDevice* prot = (MProtPrintingDevice*)aData;
    delete prot;
    LOG("[CProtocolsLoader::CleanupProt]\t End");
    }	

//  End of File
