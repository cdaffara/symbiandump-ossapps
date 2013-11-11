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

#ifndef NMMAILBOXENTRYITEM_H
#define NMMAILBOXENTRYITEM_H

#include <cpsettingformentryitemdata.h>
#include <nmcommon.h>

class CpItemDataHelper;
class HbIcon;
class HbDataFormModelItem;
class QString;

class NmMailboxEntryItem : public CpSettingFormEntryItemData
{
Q_OBJECT

public:

    explicit NmMailboxEntryItem(CpItemDataHelper &itemDataHelper,
        const NmId &mailboxId,
        const QString &mailboxName = QString(),
        const HbIcon &mailboxIcon = HbIcon(),
        const HbDataFormModelItem *parent = NULL,
        const QString &mailboxDescription = QString());

    virtual ~NmMailboxEntryItem();

    CpBaseSettingView *createSettingView() const;

    const NmId &id() const;

private:

    Q_DISABLE_COPY(NmMailboxEntryItem)

private: // data

    NmId mMailboxId;
};

#endif // NMMAILBOXENTRYITEM_H
