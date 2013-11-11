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
* Description: NMail application service interface used for displaying the
*              messages contained by the given mailbox. The service utilizes
*              thr Qt highway framework.
*/

//  INCLUDES
#include "nmuiheaders.h" // Includes also the class header.


/*!
    \class NmMailboxServiceInterface
    \brief A service interface for displaying the messages contained by the
           given mailbox.
*/


/*!
    Class constructor.
*/
NmMailboxServiceInterface::NmMailboxServiceInterface(QObject *parent,
        NmUiEngine &uiEngine,
        NmApplication *application)
    : XQServiceProvider(emailFullServiceNameMailbox, parent),
      mUiEngine(uiEngine),
      mApplication(application),
      mAsyncReqId(0)
{
    publishAll();
}


/*!
    Class desctructor.
*/
NmMailboxServiceInterface::~NmMailboxServiceInterface()
{
    NM_FUNCTION;
}


/*!
    Called by some external application when the content of a mailbox needs to
    be viewed.
    \param data The ID of the mailbox to open.
*/
void NmMailboxServiceInterface::displayInboxByMailboxId(QVariant data)
{
    NM_FUNCTION;
    NM_TIMESTAMP("Opening message list view starts.");
    
    // Get the given ID and convert it into NmId type.
    NmId mailboxNmId(data.toULongLong());

    mAsyncReqId = setCurrentRequestAsync();

    // Make sure that app stays background if user presses back in message list view
    bool visible = false;
    if(mApplication){
        visible = mApplication->updateVisibilityState();
    }

    if (mailboxExistsById(mailboxNmId)) {
        // Fetch inbox id
        NmId inboxId = mUiEngine.standardFolderId(mailboxNmId, NmFolderInbox);
        // Bring the application to the foreground.
        if (!XQServiceUtil::isEmbedded()) {
            XQServiceUtil::toBackground(false);
        }
        if (mApplication) {
            HbMainWindow *mainWindow = mApplication->mainWindow();
            mainWindow->show();

            // Launch the message list view.
            NmUiStartParam *startParam =
                new NmUiStartParam(NmUiViewMessageList,
                                   mailboxNmId,
                                   inboxId, // folder id
                                   0, // message id
                                   NmUiEditorCreateNew, // editor start mode
                                   NULL, // address list
                                   NULL, // attachment list
                                   true); // start as service
            mApplication->enterNmUiView(startParam);
        }

        completeRequest(mAsyncReqId, 0);
    }
    else {
        // No mailbox found with the given ID or Inbox ID is not known

        // if started as embedded, do not hide the app
		if (!XQServiceUtil::isEmbedded() && !visible) {
			XQServiceUtil::toBackground(true);
		}

        completeRequest(mAsyncReqId, 1);

        // Close the application if started as a service.
        if (XQServiceUtil::isService()) {
            connect(this, SIGNAL(returnValueDelivered()),
                    mApplication, SLOT(delayedExitApplication()));
        }
    }
}


/*!
    Resolves whether a mailbox with the given ID exists or not.
    \param mailboxId The mailbox ID to look for.
    \return True if a mailbox with the given ID exists, false otherwise.
*/
bool NmMailboxServiceInterface::mailboxExistsById(const NmId &mailboxId) const
{
    NM_FUNCTION;
    //model needs to be refreshed because in some cases, new mailbox
    //has not been dynamically inserted into the model yet, 
    //thus returning incorrect count.
    mUiEngine.refreshMailboxListModel();
    
    const NmMailboxListModel& mailboxListModel = mUiEngine.mailboxListModel();
    int mailboxCount = mailboxListModel.rowCount();

    QModelIndex modelIndex;
    QVariant mailbox;
    NmMailboxMetaData *mailboxMetaData = NULL;
    NmId currentId;

    // Try to find the mailbox with the given ID.
    for (int i = 0; i < mailboxCount; ++i) {
        modelIndex = mailboxListModel.index(i, 0);
        mailbox = mailboxListModel.data(modelIndex);
        mailboxMetaData = mailbox.value<NmMailboxMetaData*>();
        if (mailboxMetaData) {
            currentId = mailboxMetaData->id();
        }

        if (currentId.id() == mailboxId.id()) {
            // Found a mailbox with the matching ID.
            return true;
        }
    }

    // No mailbox exist with the given ID.
    return false;
}


// End of file.
