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

// Protection against nested includes
#ifndef DIRECTPRINTTEXTCAPABILITY_H
#define DIRECTPRINTTEXTCAPABILITY_H

// User includes
#include "directprintbasecapability.h"

/**
 *
 *  Capability type for TInt type of settings
 *
 */
class CDirectPrintTextCapability : public CDirectPrintBaseCapability
	{
public:
	/** Destructor */
	virtual ~CDirectPrintTextCapability();

public: // Methods derived from CBaseCapability

	virtual HBufC* ListBoxTextL();
	virtual CDirectPrintBaseCapability* CloneL();
	virtual CAknSettingItem* SettingItemLC(TInt aIndex);

public: // Data
	TBuf<50> iText;
	};

#endif  //  DIRECTPRINTTEXTCAPABILITY_H

//  End of File
