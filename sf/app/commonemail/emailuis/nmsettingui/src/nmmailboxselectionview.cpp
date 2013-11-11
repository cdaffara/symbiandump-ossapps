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

#include <QScopedPointer>
#include <QSignalMapper>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbPushButton>
#include <HbDataFormViewItem>
#include <cpitemdatahelper.h>
#include <nmmailbox.h>

#include "nmmailboxentryitem.h"
#include "nmmailboxselectionview.h"
#include "nmsettingsviewfactory.h"
#include "nmmailboxsettingsmanager.h"

/*!
    \class NmMailboxSelectionView
    \brief Selection view for the mailbox accounts.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmMailboxSelectionView.

    Constructs the model and populates it with the mailbox items.
*/
NmMailboxSelectionView::NmMailboxSelectionView(
    const NmSettingsViewFactory &settingsFactory,
    NmMailboxSettingsManager &settingsManager,
    const QList<NmMailbox *> &mailboxList,
    QGraphicsItem *parent)
: CpBaseSettingView(0, parent),
  mSettingsManager(settingsManager),
  mSettingsFactory(settingsFactory),
  mRefreshForm(false)
{
    NM_FUNCTION;

    QScopedPointer<QSignalMapper> signalMapper(new QSignalMapper());

    // Connect the form's activated signal.
    HbDataForm *form = qobject_cast<HbDataForm*>(widget());
    connect(form, SIGNAL(activated(QModelIndex)),
        this, SLOT(itemActivate(QModelIndex)));

    QScopedPointer<CpItemDataHelper> itemHelper(new CpItemDataHelper());
    QScopedPointer<HbDataFormModel> model(new HbDataFormModel());

    // Ownership is not transferred yet.
    mItemDataHelper = itemHelper.data();
    mModel = model.data();

    // populate data.
    populateDataModel(mailboxList);

    (void)model.take();
    (void)itemHelper.take();
    mSignalMapper = signalMapper.take();

    // Set the localized title string.
    setTitle(hbTrId("txt_mail_title_mail"));
}

/*!
    Destructor of NmMailboxSelectionView.
*/
NmMailboxSelectionView::~NmMailboxSelectionView()
{
    NM_FUNCTION;

    delete mItemDataHelper;
    delete mModel;
    delete mSignalMapper;
}

/*!
    Handles the mailbox button click signal. Calls the SettingsViewFactory
    to construct the setting view for the selected mailbox account.

    \param item NmMailBoxEntryItem mapped for the button.

*/
void NmMailboxSelectionView::buttonClick(QObject *item)
{
    NM_FUNCTION;

    NmMailboxEntryItem *entryItem = static_cast<NmMailboxEntryItem *>(item);
    const NmId &id = entryItem->id();
    const QString &name = entryItem->text();
    mSettingsFactory.launchSettingView(id, name);
}

/*!
    Handels the dataform's activated signal when item is shown.
    Maps the signal/mailbox item for the buttons.

    \param index Data model index.

*/
void NmMailboxSelectionView::itemActivate(const QModelIndex &index)
{
    NM_FUNCTION;

    HbDataForm *form = qobject_cast<HbDataForm*>(widget());
    HbDataFormModel *model = static_cast<HbDataFormModel *>(form->model());
    NmMailboxEntryItem *item = static_cast<NmMailboxEntryItem *>(model->itemFromIndex(index));

    if (item && !mSignalMapper->mapping(item)) {

        HbDataFormViewItem *viewItem =
            static_cast<HbDataFormViewItem *>(form->itemByIndex(index));
        HbPushButton *button =
            static_cast<HbPushButton *>(viewItem->dataItemContentWidget());

        connect(button, SIGNAL(pressed()), mSignalMapper, SLOT(map()));

        mSignalMapper->setMapping(button, item);

        connect(mSignalMapper, SIGNAL(mapped(QObject *)),
            this, SLOT(buttonClick(QObject *)), Qt::UniqueConnection);
    }
}

/*!
    Populates the data model for the view.

    \param mailboxList List of all the available mailbox accounts.

*/
void NmMailboxSelectionView::populateDataModel(const QList<NmMailbox *> &mailboxList)
{
    NM_FUNCTION;

    mModel->clear();
    foreach (NmMailbox *mailbox, mailboxList) {
        NmId id = mailbox->id();
        QScopedPointer<NmMailboxEntryItem> item(
            new NmMailboxEntryItem(*mItemDataHelper, id, mailbox->name(),
                mSettingsManager.mailboxIcon(id)));
        mModel->appendDataFormItem(item.data());
        item.take();
    }
    HbDataForm *form = qobject_cast<HbDataForm*>(widget());
    if (form) {
        form->setModel(mModel);
	}
}

/*!
    Handles mailbox list changes.

    \param mailboxId mailbox id
    \param type type of change \sa nmsettingscommon.h
*/
void NmMailboxSelectionView::mailboxListChanged(const NmId &mailboxId,
    NmSettings::MailboxEventType type)
{
    NM_FUNCTION;

    switch (type) {
        case NmSettings::MailboxDeleted: {
            // Search correct item.
            QModelIndex itemModelIndex;
            bool found(false);
            const int rowCount(mModel->rowCount());
            for (int rowIndex(0); rowIndex < rowCount && !found; ++rowIndex) {
                itemModelIndex = mModel->index(rowIndex, 0);
                NmMailboxEntryItem *entryItem = static_cast<NmMailboxEntryItem *>(
                    mModel->itemFromIndex(itemModelIndex));
                if (entryItem && (entryItem->id() == mailboxId)) {
                    found = true;
                }
            }

            if (found) {
                // There is nothing that we can do if removing fails.
                (void)mModel->removeItem(itemModelIndex);
            }
            break;
        }
        default: {
            break;
        }
    }
}

/*!
    Handles mailbox property changes.

    \param mailboxId mailbox id
    \param property changed property \sa nmsettingscommon.h
    \param value changed value
*/
void NmMailboxSelectionView::mailboxPropertyChanged(const NmId &mailboxId,
    QVariant property, QVariant value)
{
    NM_FUNCTION;

    // Search correct item.
    NmMailboxEntryItem *entryItem = NULL;
    const int itemCount(mModel->rowCount());
    for (int itemIndex(0); itemIndex < itemCount; ++itemIndex) {
        entryItem = static_cast<NmMailboxEntryItem *>(
            mModel->itemFromIndex(mModel->index(itemIndex, 0)));
        if (entryItem && (entryItem->id() == mailboxId)) {
            break;
        }
        entryItem = 0;
    }

    switch (property.toInt()) {
        case NmSettings::MailboxName:
            if (entryItem) {
                entryItem->setText(value.toString());
                mRefreshForm = true;
            }
            break;
        default:
            break;
    }
}
