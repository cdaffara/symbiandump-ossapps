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

#ifndef __CMSGTESTPROPERTYMAPPAIR_H__
#define __CMSGTESTPROPERTYMAPPAIR_H__

#include "tmsgtestpropertymapentry.h"

class CMsgTestPropertyMapPair
	{
public:
	IMPORT_C CMsgTestPropertyMapPair(TMsgTestPropertyMapEntry* aFlag, TMsgTestPropertyMapEntry* aCall);

public:	
	TMsgTestPropertyMapEntry* flag;
	TMsgTestPropertyMapEntry* call;
	};

#endif //__CMSGTESTPROPERTYMAPPAIR_H__
