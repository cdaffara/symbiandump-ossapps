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
* Description: Declaration of the class TestClockSettingsViewPlugin.
*
*/

#ifndef UNITTEST_CLOCKSETTINGSVIEWPLUGIN_H
#define UNITTEST_CLOCKSETTINGSVIEWPLUGIN_H

// System includes
#include <QObject>

class TestClockSettingsViewPlugin : public QObject
{
	Q_OBJECT

public:
	TestClockSettingsViewPlugin();
	virtual ~TestClockSettingsViewPlugin();

private slots:
	// Test case setup.
	void init();
	void cleanup();

	// Test cases go here.
	void testPluginLoadUnLoad();
};

#endif // UNITTEST_CLOCKSETTINGSVIEWPLUGIN_H

// End of file	--Don't remove this.
