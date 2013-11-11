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
#ifndef CPVOLUMECONTROLLER_H
#define CPVOLUMECONTROLLER_H

#include <QObject>

class CpProfileModel;
class HbDataFormModelItem;
class CpItemDataHelper;
class XQSettingsManager;
class XQSettingsKey;
class QVariant;

class CpVolumeController : public QObject
{
	Q_OBJECT
public:
    CpVolumeController(CpProfileModel *profileModel,
		const QList<HbDataFormModelItem *> &itemList,
		CpItemDataHelper &itemDataHelper);
	virtual ~CpVolumeController();

private slots:
    void silenceModeChange(bool isSilence);
    void masterVolumeChange(int value);
    void masterVibraChange(int state);
    
    void settingValueChanged(const XQSettingsKey &key, const QVariant &value);
    
private:
	void updateUi();	
	void setSilderSpeakerIcon(bool isSpeakerOff);
private:
	CpProfileModel *mProfileModel;
	QList<HbDataFormModelItem *> mItemList;
	XQSettingsManager *mSettingManager;
};

#endif
