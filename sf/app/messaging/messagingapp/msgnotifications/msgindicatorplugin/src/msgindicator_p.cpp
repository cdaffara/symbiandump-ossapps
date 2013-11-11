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
 * Description:Message Indicator private class. 
 * Gets the indicaton information from conversation server.
 *
 */
#include <ccsclientconversation.h>
#include <ccsrequesthandler.h>
#include <ccsconversationentry.h>

#include "msgindicator_p.h"  
#include "msgindicator.h"
#include "debugtraces.h"
#include <xqconversions.h>

#define LOC_RECEIVED_FILES hbTrId("Received Files")

// ----------------------------------------------------------------------------
// MsgIndicatorPrivate::MsgIndicatorPrivate
// @see MsgIndicatorPrivate.h
// ----------------------------------------------------------------------------
MsgIndicatorPrivate::MsgIndicatorPrivate(MsgIndicator* inidcator) :
    q_ptr(inidcator), mCvServer(NULL)
{
    initL();
}

// ----------------------------------------------------------------------------
// MsgIndicatorPrivate::~MsgIndicatorPrivate
// @see MsgIndicatorPrivate.h
// ----------------------------------------------------------------------------
MsgIndicatorPrivate::~MsgIndicatorPrivate()
{
    if (mCvServer) {
        delete mCvServer;
        mCvServer = NULL;
    }
}

// ----------------------------------------------------------------------------
// MsgIndicatorPrivate::initL()
// @see MsgIndicatorPrivate.h
// ----------------------------------------------------------------------------
void MsgIndicatorPrivate::initL()
{
    mCvServer = CCSRequestHandler::NewL();
}

// ----------------------------------------------------------------------------
// MsgIndicatorPrivate::getIndicatorInfo
// @see MsgIndicatorPrivate.h
// ----------------------------------------------------------------------------
void MsgIndicatorPrivate::getIndicatorInfo(MsgInfo& indicatorData)
{
    TRAP_IGNORE(getIndicatorInfoL(indicatorData));
}

// ----------------------------------------------------------------------------
// MsgIndicatorPrivate::getIndicatorInfoL
// @see MsgIndicatorPrivate.h
// ----------------------------------------------------------------------------
void MsgIndicatorPrivate::getIndicatorInfoL(MsgInfo& indicatorData)
{
    RPointerArray<CCsClientConversation> *clientConversationList = new RPointerArray<
        CCsClientConversation> ();
    CleanupStack::PushL(clientConversationList);
    mCvServer->GetConversationUnreadListL(clientConversationList);
    CleanupStack::Pop();

    QStringList nameList;    
    int count = clientConversationList->Count();
    
    indicatorData.mMsgCount = 0;
    
    for (int loop = 0; loop < count; ++loop) {
        
        CCsClientConversation* conversation =
            static_cast<CCsClientConversation*> ((*clientConversationList)[loop]);
        
        CCsConversationEntry* convEntry = conversation->GetConversationEntry();

        indicatorData.mMsgCount += conversation->GetUnreadMessageCount();
        indicatorData.mConversationId = static_cast<int>(conversation->GetConversationEntryId());
        
        HBufC* description = convEntry->Description();
        if (description && (count == 1)) {
            // Only take the description from convEntry when there is one entry.
            // description will contain the name list when there are more entries.
            QString descText;
            descText = XQConversions::s60DescToQString(*description);
            descText.replace(QChar::ParagraphSeparator, QChar::LineSeparator);
            descText.replace('\r', QChar::LineSeparator);
            indicatorData.mDescription = descText;
        }

        if((indicatorData.mMessageType = static_cast<int>(convEntry->GetType()))==ECsBlueTooth){
            // if we have bluetooth messages, nameList should contain the LOC_RECEVED_FILES
            // string. DisplayName/number are not needed.
            nameList.append(LOC_RECEIVED_FILES);
            indicatorData.mMessageType = ECsBlueTooth ;
            continue;
        }
        else { // non-bluetooth
            HBufC* displayName = conversation->GetDisplayName();
            HBufC* number = convEntry->Contact();

            if (displayName) {
                nameList.append(XQConversions::s60DescToQString(*displayName));
            }
            else if (number) {
                nameList.append(XQConversions::s60DescToQString(*number));
            }
        }
    }

    nameList.removeDuplicates();
    nameList.sort();
    
    if (nameList.count() > 1) {
        // more than 1 sender. Concatenate the names in the description 
        
        indicatorData.mFromSingle = false;
        indicatorData.mDescription = nameList.join(QString(", "));
        indicatorData.mMessageType = ECsSMS;
    }
    else if (nameList.count() == 1){
        // only 1 sender.
        // displayname will have the name of the sender.
        // description will contain latest message if more than 1 message
        indicatorData.mFromSingle = true;    
        indicatorData.mDisplayName.append(nameList.at(0));
    }
    
    clientConversationList->ResetAndDestroy();
    delete clientConversationList;
}

