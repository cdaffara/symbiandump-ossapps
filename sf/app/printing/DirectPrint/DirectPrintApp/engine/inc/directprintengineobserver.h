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
#ifndef __DIRECTPRINTENGINEOBSERVER_H__
#define __DIRECTPRINTENGINEOBSERVER_H__

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

class MDirectPrintEngineObserver
	{
public:
	enum TDirectPrintEngineEvents
		{
		EDirectPrintEngineEventStartCreatePrintData,
		EDirectPrintEngineEventFinishCreatePrintData,
		EDirectPrintEngineEventCancelCreatePrintData,
		EDirectPrintEngineEventErrorCreatePrintData,

		EDirectPrintEngineEventErrorConnectBearer,
		EDirectPrintEngineEventConnectedBearer,
		// add any events, if needed

		// end of event
		EDirectPrintEngineEventEnd
		};

public:
	virtual void HandlePrintEventL(TDirectPrintEngineEvents aEvent, TInt aError, const TDesC& aParam) = 0;
	};

#endif // __DIRECTPRINTENGINEOBSERVER_H__
// End of file
