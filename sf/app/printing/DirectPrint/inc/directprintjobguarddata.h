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
#ifndef DIRECTPRINTJOBGUARDDATA_H
#define DIRECTPRINTJOBGUARDDATA_H

// System includes
#include <e32def.h>

class TDirectPrintJobGuardData
	{
public:

	enum TCallbackNum
		{
		EProgressEventCb = 0,
		EErrorEventCb,
		EStatusEventCb,
		EImageEventCb
		};

public:

	TInt iStatus;
	TInt iPercentCompletion;
	TInt iJobStateCode;
	TInt iError;
	TInt iErrorStringCode;
	TInt iFsBitmapHandle;
	TCallbackNum iCb;
	};

#endif // DIRECTPRINTJOBGUARDDATA_H

//  End of File
