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
* Description:  Implementation of the state cenrep handler.
*
*/

#include "ftustatecenrephandler.h"

#include <xqsettingsmanager.h>
#include <QDateTime>


/**
* Ftu Wizard plugin configuration
*/
const qint32 KCrUidFtuWizardProvider = {0x20026F9B};

/**
* Describes number of ftu wizard plug-ins listed in the configuration
* Type: int
*/
const quint32 KFtuNumberOfWizardPlugins = {0x00000001};

/**
* Wizard is considered complete if its cenrep has 1. If value is 0, it is incomplete
* Type: int
*/ 
const int KWizardComplete = 1;

FtuStateCenrepHandler::FtuStateCenrepHandler(QObject *aParent)
                    :QObject(aParent)
                    , mTotalNumberOfPlugins(0)
{
	mSettingsManager = new XQSettingsManager();

	// Get total number of plugins registered from Wizard provider
	XQSettingsKey numberOfPluginsKey(XQSettingsKey::TargetCentralRepository,
                                     KCrUidFtuWizardProvider,
                                     KFtuNumberOfWizardPlugins);
    bool ok = false;
    mTotalNumberOfPlugins = mSettingsManager->readItemValue(
                                        numberOfPluginsKey).toInt(&ok);
	if (!ok)
        mTotalNumberOfPlugins = 0;
}

/**
 * Destructor.
 * @since S60 ?S60_version.
 */
FtuStateCenrepHandler::~FtuStateCenrepHandler()
{
	delete mSettingsManager;
}
	

/**
 * Gets info of plugin at an index 1 if complete, 0 if incomplete.
 * @since S60 ?S60_version.
 */
int FtuStateCenrepHandler::getPluginInfo(int index)
{
	bool ok = false;
	XQSettingsKey reader(XQSettingsKey::TargetCentralRepository, KCrUidFtuStateProvider, index + KBaseIndex);
	int pluginInfo = mSettingsManager->readItemValue(reader).toInt(&ok);  
	if(!ok){
		return -1;
	}
	else{
		return pluginInfo;
	}
}
	
/**
 * Updates plugin info in cenrep.
 * @since S60 ?S60_version.
 */
 int FtuStateCenrepHandler::updatePluginInfo(int indexPluginCompleted)
 {
	 bool ok = false;
    //FTU HS widget is listening to wizard completion
    XQSettingsKey  wizardInfoWriter(XQSettingsKey::TargetCentralRepository, KCrUidFtuStateProvider, indexPluginCompleted + KBaseIndex);
    ok = mSettingsManager->writeItemValue(wizardInfoWriter, KWizardComplete);
    if(!ok)
		return -1;
 	return 0;
 }

int FtuStateCenrepHandler::registeredPluginCount()
{
    return mTotalNumberOfPlugins;
}

