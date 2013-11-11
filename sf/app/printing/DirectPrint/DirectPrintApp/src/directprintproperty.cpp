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

#include "directprintproperty.h"

EXPORT_C CPrinterProperty::CPrinterProperty()
	{
	}

EXPORT_C CPrinterProperty::~CPrinterProperty()
	{
	iPropertyItems.Close();
	}

EXPORT_C void CPrinterProperty::InternalizeL(RReadStream& aStream)
	{
	iPrinterId = aStream.ReadInt32L();
	iApplicationId = aStream.ReadInt32L();
	iPropertyItems.Reset();
	TInt count = aStream.ReadInt32L();
	for (TInt i=0; i<count; i++)
		{
		TDirectPrintPropertyData item;
		item.InternalizeL(aStream);
		iPropertyItems.Append(item);
		}
	}

EXPORT_C void CPrinterProperty::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iPrinterId);
	aStream.WriteInt32L(iApplicationId);
	TInt count = iPropertyItems.Count();
	aStream.WriteInt32L(count);
	for (TInt i=0; i<count; i++)
		{
		iPropertyItems[i].ExternalizeL(aStream);
		}
	}

EXPORT_C CPrinterProperty& CPrinterProperty::operator=(const CPrinterProperty& aProperty)
	{
	iPrinterId = aProperty.iPrinterId;
	iApplicationId = aProperty.iApplicationId;
	iPropertyItems.Reset();
	TInt count = aProperty.iPropertyItems.Count();
	for (TInt i=0; i<count; i++)
		{
		iPropertyItems.Append(aProperty.iPropertyItems[i]);
		}
	return *this;
	}

