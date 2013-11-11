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
* Description: Definition file for class TestClockSettingsViewPlugin.
*
*/

// System include
#include <QtTest/QtTest>
#include <ClockSettingsViewInterface>

// User includes
#include "unittest_clocksettingsviewplugin.h"


/*!
	\class TestClockSettingsViewPlugin

	Unit test class for Clock settings view Plugin apis
 */


/*!
	Constructor
 */

TestClockSettingsViewPlugin::TestClockSettingsViewPlugin()
{
}

/*!
	Destructor
 */
TestClockSettingsViewPlugin::~TestClockSettingsViewPlugin()
{
}

/*!
	Initialise before calling the test case
 */
void TestClockSettingsViewPlugin::init()
{
}

/*!
	Do cleanup after each test case
 */
void TestClockSettingsViewPlugin::cleanup()
{
}

/*!
	Test the api of clock setting plugin
 */

void TestClockSettingsViewPlugin::testPluginLoadUnLoad()
{
	QDir dir(CLOCK_SETTINGSVIEW_PLUGIN_PATH);
	QString pluginName = dir.absoluteFilePath(CLOCK_SETTINGSVIEW_PLUGIN_NAME);
	
	// Create Clock setting view object
	QPluginLoader *pluginLoader = new QPluginLoader(pluginName);
	
	// Load the plugin
	QVERIFY(pluginLoader->load());

	QObject *plugin = qobject_cast<QObject*> (pluginLoader->instance());

	ClockSettingsViewInterface* interface =
			qobject_cast<ClockSettingsViewInterface*>(plugin);

	QVERIFY(interface);

	// Unload the plugin
	QVERIFY(pluginLoader->unload());
	delete pluginLoader;
}

// End of file	--Don't remove this.
