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

#ifndef CPNETWORKPLUGINFORM_H
#define CPNETWORKPLUGINFORM_H

#include <hbdataform.h>
#include <QStringList>
#include <QSharedPointer>
#include <psetnetworkwrapper.h>
#include <hbmessagebox.h>

class HbDataFormModelItem;
class SettingsWrapper;
class PSetWrapper;
class HbListWidget;
class PSetNetworkWrapper;
class CpSettingFormItemData;
class PsUiNotes;
class CellularDataSettings;

class CpNetworkPluginForm : public HbDataForm
{
    Q_OBJECT
    
public:
    explicit CpNetworkPluginForm(QGraphicsItem *parent = 0);
    virtual ~CpNetworkPluginForm();

    void searchAvailableNetworks();
    
signals:
    void showGlobalProgressNote(int &noteId, const QString& text);
    void showGlobalNote(
        int &noteId, const QString& text, HbMessageBox::MessageBoxType msgBoxType);
    void showNotificationDialog(const QString& text);
    void cancelNote(int noteId);

public slots:
    void cellularDataUseHomeStateChanged(int index);
    void cellularDataUseRoamStateChanged(int index);
    void networkModeStateChanged(int index);
    void operatorSelectionStateChanged();
    void networkAccessModeGot(int mode);
    void availableNetworksGot(
        QList<PSetNetworkWrapper::NetworkInfo*> &m_networkInfoList);
    void networkReqestFailed(
        PSetNetworkWrapper::ErrorCode error, 
        PSetNetworkWrapper::RequestType type);
    void userCancel();
    void handleSearchingNetworks(PSetNetworkWrapper::RequestType &type);
    void handleRequestingSelectedNetwork(bool ongoing);
    void handleNetworkChanged(
        PSetNetworkWrapper::NetworkInfo& currentInfo,
        PSetNetworkWrapper::RegistrationStatus& status);
    void finishedManualSelectiondialog(HbAction* action);
    void chageVisbilityOfManualNetworkSelection(bool visible);

protected:     
    HbDataFormModelItem *createNetworkModeItem();
    HbDataFormModelItem *createOperatorSelectionItem();
    HbDataFormModelItem *createHomeDataUsageItem();
    HbDataFormModelItem *createRoamDataUsageItem();
    
    void automaticOperatorSelection();
    void manualOperatorSelection();
    
    void dualModeSelection();
    void umtsSelection();
    void gsmSelection();
    
    HbDialog* createDialog(const QString& heading) const;
    void addItemToListWidget(HbListWidget* w, const QString& item, 
                            const int& data, const HbIcon& primaryIcon) const;
    
    void showManualSelectiondialog();
    void restoreOperatorSelectionUi();
    QString networkName(PSetNetworkWrapper::NetworkInfo &info);

    void connectToNetworkWrapper(PSetNetworkWrapper &wrapper);
    void connectToPhoneNotes(PsUiNotes &notes);

    QString primaryIconForNetwork(const PSetNetworkWrapper::NetworkInfo &info);
    
private:
    
    QStringList mNetworkModeOptions;
    QStringList mOperatorSelectionOptions;
    QStringList mCellularUseHomeOptions;
    QStringList mCellularUseRoamOptions;
    int m_activeNoteId;
    int m_activeProgressNoteId;
    // Own
    PSetWrapper *m_pSetWrapper;
    SettingsWrapper *m_cpSettingsWrapper;
    QSharedPointer<CellularDataSettings> mCellularSettings;
    HbDialog *m_dialog; 
    
    // Not own
    PSetNetworkWrapper *m_psetNetworkWrapper;
    CpSettingFormItemData *m_NetworkModeOptionsItemData;
    CpSettingFormItemData *m_NetworkOperatorSelectionItemData;
    CpSettingFormItemData *mCellularUseHomeSelectionItemData;
    CpSettingFormItemData *mCellularUseRoamSelectionItemData;
    QList<PSetNetworkWrapper::NetworkInfo*> *m_networkInfoList;
};

#endif  // CPNETWORKPLUGINFORM_H
