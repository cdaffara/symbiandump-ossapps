/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition file for the class AlarmListener
*
*/



#ifndef ALARMLISTENER_H
#define ALARMLISTENER_H

#include <e32base.h>
#include <asclisession.h>
#include <asshddefs.h>

class AlarmClient;

class AlarmListener : public CActive
{
public:
	AlarmListener(AlarmClient* client, RASCliSession& session);
	~AlarmListener();
	void start();
	void stop();

protected:
	void RunL();
	void DoCancel();

private:
	AlarmClient* mClient;

	RASCliSession& mSession;
	TAlarmId mAlarmId;
};

#endif // ALARMLISTENER_H

// End of file
