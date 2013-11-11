/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: UI utilities class
*
*/

#include "nmuiheaders.h"

static const int NmMegabyte = 1048576;
static const qreal NmMinAttachmentSize = 0.1;
// taken from http://www.regular-expressions.info/email.html
static const QRegExp NmEmailAddressPattern("[A-Za-z\\d!#$%&'*+/=?^_`{|}~-]+"
                                         "(?:"
                                         "\\."
                                         "[A-Za-z\\d!#$%&'*+/=?^_`{|}~-]+"
                                         ")*"
                                         "@"
                                         "(?:"
                                         "[A-Za-z\\d]"
                                         "(?:"
                                         "[A-Za-z\\d-]*[A-Za-z\\d]"
                                         ")?"
                                         "\\."
                                         ")+"
                                         "[A-Za-z\\d]"
                                         "(?:"
                                         "[A-Za-z\\d-]*[A-Za-z\\d]"
                                         ")?"
                                        );

static const QRegExp NmEmailDisplayNamePattern("<"+NmEmailAddressPattern.pattern()+">$");
static const QRegExp NmEmailGreaterThenLessThen(">|<");
/*!
   Gets valid, invalid or all the recipients from a message
 */
void NmUtilities::getRecipientsFromMessage( const NmMessage &message,
    QList<NmAddress> &recipients,
    NmAddressValidationType type )
{
    NM_FUNCTION;

    // Get envelope from message
    const NmMessageEnvelope &env = message.envelope();
    
    // validate TO addresses
    QList<NmAddress> toRecipients = env.toRecipients();
    int recipientCount = toRecipients.count();

    for (int i = 0; i < recipientCount; ++i) {
        bool validAddress = isValidEmailAddress(toRecipients.at(i).address());

        if (type == NmDefault ||
            type == NmValidAddress && validAddress ||
            type == NmInvalidAddress && !validAddress) {
            recipients.append(toRecipients.at(i));
        }
    }

    // validate CC addresses
    QList<NmAddress> ccRecipients = env.ccRecipients();
    recipientCount = ccRecipients.count();

    for (int i = 0; i < recipientCount; ++i) {
        bool validAddress = isValidEmailAddress(ccRecipients.at(i).address());

        if (type == NmDefault ||
            type == NmValidAddress && validAddress ||
            type == NmInvalidAddress && !validAddress) {
            recipients.append(ccRecipients.at(i));
        }
    }

    // validate BCC addresses
    QList<NmAddress> bccRecipients = env.bccRecipients();
    recipientCount = bccRecipients.count();

    for (int i = 0; i < recipientCount; ++i) {
        bool validAddress = isValidEmailAddress(bccRecipients.at(i).address());

        if (type == NmDefault ||
            type == NmValidAddress && validAddress ||
            type == NmInvalidAddress && !validAddress) {
            recipients.append(bccRecipients.at(i));
        }
    }
}

/*!
  Validates the given string against an email address pattern.
*/
bool NmUtilities::isValidEmailAddress( const QString &emailAddress )
{
    NM_FUNCTION;

    return NmEmailAddressPattern.exactMatch(emailAddress);
}

/*!
  Generates a display string from an NmAddress object.
*/
QString NmUtilities::addressToDisplayName( const NmAddress &address )
{
    NM_FUNCTION;

    QString emailAddress = address.address();
    QString displayName = address.displayName();

    QString ret;
    if (displayName.length() > 0 && displayName != emailAddress) {
        ret = displayName + " <" + emailAddress + ">";
    }
    else {
        ret = emailAddress;
    }
    return ret;
}

/*!
  Returns an NmAddress object that is parsed from a display name string.
*/
bool NmUtilities::parseEmailAddress( const QString &emailAddress, NmAddress &address )
{
    NM_FUNCTION;

    bool foundAddress(false);

    QRegExp rx(NmEmailAddressPattern);
    // locate the email address in the string
    int pos = rx.indexIn(emailAddress);
    if (pos != -1) {
        // extract the email address...
        int matchedLen = rx.matchedLength();
        QString addr = emailAddress.mid(pos, matchedLen);
        address.setAddress(addr);

        // ...and use the rest as display name
        QString name = emailAddress.left(pos) + emailAddress.mid(pos + matchedLen);
        address.setDisplayName(cleanupDisplayName(name));

        foundAddress = true;
    }

    return foundAddress;
}

/*!
  Cleans up display name by stripping extra characters from the beginning and end of the string.
*/
QString NmUtilities::cleanupDisplayName( const QString &displayName )
{
    NM_FUNCTION;

    // find the first and last position that is NOT one of the characters below
    QRegExp rx("[^\\s\"<>]");
    int firstPos = std::max(rx.indexIn(displayName), 0);
    int lastPos = rx.lastIndexIn(displayName);

    if (lastPos < 0) {
        lastPos = displayName.length() - 1;
    }

    return displayName.mid(firstPos, lastPos - firstPos + 1);
}

/*!
  Opens file specified by XQSharableFile handle. Usually used by viewer
  for opening attachments from message store as RFiles
*/
int NmUtilities::openFile(XQSharableFile &file, QObject* listener)
{
    NM_FUNCTION;

    int ret(NmNotFoundError);
    XQApplicationManager aiwMgr;
    XQAiwRequest *request(NULL);
    request = aiwMgr.create(file);
    // Create request for the sharable file
    if (request)
    {
        QObject::connect(request, SIGNAL(requestOk(const QVariant&)),
                            listener, SLOT(fileOpenCompleted(const QVariant&)));
        QObject::connect(request, SIGNAL(requestError(int, const QString&)),
                                    listener, SLOT(fileOpenError(int, const QString&)));
        // Set request arguments
        QList<QVariant> args;
        args << qVariantFromValue(file);
        request->setArguments(args);
        request->setSynchronous(false);
        // Send the request, ownership of request is transferred
        bool res = request->send();
        if (res) {
        // Request ok, set error status.
        ret = NmNoError;
        }
    }
    return ret;
}

/*!
   Opens an attachment part via QtHighway. Message part object that is passed
   to this function must contain binary content, because it is written to a
   temporary file. List passed is appended with the new temporary filename
   that is opened. This can be then used to delete the temporary files created
   with for example NmUtilities::deleteTempFiles function.
   
   \param part attachment message part which should contain binary content.
   \param tmpFiles list of filenames that is appended with new one.
   \param listener XQAiwRequest listener for listening request send completion
   \retun int NmGeneralError, if the above requirements aren't filled. Also
              if file type isn't supported.
*/
int NmUtilities::openAttachment(NmMessagePart *part, 
                                QList<QString> &tmpFiles,
                                QObject* listener)
{
    int ret(NmGeneralError);

    if (part) {
        // Create a temp file that points to system temp folder.
        QFile file(QDir::tempPath()+QDir::separator()+part->attachmentName());
            
        if (file.open(QIODevice::ReadWrite)) {
            // Write content to file and close it so it can be used by other
            // processes.
            if (file.write(part->binaryContent()) > 0) {
                file.close();
                
                // Create a request from the file.
                XQApplicationManager aiwMgr;
                XQAiwRequest *request = aiwMgr.create(file);
                if (request)
                {
                    QObject::connect(request, SIGNAL(requestOk(const QVariant&)),
                                        listener, SLOT(fileOpenCompleted(const QVariant&)));
                    QObject::connect(request, SIGNAL(requestError(int, const QString&)),
                                        listener, SLOT(fileOpenError(int, const QString&)));
                    // Set request argument.
                    QList<QVariant> args;
                    args << file.fileName();
                    request->setArguments(args);
                    if (listener) {
                        request->setSynchronous(false);
                    }
                    // Send the request
                    bool res = request->send();
                    if (res) {
                        // Request ok, set error status.
                        ret = NmNoError;
                    }
                }
                if (!listener) {
                    delete request;    
                }
            }
            tmpFiles.append(file.fileName());
        }
    }
    return ret;
}

/*
 * Deletes files represented in the given list. Also clears
 * the list after deletion.
 * 
 * \param tmpFiles list of files to be removed.
 */
void NmUtilities::deleteTempFiles(QList<QString> &tmpFiles)
{
    foreach (QString fileName,tmpFiles) {
        QFile::remove(fileName);
    }
    tmpFiles.clear();
}

/*!
 * Truncate a string to a specific length. If length is less than
 * the string, ellipses are added to the end of the string.
 */
QString NmUtilities::truncate( const QString &string, int length )
{
    NM_FUNCTION;

    if (string.length() <= length) {
        return string;
    }

    QString padding = "...";

    return string.mid(0, length - padding.length()) + padding;
}

/*!
 * Shows an error note. Used by at least editor and viewer classes.
 *
 */
void NmUtilities::displayErrorNote(QString noteText)
{
    NM_FUNCTION;

	HbNotificationDialog *note = new HbNotificationDialog();

	note->setIcon(HbIcon(QLatin1String("note_warning")));
	note->setTitle(noteText);
	note->setTitleTextWrapping(Hb::TextWordWrap);
	note->setDismissPolicy(HbPopup::TapAnywhere);
	note->setAttribute(Qt::WA_DeleteOnClose);
	note->setSequentialShow(false);

	note->show();
}

/*!
    Function returns localized attachment size string based
    on attachment size in bytes
 */
QString NmUtilities::attachmentSizeString(const int sizeInBytes)
{
    NM_FUNCTION;

    qreal sizeMb = (qreal)sizeInBytes / (qreal)NmMegabyte;
    if (sizeMb < NmMinAttachmentSize) {
        // NmMinAttachmentSize (0.1Mb) is the minimum size shown for attachment
        sizeMb = NmMinAttachmentSize;
    }   
    return HbStringUtil::convertDigits(hbTrId("txt_mail_list_l1_mb").arg(sizeMb, 0, 'f', 1));
}

/*!
    Displays a note with Yes/No buttons. Note has no timeout, i.e. it has to be dismissed manually.
    Returns pointer to dialog so that caller can take ownership and handle deletion.
    Parameter 'receiver' is the object and 'member' is the slot where user selection is passed.
*/
HbMessageBox* NmUtilities::displayQuestionNote(
    QString noteText, QObject* receiver, const char* member)
{
    NM_FUNCTION;

    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    messageBox->setStandardButtons(HbMessageBox::Yes | HbMessageBox::No);  
    messageBox->setText(noteText);
    messageBox->setTimeout(HbMessageBox::NoTimeout); // Note has to be dismissed manually
    messageBox->open(receiver, member);
    return messageBox;
}

/*!
 * displays an warning note.
 */
HbMessageBox* NmUtilities::displayWarningNote(QString noteText, QObject* receiver, const char* member)
{
    NM_FUNCTION;

    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeWarning);
    messageBox->setText(noteText);
    messageBox->setTimeout(HbMessageBox::NoTimeout); // Note has to be dismissed manually
    messageBox->open(receiver, member);
    return messageBox;
}

/*!
    Function returns localized "Original message" header
    in html format based on envelope
*/
QString NmUtilities::createReplyHeader(const NmMessageEnvelope &env)
{
    NM_FUNCTION;

    QString ret = "<html><body>";
    // Two empty lines before reply header.
    ret+="<br><br>";
    // Append "----- Original message ----" text
    ret+=hbTrId("txt_mail_editor_reply_original_msg");
    // Append sender
    ret+="<br>";
    ret+=hbTrId("txt_mail_editor_reply_from");
    ret+=" ";
    if (env.sender().displayName().length()){
        ret+=env.sender().displayName();
    }
    else{
        ret+=env.sender().address();
    }
    // Append sent time
    ret+="<br>";
    ret+=hbTrId("txt_mail_editor_reply_sent");
    ret+=" ";
    HbExtendedLocale locale = HbExtendedLocale::system();
    QDateTime localTime = env.sentTime().addSecs(locale.universalTimeOffset());
    QDate sentLocalDate = localTime.date();
    ret+=locale.format(sentLocalDate, r_qtn_date_usual);
    // Append to recipients
    const QList<NmAddress> &toList = env.toRecipients();
    if (toList.count()){
        ret+="<br>";
        ret+=hbTrId("txt_mail_editor_reply_to");
        ret+=" ";
        for (int i=0;i<toList.count();i++){
            if (toList[i].displayName().length()){
                ret+=toList[i].displayName();
            }
            else{
                ret+=toList[i].address();
            }
            if (i!=toList.count()-1){
                ret+=";";
            }
        }
    }
    // Append cc recipients
    const QList<NmAddress> &ccList = env.ccRecipients();
    if (ccList.count()){
        ret+="<br>";
        ret+=hbTrId("txt_mail_editor_reply_cc");
        ret+=" ";
        for (int i=0;i<ccList.count();i++){
            if (ccList[i].displayName().length()){
                ret+=ccList[i].displayName();
            }
            else{
                ret+=ccList[i].address();
            }
            if (i!=toList.count()-1){
                ret+=";";
            }
        }
    }
    // Append subject if there is subject to display
    if (env.subject().length()){
        ret+="<br>";
        ret+=hbTrId("txt_mail_editor_reply_subject");
        ret+=" ";
        ret+=env.subject();
    }
    // Append priority if it is other than normal
    if (env.priority()!=NmMessagePriorityNormal){
        ret+="<br>";
        ret+=hbTrId("txt_mail_editor_reply_importance");
        ret+=" ";
        if (env.priority()==NmMessagePriorityLow){
            ret+=hbTrId("txt_mail_editor_reply_importance_low");
        }
        else {
            ret+=hbTrId("txt_mail_editor_reply_importance_high");
        }
    }
    ret+="<br></body></html>";
    return ret;
}

/*
 * Function creates NmAddress object from a string, which can
 * be just an email address or display name and email address in format
 * Firstname Lastname <firstname.lastname@example.com>.
 * \param str email address string
 * \return NmAddress with address and possible display name 
 */
NmAddress *NmUtilities::qstringToNmAddress(QString str)
{
    QRegExp NmEmailDisplayNamePattern("<"+NmEmailAddressPattern.pattern()+">$");
    NmAddress *nmAddress = NULL;
    str = str.simplified();
    if (str.contains(NmEmailDisplayNamePattern)) {
        QString original = str;
        QString displayName = str.remove(NmEmailDisplayNamePattern).trimmed();
        QRegExp NmEmailGreaterThenLessThen(">|<");
        QString address = original.remove(displayName).remove(NmEmailGreaterThenLessThen).trimmed();
        nmAddress = new NmAddress(displayName,address);
    }
    else {
        nmAddress = new NmAddress(str);
    }
    return nmAddress;
}

/*
 * Function creates a list of NmAddress objects from a string list, that contains email addresses.
 * \param const QStringList &strlist list of email address strings
 * \return QList<NmAddress*> *NmAddresses list of NmAddress objects. Returns NULL if strlist does
 * not contain strings.
 */
QList<NmAddress*> *NmUtilities::qstringListToNmAddressList(const QStringList &strlist)
{
    QList<NmAddress*> *addresses = NULL;
    if(!strlist.isEmpty()) {
        addresses = new QList<NmAddress*>;
        QString tempString;
        foreach(tempString, strlist) {
            NmAddress *address = new NmAddress(tempString);
            addresses->append(address);
        }
    }
    return addresses;
}
