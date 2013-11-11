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
* Description:
*
*/

#include "hsloadscenestate.h"
#include "hsscene.h"
#include "hsgui.h"

#ifdef Q_OS_SYMBIAN
#include <XQSettingsManager>
#include <startupdomainpskeys.h>
#endif


/*!
    \class HsLoadSceneState
    \ingroup group_hshomescreenstateplugin
    \brief View part of the home screen idle state.

    Loads the home screen scene.
*/

/*!
    Constructs a new load scene state with the given \a parent.
*/
HsLoadSceneState::HsLoadSceneState(QState *parent)
    : QState(parent)
#ifdef Q_OS_SYMBIAN
      ,mStartupKey(KPSUidStartup.iUid, KPSStartupUiPhase),
      mSettingsMgr(0)
#endif
{
    connect(this, SIGNAL(entered()), SLOT(action_loadScene()));
#ifdef Q_OS_SYMBIAN

    mSettingsMgr = new XQSettingsManager(this);

#endif //Q_OS_SYMBIAN
}

/*!
    Destroys this idle state.
*/
HsLoadSceneState::~HsLoadSceneState()
{
}

/*!
    \fn HsLoadSceneState::event_idle()

    Loads scene information and initiates a transition to idle state if ready.
*/

/*!
    Loads the home screen scene.
*/
void HsLoadSceneState::action_loadScene()
{
    HsScene::setInstance(new HsScene);
    HsScene::instance()->load();

#ifdef Q_OS_SYMBIAN

    int startupPhase = mSettingsMgr->readItemValue(mStartupKey).toInt();
    qDebug() << "HsLoadSceneState::action_loadScene boot phase " << startupPhase;

    if (EStartupUiPhaseAllDone != startupPhase) {
        qDebug() << "HsLoadSceneState::HsLoadSceneState, starting to listen key";

        connect(mSettingsMgr,SIGNAL(valueChanged(XQSettingsKey, const QVariant &)), this,
			          SLOT(handleKeyChange(XQSettingsKey, const QVariant &)));

        mSettingsMgr->startMonitoring(mStartupKey);
    } else {
        // We're already at the phase in boot where we can draw ourself to the foreground.
        showUi();
		}

#else

    showUi();

#endif
}
#ifdef Q_OS_SYMBIAN
/*!
    \fn HsLoadSceneState::handleKeyChange()

    Handles the key change. Activates hs ui when appropriate.
*/

void HsLoadSceneState::handleKeyChange(XQSettingsKey key, const QVariant &value)
{
		qDebug() << "HsLoadSceneState::handleKeyChange key " << key.key()
		         << "value " << value.toInt();

		if (key.key() == KPSStartupUiPhase && value.toInt() == EStartupUiPhaseAllDone) {
			showUi();
    		mSettingsMgr->stopMonitoring(mStartupKey);
		}
}
#endif
/*!
    \fn HsLoadSceneState::showUi()

    Makes the main window visible, signals transition to idle state and
    brings the hs app to the foreground.
*/

void HsLoadSceneState::showUi()
{
    qDebug() << "HsLoadSceneState::showUi";
  
    HsGui::instance()->show();
    emit event_history();
}
