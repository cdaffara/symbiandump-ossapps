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
    \class NmStartParamDataHelper
    \brief A helper class for processing the data given to the actual service.
*/
class NmStartParamDataHelper
{
public:

    /*!
        Class constructor.
    */
    inline NmStartParamDataHelper()
    : mSubject(NULL),
      mToAddresses(NULL),
      mCcAddresses(NULL),
      mBccAddresses(NULL),
      mAttachmentList(NULL),
      mEditorStartMode(NmUiEditorCreateNew),
	  mBodyText(NULL),
	  mMimeType(NULL)
    {
        NM_FUNCTION;
    }

    /*!
        Class destructor.
    */
    inline ~NmStartParamDataHelper()
    {
        NM_FUNCTION;
    }

    /*!
        Extracts the data from the given QVariant into the class members.
        \param data QVariant containing the data.
        \return True if success, false otherwise.
    */
    inline bool extractData(const QVariant &data)
    {
        NM_FUNCTION;

        bool success(false);

        if (data.canConvert(QVariant::Map)) {
            // The given data may contain a mail subject and recipient lists.
            QMap<QString, QVariant> map = data.toMap();
            success = processMap(map);
        }
        else if (data.canConvert(QVariant::StringList)) {
            QStringList attachmentList = data.toStringList();
            mAttachmentList = new QStringList(attachmentList);
            success = true;
        }
        else {
            // Data type not supported!
            NM_ERROR(1,"NmStartParamDataHelper::extractData(): data type not supported");
        }

        // Determine the editor start mode.
        if (mToAddresses || mCcAddresses || mBccAddresses) {
            mEditorStartMode = NmUiEditorMailto;
        }
        else if (mAttachmentList) {
            mEditorStartMode = NmUiEditorCreateNew;
        }

        return success;
    }

    /*!
        Deletes the class members. Must be used if NmUiStartParam does not
        take ownership of the members.
    */
    inline void deleteData()
    {
        NM_FUNCTION;

        delete mSubject;
        mSubject = NULL;

        if (mToAddresses) {
            qDeleteAll(*mToAddresses);
            delete mToAddresses;
            mToAddresses = NULL;
        }

        if (mCcAddresses) {
            qDeleteAll(*mCcAddresses);
            delete mCcAddresses;
            mCcAddresses = NULL;
        }

        if (mBccAddresses) {
            qDeleteAll(*mBccAddresses);
            delete mBccAddresses;
            mBccAddresses = NULL;
        }

        delete mAttachmentList;
        mAttachmentList = NULL;

        delete mBodyText;
        mBodyText = NULL;
		
		delete mMimeType;
		mMimeType = NULL;
    }


private:

    /*!
        Extracts the data from the given map into the class members.
        \param map The map to extract the data from.
        \return True if success, false otherwise.
    */
    inline bool processMap(const QMap<QString, QVariant> &map)
    {
        NM_FUNCTION;

        QMap<QString, QVariant>::const_iterator i = map.constBegin();
        QString key;
        QVariant value;

        while (i != map.constEnd()) {
            key = i.key();
            value = i.value();

            if (!key.compare(emailSendSubjectKey, Qt::CaseInsensitive) &&
                value.type() == QVariant::String) {
                // Extract the mail subject.
                if (mSubject) {
                    // A subject string has already been extracted! Discard the
                    // old subject.
                    delete mSubject;
                }

                mSubject = new QString(value.toString());
            }
            else if (!key.compare(emailSendToKey, Qt::CaseInsensitive)) {
                // Extract the "to" recipients.
                addAddressesToList(value, &mToAddresses);
            }
            else if (!key.compare(emailSendCcKey, Qt::CaseInsensitive)) {
                // Extract the "cc" recipients.
                addAddressesToList(value, &mCcAddresses);
            }
            else if (!key.compare(emailSendBccKey, Qt::CaseInsensitive)) {
                // Extract the "bcc" recipients.
                addAddressesToList(value, &mBccAddresses);
            }
            else if (!key.compare(emailSendBodyTextKey, Qt::CaseInsensitive)) {
                // Make sure only the last bodytext is used
                delete mBodyText;
                mBodyText = NULL;
                // Extract the "body" text
                mBodyText = new QString(value.toString());
            }
            else if (!key.compare(emailSendAttachmentKey, Qt::CaseInsensitive)) {
                // Extract the "attachment" part
                if(!mAttachmentList) {
				    mAttachmentList = new QStringList();
				}
				mAttachmentList->append(value.toStringList());
            }
            else if (!key.compare(emailSendBodyMimeTypeKey, Qt::CaseInsensitive)) {
			    delete mMimeType;
				mMimeType = NULL;
                // Extract the "mime type" part
				mMimeType = new QString(value.toString());
            }

            ++i;
        }

        // In principle it is ok even if the map does not contain any data e.g.
        // in a case where the editor view needs to be opened without any
        // predefined data. Therefore, we always return true.
        return true;
    }

    /*!
        Appends the given addresses into the given list.
        \param address The addresses to append.
        \param list The list where the addresses are appended to.
    */
    inline void addAddressesToList(const QVariant &addresses,
                                   QList<NmAddress*> **list)
    {
        NM_FUNCTION;

        if (!list) {
            // Invalid argument!
            return;
        }

        QVariant::Type dataType = addresses.type();
        QList<NmAddress*> foundAddresses;

        switch (dataType) {
            case QVariant::String: {
                // A single address.
                NmAddress *address = NmUtilities::qstringToNmAddress(addresses.toString());
                foundAddresses.append(address);
                break;
            }
            case QVariant::StringList: {
                // A list of addresses.
                QStringList addressList = addresses.toStringList();

                foreach (QString addressAsString, addressList) {
                    NmAddress *address = NmUtilities::qstringToNmAddress(addressAsString);
                    foundAddresses.append(address);
                }

                break;
            }
            default: {
                break;
            }
        }

        if (foundAddresses.count()) {
            // Append the found addresses into the given list.
            if (!(*list)) {
                *list = new QList<NmAddress*>();
            }

            (*list)->append(foundAddresses);
        }
    }


public: // Data

    QString *mSubject; // Not owned.
    QList<NmAddress*> *mToAddresses; // Not owned.
    QList<NmAddress*> *mCcAddresses; // Not owned.
    QList<NmAddress*> *mBccAddresses; // Not owned.
    QStringList *mAttachmentList; // Not owned.
    NmUiEditorStartMode mEditorStartMode;
    QString *mBodyText;
    QString *mMimeType;
};



/*!
    \class NmSendServiceInterface
    \brief NMail application service interface which provides an email sending
           interface for other application using the Qt Highway.
*/

/*!
    Class constructor.
*/
NmSendServiceInterface::NmSendServiceInterface(QString interfaceName,
                                               QObject *parent,
                                               NmUiEngine &uiEngine,
                                               NmApplication *application)
    : XQServiceProvider(interfaceName, parent),
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
NmSendServiceInterface::~NmSendServiceInterface()
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
void NmSendServiceInterface::selectionDialogClosed(NmId &mailboxId)
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
    Used for sending email messages from external applications. Used by the
    Share UI. In addition, the interface can be used to send email to a contact
    from another application. The method sets the given data (e.g. recipients,
    attachments) to a mail composer (editor) view and displays it.

    \param data If used by Share UI, will contain the list of filenames to
                attach. Can also contain a map with key value pairs containing
                subject and recipient data.
*/
void NmSendServiceInterface::send(QVariant data)
{
    NM_FUNCTION;

    HbMainWindow *mainWindow(NULL);

    // Make sure that qmail stays background if user presses back in editorview
    if (mApplication) {
        mApplication->updateVisibilityState();

        mainWindow = mApplication->mainWindow();
        mCurrentView = mainWindow->currentView();

        // Hide the current view.
        if (mCurrentView) {
            mCurrentView->hide();
        }
    }

    // Check the given data.
    NmStartParamDataHelper dataHelper;
    bool validData = dataHelper.extractData(data);

    NmMailboxListModel &mailboxListModel = mUiEngine.mailboxListModel();
    const int count = mailboxListModel.rowCount();
    NmId mailboxId(0);

    mAsyncReqId = setCurrentRequestAsync();

    if (!validData) {
        // Failed to extract the data!
        NM_ERROR(1,"NmSendServiceInterface::send(): failed to process the given data");
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

    	mStartParam = new NmUiStartParam(
        	NmUiViewMessageEditor,
	        0, // account id
	        0, // folder id
    	    0, // message id
        	dataHelper.mEditorStartMode, // editor start mode
	        dataHelper.mToAddresses, // address list
    	    dataHelper.mAttachmentList, // attachment list
        	true, // start as service
	        dataHelper.mSubject, // message subject
	        dataHelper.mCcAddresses, // list containing cc recipient addresses
    	    dataHelper.mBccAddresses, // list containing bcc recipient addresses
    	    dataHelper.mBodyText, // body text
    	    dataHelper.mMimeType // body text mime type
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

            connect(mSelectionDialog, SIGNAL(selectionDialogClosed(NmId&)),
                    this, SLOT(selectionDialogClosed(NmId&)));
            mSelectionDialog->open();

            // launch the editor when the dialog is closed
        }
    }
}

/*!
    Called when mailbox id is know and editor can be opened
    \param mailboxId mailbox using in editor
 */
void NmSendServiceInterface::launchEditorView(NmId mailboxId)
{
    NM_FUNCTION;
    NM_COMMENT(QString("NmSendServiceInterface::launchEditorView(): mailboxId=%1").arg(mailboxId.id()));

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

void NmSendServiceInterface::cancelService()
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
