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

#ifndef NMSETTINGSVIEWFACTORY_H
#define NMSETTINGSVIEWFACTORY_H

#include <cpsettingformentryitemdata.h>

#include "nmsettinguidef.h"
#include "nmsettingscommon.h"

class NmMailboxSettingsManager;
class CpItemDataHelper;
class QString;
class HbIcon;
class HbDataFormModelItem;
class NmId;
class NmSettingsViewLauncher;
class HbView;

class NMAILSETTINGUI_EXPORT NmSettingsViewFactory : public CpSettingFormEntryItemData
{
    Q_OBJECT

public:

    explicit NmSettingsViewFactory(CpItemDataHelper &itemDataHelper,
        const QString &text = QString(),
        const QString &description = QString(),
        const HbIcon &icon = HbIcon(),
        const HbDataFormModelItem *parent = NULL);

    explicit NmSettingsViewFactory(const NmSettingsViewLauncher *viewLauncher,
        CpItemDataHelper &itemDataHelper,
        const QString &text = QString(),
        const QString &description = QString(),
        const HbIcon &icon = HbIcon(),
        const HbDataFormModelItem *parent = NULL);

    ~NmSettingsViewFactory();

    CpBaseSettingView *createSettingView() const;

    void launchSettingView(const NmId &mailboxId,
                           const QString &mailboxName) const;

signals:

	void aboutToClose();

private slots:

    void backPress();

private:

    Q_DISABLE_COPY(NmSettingsViewFactory)

private: // data

    // Owned.
    NmMailboxSettingsManager *mSettingsManager;

    // Not owned.
    const NmSettingsViewLauncher *mSettingsViewLauncher;

    // Not Owned.
    mutable HbView* mPrevView;
};


#endif // NMSETTINGSVIEWFACTORY_H

// End of file.
