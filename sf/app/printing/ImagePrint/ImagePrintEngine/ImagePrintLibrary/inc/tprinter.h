/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the TPrinter class definition.
*
*/


#ifndef TPRINTER_H
#define TPRINTER_H

#include <e32base.h>

/**
 *	@brief A printer abstraction.
 *
 *	Class representing the basic properties of a printing device.
 *	Published as part of the ImagePrint Server API.
 */
class TPrinter
	{
	public:
		enum TPrinterProperty
			{
			Cached = 1,
			SupportsPreview = 2
			};
		enum TPrinterVendor
			{
			EVendorNone = 0, // No Vendor
			EVendorHp
			// Add other vendors
			};
		TInt		iPrinterID;
		TBuf<84>	iDisplayName;
		TUint		iProperties;
		TUint		iProtocol;
		TUint		iVendor;
	};

#endif // TPRINTER_H

//  End of File
