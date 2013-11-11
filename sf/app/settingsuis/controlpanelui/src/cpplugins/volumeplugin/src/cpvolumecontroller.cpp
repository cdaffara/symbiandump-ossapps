
/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#include "cpvolumecontroller.h"
#include <cplogger.h>
#include <cpitemdatahelper.h>
#include <hbdataformmodelitem.h>
#include "cpvolumegroupitemdata.h"
#ifdef Q_OS_SYMBIAN
	#include <cpprofilemodel.h>
#endif
//#include <hbslider.h>
//#include <hbpushbutton.h>

#include <xqsettingsmanager.h>
#include <ProfileEngineInternalCRKeys.h>
#include <ProfileInternal.hrh>

CpVolumeController::CpVolumeController(CpProfileModel *profileModel,
                                       const QList<HbDataFormModelItem *> &itemList,
                                       CpItemDataHelper &itemDataHelper) : 
                                       mProfileModel(profileModel),
                                       mItemList(itemList),
                                       mSettingManager(0)
															 
{
	//itemDataHelper.addConnection(mMasterVolumeItem,SIGNAL(beepActivated()),this,SLOT(onBeepActivated()));
	//itemDataHelper.addConnection(mMasterVolumeItem,SIGNAL(silentActivated()),this,SLOT(onSilentActivated()));
	//itemDataHelper.addConnection(mMasterVolumeItem,SIGNAL(normalValueChanged(int)),this,SLOT(onNormalValueChanged(int)));
	//updateMasterVolumeValue();
    
    
    itemDataHelper.addConnection(
                mItemList.at(CpVolumeGroupItemData::EVolumeSilenceItem), SIGNAL(toggled(bool)),
                this, SLOT(silenceModeChange(bool))
            );
    
    
    itemDataHelper.addConnection(
                mItemList.at(CpVolumeGroupItemData::EVolumeMasterVolumeItem), SIGNAL(valueChanged(int)),
                this, SLOT(masterVolumeChange(int))
            );
    itemDataHelper.addConnection(
                mItemList.at(CpVolumeGroupItemData::EVolumeMasterVibraItem), SIGNAL(stateChanged(int)),
                this, SLOT(masterVibraChange(int))
            );
    updateUi();
    
    mSettingManager = new XQSettingsManager();
    
    XQCentralRepositorySettingsKey silenceKey(KCRUidProfileEngine.iUid,KProEngSilenceMode);
    mSettingManager->startMonitoring(silenceKey,XQSettingsManager::TypeInt);
   
    XQCentralRepositorySettingsKey masterVolumeKey
                                    (KCRUidProfileEngine.iUid,KProEngMasterVolume);
    mSettingManager->startMonitoring(masterVolumeKey,XQSettingsManager::TypeInt);
    
    XQCentralRepositorySettingsKey masterVibraKey
                                    (KCRUidProfileEngine.iUid,KProEngMasterVibra);
    mSettingManager->startMonitoring(masterVibraKey,XQSettingsManager::TypeInt);
    
    connect(mSettingManager, SIGNAL(valueChanged (XQSettingsKey, QVariant)),
            this, SLOT(settingValueChanged(XQSettingsKey, QVariant)));
}

CpVolumeController::~CpVolumeController()
{
    delete mSettingManager;
}

void CpVolumeController::silenceModeChange(bool isSilence)
{
#ifdef Q_OS_SYMBIAN
	mProfileModel->setSilenceMode(isSilence);
#endif
}


void CpVolumeController::masterVolumeChange(int value)
{
#ifdef Q_OS_SYMBIAN	
    mProfileModel->setMasterVolume(value);
	HbDataFormModelItem *masterVolume = mItemList.at(CpVolumeGroupItemData::EVolumeMasterVolumeItem);
	masterVolume->setContentWidgetData("value",value);
#endif
}

void CpVolumeController::masterVibraChange(int state)
{
#ifdef Q_OS_SYMBIAN
    mProfileModel->setMasterVibra(state);
#endif
}

void CpVolumeController::updateUi()
{
#ifdef Q_OS_SYMBIAN
	bool isSilenceMode = mProfileModel->silenceMode();
	HbDataFormModelItem *silenceIndicator = mItemList.at(CpVolumeGroupItemData::EVolumeSilenceItem);
	if(silenceIndicator) {
			silenceIndicator->setContentWidgetData("checked",isSilenceMode);		
	}
	HbDataFormModelItem *masterVolume = mItemList.at(CpVolumeGroupItemData::EVolumeMasterVolumeItem);
	if (masterVolume) {
	    CPFW_LOG("::updateMasterVolumeValue(), Start using profile model.");
        setSilderSpeakerIcon(isSilenceMode);
	    masterVolume->setEnabled(!isSilenceMode);
	    //masterVolume->setContentWidgetData("enabled",!isSilenceMode);
	    masterVolume->setContentWidgetData("value",mProfileModel->masterVolume());
	}
	HbDataFormModelItem *masterVibra = mItemList.at(CpVolumeGroupItemData::EVolumeMasterVibraItem);
	if (masterVibra) {
        masterVibra->setContentWidgetData("checkState",(mProfileModel->masterVibra()?2:0));
	} 
	
#endif
}

void CpVolumeController::settingValueChanged(const XQSettingsKey &key, const QVariant &value)
{
    if (key.uid() == KCRUidProfileEngine.iUid && key.key() == KProEngSilenceMode) {
        HbDataFormModelItem *masterVolume = mItemList.at(CpVolumeGroupItemData::EVolumeMasterVolumeItem);
        if (masterVolume) {
            setSilderSpeakerIcon(value.toBool());
            masterVolume->setEnabled(!value.toBool());
        }
        HbDataFormModelItem *silenceMode = mItemList.at(CpVolumeGroupItemData::EVolumeSilenceItem);
        if (silenceMode) {
            silenceMode->setContentWidgetData("checked",value.toInt() != 0);
        }
    }
    else if (key.uid() == KCRUidProfileEngine.iUid && key.key() == KProEngMasterVolume) {
        HbDataFormModelItem *masterVolume = mItemList.at(CpVolumeGroupItemData::EVolumeMasterVolumeItem);
        if (masterVolume) {
            masterVolume->setContentWidgetData("value",value.toInt());
        }
    }
    else if (key.uid() == KCRUidProfileEngine.iUid && key.key() == KProEngMasterVibra) {
        HbDataFormModelItem *masterVibra = mItemList.at(CpVolumeGroupItemData::EVolumeMasterVibraItem);
        if (masterVibra) {
            masterVibra->setContentWidgetData("checkState",(value.toInt() ? 2 : 0));
        }
    }
}
/**
 * set speaker icon for volume silder 
 */
void CpVolumeController::setSilderSpeakerIcon(bool isSpeakerOff)
{
    HbDataFormModelItem *masterVolume = mItemList.at(CpVolumeGroupItemData::EVolumeMasterVolumeItem);
    if (masterVolume != 0) {
        QMap<QString, QVariant> iconMaps;
        iconMaps = masterVolume->contentWidgetData("elementIcons").toMap();
        if (isSpeakerOff) {
            iconMaps.insert("IconElement", QVariant("qtg_mono_speaker_off"));
        }
        else {
            iconMaps.insert("IconElement", QVariant("qtg_mono_speaker"));
        }
        masterVolume->setContentWidgetData("elementIcons", QVariant(iconMaps));
    }
}

//End of File
