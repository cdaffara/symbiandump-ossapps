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
* Description:
*
*/

#include "hshomescreenclientserviceprovider.h"
#include "hswidgetcomponentdescriptor.h"
#include "hscontentservice.h"
#include "hsscene.h"
#include "hspage.h"
#include "hswallpaper.h"
#include "hswidgetcomponentregistry.h"
#include "hsconfiguration.h"
#include "hsspinnerdialog.h"

namespace
{
    const char gInterfaceName[] = "com.nokia.symbian.IHomeScreenClient";
}


/*!
    \class HsHomeScreenClientServiceProvider
    \ingroup group_hsapplication
    \brief TODO
*/

/*!
    Constructor.
*/
HsHomeScreenClientServiceProvider::HsHomeScreenClientServiceProvider(QObject *parent)
  : XQServiceProvider(gInterfaceName, parent),
    mWaitDialog(0),
    mShowAnimation(false),
    mAsyncRequestIndex(0),
    mReturnValue(false),
    mWallpaper(0)
{
    publishAll();
}

/*!
    Destructor.
*/
HsHomeScreenClientServiceProvider::~HsHomeScreenClientServiceProvider()
{
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Adds new widget, with the give \a uri and \a preferences,
    to the active home screen page.
*/
bool HsHomeScreenClientServiceProvider::addWidget(const QString &uri, const QVariantHash &preferences)
{
    return HsContentService::instance()->addWidget(uri, preferences);
}

/*!
    Changes \a fileName image to the active home screen page's wallpaper.
*/
void HsHomeScreenClientServiceProvider::setWallpaper(const QString &fileName)
{
#ifndef HOMESCREEN_TEST //We can't use QtHighway at unit tests due to missing service client connection
    mAsyncRequestIndex = setCurrentRequestAsync();
#endif
    if ( !mWaitDialog ) {
        mWaitDialog = new HsSpinnerDialog();
        }
    mWaitDialog->start();
    mShowAnimation = true;
    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::PageWallpapers) {
        mWallpaper = HsScene::instance()->activePage()->wallpaper();
    } else {
        mWallpaper = HsScene::instance()->wallpaper();
    }
    connect(mWallpaper, SIGNAL(imageSet()), SLOT(onImageSet()));
    connect(mWallpaper, SIGNAL(imageSetFailed()),
            SLOT(onImageSetFailed()));
    mWallpaper->setImage(fileName);
}

/*!
    Changes the active page's wallpaper based on the \a portraitFileName and \a landscapeFileName.
*/
void HsHomeScreenClientServiceProvider::setWallpaper(const QString &portraitFileName, const QString &landscapeFileName)
{
#ifndef HOMESCREEN_TEST //We can't use QtHighway at unit tests due to missing service client connection
    mAsyncRequestIndex = setCurrentRequestAsync();
#endif
    if (!mWaitDialog) {
        mWaitDialog = new HsSpinnerDialog();
    }
    mWaitDialog->start();
    mShowAnimation = true;
    if (HSCONFIGURATION_GET(sceneType) == HsConfiguration::PageWallpapers) {
        mWallpaper = HsScene::instance()->activePage()->wallpaper();
    } else {
        mWallpaper = HsScene::instance()->wallpaper();
    }
    connect(mWallpaper, SIGNAL(imageSet()), 
        SLOT(onImageSet()));
    connect(mWallpaper, SIGNAL(imageSetFailed()),
        SLOT(onImageSetFailed()));
    
    mWallpaper->setImages(portraitFileName, landscapeFileName);
}

/*!
    \internal
    Called when wallpaper image has been set successfully
*/
void HsHomeScreenClientServiceProvider::onImageSet()
{
    mWallpaper->disconnect(this);
    stopAnimation();
#ifndef HOMESCREEN_TEST //We can't use QtHighway at unit tests due to missing service client connection
    completeRequest(mAsyncRequestIndex, QVariant(true));
#endif
}

/*!
    \internal
    Called when error has happened in wallpaper image setting
*/
void HsHomeScreenClientServiceProvider::onImageSetFailed()
{
    mWallpaper->disconnect(this);
    stopAnimation();
#ifndef HOMESCREEN_TEST //We can't use QtHighway at unit tests due to missing service client connection
    completeRequest(mAsyncRequestIndex, QVariant(false));
#endif
}

/*!
    Stops progress animation
*/
void HsHomeScreenClientServiceProvider::stopAnimation()
{
    if (mShowAnimation) {
        mWaitDialog->stop();
        mWaitDialog=0;
    }
}

/*!

*/
bool HsHomeScreenClientServiceProvider::widgetUninstalled(const QVariantHash &widgetDescriptor)
{
    HsWidgetComponentDescriptor widgetComponent = widgetComponentDescriptor(widgetDescriptor);
    HsWidgetComponentRegistry* widgetRegistry = HsWidgetComponentRegistry::instance();
    widgetRegistry->uninstallComponent(widgetComponent);
    return true;
}


#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!

*/
HsWidgetComponentDescriptor HsHomeScreenClientServiceProvider::widgetComponentDescriptor(const QVariantHash& widgetDescriptor)
{
    HsWidgetComponentDescriptor widget; 
    widget.setInstallationPath(widgetDescriptor["installationPath"].toString());
    widget.setUri(widgetDescriptor["uri"].toString());
    widget.setTitle(widgetDescriptor["title"].toString());
    widget.setDescription(widgetDescriptor["description"].toString());
    widget.setIconUri(widgetDescriptor["iconUri"].toString());
    widget.setHidden(widgetDescriptor["hidden"].toString());
    widget.setServiceXml(widgetDescriptor["serviceXml"].toString());
    widget.setVersion(widgetDescriptor["version"].toString());
    return widget;
}
