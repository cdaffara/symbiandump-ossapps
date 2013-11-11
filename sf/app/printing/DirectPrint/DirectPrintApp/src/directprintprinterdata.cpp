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

// INCLUDE FILES
#include <s32strm.h>

#include "directprintprinterdata.h"

EXPORT_C void TDirectPrintPrinterData::InternalizeL(RReadStream& aStream)
	{
	iPrinterId = aStream.ReadInt32L();
	TInt length = aStream.ReadInt32L();
	if(length > 0)
		{
		aStream.ReadL(iPrinterName, length);
		}
	iPrinterDriver = TUid::Uid(aStream.ReadInt32L());
	length = aStream.ReadInt32L();
	if(length > 0)
		{
		aStream.ReadL(iDriverName, length);
		}
	iPrinterType = aStream.ReadInt32L();
	length = aStream.ReadInt32L();
	if(length > 0)
		{
		aStream.ReadL(iTypeName, length);
		}
	}

EXPORT_C void TDirectPrintPrinterData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iPrinterId);
	aStream.WriteInt32L(iPrinterName.Length());
	if (iPrinterName.Length() > 0)
		{
		aStream.WriteL(iPrinterName);
		}
	aStream.WriteInt32L(iPrinterDriver.iUid);
	aStream.WriteInt32L(iDriverName.Length());
	if (iDriverName.Length() > 0)
		{
		aStream.WriteL(iDriverName);
		}
	aStream.WriteInt32L(iPrinterType);
	aStream.WriteInt32L(iTypeName.Length());
	if (iTypeName.Length() > 0)
		{
		aStream.WriteL(iTypeName);
		}
	}

EXPORT_C TDirectPrintPrinterData& TDirectPrintPrinterData::operator=(const TDirectPrintPrinterData& aData)
	{
	iPrinterId = aData.iPrinterId;
	iPrinterName.Copy(aData.iPrinterName);
	iPrinterDriver = aData.iPrinterDriver;
	iDriverName.Copy(aData.iDriverName);
	iPrinterType = aData.iPrinterType;
	iTypeName.Copy(aData.iTypeName);
	return *this;
	}

