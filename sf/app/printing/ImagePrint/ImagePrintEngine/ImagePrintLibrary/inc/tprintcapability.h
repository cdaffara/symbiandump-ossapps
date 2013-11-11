/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the TPrintCapability class definition.
*
*/


#ifndef TPRINTCAPABILITY_H
#define TPRINTCAPABILITY_H

#include <e32base.h>

/**
 *	@brief A printer capability abstraction.
 *
 *	Class describing a capability (i.e. paper sizes or layouts) supported by a printing device.
 *	Published as part of the ImagePrint Server API. 
 */
 
#define KMaxEnumAmount 20 
 
class TPrintCapability
	{
	public:
		enum ECapType
			{
			Int = 0,
			Enum = 1,
			Float = 2
			};
		
		TInt					iCapabilityID;
		ECapType				iType;
		TInt					iDefaultValue;
		TInt					iLow;
		TInt					iHigh;
		TInt					iEnumCount;
		TInt					iEnumCodes[KMaxEnumAmount];
	};

#endif // TPRINTCAPABILITY_H

//  End of File
