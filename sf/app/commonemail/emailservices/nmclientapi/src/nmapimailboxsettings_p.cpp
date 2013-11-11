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

#include "nmapiheaders.h"
#include <nmapisettingsmanager.h>

namespace EmailClientApi
{
NmApiMailboxSettingsPrivate::NmApiMailboxSettingsPrivate(QObject *parent) :
    QObject(parent),
    mFactory(NULL),
    mDeleteMailboxOperation(NULL)
{
    NM_FUNCTION;
    mFactory = NmApiDataPluginFactory::instance();
    Q_CHECK_PTR(mFactory);
}

NmApiMailboxSettingsPrivate::~NmApiMailboxSettingsPrivate()
{
    NM_FUNCTION;
    NmApiDataPluginFactory::releaseInstance(mFactory);
    if(mDeleteMailboxOperation && mDeleteMailboxOperation->isRunning()) {
        mDeleteMailboxOperation->cancelOperation();
    }
}

bool NmApiMailboxSettingsPrivate::listMailboxIds(QList<quint64> &idList)
{
    NM_FUNCTION;
    QList<NmId> mailboxIds;
    bool ret = false;
    idList.clear();

    NmDataPluginInterface *instance = mFactory->interfaceInstance();
    if (instance) {
        if (instance->listMailboxIds(mailboxIds) == KErrNone) {

            foreach (NmId boxId, mailboxIds) {
                        idList.append(boxId.id());
                    }

            ret = true;
            }
        }

    return ret;
}

QString NmApiMailboxSettingsPrivate::getMailboxType(quint64 mailboxId)
{
    QString mailboxType;
    //Return null string if mailbox is not resolved or some error occurs
    TRAP_IGNORE(mailboxType = resolveMailboxTypeL(mailboxId));
    NM_ERROR(!mailboxType.isNull(), QString("Can not receive mailbox type")
                                    + QString::number(mailboxId));
    return mailboxType;
}

QString NmApiMailboxSettingsPrivate::resolveMailboxTypeL(quint64 mailboxId)
{
    QString mailboxType;
    quint32 serviceId = mailboxId;
    CEmailAccounts *accounts = CEmailAccounts::NewLC();

    RArray<TImapAccount> imapAccounts;
    CleanupClosePushL(imapAccounts);
    accounts->GetImapAccountsL(imapAccounts);
    for (int i=0; i < imapAccounts.Count(); ++i) {
        if (imapAccounts[i].iImapService == serviceId) {
            mailboxType = NmApiMailboxTypeImap;
            break;
        }
    }
    CleanupStack::PopAndDestroy(&imapAccounts);

    if (mailboxType.isNull()) {
        RArray<TPopAccount> popAccounts;
        CleanupClosePushL(popAccounts);
        accounts->GetPopAccountsL(popAccounts);
        for (int i = 0; i < popAccounts.Count(); ++i) {
            if (popAccounts[i].iPopService == serviceId) {
                mailboxType = NmApiMailboxTypePop;
                break;
            }
        }
        CleanupStack::PopAndDestroy(&popAccounts);
    }
    CleanupStack::PopAndDestroy(accounts);
    return mailboxType;
}


bool NmApiMailboxSettingsPrivate::loadSettings(quint64 mailboxId, NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    QScopedPointer<NmApiSettingsManager> settingsManager(new NmApiSettingsManager(mailboxId));
    return settingsManager->load(mailboxId, data);
}

bool NmApiMailboxSettingsPrivate::saveSettings(const NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    QScopedPointer<NmApiSettingsManager> settingsManager(new NmApiSettingsManager(data.mailboxId()));
    return settingsManager->save(data);
}

bool NmApiMailboxSettingsPrivate::createMailbox(
    const QString &mailboxType,
    NmApiMailboxSettingsData &data)
{
    NM_FUNCTION;
    Q_UNUSED(mailboxType);
    Q_UNUSED(data);
    bool ret = false;

    /*QT_TRY {
        if (mailboxType==NmApiMailboxTypePop || mailboxType==NmApiMailboxTypeImap) {
            QScopedPointer<NmApiPopImapSettingsManager> popImapManager(new NmApiPopImapSettingsManager());
            popImapManager->createMailbox(mailboxType, data);
            ret = true;
        }
        else {
            ret = false;
        }
        //TODO store cenrep stuff here
    }
    QT_CATCH(...){
        ret = false;
    }*/

    return ret;
}

bool NmApiMailboxSettingsPrivate::deleteMailbox(quint64 mailboxId)
{
	Q_UNUSED(mailboxId);
    return false;

}

bool NmApiMailboxSettingsPrivate::populateDefaultSettings(
    const QString &mailboxType, NmApiMailboxSettingsData &data)
{
	NM_FUNCTION;
//    Q_UNUSED(mailboxType);
//    Q_UNUSED(data);
    QScopedPointer<NmApiPopImapSettingsManager> popImapManager(new NmApiPopImapSettingsManager());
    return popImapManager->populateDefaults(mailboxType, data);
//    return true;
}

}// namespace
