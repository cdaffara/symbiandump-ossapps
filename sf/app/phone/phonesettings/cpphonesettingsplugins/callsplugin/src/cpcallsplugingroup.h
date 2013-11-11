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
#ifndef CPCALLSPLUGINGROUP_H
#define CPCALLSPLUGINGROUP_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <hbmessagebox.h>
#include <cpsettingformitemdata.h>
#include <psetcallwaitingwrapper.h>

class HbDataFormModel;
class HbDataFormModelItem; 
class HbLineEdit;
class PSetWrapper;
class PSetCliWrapper; 
class SsSettingsWrapper; 
class PSetCallWaitingWrapper;
class SettingsWrapper;

class CpCallsPluginGroup: public CpSettingFormItemData
{
    Q_OBJECT

public:

    explicit CpCallsPluginGroup(CpItemDataHelper &helper);

    ~CpCallsPluginGroup();
    
private:     
    
    void createShowCallDurationItem();
    void createOwnVideoInReceivedCall();
    void createSoftRejectItem();
    void createCLIItem();
    void createCallWaitingtem();
    
    void connectShowCallDurationItem();
    void connectOwnVideoInReceivedCall();
    void connectSoftRejectItem();
    void connectCLIItem();
    void connectCallWaitingItem();
    
    void insertMappedListItems(); 
    
public slots:
    
    void showCallDurationStateChanged();
    void ownVideoInReceivedCallStateChanged(int index);
    void softRejectTextChanged();
    void callWaitingCurrentIndexChanged();
    void cliCurrentIndexChanged(int index);
    
    // Call waiting slots
    void handleCallWaitingGetStatus( 
        PSetCallWaitingWrapper::PsCallWaitingStatus status,
        const QList<unsigned char> &basicServiceGroupIds);
    void handleCallWaitingChanged( 
        PSetCallWaitingWrapper::PsCallWaitingCommand setting,
        int aResult );
    void handleCallWaitingRequesting( bool ongoing, bool interrupted ); 
    void handleCallWaitingError( int error );
    
signals:
    
    void showGlobalProgressNote(int &noteId, const QString& text);
    void showGlobalNote(
        int &noteId, const QString& text, HbMessageBox::MessageBoxType);
    void showGlobalErrorNote(int &noteId, int errorCode);
    void cancelNote(int noteId);
    void showNotificationDialog(const QString& text);
    
private:   

    // Setting wrappers 
    PSetWrapper *m_pSetWrapper; 
    SsSettingsWrapper *m_ssSettingsWrapper; 
    SettingsWrapper *m_cpSettingsWrapper;

    // Not own
    PSetCliWrapper* m_cliWrapper;
    PSetCallWaitingWrapper* m_callWaitingWrapper;
    
    // Not owned objects
    CpSettingFormItemData *m_DataItemShowCallDuration;
    CpSettingFormItemData *m_OwnVideoInReceivedCall;
    CpSettingFormItemData *m_DataItemSoftRejectTextEditor;
    CpSettingFormItemData *m_DataItemCLI; 
    CpSettingFormItemData *m_DataItemCallWaiting;
    
    // List data mappings 
    QMap<int, QString> m_cliSettingMap;
    
    bool m_callWaitingDistinguishEnabled;
    int m_activeNoteId;
    CpItemDataHelper &m_helper;
};
#endif // CPCALLSPLUGINGROUP_H
