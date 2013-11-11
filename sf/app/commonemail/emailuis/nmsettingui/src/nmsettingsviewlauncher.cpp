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

#include <cpitemdatahelper.h>

#include "nmsettingsviewlauncher.h"
#include "nmsettingsviewfactory.h"
#include "nmcommon.h"

/*!
    \class NmSettingsViewLauncher
    \brief Constructs and displays settings views.
*/


// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmSettingsViewLauncher.
    \return A newly constructed settings view launcher instance.
*/
NmSettingsViewLauncher::NmSettingsViewLauncher()
{
    QScopedPointer<CpItemDataHelper> helper(new CpItemDataHelper());
    mSettingsViewFactory = new NmSettingsViewFactory(this, *helper.data());
    mItemDataHelper = helper.take();
}

/*!
    Destructor of NmSettingsViewLauncher.
*/
NmSettingsViewLauncher::~NmSettingsViewLauncher()
{
    delete mSettingsViewFactory;
    delete mItemDataHelper;
}

/*!
    Constructs and shows the settings view for the given mailbox.

    \param mailboxId The id of the mailbox.
    \param mailboxName The name of the mailbox.
*/
void NmSettingsViewLauncher::launchSettingsView(
    const NmId &mailboxId,
    const QString &mailboxName)
{
    NM_FUNCTION;
    
    mSettingsViewFactory->launchSettingView(mailboxId, mailboxName);
}

// End of file.
