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
* Description:  Contains the CDirectPrintProtocolsLoader class definition.
*
*/


#include <ecom/ecom.h>

#include "directprintprotocolsloader.h"
#include "mprotprintingdevice.h"
#include "clog.h"
#include "imageprint.h"

namespace
	{
	const TUid KDirectPrintEcomUid = { 0x10208A1E };
	}

CDirectPrintProtocolsLoader* CDirectPrintProtocolsLoader::NewL()
	{		
	LOG("[CDirectPrintProtocolsLoader::NewL]\t Begin");
	CDirectPrintProtocolsLoader* self = new (ELeave) CDirectPrintProtocolsLoader();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	LOG("[CDirectPrintProtocolsLoader::NewL]\t End");
	return self;	
	}


CDirectPrintProtocolsLoader::~CDirectPrintProtocolsLoader()
	{
	LOG("[CDirectPrintProtocolsLoader::~CDirectPrintProtocolsLoader]\t Begin");
	iProtocols.ResetAndDestroy();
	iProtocolInfos.ResetAndDestroy();
	LOG("[CDirectPrintProtocolsLoader::~CDirectPrintProtocolsLoader]\t End");
	}


TInt CDirectPrintProtocolsLoader::GetNumberOfProtocolsAvailable() const
	{
	LOG("[CDirectPrintProtocolsLoader::GetNumberOfProtocolsAvailable]\t Begin");	
	LOG1("[CDirectPrintProtocolsLoader::GetNumberOfProtocolsAvailable]\t return: %d", iAvailableProtocols);
	LOG("[CDirectPrintProtocolsLoader::GetNumberOfProtocolsAvailable]\t End");
	return iAvailableProtocols;
	}


MProtPrintingDevice* CDirectPrintProtocolsLoader::GetNextProtocol()
	{
	LOG("[CDirectPrintProtocolsLoader::GetNextProtocol]\t Begin");
	if (iAvailableProtocols <= 0)
		return NULL;

	if (iProtIndex == iAvailableProtocols)
		iProtIndex = 0;

	MProtPrintingDevice* temp = iProtocols[iProtIndex];
	iProtIndex++;
	LOG("[CDirectPrintProtocolsLoader::GetNextProtocol]\t End");
	return temp;
	}


MProtPrintingDevice* CDirectPrintProtocolsLoader::GetProtocolAt(TInt aIndex)
	{
	LOG("[CDirectPrintProtocolsLoader::GetProtocolAt]\t Begin");
	if( iAvailableProtocols <= 0 ) return NULL;

	MProtPrintingDevice* temp = NULL;

	if (aIndex < 0)
		temp = (MProtPrintingDevice*) iProtocols[0];
	else
		if (aIndex >= iAvailableProtocols)
			temp = (MProtPrintingDevice*) iProtocols[iAvailableProtocols - 1];
		else
			temp = (MProtPrintingDevice*) iProtocols[aIndex];

	LOG("[CDirectPrintProtocolsLoader::GetProtocolAt]\t End");
	return temp;
	
	}


void CDirectPrintProtocolsLoader::Reset()
	{
	LOG("[CDirectPrintProtocolsLoader::Reset]\t Begin");
	iProtIndex = 0;
	LOG("[CDirectPrintProtocolsLoader::Reset]\t End");
	}


TInt CDirectPrintProtocolsLoader::GetProtocolIndex() const
	{
	LOG("[CDirectPrintProtocolsLoader::GetProtocolIndex]\t Begin");

	TInt retVal = iProtIndex - 1;
	// The next check is probably unnecessary since GetNextProtocol() should only leave iProtIndex with
	// a value between 1 and iAvailableProtocols
	if (retVal < 0)
	{
		retVal = iProtocols.Count() - 1;
	}
	LOG1("[CDirectPrintProtocolsLoader::GetProtocolIndex] return: %d", retVal);
	LOG("[CDirectPrintProtocolsLoader::GetProtocolIndex]\t End");
	return retVal;
	}


TUint CDirectPrintProtocolsLoader::SupportedProtocols() const
	{
	LOG("[CDirectPrintProtocolsLoader::SupportedProtocols]\t Begin");
	LOG1("[CDirectPrintProtocolsLoader::SupportedProtocols] return: %d", iSupportedProtocols);
	LOG("[CDirectPrintProtocolsLoader::SupportedProtocols]\t End");
	return iSupportedProtocols;
	}


TInt CDirectPrintProtocolsLoader::GetProtocolName(TInt aIndex, TDes& aName, TInt& aUid)
	{
	TInt retVal = KErrNone;

	if( iAvailableProtocols <= 0 )
		{
		retVal = KErrNotFound;
		}
	else
		{
		CDirectPrintProtocolInfo* info = NULL;

		if (aIndex < 0)
			{
			info = iProtocolInfos[0];
			}
		else
			{
			if (aIndex >= iAvailableProtocols)
				{
				info = iProtocolInfos[iAvailableProtocols - 1];
				}
			else
				{
				info = iProtocolInfos[aIndex];
				}
			}

		if (info)
			{
			aName.Copy(info->DisplayName());
			aUid = info->ImplementationUid().iUid;
			}
		}

	return retVal;
	}


CDirectPrintProtocolsLoader::CDirectPrintProtocolsLoader()
	{
	LOG("[CDirectPrintProtocolsLoader::CDirectPrintProtocolsLoader]\t Begin");
	iAvailableProtocols = 0;
	iProtIndex = 0;
	iSupportedProtocols = 0;
	LOG("[CDirectPrintProtocolsLoader::CDirectPrintProtocolsLoader]\t End");
	}

void CDirectPrintProtocolsLoader::ConstructL()
	{
	LOG("[CDirectPrintProtocolsLoader::ConstructL]\t Begin");
	LoadL();
	LOG("[CDirectPrintProtocolsLoader::ConstructL]\t End");
	}

void CDirectPrintProtocolsLoader::LoadL()
	{
	LOG("[CDirectPrintProtocolsLoader::LoadL]\t Begin");
	RImplInfoPtrArray infoArray;
	REComSession::ListImplementationsL( KDirectPrintEcomUid, infoArray );
	CleanupClosePushL( infoArray );
	MProtPrintingDevice* device = NULL;
	LOG1("CDirectPrintProtocolsLoader::LoadL infoArray.Count(): %d", infoArray.Count());
	for( TInt i = 0; i < infoArray.Count(); i++ )
		{
		LOG1("CDirectPrintProtocolsLoader::LoadL i: %d", i);
		CImplementationInformation* info = infoArray[i];
		if (info->Version() == 2)
			{
			CDirectPrintProtocolInfo* protocolInfo = CDirectPrintProtocolInfo::NewLC(*info);
			iProtocolInfos.AppendL( protocolInfo );
			CleanupStack::Pop( protocolInfo );

			TPtrC8 dataType = info->DataType();
			TEComResolverParams resolverParams;
			resolverParams.SetDataType( dataType );
			resolverParams.SetWildcardMatch( ETrue );
			TAny* prot = REComSession::CreateImplementationL( KDirectPrintEcomUid,
					_FOFF( MProtPrintingDevice, iDtor_ID_Key ), NULL, resolverParams );
			device = reinterpret_cast<MProtPrintingDevice*>(prot);
			//device->ConstructL( KNullDesC );
			//device = reinterpret_cast<MProtPrintingDevice*>(prot);

			TCleanupItem clItem( CleanupProt, device );
			CleanupStack::PushL( clItem );
			device->ConstructL( KNullDesC );
			LOG1("CDirectPrintProtocolsLoader::LoadL device->SupportedProtocols(): %d", device->SupportedProtocols());
			iProtocols.AppendL( device );
			iSupportedProtocols |= device->SupportedProtocols();
			CleanupStack::Pop(); // device
			device = NULL;
			}
		}

	infoArray.ResetAndDestroy();
	CleanupStack::PopAndDestroy(); // infoArray

	iAvailableProtocols = iProtocols.Count();
	LOG("[CDirectPrintProtocolsLoader::LoadL]\t End");
	}

void CDirectPrintProtocolsLoader::CleanupProt( TAny* aData )
    {
    LOG("[CDirectPrintProtocolsLoader::CleanupProt]\t Begin");
    MProtPrintingDevice* prot = (MProtPrintingDevice*)aData;
    delete prot;
    LOG("[CDirectPrintProtocolsLoader::CleanupProt]\t End");
    }

//  End of File
