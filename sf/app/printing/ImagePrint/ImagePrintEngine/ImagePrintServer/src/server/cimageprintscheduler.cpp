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
* Description:  
*
*/


#include "cimageprintscheduler.h"
#include "mprotprintingdevice.h"
#include "tidleevent.h"
#include "printmessagecodes.h"
#include "clog.h"

void CImagePrintScheduler::Error(TInt aError) const
	{
	LOG1("CImagePrintScheduler::Error aError: %d", aError);
	if( iIdleObserver )
		{
		TEvent event;
		event.iProtocol = iCurrentProtocol;
		event.iSeverity = ESevere;
		event.iEventType = EAsynchronousLeave;
		LOG("CImagePrintScheduler::Error informing observer");		
		iIdleObserver->StatusEvent( event, aError, ENoMessage );
		}
	LOG("CImagePrintScheduler::Error end");
	}

void CImagePrintScheduler::SetIdleObserver( MProtIdleObserver& aIdleObserver )
	{
	LOG("CImagePrintScheduler::SetIdleObserver begin");
	iIdleObserver = &aIdleObserver;
	LOG("CImagePrintScheduler::SetIdleObserver end");
	}

void CImagePrintScheduler::SetCurrentProtocol( TInt aProtocol )
	{
	LOG1("CImagePrintScheduler::SetCurrentProtocol aProtocol: %d", aProtocol);
	iCurrentProtocol = aProtocol;
	LOG("CImagePrintScheduler::SetCurrentProtocol end");
	}

//  End of File
