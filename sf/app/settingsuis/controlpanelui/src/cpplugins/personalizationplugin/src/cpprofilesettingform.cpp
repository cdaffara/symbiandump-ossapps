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

#include "cpprofilesettingform.h"
#include "cppersonalizationentryitemdata.h"
#include "cpprofilenameeditdialog.h"
#include <QStringList>
#include <hbdataformmodel.h>
#include <hbabstractviewitem.h>
#include <cpitemdatahelper.h>
#include <cpprofilemodel.h>
#include <cpprofilemonitor.h>
#include <hbslider.h>
#include <hbaction.h>

#include <QFileInfo>
#include <QFileIconProvider>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QMap>

#include <xqsettingsmanager.h>
#include <ProfileEngineInternalCRKeys.h>

CpProfileSettingForm::CpProfileSettingForm()
    : mModel(0), mItemDataHelper(new CpItemDataHelper( this )),
    mProfileModel(new CpProfileModel()), mFileIconProvider(new QFileIconProvider),
    mGeneralPage(0),mMeetingPage(0), mSettingManager(0),
    mCurrentProfileId(EProfileWrapperGeneralId), mEditProfileNameDialog(0), mOkButton(0), mCancelButton(0)
{
    this->setHeading(hbTrId("txt_cp_button_advanced_settings"));
    this->setDescription(hbTrId("txt_cp_info_select_tones_that_play_when_you_select"));    
    initModel();
    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(onDataFormActivated(QModelIndex)));
    mProfileMonitor = new CpProfileMonitor();    
    connect(mProfileMonitor, SIGNAL(profileNameModified()), this, SLOT(onProfileNameChanged()));       
}

CpProfileSettingForm::~CpProfileSettingForm()
{
    delete mModel;
    delete mProfileModel;
    delete mFileIconProvider;
    delete mSettingManager;
    delete mProfileMonitor;
    if (mEditProfileNameDialog) {
       delete mEditProfileNameDialog;
    }
    if (mOkButton) {
       delete mOkButton;
    }
    if (mCancelButton) {
       delete mCancelButton;
    }
}

void CpProfileSettingForm::onDataFormActivated(const QModelIndex &index)
{
    HbDataFormModelItem *item = mModel->itemFromIndex(index);
    if (item == mGeneralPage ) {
        mCurrentProfileId = EProfileWrapperGeneralId;        
    } else if (item == mMeetingPage) {
        mCurrentProfileId = EProfileWrapperMeetingId;        
    }
}

void CpProfileSettingForm::onProfileNameChanged()
{    
    mGeneralPage->setLabel(mProfileModel->profileName(EProfileWrapperGeneralId));
    mMeetingPage->setLabel(mProfileModel->profileName(EProfileWrapperMeetingId));
}

void CpProfileSettingForm::initModel()
{
    mModel = new HbDataFormModel();

    QString generalString( mProfileModel->profileName( EProfileWrapperGeneralId ) );
    if( generalString.isEmpty() ) {
        qDebug( "warning: general profile name is empty");
        generalString = hbTrId("txt_cp_list_general"); 
    }
    QString meetingString( mProfileModel->profileName( EProfileWrapperMeetingId ) );
    if( meetingString.isEmpty() ) {
        qDebug( "warning: meeting profile name is empty");
        meetingString = hbTrId("txt_cp_list_meeting"); 
    }
    
    mSettingManager = new XQSettingsManager();   
    
    XQCentralRepositorySettingsKey key(KCRUidProfileEngine.iUid,KProEngSilenceMode);
    
    QVariant silenceMode = mSettingManager->readItemValue(key,XQSettingsManager::TypeInt);

    mSettingManager->startMonitoring(key,XQSettingsManager::TypeInt);
    connect(mSettingManager, SIGNAL(valueChanged (XQSettingsKey, QVariant)),
            this, SLOT(settingValueChanged(XQSettingsKey, QVariant)));
    
    mGeneralPage = mModel->appendDataFormPage( generalString );
    initProfileItems(EProfileWrapperGeneralId,mGeneralPage);
    
    //initGeneralTonesGroup();
    
    //initGeneralVibraGroup();
    
    mMeetingPage = mModel->appendDataFormPage( meetingString );
    initProfileItems(EProfileWrapperMeetingId,mMeetingPage);
    //initMeetingTonesGroup();
    //initMeetingVibraGroup();
    setModel(mModel);
    settingValueChanged(key,silenceMode);
}

void CpProfileSettingForm::initProfileItems(int profileId,HbDataFormModelItem *parent)
{
    CpProfileSettings profileSettings;
    mProfileModel->profileSettings( profileId, profileSettings );
    
    QHash<int,HbDataFormModelItem*> modelItems;
    
    //ring tone item
    QFileInfo ringToneFileInfo( profileSettings.mRingTone );
    HbDataFormModelItem *modelItem = new CpPersonalizationEntryItemData( *mItemDataHelper,
            hbTrId("txt_cp_dblist_ringtone"), ringToneFileInfo.fileName(), "qtg_large_ring_tone",
            CpPersonalizationEntryItemData::TONE_Ring, profileId );
    mModel->appendDataFormItem(modelItem, parent);
    modelItems.insert(ProfileItemRingTone,modelItem);
    
    //message tone item
    QFileInfo messageToneFileInfo( profileSettings.mMessageTone );
    modelItem = new CpPersonalizationEntryItemData( *mItemDataHelper,
            hbTrId("txt_cp_dblist_message_tone"), messageToneFileInfo.fileName(), "qtg_large_message",
            CpPersonalizationEntryItemData::TONE_Message,
            profileId );
    mModel->appendDataFormItem(modelItem , parent);
    modelItems.insert(ProfileItemMessageTone,modelItem);
    
    //email tone item
    QFileInfo emailToneFileInfo( profileSettings.mEmailTone );
    modelItem = new CpPersonalizationEntryItemData( *mItemDataHelper,
            hbTrId("txt_cp_dblist_email_tone"),  emailToneFileInfo.fileName(), "qtg_large_email",
            CpPersonalizationEntryItemData::TONE_Email,
            profileId );
    mModel->appendDataFormItem(modelItem , parent);
    modelItems.insert(ProfileItemEmailTone,modelItem);
    
    //reminder tone item
    QFileInfo reminderToneFileInfo( profileSettings.mReminderTone );
    modelItem = new CpPersonalizationEntryItemData( *mItemDataHelper,
            hbTrId("txt_cp_dblist_reminder_tone"), reminderToneFileInfo.fileName(), "qtg_large_calendar",
            CpPersonalizationEntryItemData::TONE_Reminder,
            profileId );
    mModel->appendDataFormItem(modelItem , parent);
    modelItems.insert(ProfileItemReminderTone,modelItem);
    
    //notification tones item
    modelItem= mModel->appendDataFormItem(HbDataFormModelItem::CheckBoxItem,QString(),parent);
    modelItem->setContentWidgetData("text", hbTrId("txt_cp_list_notification_tones"));
    modelItem->setContentWidgetData("checkState", profileSettings.mNotificationTone ? 2 : 0);
    modelItem->setContentWidgetData("objectName", "notificationTonesCheckBox" + QString::number(profileId));
    if (profileId == EProfileWrapperGeneralId) {
        addConnection( modelItem, SIGNAL( stateChanged( int )), this, SLOT( on_general_notificationTones_stateChanged( int )));
    }
    else if (profileId == EProfileWrapperMeetingId) {
        addConnection( modelItem, SIGNAL( stateChanged( int )), this, SLOT( on_meeting_notificationTones_stateChanged( int )));
    }
    modelItems.insert(ProfileItemNotificationTones,modelItem);
    
    
    //Key and Touch Screen Tones item
    modelItem = 
            mModel->appendDataFormItem(HbDataFormModelItem::SliderItem ,QString(hbTrId("txt_cp_setlabel_key_and_touchscreen_tones")),parent);
    
    QList<QVariant> sliderElements;
    sliderElements << QVariant(HbSlider::IncreaseElement) << QVariant(HbSlider::TrackElement)
            << QVariant(HbSlider::DecreaseElement) << QVariant(HbSlider::IconElement)
            << QVariant(HbSlider::TextElement);
    modelItem->setContentWidgetData("sliderElements",sliderElements);
    modelItem->setContentWidgetData("objectName", "keyTonesSlider" + QString::number(profileId));
    
    
    //TODO: profileModel need provide Max and Min value( 0-5 ), current max value from profileModel is 3
    modelItem->setContentWidgetData( QString( "minimum" ), 0 );
    modelItem->setContentWidgetData( QString( "maximum" ), 5 );
    modelItem->setContentWidgetData( QString("value"), profileSettings.mKeyTouchScreenTone );
    QMap< QString, QVariant > elements;
    elements.insert(QString("IncreaseElement") , QVariant("qtg_mono_vol_up"));
    elements.insert(QString("DecreaseElement"), QVariant("qtg_mono_vol_down") );
    if (profileSettings.mKeyTouchScreenTone != 0) {
        elements.insert(QString("IconElement"), QVariant("qtg_mono_speaker") );
    }
    else {
        elements.insert(QString("IconElement"), QVariant("qtg_mono_speaker_off") );
    }
    
    modelItem->setContentWidgetData( QString( "elementIcons" ), elements );
    
    if (profileId == EProfileWrapperGeneralId) {
        addConnection( modelItem, SIGNAL( valueChanged( int )), this, SLOT( on_general_keysAndScreenToneSlider_ValueChanged( int )));
    }
    else if (profileId == EProfileWrapperMeetingId) {
        addConnection( modelItem, SIGNAL( valueChanged( int )), this, SLOT( on_meeting_keysAndScreenToneSlider_ValueChanged( int )));
    }
  
    modelItems.insert(ProfileItemKeyandTouchScreenTones,modelItem);
    
    
    //Touch Screen Vibra item
    modelItem = mModel->appendDataFormItem( HbDataFormModelItem::SliderItem, QString( hbTrId( "txt_cp_setlabel_touch_screen_vibra" ) ), parent );
     //TODO: profileModel need provide Max and Min value( 0-5 ), current max value from profileModel is 3
    sliderElements.clear();
    sliderElements << QVariant(HbSlider::IncreaseElement) << QVariant(HbSlider::TrackElement)
                << QVariant(HbSlider::DecreaseElement);
    modelItem->setContentWidgetData("sliderElements",sliderElements);
    modelItem->setContentWidgetData("objectName", "vibrationSlider" + QString::number(profileId));
    modelItem->setContentWidgetData( QString( "minimum" ), 0 );
    modelItem->setContentWidgetData( QString( "maximum" ), 5 );
    modelItem->setContentWidgetData( QString("value"), profileSettings.mKeyTouchScreenVibra );
    QMap< QString, QVariant > iconElements;
    iconElements.insert(QString("IncreaseElement") , QVariant("qtg_mono_vol_up"));
    iconElements.insert(QString("DecreaseElement"), QVariant("qtg_mono_vol_down") );
    modelItem->setContentWidgetData( QString( "elementIcons" ), iconElements );
    
    if (profileId == EProfileWrapperGeneralId) {
        addConnection( modelItem, SIGNAL( valueChanged( int )), this, SLOT( on_general_screenVibra_ValueChanged( int )));
    }
    else if (profileId == EProfileWrapperMeetingId) {
        addConnection( modelItem, SIGNAL( valueChanged( int )), this, SLOT( on_meeting_screenVibra_ValueChanged( int )));
    }
    
    modelItems.insert(ProfileItemTouchScreenVibra,modelItem);
    
    mProfileModelItems.insert(profileId,modelItems);
    
}



////////////////////////////////////////////////////
//general tones
void CpProfileSettingForm::on_general_notificationTones_stateChanged(int state)
{
    mProfileModel->setNotificationTone( EProfileWrapperGeneralId, checkBoxStateToBool( state ) );
}

void CpProfileSettingForm::on_general_keysAndScreenToneSlider_ValueChanged( int value )
{
    mProfileModel->setKeyTouchScreenTone( EProfileWrapperGeneralId, value );
    HbDataFormModelItem *modelItem = profileItem(EProfileWrapperGeneralId,ProfileItemKeyandTouchScreenTones);
    if (modelItem) {
        modelItem->setContentWidgetData( QString("value"), value );
        setMuteIcon(modelItem, (value == 0) );            
    }
}


//general vibra
void CpProfileSettingForm::on_general_screenVibra_ValueChanged( int value )
{
    mProfileModel->setKeyTouchScreenVibra( EProfileWrapperGeneralId, value );
    
    HbDataFormModelItem *modelItem = profileItem(EProfileWrapperGeneralId,ProfileItemTouchScreenVibra);
    if (modelItem) {
        modelItem->setContentWidgetData( QString("value"), value );
    }

}

////////////////////////////////////////////////////
//meeting Tones
void CpProfileSettingForm::on_meeting_notificationTones_stateChanged(int state)
{
    mProfileModel->setNotificationTone( EProfileWrapperMeetingId, checkBoxStateToBool( state ) );
}

void CpProfileSettingForm::on_meeting_keysAndScreenToneSlider_ValueChanged( int value )
{
    mProfileModel->setKeyTouchScreenTone( EProfileWrapperMeetingId, value );
    
    HbDataFormModelItem *modelItem = profileItem(EProfileWrapperMeetingId,ProfileItemKeyandTouchScreenTones);
    if (modelItem) {
        modelItem->setContentWidgetData( QString("value"), value );
        setMuteIcon(modelItem, (value == 0) );            
    }
}

void CpProfileSettingForm::on_meeting_screenVibra_ValueChanged( int value )
{
    mProfileModel->setKeyTouchScreenVibra( EProfileWrapperMeetingId, value );
    
    HbDataFormModelItem *modelItem = profileItem(EProfileWrapperMeetingId,ProfileItemTouchScreenVibra);
    if (modelItem) {
        modelItem->setContentWidgetData( QString("value"), value );
    }
}

bool CpProfileSettingForm::checkBoxStateToBool( int state )
{
    if( state == Qt::Checked ) {
        return true;
    } else {
        return false;
    }
}


void CpProfileSettingForm::settingValueChanged(const XQSettingsKey &key, const QVariant &value)
{
    if (key.uid() == KCRUidProfileEngine.iUid && key.key() == KProEngSilenceMode && value.isValid()) {
    
		static const int silenceSensitiveModelItemIds[] = 
		{       
			CpProfileSettingForm::ProfileItemRingTone,
			CpProfileSettingForm::ProfileItemMessageTone,
			CpProfileSettingForm::ProfileItemEmailTone,
			CpProfileSettingForm::ProfileItemReminderTone,
			CpProfileSettingForm::ProfileItemNotificationTones,
			CpProfileSettingForm::ProfileItemKeyandTouchScreenTones
		};
    
        QHash< int,QHash<int,HbDataFormModelItem*>  >::const_iterator it (mProfileModelItems.begin());
        for(; it != mProfileModelItems.end(); ++it ) {          
            for (int i = 0; i < sizeof(silenceSensitiveModelItemIds)/sizeof(silenceSensitiveModelItemIds[0]);++i) {           
                QHash<int,HbDataFormModelItem*>::const_iterator found = it.value().find(silenceSensitiveModelItemIds[i]);
                if (found != it.value().end()) {
                    if (found.key() == CpProfileSettingForm::ProfileItemKeyandTouchScreenTones) {
                        int currentValue = found.value()->contentWidgetData("value").toInt();
                        // change the mute icon when the silence mode is changed
                        bool isMute = value.toBool() || (currentValue == 0);
                        setMuteIcon(found.value(), isMute); 
                    } 
                    found.value()->setEnabled(!value.toBool());                             
                }
            }   
        }
    }
}

HbDataFormModelItem *CpProfileSettingForm::profileItem(int profileId,int profileItemId)
{
    return mProfileModelItems.value(profileId).value(profileItemId);
}

/*!
 *  Set the slider icon to mute or unmute
 *  @param isMute: identified the icon of slider, mute or unmute
 *  @param profileId: identified which slider should be changed
 */

void CpProfileSettingForm::setMuteIcon(HbDataFormModelItem *sliderItem, bool isMute)
{
    if (sliderItem == 0) {
        return;
    }
    //VolumeSliderItem will be depreacted, so ignore the assert about it
    if (sliderItem->type() != HbDataFormModelItem::SliderItem) {
        return;
    }
    
    QMap<QString, QVariant> elements = sliderItem->contentWidgetData("elementIcons").toMap();
    
    if (isMute) {        
        elements.insert(QString("IconElement"), QVariant("qtg_mono_speaker_off"));
    }
    else {
        elements.insert(QString("IconElement"), QVariant("qtg_mono_speaker"));
    }
    sliderItem->setContentWidgetData( QString( "elementIcons" ), elements ); 
}

void CpProfileSettingForm::on_editNameAction_triggered()
{    
    if (mEditProfileNameDialog) {       
        delete mEditProfileNameDialog;
        delete mCancelButton;
        delete mOkButton;
    }  
    mProfileName =  mProfileModel->profileName(mCurrentProfileId);
    mCancelButton = new HbAction(hbTrId("txt_common_button_cancel"));
    mOkButton = new HbAction(hbTrId("txt_common_button_ok"));
    mEditProfileNameDialog = new CpProfileNameEditDialog();
    mEditProfileNameDialog->addAction(mOkButton);
    mEditProfileNameDialog->addAction(mCancelButton);
    mEditProfileNameDialog->setLineEditMaxLength(maxProfileNameLength);
    mEditProfileNameDialog->setLineEditText(mProfileName);    
    mEditProfileNameDialog->open(this, SLOT(onEditNameDialogClosed(HbAction*)));      
}

void CpProfileSettingForm::onEditNameDialogClosed(HbAction *action)
{
    QString editText(mEditProfileNameDialog->lineEditText());
    if (action == mOkButton && mProfileName != editText) {  
             
            mProfileModel->setProfileName(mCurrentProfileId, editText);       
    }
}

//End of File
