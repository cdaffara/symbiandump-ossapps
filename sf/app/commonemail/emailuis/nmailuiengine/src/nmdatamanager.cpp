/*
* Copyright (c) 2009-10 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmuiengineheaders.h"
#include "nmdatapluginfactory.h"

/*!
    \class NmDataManager
    \brief The NmDataManager class provides access to abstract list models and meta data for
    mailboxes, folders and messages. If the dataplugin can't be loaded, fatal assert is called.

*/

/*!
    Constructor
*/
NmDataManager::NmDataManager() 
:mFactory(NULL)
{
    NM_FUNCTION;
    
    mFactory = NmDataPluginFactory::instance();
}

/*!
    Destructor
*/
NmDataManager::~NmDataManager()
{
    NM_FUNCTION;
    
    NmDataPluginFactory::releaseInstance(mFactory);
}

/*!
    list mailbox ids in \a mailboxIdList array
*/
void NmDataManager::listMailboxIds(QList<NmId> &mailboxIdList)
{
    NM_FUNCTION;
    
    int count(mFactory->pluginInstances()->count());
    for (int i(0); i < count; i++) {
        NmDataPluginInterface *instance =
            mFactory->interfaceInstance(mFactory->pluginInstances()->at(i));
        if (instance) {
            instance->listMailboxIds(mailboxIdList);
        }
    }
}

/*!
    Creates mailbox instances and fill \a mailboxList array,
    caller is responsible deletion of objects in the array
*/
void NmDataManager::listMailboxes(QList<NmMailbox*> &mailboxList)
{
    NM_FUNCTION;
    
    int count = mFactory->pluginInstances()->count();
    for (int i(0); i < count; i++) {
        NmDataPluginInterface *instance =
            mFactory->interfaceInstance(mFactory->pluginInstances()->at(i));
        if (instance) {
            instance->listMailboxes(mailboxList);
        }
    }
}

/*!
    Creates folder instances from mailbox folder, Mailbox is identified with
     \a mailboxId Function fills \a folderList array,
     caller is responsible deletion of objects in the array
*/
void NmDataManager::listFolders(const NmId mailboxId, QList<NmFolder*> &folderList)
{
    NM_FUNCTION;
    
    NmDataPluginInterface *instance = mFactory->interfaceInstance(mailboxId);
    if (instance) {
        instance->listFolders(mailboxId, folderList);
    }
}

/*!
    Creates message instances under folder which id is \a folderId
    Function fills \a messageList array, caller is responsible deletion of
    instances in the array
*/
void NmDataManager::listMessages(
        const NmId &mailboxId,
        const NmId &folderId,
        QList<NmMessageEnvelope*> &messageEnvelopeList)
{
    NM_FUNCTION;
    
    NmDataPluginInterface *instance = mFactory->interfaceInstance(folderId);
    if (instance) {
        instance->listMessages(mailboxId, folderId, messageEnvelopeList);
    }
}

/*!
    Returns folder meta data from mailbox which id is \a mailboxId.
    Ownership of NmFolderMetaData is transferred. NULL pointer is
    returned if folder by id is not found
*/
NmFolderMetaData *NmDataManager::folderById(
    const NmId &mailboxId,
    const NmId &folderId)
{
    NM_FUNCTION;
    
    NmFolderMetaData *folderMetaData(NULL);
    QList<NmFolder*> folderList;
    listFolders(mailboxId, folderList);
    const int folderListCount(folderList.count());
    for (int i(0); i < folderListCount; i++) {
        if (folderList[i]->folderId() == folderId) {
            // Folder by id found, fill in new metadata
            folderMetaData = new NmFolderMetaData();
            folderMetaData->setId(folderList[i]->folderId());
            folderMetaData->setName(folderList[i]->name());
            break;
        }
    }
    folderList.clear();
    return folderMetaData;
}

/*!
    Returns mailbox meta data. Ownership of NmMailboxMetaData is transferred.
    NULL pointer is returned if mailbox by id is not found
*/
NmMailboxMetaData *NmDataManager::mailboxById(const NmId &mailboxId)
{
    NM_FUNCTION;
    
    NmMailboxMetaData *mailboxMetaData(NULL);
    NmMailbox* mbox = mailbox(mailboxId);

    if (mbox) {
        mailboxMetaData = new NmMailboxMetaData();
        mailboxMetaData->setId(mbox->id());
        mailboxMetaData->setName(mbox->name());
        // Icon id should be set also
    }
    delete mbox;
    mbox = NULL;
    return mailboxMetaData;
}


/*!
    Get new NmMessageEnvelope object by id \a messageId from any folder.
    Ownership is transferred to the caller. NULL pointer is returned if
    message id is not found.
*/
NmMessageEnvelope *NmDataManager::envelopeById(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId)
{
    NM_FUNCTION;
    
    NmMessageEnvelope *msgEnvelope(NULL);
    NmMessage* msg = message(mailboxId, folderId, messageId);

    if (msg) {
    	msgEnvelope = new NmMessageEnvelope(msg->envelope());
    }
    delete msg;
    msg = NULL;
    return msgEnvelope;
}

/*!
    Get new NmMailbox object by id. Ownership is transferred to the caller.
    NULL pointer is returned if message id is not found from plugin.
*/
NmMailbox *NmDataManager::mailbox(const NmId &mailboxId)
{
    NM_FUNCTION;
    
    NmMailbox *newMailboxObject(NULL);
    NmDataPluginInterface *instance = mFactory->interfaceInstance(mailboxId);
    int retVal(NmNotFoundError);

    if (instance) {
        retVal = instance->getMailboxById(mailboxId, newMailboxObject);
    }

    if (retVal < NmNoError) {
         delete newMailboxObject;
         newMailboxObject = NULL;
         }

    return newMailboxObject;
}

/*!
    Get new NmFolder object by id. Ownership is transferred to the caller.
    NULL pointer is returned if message id is not found from plugin.
*/
NmFolder *NmDataManager::folder(
    const NmId &mailboxId,
    const NmId &folderId)
{
    NM_FUNCTION;
    
    NmFolder* newFolderObject(NULL);
    QList<NmFolder*> folderList;
    listFolders(mailboxId, folderList);
    for (int i(0); i < folderList.count(); i++) {
        if (folderList[i]->folderId() == folderId) {
            newFolderObject = new NmFolder(*folderList[i]);
            break;
        }
    }
    folderList.clear();
    return newFolderObject;
}

/*!
    Get new NmMessage object by id from any folder.
    Ownership is transferred to the caller. NULL pointer is returned if
    message id is not found from plugin.
*/
NmMessage *NmDataManager::message(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId)
{
    NM_FUNCTION;
    
    NmMessage *newMessageObject(NULL);
    NmDataPluginInterface *instance = mFactory->interfaceInstance(mailboxId);

    int retVal(NmNotFoundError);
    if (instance) {
        retVal = instance->
            getMessageById(mailboxId, folderId, messageId, newMessageObject);
        }

    if (retVal < NmNoError) {
        // Return null object if fetching failed
        delete newMessageObject;
        newMessageObject = NULL;
    }
    return newMessageObject;
}

/*!
    Get content to message part. Parameter messagePart must
    contain messagePartId.
*/
int NmDataManager::contentToMessagePart(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    NmMessagePart &messagePart)
{
    NM_FUNCTION;
    
    int retVal(NmNotFoundError);
    NmDataPluginInterface *instance = mFactory->interfaceInstance(mailboxId);
    if (instance) {
        retVal =
            instance->contentToMessagePart(mailboxId, folderId, messageId, messagePart);
    }
    return retVal;
}

/*!
	Function for getting standard folder id from plugin
*/
NmId NmDataManager::getStandardFolderId(
        const NmId &mailboxId,
        NmFolderType folderType )
{
    NM_FUNCTION;
    
    NmId folderId;
    NmDataPluginInterface *instance = mFactory->interfaceInstance(mailboxId);
    if (instance) {
        folderId =
            instance->getStandardFolderId(mailboxId, folderType);
    }
    return folderId;
}


/*!
    Returns folder type by id

    \param mailboxId The ID of the mailbox containing the folder

    \param folderId The ID of the folder 

    \return Folder type
*/
NmFolderType NmDataManager::folderTypeById(NmId mailboxId, NmId folderId)
{
    NM_FUNCTION;
    
    NmFolderType folderType(NmFolderOther);
    if (getStandardFolderId(mailboxId,NmFolderInbox)==folderId){
        folderType=NmFolderInbox;
    }
    else if (getStandardFolderId(mailboxId,NmFolderOutbox)==folderId){
        folderType=NmFolderOutbox; 
    }
    else if (getStandardFolderId(mailboxId,NmFolderDrafts)==folderId){
        folderType=NmFolderDrafts;
    }
    else if (getStandardFolderId(mailboxId,NmFolderSent)==folderId){
        folderType=NmFolderSent; 
    }    
    else if (getStandardFolderId(mailboxId,NmFolderDeleted)==folderId){
        folderType=NmFolderDeleted;  
    }    
    return folderType;
}
