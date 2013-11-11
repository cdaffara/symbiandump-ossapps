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
#include "hswidgetcomponent.h"

#include "caentry.h"
#include "hsapp_defs.h"
/*!

*/
void TestHsDomainModel::testWidgetComponent()
{
	QScopedPointer<HsWidgetComponent> compo(new HsWidgetComponent("mockwidgetplugin"));
    QVERIFY(!compo.isNull());
    QString path = compo->rootPath();
    QVERIFY(!compo->isAvailable()); // is not in ca store
    QVERIFY(compo->uri() == "mockwidgetplugin");
    compo->uninstallTranslator();
}


void TestHsDomainModel::testWidgetComponentEmits()
{
    
    QScopedPointer<HsWidgetComponent> compo(new HsWidgetComponent("mockwidgetplugin"));
    
    QSignalSpy aboutToUninstallSpy(compo.data(),SIGNAL(aboutToUninstall()));
    QSignalSpy uninstalledSpy(compo.data(),SIGNAL(uninstalled()));
    QSignalSpy unavailableSpy(compo.data(),SIGNAL(unavailable()));
    QSignalSpy availableSpy(compo.data(),SIGNAL(available()));
    QSignalSpy updatedSpy(compo.data(),SIGNAL(updated()));
    
    compo->emitAboutToUninstall();
    QVERIFY(aboutToUninstallSpy.count() == 1);
    // should not emit again
    compo->emitAboutToUninstall();
    QVERIFY(aboutToUninstallSpy.count() == 1);
    
    compo->emitUninstalled();
    QVERIFY(uninstalledSpy.count() == 1);
    // should not emit again
    compo->emitUninstalled();
    QVERIFY(uninstalledSpy.count() == 1);
    
    compo->emitUnavailable();
    QVERIFY(unavailableSpy.count() == 1);
    // should not emit again
    compo->emitUnavailable();
    QVERIFY(unavailableSpy.count() == 1);
    
    compo->emitAvailable();
    QVERIFY(availableSpy.count() == 1);
    // should not emit again
    compo->emitAvailable();
    QVERIFY(availableSpy.count() == 1);
    
    compo->emitAboutToUninstall();
    compo->emitUpdated();
    QVERIFY(updatedSpy.count() == 1);
    // should not emit again
    compo->emitUpdated();
    QVERIFY(updatedSpy.count() == 1);
}
