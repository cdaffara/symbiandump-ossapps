/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ENVIRONMENTCHANGENOTIFIER_H_
#define ENVIRONMENTCHANGENOTIFIER_H_

// System includes
#include <e32base.h>

// Forward declaration
class CEnvironmentChangeNotifier;
class TimezoneClient;

const int KInitialEvent = (
		EChangesLocale |
		EChangesMidnightCrossover |
		EChangesThreadDeath |
		EChangesPowerStatus |
		EChangesSystemTime |
		EChangesFreeMemory |
		EChangesOutOfMemory);

class EnvironmentChangeNotifier : public CBase
{
public:
	EnvironmentChangeNotifier(TimezoneClient *client);
	~EnvironmentChangeNotifier();

public:
	static int environmentCallback(TAny* obj);
	
private:
	void notifyEnvChange();

private:
	CEnvironmentChangeNotifier *mNotifier;
	TimezoneClient *mTzClient;
};

#endif /* ENVIRONMENTCHANGENOTIFIER_H_ */

// End of file -- Do not remove this.
