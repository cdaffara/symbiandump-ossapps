/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main test class for HsHomeScreenClientServiceProvider library.
*
*/

#include <QSignalSpy>
#include "t_hsapplication.h"
#include "hshomescreenclientserviceprovider.h"
#include "hswallpaper.h"
#include "hsscene.h"
#include "hspage.h"
#include "hsconfiguration.h"
#include "hswidgetcomponentdescriptor.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testCreateClientServiceProvider()
{
    HsHomeScreenClientServiceProvider *provider = new HsHomeScreenClientServiceProvider();
    QVERIFY(provider);
    delete provider;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testAddWidget()
{
    HsHomeScreenClientServiceProvider *provider = new HsHomeScreenClientServiceProvider();
    QVERIFY(provider);

    QVERIFY(!provider->addWidget(QString(), QVariantHash()));
    QVERIFY(provider->addWidget(QString("NotEmpty"), QVariantHash()));

    delete provider;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testSetWallpaper()
{
    HsConfiguration::setInstance(new HsConfiguration);
    HsScene *scene = new HsScene;
    HsScene::setInstance(scene);
    HsPage *page = new HsPage;
    page->load();
    scene->addPage(page);
    scene->setActivePage(page);
    
    HsHomeScreenClientServiceProvider *provider = new HsHomeScreenClientServiceProvider();
    QVERIFY(provider);
        
    provider->setWallpaper(QString());
    QVERIFY(provider->mWallpaper);

    provider->setWallpaper("default_portrait.png");
    QVERIFY(provider->mWallpaper);

    delete provider;
    delete scene;
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testWidgetComponentDescriptor()
{
    HsHomeScreenClientServiceProvider *provider = new HsHomeScreenClientServiceProvider();
    QVERIFY(provider);

    QVERIFY(provider->widgetComponentDescriptor(QVariantHash()).description().isEmpty());

    delete provider;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplication::testWidgetUninstalled()
{
    HsHomeScreenClientServiceProvider *provider = new HsHomeScreenClientServiceProvider();
    QVERIFY(provider);
        
    QVERIFY(provider->widgetUninstalled(QVariantHash()));

    delete provider;
}
