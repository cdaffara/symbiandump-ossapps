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

/**
 @file
 @internalTechnology
 @released
*/

#ifndef __TMOBILITYTESTPROPERTYMAPACCESS_H__
#define __TMOBILITYTESTPROPERTYMAPACCESS_H__

#include <e32base.h>
#include "tmsgtestpropertymapentry.h"

class TMobilityTestPropertyMapAccess
	{
public:
	IMPORT_C static const TMsgTestPropertyMapEntry* Get(TDesC* aName);
	IMPORT_C static const TMsgTestPropertyMapEntry* Get(TInt aValue);
	};

#endif //__TMOBILITYTESTPROPERTYMAP_H__
