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
* Description: NMail Application Launcher interface used for interfacing between
*              QT highway and other applications
*
*/

//  INCLUDES
#include "nmuiheaders.h"

/*!
    \class NmUriServiceInterface
    \brief NMail application service interface which provides an email sending
           interface for other application using the Qt Highway.
*/

/*!
    Class constructor.
*/
NmUriServiceInterface::NmUriServiceInterface(QObject *parent,
                                               NmUiEngine &uiEngine,
                                               NmApplication *application)
    : XQServiceProvider(emailServiceName+"."+XQI_URI_VIEW, parent),
      mApplication(application),
      mUiEngine(uiEngine),
      mAsyncReqId(0),
      mStartParam(NULL),
      mSelectionDialog(NULL),
      mCurrentView(NULL)
{
    publishAll();
}


/*!
    Class desctructor.
*/
NmUriServiceInterface::~NmUriServiceInterface()
{
    NM_FUNCTION;
    
    delete mStartParam;
    delete mSelectionDialog;
}


/*!
    Queries the user for a mailbox to use.
    \param mailboxId Where the ID of the selected mailbox is set.
    \return True if a mailbox was selected, false otherwise.
*/
void NmUriServiceInterface::selectionDialogClosed(NmId &mailboxId)
{
    NM_FUNCTION;
    
    if (mailboxId.id()) { // mailbox selected
        launchEditorView(mailboxId);
    }
    else {
        cancelService();
    }
}


/*!
    Used from external applications to handle mailto: uri's.

    \param data <mailto:> uri
*/
bool NmUriServiceInterface::view(const QString& uri)
{
    NM_FUNCTION;
      
    HbMainWindow *mainWindow(NULL);
    if (mApplication) {
        // Make sure that nmail stays background if user presses back in editorview
        mApplication->updateVisibilityState();
        
        mainWindow = mApplication->mainWindow();
        if (mainWindow) {
            mCurrentView = mainWindow->currentView();  
        } 
    }

    // Hide the current view.
    if (mCurrentView) {
        mCurrentView->hide();
    }
    
    // Check the given data.
    NmUriParser uriParser;
    bool validData = uriParser.extractData(uri);
    
     
    NmMailboxListModel &mailboxListModel = mUiEngine.mailboxListModel();
    const int count = mailboxListModel.rowCount();
    NmId mailboxId(0);

    mAsyncReqId = setCurrentRequestAsync();

    if (!validData) {
        // Failed to extract the data!
        NM_ERROR(1,"NmUriServiceInterface::view(): failed to process the given data");
        cancelService();
    }
    else if (count == 0) {
        HbDeviceMessageBox note(hbTrId("txt_mail_dialog_no_mailboxes_defined"),
                          HbMessageBox::MessageTypeInformation);
        note.setTimeout(HbMessageBox::NoTimeout);
        note.show();
        cancelService();
    }
    else { // count > 0
        if (mainWindow) {
            mainWindow->show();
        }
        
        // The ownership of these is passed to NmApplication in launchEditorView()
        QList<NmAddress*> *toAddresses = NmUtilities::qstringListToNmAddressList(uriParser.toAddresses());
        QList<NmAddress*> *ccAddresses = NmUtilities::qstringListToNmAddressList(uriParser.ccAddresses());
        QList<NmAddress*> *bccAddresses = NmUtilities::qstringListToNmAddressList(uriParser.bccAddresses());
        
        QString* subject = new QString(uriParser.subject());
        QString* bodyText = new QString(uriParser.bodyText());
       
    	mStartParam = new NmUiStartParam(
        	NmUiViewMessageEditor,
	        0, // account id
	        0, // folder id
    	    0, // message id
    	    NmUiEditorMailto, // editor start mode
    	    toAddresses,
    	    NULL, // attachment list
        	true, // start as service
        	subject, // message subject
            ccAddresses, // list containing cc recipient addresses
            bccAddresses, // list containing bcc recipient addresses
            bodyText // body text
	    );
	    
        if (count == 1) {
            // A single mailbox exists.
            QModelIndex modelIndex = mailboxListModel.index(0, 0);
            QVariant mailbox(mailboxListModel.data(modelIndex));
            NmMailboxMetaData *mailboxMetaData = mailbox.value<NmMailboxMetaData*>();
            mailboxId = mailboxMetaData->id();
        	launchEditorView(mailboxId);
        }
        else { // count > 1
            if (!mSelectionDialog) {
                mSelectionDialog =
                    new NmMailboxSelectionDialog(mUiEngine.mailboxListModel());
            }
            
            if (!XQServiceUtil::isEmbedded()) {
                XQServiceUtil::toBackground(false);
            }
            
            connect(mSelectionDialog,SIGNAL(selectionDialogClosed(NmId&)),
                this,SLOT(selectionDialogClosed(NmId&)));
            mSelectionDialog->open();

            // launch the editor when the dialog is closed
        }
    }
    return true;
}

/*!
    Called when mailbox id is known and editor can be opened
    \param mailboxId mailbox using in editor
 */
void NmUriServiceInterface::launchEditorView(NmId mailboxId)
{
    NM_FUNCTION;
    NM_COMMENT(QString("NmUriServiceInterface::launchEditorView(): mailboxId=%1").arg(mailboxId.id()));
    
    // Make the previous view visible again.
    if (mCurrentView) {
        mCurrentView->show();
        mCurrentView = NULL;
    }

    if (mStartParam) {
        // Make sure the NMail application is in the foreground
        if (!XQServiceUtil::isEmbedded()) {
            XQServiceUtil::toBackground(false);    
        }
        
        mStartParam->setMailboxId(mailboxId);
        mApplication->enterNmUiView(mStartParam);
        mStartParam = NULL; // ownership passed
    }
    completeRequest(mAsyncReqId, 1);
    mAsyncReqId = 0;
}

void NmUriServiceInterface::cancelService()
{
    NM_FUNCTION;
    
    delete mStartParam;
    mStartParam = NULL;

    // If the service was started as embedded, do not hide the app.
    if (!XQServiceUtil::isEmbedded()) {
        XQServiceUtil::toBackground(true);
    }

    completeRequest(mAsyncReqId, 0);
    mAsyncReqId = 0;

    // If started as service, the application must be closed now.
    if (XQServiceUtil::isService()) {
        connect(this, SIGNAL(returnValueDelivered()),
            mApplication, SLOT(delayedExitApplication()));
    }
    else {
        // Make the previous view visible again
        if (mCurrentView) {
            mCurrentView->show();
            mCurrentView = NULL;
        }
    }
}

// End of file.
