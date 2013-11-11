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
#ifndef CPDIVERTPLUGINGROUP_H
#define CPDIVERTPLUGINGROUP_H

#include <QMap>
#include <QString>
#include <QQueue>
#include <cpsettingformitemdata.h>
#include <psetcalldivertingwrapper.h>
#include "cptelephonyutilsdefs.h"
#include "cppluginlogging.h"


class HbDataFormViewItem; 
class PSetWrapper;
class SsSettingsWrapper;
class HbComboBox;
class PSCallDivertingCommand;
class HbListWidget;
class CpDivertItemData;
class QEventLoop; 
class HbLineEdit; 
class HbDataFormModelItem;
class CpDivertQuery;
class QModelIndex;

using namespace CpTelephonyUtils;

/*!
    \class CpDivertPluginGroup
    \brief The class CpDivertPluginGroup defines items shown on UI. 
 */
class CpDivertPluginGroup : public CpSettingFormItemData
{
    Q_OBJECT

    enum DivertRequest{
        ActivateDivert = 0, 
        DeactivateDivert, 
        CheckDivertStatus
    };  

    class CpDivertRequestQueueItem
    {
    public:
        DivertRequest request;
        CpDivertItemData *item;
        
        bool operator==(const CpDivertRequestQueueItem& aItem) const{
            return (this->item == aItem.item) &
                    (this->request == aItem.request);
        }
    };

    
public:

    explicit CpDivertPluginGroup(CpItemDataHelper &helper);
    
    ~CpDivertPluginGroup();

public slots:
    
    // PSetCallDivertingWrapper signal handling
    /**
     Set info request results
     */
    void handleDivertingChanged(
            const PSCallDivertingCommand& aSetting, 
            bool aPlural);
    
    /**
     Get info request results
     */
    void handleDivertingStatus(
        QList<PSCallDivertingStatus*>& divertList, 
        bool plural);
    
    /**
     Request failed.
     */
    void handleDivertingError(int aReason);

    /**
     Adaptation has handled previous request.
     */
    void divertRequestProcessed();
    
    /**
     Slot which handles change requests according UI.
     */
    void changeDivertingStateRequested(CpDivertItemData &item);

    /**
     Slot which handles item updating when it is needed.
     */
    void itemShown(const QModelIndex& item);
    
    /**
     Slot which handles activate divert query result
     */
    void divertQueryOk(PSCallDivertingCommand &command);
    void divertQueryCancelled(PSCallDivertingCommand &command);
private:     
    
    /**
     Creates group items.
     */
    void createVoiceCallItems(CpSettingFormItemData *group);

    /**
     Creates group items.
     */
    void createVideoCallItems(CpSettingFormItemData *parent);
    
    /**
     Creates single item.
     */
    CpDivertItemData *createDivertItem(
            PsCallDivertingCondition condition,
            PsServiceGroup serviceGroup,
            const QString &label,
            const QString &queryLabel,
            bool needTimeoutInfo,
            CpSettingFormItemData *parent = 0);
    
    /**
     Send comand to psetwrappercalldiverting.
     */
    void setCallDiverting(PSCallDivertingCommand& command);
    
    /**
     Desides which bsc parameters to use.
     */
    int bscParam(PsServiceGroup serviceGroup);
    
    /**
     Adds divert data item for wanted parent.
     */
    void addDivertDataItem(
            CpDivertItemData* item, CpSettingFormItemData *parent);

    /**
     Process next divert request.
     */
    void processDivertingRequestQueue();

    /**
     Add new request to divert queue.
     */
    void addToDivertingRequestQueue(
            DivertRequest request, 
            CpDivertItemData &item);
    
    /**
     Changes divertItemData data.
     */
    void changeItemData(
            PsServiceGroup serviceGroup,
            PsCallDivertingCondition condition, 
            PsCallDivertingStatus status,
            const QString& number, 
            int timeout);
    
    /**
     Reverts divertItemData data to the last known valid values.
     */
    void revertItemData(
            PsServiceGroup serviceGroup, PsCallDivertingCondition condition);

    /**
     Helper function which fetches model item according service and condition.
     */
    CpSettingFormItemData* item(
            PsService service, PsCallDivertingCondition condition);
    
    /**
     Helper function which dims divert items when divert request is ongoing.
     */
    void disableDivertItems();
    
    /**
     Helper function which enables divert items when divert request is complete.
     */
    void enableDivertItems();
    
private:

    /**
     Synchronizes content and status of divert options dependent from the 
     status of some other divert.
     */
    void updateDependentDivertOptions(bool fetchFromNetwork = false);
    void activateDependentDivertOption(CpDivertItemData* item, bool fetchFromNetwork);
    void deactivateDependentDivertOption(CpDivertItemData* item) const;
    
    /**
     Synchronizes content and status of not available divert option based on
     statuses of CFB, CFNry and CFNrc.
     */
    void updateNotAvailableDivertOption();

    
    bool isDivertSettingsItem(HbDataFormModelItem* modelItem);
     
private:   // data

    // Setting wrappers 
    PSetWrapper *m_pSetWrapper;
    SsSettingsWrapper *m_ssSettingsWrapper;
    
    // Not own
    PSetCallDivertingWrapper* m_callDivertingWrapper;

    // Not owned objects
    CpDivertItemData *m_DataItemVoiceAllCalls;
    CpDivertItemData *m_DataItemVoiceIfBusy;
    CpDivertItemData *m_DataItemVoiceIfNotAnswered;
    CpDivertItemData *m_DataItemVoiceIfOutOfReach;
    CpDivertItemData *m_DataItemVoiceIfNotAvailable;
    
    CpDivertItemData *m_DataItemVideoAllCalls;
    CpDivertItemData *m_DataItemVideoIfBusy;
    CpDivertItemData *m_DataItemVideoIfNotAnswered;
    CpDivertItemData *m_DataItemVideoIfOutOfReach;
    CpDivertItemData *m_DataItemVideoIfNotAvailable;

    int m_activeNoteId;
    int m_activeProgressNoteId;

    /**
     Divert request queue
     */
    QQueue<CpDivertRequestQueueItem> m_divertRequestQueue;

    CpItemDataHelper &m_helper;

    int m_timerId;
    
    PsServiceGroup m_serviceGroup;
    
    CpDivertQuery *m_activateDivertPopUp;
};

#endif // CPDIVERTPLUGINGROUP_H


