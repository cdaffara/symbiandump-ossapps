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
* Description:  Wallpaper test.
*
*/

#include "t_hsdomainmodel.h"
#include "hswidgetcomponentregistry.h"
#include "hswidgetcomponent.h"
#include "hswidgetcomponentdescriptor.h"
#include "caentry.h"
#include "hsapp_defs.h"
/*!

*/
void TestHsDomainModel::testWidgetComponentRegistry()
{
    HsWidgetComponent* compo = HsWidgetComponentRegistry::instance()->component("mockwidgetplugin");
    QVERIFY(compo);
    QSignalSpy spy(compo,SIGNAL(aboutToUninstall()));
    HsWidgetComponentDescriptor widgetDescriptor;
    widgetDescriptor.setUri("mockwidgetplugin");
    // this deletes compo
    HsWidgetComponentRegistry::instance()->uninstallComponent(widgetDescriptor);
    QVERIFY(spy.count()==1);
    spy.clear();
    widgetDescriptor.setUri("notfoundwidget");
    HsWidgetComponentRegistry::instance()->uninstallComponent(widgetDescriptor);
    QVERIFY(spy.count()==0);
}

void TestHsDomainModel::testWidgetComponentRegistryOnEntryChanged()
{
    HsWidgetComponent* compo = HsWidgetComponentRegistry::instance()->component("mockwidgetplugin");
    QVERIFY(compo);
    CaEntry caEntry;
    caEntry.setAttribute(Hs::widgetPathAttributeName, "mockwidgetplugin");
    HsWidgetComponentRegistry::instance()->onEntryChanged(caEntry, AddChangeType);
    HsWidgetComponentRegistry::instance()->onEntryChanged(caEntry, RemoveChangeType);
    caEntry.setFlags(MissingEntryFlag);
    HsWidgetComponentRegistry::instance()->onEntryChanged(caEntry, RemoveChangeType);
    HsWidgetComponentRegistry::instance()->onEntryChanged(caEntry, UpdateChangeType);
    // invalid.
    HsWidgetComponentRegistry::instance()->onEntryChanged(caEntry, (ChangeType) 100);
    
    // there isnt really a good way to verify results, just calling these functions...
    
}
