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

#ifndef __DIRECTPRINTAPP_PAN__
#define __DIRECTPRINTAPP_PAN__

#include <e32std.h>

/** DirectPrintApp application panic codes */
enum TDirectPrintAppPanics
	{
	EDirectPrintAppUi = 1,
	EDirectPrintNoObserver,
	EDirectPrintNullPointer,

	EDirectPrintEnd
	};

inline void Panic(TDirectPrintAppPanics aReason)
	{
	_LIT(applicationName, "DirectPrintApp");
	User::Panic(applicationName, aReason);
	}

#endif // __DIRECTPRINTAPP_PAN__
