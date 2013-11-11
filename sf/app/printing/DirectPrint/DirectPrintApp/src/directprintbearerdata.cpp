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

#include "directprintbearerdata.h"

EXPORT_C void TDirectPrintBearerData::InternalizeL(RReadStream& aStream)
	{
	iPrinterId = aStream.ReadInt32L();
	iBearerId = aStream.ReadInt32L();
	TInt length = aStream.ReadInt32L();
	if(length > 0)
		{
		aStream.ReadL(iBearerName, length);
		}
	length = aStream.ReadInt32L();
	if(length > 0)
		{
		aStream.ReadL(iAccessPoint, length);
		}
	length = aStream.ReadInt32L();
	if(length > 0)
		{
		aStream.ReadL(iHostName, length);
		}
	iHostIp.SetAddress(aStream.ReadInt32L());
	iPort = aStream.ReadInt32L();
	length = aStream.ReadInt32L();
	if(length > 0)
		{
		aStream.ReadL(iUserName, length);
		}
	length = aStream.ReadInt32L();
	if(length > 0)
		{
		aStream.ReadL(iQueName, length);
		}
	}

EXPORT_C void TDirectPrintBearerData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iPrinterId);
	aStream.WriteInt32L(iBearerId);
	aStream.WriteInt32L(iBearerName.Length());
	if (iBearerName.Length() > 0)
		{
		aStream.WriteL(iBearerName);
		}
	aStream.WriteInt32L(iAccessPoint.Length());
	if (iAccessPoint.Length() > 0)
		{
		aStream.WriteL(iAccessPoint);
		}
	aStream.WriteInt32L(iHostName.Length());
	if (iHostName.Length() > 0)
		{
		aStream.WriteL(iHostName);
		}
	aStream.WriteInt32L(iHostIp.Address());
	aStream.WriteInt32L(iPort);
	aStream.WriteInt32L(iUserName.Length());
	if (iUserName.Length() > 0)
		{
		aStream.WriteL(iUserName);
		}
	aStream.WriteInt32L(iQueName.Length());
	if (iQueName.Length() > 0)
		{
		aStream.WriteL(iQueName);
		}
	}

EXPORT_C TDirectPrintBearerData& TDirectPrintBearerData::operator=(const TDirectPrintBearerData& aData)
	{
	iPrinterId = aData.iPrinterId;
	iBearerId = aData.iBearerId;
	iBearerName.Copy(aData.iBearerName);
	iAccessPoint.Copy(aData.iAccessPoint);
	iHostName.Copy(aData.iHostName);
	//iHostIp.SetAddress(aData.iHostIp.Address());
	iHostIp = aData.iHostIp;
	iPort = aData.iPort;
	iUserName.Copy(aData.iUserName);
	iQueName.Copy(aData.iQueName);
	return *this;
	}

