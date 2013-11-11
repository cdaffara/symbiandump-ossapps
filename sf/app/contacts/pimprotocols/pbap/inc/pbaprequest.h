// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PBAPREQUEST_H
#define PBAPREQUEST_H

#include <e32def.h>

namespace SymbianPBAP
	{

	//the 64-bit const value for attribute filters
	const TUint64 KPbapAttributeAll 			= 0x0;			// All attributes
	const TUint64 KPbapAttributeMaskCallDateTime	= 0x10000000;	// X-IRMC-CALL-DATETIME

	enum TOrder
		{
		EIndexed,
		EAlphabetical,
		EPhonetical,
		EUnknownOrder
		};
		
	enum TSearchAttribute
		{
		EName,
		ENumber,
		ESound,
		EUnknownSearchAttribute
		};
	}
	
#endif //PBAPREQUEST_H
