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
* Description:  Contains the TDirectPrintCapability class definition.
*
*/


// Protection against nested includes
#ifndef DIRECTPRINTCAPABILITY_H
#define DIRECTPRINTCAPABILITY_H

// System includes
#include <e32base.h>

/**
 *	@brief A printer capability abstraction.
 *
 *	Class describing a capability (i.e. paper sizes or layouts) supported by a printing device.
 *	Published as part of the ImagePrint Server API. 
 */
 
#define KMaxEnumAmount 20 
 
class TDirectPrintCapability
	{
	public:
		enum ECapType
			{
			Int = 0,
			Enum = 1,
			Float = 2,
			Text = 3
			};
		
		TInt					iCapabilityID;
		ECapType				iType;
		TInt					iDefaultValue;
		TInt					iLow;
		TInt					iHigh;
		TInt					iEnumCount;
		TInt					iEnumCodes[KMaxEnumAmount];
	};

#endif // DIRECTPRINTCAPABILITY_H

//  End of File
