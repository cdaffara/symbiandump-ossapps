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

#ifndef CPPROFILESETTINGFORM_H
#define CPPROFILESETTINGFORM_H

#include <hbdataform.h>
#include <QHash>
#include "cpprofilemodel.h"

class HbDataFormModel;
class HbDataFormModelItem;
class CpItemDataHelper;
class CpProfileMonitor;
class QFileIconProvider;
class XQSettingsManager;
class XQSettingsKey;
class QVariant;
class HbAction;
class CpProfileNameEditDialog;

//max profile name length is 64 bytes.
const int maxProfileNameLength = 64;

class CpProfileSettingForm : public HbDataForm
{
    Q_OBJECT
public:
    
    enum ProfileItemId {
        ProfileItemRingTone,
        ProfileItemMessageTone,
        ProfileItemEmailTone,
        ProfileItemReminderTone,
        ProfileItemNotificationTones,
        ProfileItemKeyandTouchScreenTones,
        ProfileItemTouchScreenVibra
    };
    
    CpProfileSettingForm();
    virtual ~CpProfileSettingForm();       
    
private slots:
    
    //general tones
    void on_general_notificationTones_stateChanged(int state);
    void on_general_keysAndScreenToneSlider_ValueChanged( int value );
    //general vibra
    void on_general_screenVibra_ValueChanged( int value );

    //meeting tones
    void on_meeting_notificationTones_stateChanged(int state);
    void on_meeting_keysAndScreenToneSlider_ValueChanged( int value );
    
    //meeting vibar
    void on_meeting_screenVibra_ValueChanged( int value );
    
    void settingValueChanged(const XQSettingsKey &key, const QVariant &value);
    
    void onDataFormActivated(const QModelIndex &index);
    void onProfileNameChanged();    
    void on_editNameAction_triggered();
    void onEditNameDialogClosed(HbAction *action);

private:
    void initModel();
    void initGeneralTonesGroup();
    //void initGeneralVibraGroup();
    void initMeetingTonesGroup();
    //void initMeetingVibraGroup();
    bool checkBoxStateToBool( int state );
    
 //   void initRingToneGroup(HbDataFormModelItem *parent);
 //   void initMessageToneGroup(HbDataFormModelItem *parent);
 //   void initAlertToneGroup(HbDataFormModelItem *parent);
 //   void initKeyAndScreenToneGroup(HbDataFormModelItem *parent);
    
    void initProfileItems(int profileId,HbDataFormModelItem *parent);
    HbDataFormModelItem *profileItem(int profileId,int profileItemId);
    void setMuteIcon(HbDataFormModelItem *silderItem, bool isMute);
private:
    HbDataFormModel *mModel;
    CpItemDataHelper *mItemDataHelper;
    CpProfileModel  *mProfileModel;
    CpProfileMonitor *mProfileMonitor;
    QFileIconProvider *mFileIconProvider;
    
    HbDataFormModelItem *mGeneralPage;
    HbDataFormModelItem *mMeetingPage;    
  //  HbDataFormModelItem *mGeneralKeysAndScreenToneSlider;
 //   HbDataFormModelItem *mGeneralSreenVibra;
 //   HbDataFormModelItem *mMeetingKeysAndScreenToneSlider;
  //  HbDataFormModelItem *mMeetingSreenVibra;
    
    XQSettingsManager *mSettingManager;    
    QHash< int,QHash<int,HbDataFormModelItem*>  > mProfileModelItems;
    ProfileWrapperProfileId mCurrentProfileId;
    CpProfileNameEditDialog *mEditProfileNameDialog;
    HbAction *mOkButton;
    HbAction *mCancelButton;
    QString mProfileName;
    //QStringList mProfileNameList;
};


#endif //CPPROFILESETTINGFORM_H
