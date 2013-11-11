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

#ifndef HSHOMESCREENCLIENTSERVICEPROVIDER_H
#define HSHOMESCREENCLIENTSERVICEPROVIDER_H

#include <QVariant>

#include <xqserviceprovider.h>
#include "hswidgetcomponentdescriptor.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(t_hsapplication)

class HsSpinnerDialog;
class HsWallpaper;

class HsHomeScreenClientServiceProvider : public XQServiceProvider
{
    Q_OBJECT

public:
    HsHomeScreenClientServiceProvider(QObject *parent = 0);
    ~HsHomeScreenClientServiceProvider();
private:
    void startAnimation();
    void stopAnimation();

signals:
    void requestFinished();
    
public slots:
    bool addWidget(const QString &uri,const QVariantHash &preferences);
    void setWallpaper(const QString &fileName);
    void setWallpaper(const QString &portraitFileName, const QString &landscapeFileName);
    bool widgetUninstalled(const QVariantHash &widgetDescriptor);
    void onImageSet();
    void onImageSetFailed();    
    
private:
    HsWidgetComponentDescriptor widgetComponentDescriptor(const QVariantHash& widgetDescriptor);
    HsSpinnerDialog *mWaitDialog;
    bool mShowAnimation;
    int mAsyncRequestIndex;
    QVariant mReturnValue;
    HsWallpaper *mWallpaper;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsapplication)
};

#endif // HSHOMESCREENCLIENTSERVICEPROVIDER_H
