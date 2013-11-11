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
* Description:  Contains the CCapabilityArray class definition.
*
*/


#ifndef CCAPABILITYARRAY_H
#define CCAPABILITYARRAY_H

#include <e32base.h>
#include <barsread.h> 

#include "ccapabilityinfo.h"

/**
 *	@brief Class that stores an array of CCapabilityInfo objects.
 *
 *	class CCapabilityArray - Holds instances of all the capabilities for a particular
 *	transport type (protocol).  This array is populated during the constructor of the protocol object.
 *	One array for each transport type.
 */
class CCapabilityArray : public CBase
    {
	public:
		static CCapabilityArray* NewL(TResourceReader& aReader);
		~CCapabilityArray();
		TInt Count() const;
		CCapabilityInfo* operator[](TInt aIndex) const;
		CCapabilityInfo* LinkedCapability(TInt aLink) const;
	
	private:
		void ConstructL(TResourceReader& aReader);
		void AddDataL(TResourceReader& aReader);
	
	private:
		RPointerArray<CCapabilityInfo> iDataArray;
    };

#endif // CCAPABILITYARRAY_H

//  End of File
