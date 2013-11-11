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

#include <QInputContext>

#include <HbInstance>
#include <HbMainWindow>
#include <HbAction>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDocumentLoader>
#include <HbLineEdit>
#include <HbVkbHostBridge>

#include "nmmailboxsettingview.h"
#include "nmmailboxsettingsmanager.h"
#include "nmsettingsformcustomitems.h"


static const char *NMSETTINGUI_SETTING_VIEW_XML = ":/docml/nmmailboxsettingview.docml";
static const char *NMSETTINGUI_SETTING_VIEW_FORM = "mailboxSettingViewForm";


/*!
    \class NmMailboxSettingView
    \brief Setting view for the mailbox specific settings.

*/


// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmMailboxSettingView.

    Creates the setting form and form model that contain the setting items.
    Populates the form model items from the correct settings plug-in through
    NmMailboxSettingsManager.
*/
NmMailboxSettingView::NmMailboxSettingView(const NmId &mailboxId,
    const QString &mailboxName,
    NmMailboxSettingsManager& settingsManager,
    QGraphicsItem *parent)
    : CpBaseSettingView(0, parent),
      mForm(NULL),
      mModel(NULL),
      mMailboxId(mailboxId.id())
{
    NM_FUNCTION;

    setTitle(mailboxName);

    HbDocumentLoader documentLoader;
    bool documentLoaded = false;
    QObjectList objectList;
    objectList.append(this);

    documentLoader.setObjectTree(objectList);
    QObjectList widgetList =
        documentLoader.load(NMSETTINGUI_SETTING_VIEW_XML, &documentLoaded);

    if (documentLoaded && widgetList.count()) {
        // Get the form widget.
        mForm = qobject_cast<HbDataForm*>(
            documentLoader.findWidget(NMSETTINGUI_SETTING_VIEW_FORM));
    }

    if (mForm) {

        qRegisterMetaType<QModelIndex>("QModelIndex");

        connect(mForm, SIGNAL(activated(QModelIndex)),
                this, SLOT(itemActivated(QModelIndex)));
        connect(mForm, SIGNAL(itemShown(QModelIndex)),
                this, SLOT(disablePredictiveText(QModelIndex)));


        // Fix for dataform item recycling.
        mForm->setItemRecycling(false);

        // Set the form for the view.
        setWidget(mForm);

        // Make the custom items available.
        NmSettingsFormCustomItems *customItems = new NmSettingsFormCustomItems(mForm);
        QList<HbAbstractViewItem *> prototypes  = mForm->itemPrototypes();
        prototypes.append(customItems);
        mForm->setItemPrototypes(prototypes);

        // Set up the model.
        mModel = new HbDataFormModel();
        settingsManager.populateModel(*mModel, *mForm, mailboxId);
        mForm->setModel(mModel);
    }
}


/*!
    Destructor of NmMailboxSettingView.
*/
NmMailboxSettingView::~NmMailboxSettingView()
{
    NM_FUNCTION;

    delete mForm;
    delete mModel;
}


/*!
    Private slot for handling mailbox list event changes.

    \param mailboxId The ID of the mailbox of which list has been changed.
    \param type The type of the change.
*/
void NmMailboxSettingView::mailboxListChanged(const NmId &mailboxId,
    NmSettings::MailboxEventType type)
{
    NM_FUNCTION;

    Q_UNUSED(mailboxId);
    Q_UNUSED(type);

    HbAction *action = navigationAction();
    if (action) {
        action->activate(QAction::Trigger);
    }
}


/*!
    Private slot for handling mailbox property changes.

    \param mailboxId The ID of the mailbox of which property was changed.
    \param property The type of the property that changed.
    \param value The new value related to the change.
*/
void NmMailboxSettingView::mailboxPropertyChanged(const NmId &mailboxId,
                                                  QVariant property,
                                                  QVariant value)
{
    NM_FUNCTION;

    Q_UNUSED(mailboxId);

    switch (property.toInt()) {
        case NmSettings::MailboxName: {
            setTitle(value.toString());
            break;
        }
        default: {
            break;
        }
    }
}

/*!
    Returns the mailbox id for this mailbox setting view.
*/
NmId NmMailboxSettingView::mailboxId()
{
    return mMailboxId;
}

/*!
    Called when item is activated on the view.

    \param index Index to the pressed item.
*/
void NmMailboxSettingView::itemActivated(const QModelIndex &index)
{
    NM_FUNCTION;

    int type(index.data(HbDataFormModelItem::ItemTypeRole).toInt());


    if (type == HbDataFormModelItem::GroupItem) {

        // Scroll the groupitem to top.
        if (!mForm->isExpanded(index)) {
            QMetaObject::invokeMethod(this, "queuedScroll",
                Qt::QueuedConnection, Q_ARG(QModelIndex, index));
        }else {
            // Hide the virtual keyboard
            QInputContext *ic = qApp->inputContext();
            if (ic) {
                QEvent *closeEvent = new QEvent(QEvent::CloseSoftwareInputPanel);
                ic->filterEvent(closeEvent);
                delete closeEvent;
            }
        }
    }


}

/*!
    Called when item is shown on the view.

    \param index Index to the shown item.
*/
void NmMailboxSettingView::disablePredictiveText(const QModelIndex &index)
{
    NM_FUNCTION;

    int type(index.data(HbDataFormModelItem::ItemTypeRole).toInt());

    if (type == HbDataFormModelItem::TextItem) {
        // Turn off predictive input for line-edit.
        HbDataFormViewItem *item = static_cast<HbDataFormViewItem *>(mForm->itemByIndex(index));
        HbWidget *widget = item->dataItemContentWidget();
        if (widget) {
            widget->setInputMethodHints(Qt::ImhNoPredictiveText);
            widget->installEventFilter(this);
		}
    }
}

/*!
    Called when view needs to be scrolled.

    \param index Index to the item.
*/
void NmMailboxSettingView::queuedScroll(const QModelIndex &index)
{
    NM_FUNCTION;

    mForm->scrollTo(index, HbAbstractItemView::PositionAtTop);
}

/*!
    Forces HbLineEdit to emit editingFinished when focus has been removed from it, if VKB is open.
    NOTE: This can be removed when VKB handling is fixed by orbit so that editingFinished signal is
          emitted when group box is collapsed.

    \param watched object that eventFilter has installed to.
    \param event occured event.
*/
bool NmMailboxSettingView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QFocusEvent::FocusOut) {
        QGraphicsItem *item = qobject_cast<QGraphicsItem *>(watched);
        if (item && item->type() == Hb::ItemType_LineEdit) {
            HbVkbHostBridge *instance = HbVkbHostBridge::instance();
            // Only force emit if VKB is open.
            if(instance && instance->keypadStatus() == HbVkbHost::HbVkbStatusOpened) {
                HbLineEdit *edit = static_cast<HbLineEdit *>(item);
                // Save settings.
                connect(this, SIGNAL(emitEditingFinished()), edit, SIGNAL(editingFinished()));
                emit emitEditingFinished();
                disconnect(this, SIGNAL(emitEditingFinished()), edit, SIGNAL(editingFinished()));
            }
        }
    }
    // Event has not been consumed.
    return false;
}

// End of file.
