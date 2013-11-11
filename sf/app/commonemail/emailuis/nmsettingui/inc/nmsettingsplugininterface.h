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

#ifndef NMSETTINGSPLUGININTERFACE_H
#define NMSETTINGSPLUGININTERFACE_H

#include <QtPlugin>
#include <QVariant>

#include "nmsettingscommon.h"

/*!
    \class NmSettingsPluginInterface
    \brief The class NmSettingsPluginInterface is an interface for mailbox settings.
           Mailbox settings can be displayed in mailbox settings view.
 */

class HbDataForm;
class HbDataFormModel;
class NmId;

class NmSettingsPluginInterface
{

public:

    /*!
        Destructor of NmSettingsPluginInterface.
    */
    virtual ~NmSettingsPluginInterface()
    {
    }

    /*!
        Populates form model with mailbox settings.
        Model can be populated with HbDataFormModelItems.
        Items that are added to model can be displayed in mailbox settings view.
        Signals that are emitted by items that are added to form model, needs to be handled by
        realizing class.

        \param model Form model where items are added.
        \param form Used to add connections.
        \param mailboxId Specific mailbox
        \return <code>true</code> if plug-in contains certain mailbox otherwise <code>false</code>
    */
    virtual bool populateModel(HbDataFormModel &model, HbDataForm &form, const NmId &mailboxId) = 0;

signals:

    /* These signals are available when using the interface  */
    virtual void mailboxListChanged(const NmId &mailboxId, NmSettings::MailboxEventType type) = 0;
    virtual void mailboxPropertyChanged(const NmId &mailboxId, QVariant property, QVariant value) = 0;
    virtual void goOnline(const NmId &mailboxId) = 0;
    virtual void goOffline(const NmId &mailboxId) = 0;

public slots:

	virtual void aboutToClose() = 0;
};

Q_DECLARE_INTERFACE(NmSettingsPluginInterface, "sf.app.commonmail.emailuis.nmailsettingui.nmsettingsplugininterface/1.0");

#endif // NMSETTINGSPLUGININTERFACE_H
