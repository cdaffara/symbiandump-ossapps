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
 *     Email message related operations
 */
 
#include "nmapiheaders.h"


namespace EmailClientApi
{

/*!
    Class constructor.
*/
NmApiMessageManagerPrivate::NmApiMessageManagerPrivate(QObject *parent,quint64 mailboxId)
: QObject(parent),
  mMailBoxId(0),
  mMailBox(NULL),
  mMailPlugin(NULL),
  mEngine(NULL)
{
    NM_FUNCTION;
    NmId mailBoxId(mailboxId);
    mMailBoxId = TFSMailMsgId(mailBoxId);
    mEngine = NmApiEngine::instance();
    initialise();
}

/*!
    Class destructor.
*/
NmApiMessageManagerPrivate::~NmApiMessageManagerPrivate()
{
    NM_FUNCTION;
    
    if (mMailBox){
        delete mMailBox;
        mMailBox = NULL;
    }
    
    NmApiEngine::releaseInstance(mEngine);
}

/*!
 \fn createDraftMessage 
 \param initData Initializing data. Can contain NULL value.
 \return Create draft message operation.
 
 Returns create draft message operation.
 */
QPointer<NmApiOperation> NmApiMessageManagerPrivate::createDraftMessage(const QVariant *initData)
{
    NM_FUNCTION;
    if (mMailBox){
        QPointer<NmApiOperation> operation = new NmApiOperationCreateDraftMessage(initData, *mMailBox, this);
        return operation;
    }
    return NULL;
}

/*!
 \fn sendMessage 
 \param message message to be sent.
 \return Send message operation.
 
 Returns Send message operation.
 */
QPointer<NmApiOperation> NmApiMessageManagerPrivate::sendMessage(const NmApiMessage &message)
{
    NM_FUNCTION;
    if (mMailBox){
        QPointer<NmApiOperation> operation = new NmApiOperationSendMessage(message, *mMailBox, this);
        return operation;
    }
    return NULL;
}

/*!
 \fn saveMessage 
 \param message Message to be saved.
 \return Save message operation.
 
 Saves email message. Returns save message operation. 
 */
QPointer<NmApiOperation> NmApiMessageManagerPrivate::saveMessage(const NmApiMessage &message)
{
    return NULL;
}

/*!
 \fn initialise
 
 Initializes mailbox and mailplugin.
 */
void NmApiMessageManagerPrivate::initialise()
{
    NM_FUNCTION;
    RPointerArray<CFSMailPlugin> mailPlugins;
    if (mEngine) {
        mEngine->listMailPlugins(mailPlugins);
    
        for (int i = 0; i < mailPlugins.Count() && !mMailPlugin; i++){
            if (mailPlugins[i]->Id() == mMailBoxId.PluginId()) {
                mMailPlugin = mailPlugins[i];
            }
        }
        
        if (mMailPlugin) {
            TRAPD(err, mMailBox = mMailPlugin->GetMailBoxByUidL(mMailBoxId));
            Q_UNUSED(err);
        }
    }
}

} //namespace


