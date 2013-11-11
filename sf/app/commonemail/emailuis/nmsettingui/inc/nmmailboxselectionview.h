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

#ifndef NMMAILBOXSELECTIONVIEW_H
#define NMMAILBOXSELECTIONVIEW_H


#include <QVariant>
#include <cpbasesettingview.h>

#include "nmsettingscommon.h"

class QGraphicsItem;
class QSignalMapper;
class HbDataFormModel;
class HbDataFormModelItem;
class CpSettingFormItemData;
class NmSettingsViewFactory;
class NmMailboxSettingsManager;
class NmMailboxEntryItem;
class NmMailbox;
class NmId;

class NmMailboxSelectionView : public CpBaseSettingView
{
    Q_OBJECT

public:

    explicit NmMailboxSelectionView(
        const NmSettingsViewFactory &settingsFactory,
        NmMailboxSettingsManager &settingsManager,
        const QList<NmMailbox *> &mailboxList,
        QGraphicsItem *parent = NULL);

    virtual ~NmMailboxSelectionView();

public slots:

    void buttonClick(QObject *item);
    void itemActivate(const QModelIndex &index );

private slots:

    void mailboxListChanged(const NmId &mailboxId, NmSettings::MailboxEventType type);
    void mailboxPropertyChanged(const NmId &mailboxId, QVariant property, QVariant value);


private:

    Q_DISABLE_COPY(NmMailboxSelectionView)
    void populateDataModel(const QList<NmMailbox *> &mailboxList);

private: // data

    NmMailboxSettingsManager &mSettingsManager;
    const NmSettingsViewFactory &mSettingsFactory;
    // Owned.
    CpItemDataHelper *mItemDataHelper;
    // Owned.
    HbDataFormModel *mModel;
    // Owned.
    QSignalMapper *mSignalMapper;
    bool mRefreshForm;
};

#endif // NMMAILBOXSELECTIONVIEW_H
