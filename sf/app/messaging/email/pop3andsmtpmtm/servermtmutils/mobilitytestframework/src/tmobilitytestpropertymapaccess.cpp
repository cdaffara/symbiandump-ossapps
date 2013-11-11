// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "tmobilitytestpropertymapaccess.h"
#include "mobilitytestpropertymap.h"

EXPORT_C const TMsgTestPropertyMapEntry* TMobilityTestPropertyMapAccess::Get(TDesC* aName)
	{
	TInt i=0;

	while (propertyMapList[i].value != 0xffffffff)
		{
		TBufC8<80> buf8((const unsigned char*)(propertyMapList[i].name));

		TBuf<80> buf;
		buf.Copy(buf8);

		if (aName->CompareF(buf) == 0)
			{
			return &propertyMapList[i];
			}

		++i;
		}

	return NULL;
	}

EXPORT_C const TMsgTestPropertyMapEntry* TMobilityTestPropertyMapAccess::Get(TInt aValue)
	{
	TInt i=0;

	while (propertyMapList[i].value != 0xFFFFFFFF)
		{
		if (propertyMapList[i].value == aValue)
			{
			return &propertyMapList[i];
			}
		
		++i;
		}

	return NULL;
	}
