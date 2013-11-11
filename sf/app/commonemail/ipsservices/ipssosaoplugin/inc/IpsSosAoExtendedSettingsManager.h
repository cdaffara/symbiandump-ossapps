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

#ifndef NMIPSSOSAOEXTENDEDSETTINGSMANAGER_H
#define NMIPSSOSAOEXTENDEDSETTINGSMANAGER_H

#include <qvariant.h>
#include "nmipssettingitems.h"

class NmId;
class XQSettingsManager;
class XQCentralRepositoryUtils;

/**
 * class to handle settings stored in central repository
 */
NONSHARABLE_CLASS (NmIpsSosAoExtendedSettingsManager)
{
public:
    /**
     * constructor
     * @param mailbox who's settings we are handling
     */
    NmIpsSosAoExtendedSettingsManager(const NmId &mailboxId);
    
    /**
     * destructor
     */
    virtual ~NmIpsSosAoExtendedSettingsManager();
    
    /**
     * reads a setting from cen rep
     * @param settingItem id of the setting
     * @param settingValue a return parameter
     * @return was read a success
     */
    bool readSetting(
            IpsServices::SettingItem settingItem, 
            QVariant &settingValue) const;
    
    /**
     * writes a setting into cen rep
     * @param settingItem id of the setting
     * @param settingValue value of the setting to be stored
     * @return was write a success
     */
    bool writeSetting(
            IpsServices::SettingItem settingItem, 
            const QVariant &settingValue);
    
	bool writeSetting(
		int profileMode, 
		IpsServices::SettingItem settingItem, 
		const QVariant &settingValue);
	
    /**
     * deletes settings
     */
    void deleteSettings();
    
    /**
     * creates a new key-value pair
     * @param settingItem id of the setting
     * @param settingValue value of the setting to be stored
     * @return was creation a success
     */
    bool createKeyValuePair(
            const IpsServices::SettingItem settingItem, 
            const QVariant &settingValue);

private:
    bool writeSettingToCenRep(
	qint32 profileOffset, 
	IpsServices::SettingItem settingItem, 
	const QVariant &settingValue);
	
    QVariant readFromCenRep(quint32 key) const;
    bool writeToCenRep(quint32 key, const QVariant &value) const;
    void calculateMailboxOffset();
    void calculateActiveProfileOffset();
    qint32 convertToProfileOffset(int profile);

private: // data
    const NmId &mMailboxId;
    qint32 mMailboxOffset;
    qint32 mActiveProfileOffset;
    XQSettingsManager* mSettingsManager; // Owned.
};

#endif // NMIPSSOSAOEXTENDEDSETTINGSMANAGER_H
