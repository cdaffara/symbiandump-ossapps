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
* Description:  Main test class for contactwidgethsplugin library.
*
*/


#include <QtTest/QtTest>
#include "t_contactwidgethsplugin.h"
#include "contactwidgethsplugin.h"

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>
#include <qservicemanager.h>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestContactWidgetPlugin::init()
{
    mContactWidgetPlugin = new ContactWidgetHsPlugin;
    QVERIFY(mContactWidgetPlugin);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestContactWidgetPlugin::cleanup()
{
    if (mContactWidgetPlugin) {
        delete mContactWidgetPlugin;
        mContactWidgetPlugin = 0;
    QVERIFY(mContactWidgetPlugin == 0);
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestContactWidgetPlugin::testCreateInstance()
{
    QServiceManager serviceManager;
    ContactWidgetHsPlugin plugin;
    QServiceFilter serviceFilter("com.nokia.symbian.IHomeScreenWidget");
    serviceFilter.setServiceName("contactwidgethsplugin");
    QList<QServiceInterfaceDescriptor> serviceInterfaceDescriptorList =
        serviceManager.findInterfaces(serviceFilter);
    int len = serviceInterfaceDescriptorList.size();
    QVERIFY(len > 0);
    QServiceInterfaceDescriptor serviceInterfaceDescriptor =
        serviceInterfaceDescriptorList[0];
    QObject *handler = plugin.createInstance(
        serviceInterfaceDescriptor, NULL, NULL);
    QVERIFY(handler);
    delete handler;  
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestContactWidgetPlugin::testCreateInvalidInstance()
{
    QServiceInterfaceDescriptor descriptor;
    
    QObject *instanceValue = NULL;
    instanceValue = mContactWidgetPlugin->createInstance(descriptor, NULL, NULL);
    
    QVERIFY(instanceValue == NULL);
    delete instanceValue;
}

QTEST_MAIN(TestContactWidgetPlugin)
