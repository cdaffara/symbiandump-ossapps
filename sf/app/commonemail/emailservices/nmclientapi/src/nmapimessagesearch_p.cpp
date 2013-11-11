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
*       Email messages search private implementation.
*/ 

#include "nmapiheaders.h"

/*!
    \class NmApiMessageSearchPrivate
    \brief A Qt wrapper class for MFSMailBoxSearchObserver.
*/

/*!
    Class constructor.
*/
NmApiMessageSearchPrivate::NmApiMessageSearchPrivate(const quint64 mailboxId, QObject *parent) 
: QObject(parent), 
  mMailBox(NULL),
  mIsRunning(false),
  mEngine(NULL)
{
    NM_FUNCTION;
    NmId mailBoxId(mailboxId);
    mMailBoxId = TFSMailMsgId(mailBoxId);
    mEngine = NmApiEngine::instance();
}

/*!
    Class destructor.
*/
NmApiMessageSearchPrivate::~NmApiMessageSearchPrivate()
{
    NM_FUNCTION;
    
    mSearchStrings.ResetAndDestroy();

    if (isRunning()) {
        cancel();
    }

    if (mMailBox){
        delete mMailBox;
        mMailBox = NULL;
    }
    
    NmApiEngine::releaseInstance(mEngine);
}

/*!
 \fn isRunning 
 \return true if search operation is running otherwise returns false.
 
 Returns information about search operation running.
 */
bool NmApiMessageSearchPrivate::isRunning() const
{
    return mIsRunning;
}

/*!
 \fn start 
 \return true if operation was started otherwise returns false.
 
 Starts search operation for given criterias if operation is not running. Only one search can be performed at a time. 
 Each found message signaled with messageFound(NmApiMessage &message).
 Search completed signaled with searchComplete(int result).
 Cancellation signalled with canceled().
 */
bool NmApiMessageSearchPrivate::start()
{
    NM_FUNCTION;
    
    bool ret = false;

    if (!isRunning() && mSearchStrings.Count() > 0) {
        if (!mMailBox) {
            initialiseMailbox();
        }
        
        if (mMailBox) {
            TRAPD(err, mMailBox->SearchL(mSearchStrings, mSortCriteria, *this));
            if (err == KErrNone) {
                mIsRunning = true;
                ret = true;
            }
        }
    }
 
    return ret;
}

/*!
    From MFSMailBoxSearchObserver.
    Converts the ID of the received message into NmId and emits it with the
    signal notifying that a match was found.

    \param aMatchMessage A message that matches the search criteria. This class
                         takes the ownership of the given instance.
*/
void NmApiMessageSearchPrivate::MatchFoundL(CFSMailMessage *aMatchMessage)
{
    NM_FUNCTION;
    
    if (aMatchMessage) {
        NmMessage *nmmessage = aMatchMessage->GetNmMessage();
        EmailClientApi::NmApiMessage message = NmToApiConverter::NmMessage2NmApiMessage(*nmmessage);
        emit messageFound(message);
        
        delete nmmessage;
        nmmessage = NULL;
        
        delete aMatchMessage;
        aMatchMessage = NULL;
     }
}


/*!
    From MFSMailBoxSearchObserver.
    Emits a signal to notify that the search is complete.
*/
void NmApiMessageSearchPrivate::SearchCompletedL()
{
    NM_FUNCTION;
    mIsRunning = false;
    emit searchComplete(EmailClientApi::NmApiNoError); 
}


/*!
    From MFSMailBoxSearchObserver.
*/
void NmApiMessageSearchPrivate::ClientRequiredSearchPriority(
    TInt *apRequiredSearchPriority)
{
    Q_UNUSED(apRequiredSearchPriority);
}

/*!
 \fn cancel 
 
 Cancels current search operation. If operation is not running, nothing happens.  
 Cancellation signalled with canceled().
 */
void NmApiMessageSearchPrivate::cancel()
{
    NM_FUNCTION;
    
    if (isRunning()){
        if (mMailBox) {
            mMailBox->CancelSearch();
            mMailBox->ClearSearchResultCache();

            mSearchStrings.ResetAndDestroy();
            mSortCriteria.iField = EFSMailDontCare;
            mSortCriteria.iOrder = EFSMailAscending;
            mIsRunning = false;
            emit canceled();
        }
    }
}

/*!
 \fn initialise 
 \param searchStrings List of search criteria strings.
 \param sortCriteria Sort criteria for search.
 \return true if initializing is successfully completed otherwise returns false.
 
 Initialize search criterias.  
 */
bool NmApiMessageSearchPrivate::initialise(const QList<QString> &searchStrings, EmailClientApi::NmApiMailSortCriteria sortCriteria)
{
    NM_FUNCTION;
    bool ret = false;

    if (!isRunning() && searchStrings.count() > 0) {
        bool appendError = false;
        mSortCriteria.iField = TFSMailSortField(sortCriteria.iField);
        mSortCriteria.iOrder = TFSMailSortOrder(sortCriteria.iOrder);
       
        mSearchStrings.ResetAndDestroy();
        
        for (int i = 0; i < searchStrings.count() && !appendError; ++i) {
            HBufC *buffer = XQConversions::qStringToS60Desc(searchStrings.at(i));
            TRAPD(err, mSearchStrings.AppendL(buffer));
            if (err != KErrNone) {
                appendError = true;
            }
        }
        
        if (!appendError) {
            ret = true;
        }
        else {
            mSearchStrings.ResetAndDestroy();
        }
    }
    
    return ret;
}

/*!
 \fn initialiseMailbox 
 
 Initializes mailbox.  
 */
void NmApiMessageSearchPrivate::initialiseMailbox()
{
    NM_FUNCTION;
    RPointerArray<CFSMailPlugin> mailPlugins;
    if (mEngine) {
        mEngine->listMailPlugins(mailPlugins);

        for (int i = 0; i < mailPlugins.Count() && !mMailBox; i++){
            if (mailPlugins[i]->Id() == mMailBoxId.PluginId()) {
                TRAPD(err, mMailBox = mailPlugins[i]->GetMailBoxByUidL(mMailBoxId));
                Q_UNUSED(err);
            }
        }
    }
}
// End of file.
