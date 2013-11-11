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
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef DIRECTPRINTBASECAPABILITY_H
#define DIRECTPRINTBASECAPABILITY_H

// System includes
#include <e32std.h>
#include <e32base.h>
#include <aknsettingitemlist.h>

//  TYPE DEFINITIONS
typedef TBuf<64> TCapabilityTitle;

/**
 *  
 *  Capability base class, pure virtual, needs to be derived
 *
 */
class CDirectPrintBaseCapability : public CBase
	{
	public:     // New methods

		virtual ~CDirectPrintBaseCapability() {};

		/**
		 *  Creates text for the setting item list box
		 *  
		 *  @return Correctly formatted list box text
		 */
		virtual HBufC* ListBoxTextL() = 0;

		/**
		 *  Clones and returns copy of itself
		 *
		 *  @return Cloned copy of the instance itself
		 */
		virtual CDirectPrintBaseCapability* CloneL() = 0;

		/**
		 *  Creates CAknSettingItem for the list box
		 *  
		 *  @return CAknSettingItem object
		 */
		virtual CAknSettingItem* SettingItemLC(TInt aIndex) = 0;

	public:     // Data

		// The index on the list from the top (0 = topmost)
		TBool iIndexOnList;

		// Title of this capability
		TCapabilityTitle iTitle;

		// UID of the capability
		TInt iUid;

		// Value
		TInt iValue;
	};

#endif  // DIRECTPRINTBASECAPABILITY_H

//  End of File
