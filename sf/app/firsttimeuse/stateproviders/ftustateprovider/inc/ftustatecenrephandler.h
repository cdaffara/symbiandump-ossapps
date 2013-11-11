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
* Description:  Declaration of the wizard menu state.
*
*/


#ifndef FTU_STATES_CENREPHANDLER_H
#define FTU_STATES_CENREPHANDLER_H

#include <QObject>
#include "ftutest_global.h"

class XQSettingsManager;

/**
* Ftu State provider
*/
const qint32 KCrUidFtuStateProvider = {0x20026f99};

//index 0 is unused in cenrep, 1 for datetime of last completed plugin
const int KBaseIndex = 2;

/** @ingroup group_ftustateprovider
 * @brief The state where wizard menu is shown
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class FtuStateCenrepHandler : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.     
     * @param aParent Owner.
     */
    FtuStateCenrepHandler(QObject *aParent = 0);

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
	virtual ~FtuStateCenrepHandler();
	

    /**
     * Gets info of plugin at an index 1 if complete, 0 if incomplete.
     * @since S60 ?S60_version.
     */
    int getPluginInfo(int index);
	
	/**
     * Updates plugin info in cenrep.
     * @since S60 ?S60_version.
     */
    int updatePluginInfo(int indexPluginCompleted);
    
    /**
     * Returns Total number of plugins registered.
     * @since S60 ?S60_version.
     */
    int registeredPluginCount();

private:

    XQSettingsManager* mSettingsManager;
    int mTotalNumberOfPlugins;
};

#endif //FTU_STATES_CENREPHANDLER_H
