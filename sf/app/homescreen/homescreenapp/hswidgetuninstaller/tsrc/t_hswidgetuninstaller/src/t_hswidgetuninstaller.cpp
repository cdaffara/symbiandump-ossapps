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
* Description:  Main test class for hswidgetuninstaller.
*
*/


#include "t_hswidgetuninstaller.h"
#include "hswidgetinstallersender.h"
#include "hswidgetcomponentparser.h"




void t_hswidgetuninstaller::initTestCase()
{
#ifndef NFT
    QDir::setCurrent("C:/hsresources/testresources");
#else      
    QDir::setCurrent("Z:/hsresources/testresources");
#endif   
}

void t_hswidgetuninstaller::cleanupTestCase()
{
}

void t_hswidgetuninstaller::testWidgetInstallerSender()
{
    HsWidgetInstallerSender sender;
    HsWidgetComponentDescriptor widgetDescriptor;
    QString functionName("widgetUninstalled(QVariantHash)");
    sender.widgetChanged(functionName, widgetDescriptor);
    // nothing to verify
}

void t_hswidgetuninstaller::testWidgetComponentParser_data()
{    
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QString>("expecteduri");
    QTest::addColumn<bool>("expectederror");
    QTest::addColumn<QString>("expectedtranslationfile");
    QTest::newRow("notfound") << "notfound.manifest" << "" << true << "";
    QTest::newRow("empty.manifest") << "empty.manifest" << "" << true << "";
    QTest::newRow("ok_new") << "ok_new.manifest" << "helloworldwidgetplugin" << false << "helloworldtranslation";
    QTest::newRow("ok_old") << "ok_old.manifest" << "helloworldwidgetplugin" << false << "";
    QTest::newRow("fail_new.manifest") << "fail_new.manifest" << "" << true << "";
    QTest::newRow("version10.manifest") << "version10.manifest" << "helloworldwidgetplugin" << false << "";
    QTest::newRow("version12.manifest") << "version12.manifest" << "" << true << "";
}

void t_hswidgetuninstaller::testWidgetComponentParser()
{
    QFETCH(QString, filename);
    QFETCH(QString, expecteduri);
    QFETCH(bool, expectederror);
    QFETCH(QString, expectedtranslationfile);
    HsWidgetComponentParser componentParser(filename);
    HsWidgetComponentDescriptor widgetDescriptor = componentParser.widgetComponentDescriptor();
    QCOMPARE(widgetDescriptor.uri(), expecteduri);
    QCOMPARE(componentParser.error(), expectederror);
    QCOMPARE(widgetDescriptor.translationFilename(), expectedtranslationfile);
}


QTEST_MAIN(t_hswidgetuninstaller)
