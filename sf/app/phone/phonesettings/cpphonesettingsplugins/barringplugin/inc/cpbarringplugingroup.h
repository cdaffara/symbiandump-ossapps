/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CPBARRINGPLUGINGROUP_H
#define CPBARRINGPLUGINGROUP_H

#include <QQueue>
#include <cpsettingformitemdata.h>
#include <psetcallbarringwrapper.h>

class PSetWrapper;
class QModelIndex;
class PsUiNotes;
class QValidator;
class HbDataFormModel;

    
/*!
    \class CpBarringPluginGroup
    \brief The class CpBarringPluginGroup defines barring items shown on UI. 
 */
class CpBarringPluginGroup : public CpSettingFormItemData
{
    Q_OBJECT
    
public:
    
    // Change barring password operation phases
    enum ChangeBarringPasswordPhase
    {
        NonePhase,
        CurrentPasswordPhase,
        NewPasswordPhase,
        VerifyNewPasswordPhase
    };

    explicit CpBarringPluginGroup(CpItemDataHelper &helper);
    ~CpBarringPluginGroup();

public slots:
    
    void itemShown(const QModelIndex& item);
    
private:
    
    void setupConnectionsToWrapper();
    
    void createBarringItems();
    CpSettingFormItemData *createBarringItem(
        const HbDataFormModelItem::DataItemType &itemType,
        const QString &label,
        const QString &widgetTextData,
        const PSetCallBarringWrapper::BarringType &barringType);
    
    void processBarringStatusRequestQueue();
    bool updateDependentBarringProgramStatuses(
        const CpSettingFormItemData &changedBarringItem);
    CpSettingFormItemData &barringItemByProgram(
        const PSetCallBarringWrapper::BarringType &barringProgram);
    void revertCheckStateOfItem(
        CpSettingFormItemData *barringItem);
    void updateCheckStateOfItem(
        CpSettingFormItemData &barringItem, 
        const Qt::CheckState &newState);
    
private slots:

    void barringStatusRequestCompleted(
        int result,
        const QList<unsigned char> & basicServiceGroupIds,
        PSetCallBarringWrapper::BarringStatus status);
    
    void enableBarringRequestCompleted(
        int result,
        PSetCallBarringWrapper::BarringType barringType,
        PSetCallBarringWrapper::BarringStatus barringStatus, 
        bool plural);

    void disableBarringRequestCompleted(
        int result,
        PSetCallBarringWrapper::BarringType barringType,
        PSetCallBarringWrapper::BarringStatus barringStatus, 
        bool plural);
    
    void barringPasswordChangeRequestCompleted(int result);
    
    void changeBarringStateRequested(int checkState);
    
    void changeBarringPasswordRequested();
    
    void completeBarringStateChangeRequestHandling(
            QString barringPassword,
            bool okPressed);
    
    void changeBarringPasswordPhasesHandling(
            QString barringPassword,
            bool okPressed);
    
private: 
    
    CpItemDataHelper &m_helper;
    QScopedPointer<PSetWrapper> m_pSetWrapper;
    PSetCallBarringWrapper *m_barringWrapper;
    CpSettingFormItemData *m_editBarringPasswordItem;
    QQueue<CpSettingFormItemData *> m_barringRequestQueue;
    bool m_barringStatusRequestOngoing;
    int m_activeNoteId;
    int m_activeProgressNoteId;
    PsUiNotes* m_phoneNotes;
    QValidator *m_barringPasswordValidator;
    bool m_delayedBarringActivationNote;
    CpSettingFormItemData* m_clickedBarringItem; // Not own.
    ChangeBarringPasswordPhase m_changeBarringPasswordPhase;
    QString m_currentPassword;
    QString m_newPassword;
    QString m_newPasswordVerified;
    QValidator* m_verifyPasswordValidator; // Own.
    HbDataFormModel* m_model;
};

#endif // CPBARRINGPLUGINGROUP_H
