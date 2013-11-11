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
 * Description:Message send functionality handler.
 *
 */

#include "msgsendutil.h"
#include "debugtraces.h"
// plugin loader and plugin interface
#include "unieditorpluginloader.h"
#include "unieditorplugininterface.h"
// general editor utilities
#include "UniEditorGenUtils.h"

#include <xqconversions.h>
#include <commonphoneparser.h> // commonengine

//---------------------------------------------------------------
// MsgSendUtil::MsgSendUtil
// Constructor
//--------------------------------------------------------------
MsgSendUtil::MsgSendUtil(QObject* parentObject) :
    QObject(parentObject), mPluginLoader(0), mUniEditorGenUtils(0)
    {
    // Load Plugins for send
    mPluginLoader = new UniEditorPluginLoader(this);
    mUniEditorGenUtils = q_check_ptr(new UniEditorGenUtils());
    }

//---------------------------------------------------------------
// MsgSendUtil::~MsgSendUtil
// Desctructor
//--------------------------------------------------------------
MsgSendUtil::~MsgSendUtil()
    {
    if( mUniEditorGenUtils)
        {
        delete mUniEditorGenUtils;
        mUniEditorGenUtils = NULL;
        }
    }

//---------------------------------------------------------------
// MsgSendUtil::saveToDrafts
// @see header file
//--------------------------------------------------------------
TMsvId MsgSendUtil::saveToDrafts(ConvergedMessage& msg)
    {
    QDEBUG_WRITE("START MsgSendUtil::saveToDraft");
    bool emailOverSmsSupported = false;
    TMsvId id = -1;

    if( checkEmailOverSms(msg, emailOverSmsSupported)
        && checkMaxRecipientCount(msg)
        && checkMaxMsgSizeLimit(msg) )
        {
        UniEditorPluginInterface* pluginInterface =
                        mPluginLoader->getUniEditorPlugin(msg.messageType());
         id = pluginInterface->convertTo(&msg);
        }
    QDEBUG_WRITE("END MsgSendUtil::saveToDraft");
    return id;
    }

//---------------------------------------------------------------
// MsgSendUtil::send
// @see header file
//--------------------------------------------------------------
int MsgSendUtil::send(ConvergedMessage& msg)
{
    QDEBUG_WRITE("START MsgSendUtil::send");
    int retValue = KErrGeneral;
    bool emailOverSmsSupported = false;
    long int id = saveToDrafts(msg);
    if(id != -1)
    {
        checkEmailOverSms(msg, emailOverSmsSupported);
        UniEditorPluginInterface* pluginInterface =
            mPluginLoader->getUniEditorPlugin(msg.messageType());
        if( validateService(pluginInterface, emailOverSmsSupported) )
        {
            pluginInterface->send(id);
            retValue = KErrNone;
        }
        else
        {
            //Delete the entry created in drafts as we are switching to settings
            pluginInterface->deleteDraftsEntry(id);
            retValue = KErrNotFound;
        }
    }

    QDEBUG_WRITE("END MsgSendUtil::send");
    return retValue;
}

//---------------------------------------------------------------
// MsgSendUtil::isValidAddress
// @see header file
//--------------------------------------------------------------
bool MsgSendUtil::isValidAddress(const QString &address)
{
    bool isValid = false;
    if (!address.isEmpty()) {

        HBufC *tempAddr = XQConversions::qStringToS60Desc(address);

        // 1. perform number validation
        isValid = CommonPhoneParser::IsValidPhoneNumber(*tempAddr, CommonPhoneParser::ESMSNumber);

        // 2. if number validity fails, then perform email addr validation
        if (!isValid) {
            // additional check for MMS only
            UniEditorGenUtils* genUtils = q_check_ptr(new UniEditorGenUtils);
            isValid = genUtils->IsValidEmailAddress(*tempAddr);
            delete genUtils;
        }
        delete tempAddr;
    }
    return isValid;
}

//---------------------------------------------------------------
// MsgSendUtil::checkEmailOverSms
// @see header file
//--------------------------------------------------------------
bool MsgSendUtil::checkEmailOverSms(ConvergedMessage& msg, bool& emailOverSmsSupported)
    {
    QDEBUG_WRITE("START MsgSendUtil::checkEmailOverSms");
    if (msg.messageType() == ConvergedMessage::Sms)
        {
        bool emailAddrPresent = false;
        TRAPD(err,emailAddrPresent =
                    mUniEditorGenUtils->VerifyEmailAddressesL(msg.toAddressList()));
        if (err == KErrNone && emailAddrPresent)
            {
            TRAPD(err, emailOverSmsSupported = mUniEditorGenUtils->AcceptEmailAddressesL());
            if (err == KErrNone && !emailOverSmsSupported)
                {
                //TODO if the note is to be shown or if we have to convert the msg type
                msg.setMessageType(ConvergedMessage::Mms);
                }
            }
        }
    // if control is here, then checks are ok
    QDEBUG_WRITE("END MsgSendUtil::checkEmailOverSms");
    return true;
    }

//---------------------------------------------------------------
// MsgSendUtil::checkMaxRecipientCount
// @see header file
//--------------------------------------------------------------
bool MsgSendUtil::checkMaxRecipientCount(ConvergedMessage& msg)
    {
    QDEBUG_WRITE("START MsgSendUtil::checkMaxRecipientCount");
    bool result = true;
    // max recipient count check for sms
    if (msg.messageType() == ConvergedMessage::Sms)
        {
        int maxSmsRecipientsCount = KDefaultSmsRecipients;
        TRAP_IGNORE(maxSmsRecipientsCount = mUniEditorGenUtils->MaxSmsRecipientsL());
        if (msg.toAddressList().count() > maxSmsRecipientsCount)
            {
            //TODO show a note to user to delete some recipients
            //TODO or convert msg to MMS ??
            result = false;
            }
        }

    // max recipient count check for mms
    if (msg.messageType() == ConvergedMessage::Mms)
        {
        int maxMmsRecipientsCount = KDefaultMmsRecipients;
        TRAP_IGNORE(maxMmsRecipientsCount = mUniEditorGenUtils->MaxMmsRecipientsL());
        if ( ( msg.toAddressList().count() 
             + msg.ccAddressList().count()
             + msg.bccAddressList().count() ) > maxMmsRecipientsCount )
            {
            //TODO show a note to user to delete some recipients
            result = false;
            }
        }
    // if control is here, then all checks are ok
    QDEBUG_WRITE("END MsgSendUtil::checkMaxRecipientCount");
    return result;
    }

//---------------------------------------------------------------
// MsgSendUtil::checkMaxMsgSizeLimit
// @see header file
//--------------------------------------------------------------
bool MsgSendUtil::checkMaxMsgSizeLimit(ConvergedMessage& msg)
    {
    QDEBUG_WRITE("START MsgSendUtil::checkMaxMsgSizeLimit");
    bool result = true;
    // msg size limit check for sms
    if (msg.messageType() == ConvergedMessage::Sms)
        {
        int maxSmsMsgSize = KFirstNormalSmsLength;
        TRAP_IGNORE(maxSmsMsgSize = mUniEditorGenUtils->MaxSmsMsgSizeL());        
        if (getSmsMsgSize(msg) > maxSmsMsgSize)
            {
            // TODO: Msg size exceeded. Either convert to MMS?? or stall.
            result = false;
            }
        }

    // msg size limit check for mms
    if (msg.messageType() == ConvergedMessage::Mms)
        {
        int maxMmsMsgSize = KDefaultMaxSize;
        TRAP_IGNORE(maxMmsMsgSize = mUniEditorGenUtils->MaxMmsMsgSizeL());
        if (getMmsMsgSize(msg) > maxMmsMsgSize)
            {
            // TODO: Show note and then give chance to user to delete content
            result = false;
            }
        }
    // if control is here, then all checks are ok
    QDEBUG_WRITE("END MsgSendUtil::checkMaxMsgSizeLimit");
    return result;
    }

//---------------------------------------------------------------
// MsgSendUtil::getSmsMsgSize
// @see header file
//--------------------------------------------------------------
int MsgSendUtil::getSmsMsgSize(ConvergedMessage& msg)
    {
    QDEBUG_WRITE("START MsgSendUtil::getSmsMsgSize");
    int totalMsgSize = 0;
    totalMsgSize += mUniEditorGenUtils->UTF8Size(msg.bodyText()) +
                    mUniEditorGenUtils->UTF8Size(msg.subject());
    bool ret = false;
    TRAP_IGNORE(ret = mUniEditorGenUtils->AcceptEmailAddressesL());
    if (ret)
        {
        int emailAddrLen = longestEmailAddressSize(msg.toAddressList());
        if (emailAddrLen > 0)
            {
            // 1 char is needed by recipient and subject separator
            totalMsgSize += emailAddrLen + 1;
            }
        }
    //TODO: attachment size calculation are missing (e.g vCard/vCal etc)
    
    QDEBUG_WRITE("END MsgSendUtil::getSmsMsgSize");
    return totalMsgSize;
    }

//---------------------------------------------------------------
// MsgSendUtil::longestEmailAddressSize
// @see header file
//--------------------------------------------------------------
int MsgSendUtil::longestEmailAddressSize(ConvergedMessageAddressList addrList)
    {
    int count = addrList.count();
    int longestEmailSize = 0;
    for(int i=0; i<count; i++)
        {
        longestEmailSize = qMax(longestEmailSize, addrList.at(i)->address().length());
        }
    return longestEmailSize;
    }

//---------------------------------------------------------------
// MsgSendUtil::getMmsMsgSize
// @see header file
//--------------------------------------------------------------
int MsgSendUtil::getMmsMsgSize(ConvergedMessage& msg)
    {
    QDEBUG_WRITE("START MsgSendUtil::getMmsMsgSize");
    int totalMsgSize = 0;
    int textSize = mUniEditorGenUtils->UTF8Size(msg.bodyText());
    totalMsgSize += textSize +
    mUniEditorGenUtils->UTF8Size(msg.subject());
    ConvergedMessageAttachmentList attList = msg.attachments();
    int attCount = attList.count();
    bool hasInlineContent = false;

    for (int i=0; i<attCount; i++)
    {
        if(attList.at(i)->attachmentType() == 
            ConvergedMessageAttachment::EInline)
        {
            hasInlineContent = true;
        }
        int attSize;
        QString mimeType;
        TMsgMediaType mediaType = EMsgMediaUnknown;

        attSize = 0;
        TRAP_IGNORE(mUniEditorGenUtils->getFileInfoL(
            attList.at(i)->filePath(),
            attSize, mimeType, mediaType));
        totalMsgSize += attSize;
    }

    if( hasInlineContent && textSize > 0 )
    {    
        // attachments are present, so we add our estimates
        totalMsgSize += KEstimatedMmsSmilHeaderSize + KEstimatedMimeHeaderSize;
    }
    QDEBUG_WRITE("END MsgSendUtil::getMmsMsgSize");
    return totalMsgSize;
    }

//---------------------------------------------------------------
// MsgSendUtil::validateService
// @see header file
//--------------------------------------------------------------
bool MsgSendUtil::validateService(
        UniEditorPluginInterface* pluginInterface,
        bool emailOverSmsSupported)
    {
    QDEBUG_WRITE("START MsgSendUtil::validateService");
    bool result = true;
    //If settings are missed then show a note and launch settings dialog
    if (!pluginInterface->validateService(emailOverSmsSupported))
        {
        //TODO: show note and launch settings dialog
        
        result = false;
        }
    // if control is here, then check is ok
    QDEBUG_WRITE("END MsgSendUtil::validateService");
    return result;
    }

// EOF
