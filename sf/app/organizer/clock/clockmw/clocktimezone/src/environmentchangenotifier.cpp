/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition file for the EnvironmentChangeNotifier class.
*
*/

// System includes
#include <bacntf.h>

// User includes
#include "environmentchangenotifier.h"
#include "timezoneclient.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "environmentchangenotifierTraces.h"
#endif


/*!
	\class EnvironmentChangeNotifier

	This class listens to the enviroment changes and notifies the client.
 */

/*!
	The constructor.
 */
EnvironmentChangeNotifier::EnvironmentChangeNotifier(TimezoneClient *client)
{
	OstTraceFunctionEntry0( ENVIRONMENTCHANGENOTIFIER_ENVIRONMENTCHANGENOTIFIER_ENTRY );
	mTzClient = client;
	
	TCallBack callback(environmentCallback, this);

	mNotifier = CEnvironmentChangeNotifier::NewL(
			CActive::EPriorityStandard, callback);
	mNotifier->Start();

	OstTraceFunctionExit0( ENVIRONMENTCHANGENOTIFIER_ENVIRONMENTCHANGENOTIFIER_EXIT );
}

EnvironmentChangeNotifier::~EnvironmentChangeNotifier()
{
	OstTraceFunctionEntry0( DUP1_ENVIRONMENTCHANGENOTIFIER_ENVIRONMENTCHANGENOTIFIER_ENTRY );
	if (mNotifier) {
		mNotifier->Cancel();
		delete mNotifier;
		mNotifier = 0;
	}
	OstTraceFunctionExit0( DUP1_ENVIRONMENTCHANGENOTIFIER_ENVIRONMENTCHANGENOTIFIER_EXIT );
}

int EnvironmentChangeNotifier::environmentCallback(TAny* obj)
{
	OstTraceFunctionEntry0( ENVIRONMENTCHANGENOTIFIER_ENVIRONMENTCALLBACK_ENTRY );
	EnvironmentChangeNotifier* self =
			static_cast<EnvironmentChangeNotifier *> (obj);

	int changes = KInitialEvent;
	if (self->mNotifier) {
		changes = self->mNotifier->Change();
	}

	if (KInitialEvent <= changes) {
		// We're not concerned about handling environment changes in that range.
		OstTraceFunctionExit0( ENVIRONMENTCHANGENOTIFIER_ENVIRONMENTCALLBACK_EXIT );
		return 0;
	}

	if (changes & (EChangesMidnightCrossover |
			EChangesLocale |
			EChangesSystemTime)) {
//		emit self->timechanged(); TODO: notify timezone client.
//		
		self->notifyEnvChange();
	} else {
		// Nothing to do.
	}
	OstTraceFunctionExit0( DUP1_ENVIRONMENTCHANGENOTIFIER_ENVIRONMENTCALLBACK_EXIT );
	return 0;
}

void EnvironmentChangeNotifier::notifyEnvChange()
{
	OstTraceFunctionEntry0( ENVIRONMENTCHANGENOTIFIER_NOTIFYENVCHANGE_ENTRY );
	mTzClient->notifyTimeChange();
	OstTraceFunctionExit0( ENVIRONMENTCHANGENOTIFIER_NOTIFYENVCHANGE_EXIT );
}

// End of file -- Do not remove this.
