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

#include <HbInstance>
#include <HbMainWindow>
#include <HbView>
#include <HbMenu>
#include <HbAction>

#include "hsgui.h"
#include "hsscene.h"
#include "hsconfiguration.h"

namespace {
      const char gApplicationLibraryIconName[] = "qtg_mono_applications_all";
}

/*!
    \class HsGui
    \ingroup group_hsdomainmodel
    \brief Represents a view in the framework.
    HsGui includes common UI components for Home screen.
*/

HsGui *HsGui::mInstance(0);

struct HsGuiImpl
{
    HbView *mIdleView;
    HbMainWindow *mWindow;
    HbAction *mNavigationAction;
};



HsGui::~HsGui()
{
    if (mImpl->mNavigationAction) {
        delete mImpl->mNavigationAction;
    }

    delete mImpl;
}

HsGui *HsGui::setInstance(HsGui *instance)
{
    HsGui *old = mInstance;
    if (mInstance != instance) {
        mInstance = instance;
    }
    return old;
}

HsGui *HsGui::instance()
{
    if (!mInstance) {
        mInstance = new HsGui(QCoreApplication::instance());
    }
    return mInstance;
}

HsGui *HsGui::takeInstance()
{
    HsGui *instance = mInstance;
    mInstance = 0;
    return instance;
}

void HsGui::setupIdleUi()
{
    
}

void HsGui::cleanupIdleUi()
{
    if (mImpl->mIdleView) {
        mImpl->mIdleView->setNavigationAction(0);
        delete mImpl->mNavigationAction;
        mImpl->mNavigationAction = 0;
        
        mImpl->mWindow->removeView(mImpl->mIdleView);
        delete mImpl->mIdleView;
        mImpl->mIdleView = 0;

        // This gets deleted when mImpl->mIdleView is deleted
        // so just nullify.
    }
    
}

void HsGui::setOrientation(Qt::Orientation orientation)
{
    mImpl->mWindow->setOrientation(orientation);
}

Qt::Orientation HsGui::orientation()
{
    return mImpl->mWindow->orientation();
}

HbView *HsGui::idleView() const
{
    return mImpl->mIdleView;
}

QRectF HsGui::layoutRect() const
{
    return mImpl->mWindow->layoutRect();
}

void HsGui::show()
{
    mImpl->mWindow->raise();
    mImpl->mWindow->show();
}

HsGui::HsGui(QObject *parent):
    QObject(parent),mImpl(new HsGuiImpl)
{
    mImpl->mIdleView = 0;
    mImpl->mNavigationAction = 0;

    mImpl->mWindow = hbInstance->allMainWindows().first();
    connect(mImpl->mWindow, SIGNAL(orientationChanged(Qt::Orientation)),
        SIGNAL(orientationChanged(Qt::Orientation)));
}
