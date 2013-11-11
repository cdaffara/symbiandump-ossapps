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

#ifndef HSGUI_H
#define HSGUI_H


#include <QObject>
#include <QPointer>
#include <Hb> 
#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)
HOMESCREEN_TEST_CLASS(HomeScreenStatePluginTest)

class HbView;
class HsIdleWidget;
class HsPropertyAnimationWrapper;

struct HsGuiImpl;

class HSDOMAINMODEL_EXPORT HsGui: public QObject
{
    Q_OBJECT
public:
    ~HsGui();

    static HsGui *setInstance(HsGui *);
    static HsGui *instance();
    static HsGui *takeInstance();

    void setupIdleUi();
    void cleanupIdleUi();

    void setOrientation(Qt::Orientation);
    Qt::Orientation orientation();
    
    HbView *idleView() const;
    HsIdleWidget *idleWidget() const;
   
    QRectF layoutRect() const;
    
    void show();
    
    HsPropertyAnimationWrapper *pageChangeAnimation();
    HsPropertyAnimationWrapper *pageCrawlingAnimation();
signals:
    void orientationChanged(Qt::Orientation);
    void navigateToApplibrary();
 
private:
    HsGui(QObject *parent=0);

private:
    static HsGui *mInstance;
    HsGuiImpl *mImpl;
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
    HOMESCREEN_TEST_FRIEND_CLASS(HomeScreenStatePluginTest)
};



#endif // HSGUI_H
