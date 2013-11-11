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
* Description: NMail application service interface used for viewing a mail
*              according to given id. The service utilizes the Qt highway framework.
*/

//  INCLUDES
#include "nmuiheaders.h" // Includes also the class header.


/*!
    \class NmViewerServiceInterface
    \brief A service interface for displaying the message according to given id.
*/


/*!
    Class constructor.
*/
NmViewerServiceInterface::NmViewerServiceInterface(QObject *parent,
                                                   NmApplication *application,
                                                   NmUiEngine &uiEngine)
: XQServiceProvider(emailFullServiceNameMessage, parent),
  mApplication(application),
  mUiEngine(uiEngine),
  mAsyncReqId(0)
{
    publishAll();
}


/*!
    Class desctructor.
*/
NmViewerServiceInterface::~NmViewerServiceInterface()
{
    NM_FUNCTION;
}


/*!
    Opens the message view according to the given IDs.

    \param idList A list containing the required IDs for locating the wanted
                  message. The ID are the following (and should be placed in
                  the following order): mailbox ID, folder ID and message ID.
    \param flags Will contain EmailBackReturnsToMessageList if the message
                 list view should be added into the view stack.
*/
void NmViewerServiceInterface::viewMessage(QVariant idList, QVariant flags)
{
    NM_FUNCTION;

    mAsyncReqId = setCurrentRequestAsync();

    NmId mailboxId(0), folderId(0), messageId(0);

    // Check the given arguments.
    if (idList.canConvert(QVariant::List)) {
        QList<QVariant> ids = idList.toList();

        if (ids.count() >= 3) {
            mailboxId = ids[0].toULongLong();
            folderId = ids[1].toULongLong();
            messageId = ids[2].toLongLong();
        }
    }

    quint64 flag(EmailNoFlags);

    if (flags.canConvert(QVariant::ULongLong)) {
        flag = flags.toULongLong();
    }

    bool addMessageListViewToStack(false);

    if (flag & EmailBackReturnsToMessageList) {
        addMessageListViewToStack = true;
    }

	// Make sure the app stays on the background if the user presses back button
    // in the viewer view.
	bool visible = mApplication->updateVisibilityState();

    NmMessage *message = mUiEngine.message(mailboxId, folderId, messageId);

    if (message) {
        // Bring the application to the foreground.
        if (!XQServiceUtil::isEmbedded()) {
            XQServiceUtil::toBackground(false);
        }

        HbMainWindow *mainWindow = mApplication->mainWindow();
        mainWindow->show();

        if (addMessageListViewToStack) {
            // Launch the message list view.
            NmUiStartParam *startParam1 =
                new NmUiStartParam(NmUiViewMessageList,
                                   mailboxId,
                                   folderId,
                                   messageId,
                                   NmUiEditorCreateNew, // editor start mode
                                   NULL, // address list
                                   NULL, // attachment list
                                   true); // start as service

            mApplication->enterNmUiView(startParam1);
        }

        // Launch the message view.
        NmUiStartParam *startParam =
            new NmUiStartParam(NmUiViewMessageViewer,
                               mailboxId,
                               folderId, // folder id
                               messageId, // message id
                               NmUiEditorCreateNew, // editor start mode
                               NULL, // address list
                               NULL, // attachment list
                               !addMessageListViewToStack); // start as service?

        mApplication->enterNmUiView(startParam);

        completeRequest(mAsyncReqId, 0);
        
        delete message;
        message = NULL;
    }
    else {
        // The message was not found!

        // If started as embedded, do not hide the app.
		if (!XQServiceUtil::isEmbedded() && !visible) {
			XQServiceUtil::toBackground(true);
		}

        completeRequest(mAsyncReqId, 1);

        // Close the application if started as a service.
        if (XQServiceUtil::isService()) {
            // Exit the application when the return value is delivered.
            connect(this, SIGNAL(returnValueDelivered()),
                    mApplication, SLOT(delayedExitApplication()));
        }
    }
}


// End of file.
