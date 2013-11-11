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


#include "directprintscheduler.h"
#include "mprotprintingdevice.h"
#include "tidleevent.h"
#include "printmessagecodes.h"
#include "clog.h"

void CDirectPrintScheduler::Error(TInt aError) const
	{
	LOG1("CDirectPrintScheduler::Error aError: %d", aError);
	if( iIdleObserver )
		{
		TEvent event;
		event.iProtocol = iCurrentProtocol;
		event.iSeverity = ESevere;
		event.iEventType = EAsynchronousLeave;
		LOG("CDirectPrintScheduler::Error informing observer");		
		iIdleObserver->StatusEvent( event, aError, ENoMessage );
		}
	LOG("CDirectPrintScheduler::Error end");
	}

void CDirectPrintScheduler::SetIdleObserver( MProtIdleObserver& aIdleObserver )
	{
	LOG("CDirectPrintScheduler::SetIdleObserver begin");
	iIdleObserver = &aIdleObserver;
	LOG("CDirectPrintScheduler::SetIdleObserver end");
	}

void CDirectPrintScheduler::SetCurrentProtocol( TInt aProtocol )
	{
	LOG1("CDirectPrintScheduler::SetCurrentProtocol aProtocol: %d", aProtocol);
	iCurrentProtocol = aProtocol;
	LOG("CDirectPrintScheduler::SetCurrentProtocol end");
	}

//  End of File
