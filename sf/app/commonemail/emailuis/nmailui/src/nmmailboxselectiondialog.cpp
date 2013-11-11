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

#include "nmuiheaders.h"

static const char *NMUI_MAILBOX_SELECTION_DIALOG_XML = ":/docml/nmmailboxselectiondialog.docml";
static const char *NMUI_MAILBOX_LIST_VIEW = "mailboxListView";
static const char *NMUI_MAILBOX_SELECTION_DIALOG = "mailboxSelectionDialog";


/*!
    \class NmMailboxSelectionDialog
    \brief Selection dialog for the mailboxes. Displays a dialog from which the
           user can select a mailbox. The dialog can be cancelled by pressing
           the cancel button.
*/


/*!
     Class constructor.
*/
NmMailboxSelectionDialog::NmMailboxSelectionDialog(
    NmMailboxListModel &mailboxListModel, QGraphicsItem *parent /* = 0 */)
    : mMailboxListView(NULL),
      mMailboxSelectionDialog(NULL),
      mContentItemModel(NULL),
      mMailboxListModel(mailboxListModel),
      mParent(parent),
      mMailboxId(0)
{
    NM_FUNCTION;
}


/*!
    Class destructor.
*/
NmMailboxSelectionDialog::~NmMailboxSelectionDialog()
{
    NM_FUNCTION;
    
    delete mContentItemModel;
    delete mMailboxListView;
    delete mMailboxSelectionDialog;
}


/*!
    Initializes and displays the dialog
    Signal selectionDialogClosed will be emitted when the dialog is closed
*/
void NmMailboxSelectionDialog::open()
{
    NM_FUNCTION;
    
    mMailboxId = 0;

    // Initialize the UI and fetch the mailbox items into the list.
    if (initializeUi() && populateListItems()) {
		mMailboxSelectionDialog->setModal(true);
        mMailboxSelectionDialog->open(this,SLOT(dialogClosed(HbAction*)));
        mMailboxSelectionDialog->show();
    }
}

/*!
    Slot that is called when dialog is closed
 */
void NmMailboxSelectionDialog::dialogClosed(HbAction *action)
{
    NM_FUNCTION;
    
    Q_UNUSED(action);

    // Store the ID of the selected mailbox into the given argument.
    NM_COMMENT(QString("NmMailboxSelectionDialog::dialogClosed() return %1").
        arg(mMailboxId.id()));

    emit selectionDialogClosed(mMailboxId);
}

/*!
    Creates the view for the mailbox selection dialog.

    \return True if the widgets were loaded and set up successfully, false
            otherwise.
*/
bool NmMailboxSelectionDialog::initializeUi()
{
    NM_FUNCTION;
    
    // Use the document loader to load the widgets.
    HbDocumentLoader documentLoader;
    bool documentLoaded(false);

    QObjectList objectList;
    objectList.append(this);

    documentLoader.setObjectTree(objectList);
    mWidgetList =
        documentLoader.load(NMUI_MAILBOX_SELECTION_DIALOG_XML, &documentLoaded);

    if (documentLoaded && mWidgetList.count()) {
        // Get the dialog and the list view widgets.
        mMailboxSelectionDialog =
            qobject_cast<HbDialog*>(documentLoader.findWidget(NMUI_MAILBOX_SELECTION_DIALOG));
        mMailboxListView =
            qobject_cast<HbListView*>(documentLoader.findWidget(NMUI_MAILBOX_LIST_VIEW));
    }

    if (!mMailboxSelectionDialog || !mMailboxListView) {
        NM_ERROR(1,"NmMailboxSelectionDialog::initializeUi(): Failed to load widgets!");
        return false;
    }

    // Set up the loaded widgets.
    mMailboxSelectionDialog->setParentItem(mParent);

    // Disable timeout.
    mMailboxSelectionDialog->setTimeout(HbDialog::NoTimeout);

    // Connect the list view to the slot. When the user selects an item
    // from the list, the slot should be called.
    connect(mMailboxListView, SIGNAL(activated(QModelIndex)),
            this, SLOT(itemActivated(QModelIndex)));

    // Item model initialization, data will be populated later.
    mContentItemModel = new QStandardItemModel(this);

    // Set the model for the list view. Use default prototype for now.
    mMailboxListView->setModel(mContentItemModel);

    // Set the item prototype.
    mMailboxSelectionDialog->setContentWidget(mMailboxListView);

    return true;
}


/*!
    Populates the list items (mailboxes).

    \return True if success, false otherwise.
*/
bool NmMailboxSelectionDialog::populateListItems()
{
    NM_FUNCTION;
    
    const int count = mMailboxListModel.rowCount();

    if (!mContentItemModel || count == 0) {
        // The UI skeleton was not set up successfully or no mailboxes exist!
        return false;
    }

    NmMailboxMetaData *metaData = NULL;
    QStandardItem *item = NULL;

    EmailMailboxInfo mailboxInfo;
    for (int i = 0; i < count; ++i) {
        metaData = mailboxMetaData(i);

        if (metaData) {
            QString domainName = metaData->address();
            QString iconName = mailboxInfo.mailboxIcon(domainName);
			HbIcon mailboxIcon( iconName );

            // Construct the item and append it into the list.
            item = new QStandardItem(mailboxIcon.qicon(), metaData->name());
            mContentItemModel->appendRow(item);
        }
    }

    return true;
}


/*!
    Retrieve the metadata for the given mailbox.

    \param index The index of the mailbox in the list model.
    \return A mailbox meta data instance.
*/
NmMailboxMetaData *NmMailboxSelectionDialog::mailboxMetaData(int index) const
{
    NM_FUNCTION;
    
    QVariant mailbox = mMailboxListModel.data(mMailboxListModel.index(index, 0));
    NmMailboxMetaData *mailboxMetaData = mailbox.value<NmMailboxMetaData*>();
    return mailboxMetaData;
}


/*!
    This slot is invoked when a list item is selected.

    \param index The index of the selected item.
*/
void NmMailboxSelectionDialog::itemActivated(QModelIndex index)
{
    NM_FUNCTION;
    
    const int rowIndex = index.row();
    NmMailboxMetaData *metaData = mailboxMetaData(rowIndex);

    if (metaData && mMailboxSelectionDialog) {
        mMailboxId = metaData->id();
        mMailboxSelectionDialog->close();
    }
}


// End of file.
