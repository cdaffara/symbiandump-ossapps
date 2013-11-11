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
 * Description: Listen to conversation server events and parse the 
 * events. These parse infomation passed to notifications and indications.
 *
 */

// SYSTEM INCLUDES
#include <msvapi.h>
#include <ccsclientconversation.h>
#include <ccsrequesthandler.h>
#include <ccsconversationentry.h>
#include <xqservicerequest.h>
#include <QString>
#include <w32std.h>
#include <apgtask.h> 
#include <XQSettingsManager>
#include <xqaiwrequest.h>
#include <xqappmgr.h>
#include <xqpublishandsubscribeutils.h>
#include <xqsystemtoneservice.h>
#include <xqconversions.h>
#include <QThreadPool>
#include <ProfileEngineInternalCRKeys.h>
#include <hwrmvibra.h>

//USER INCLUDES
#include "msgnotifier.h"
#include "msgnotifier_p.h"
#include "msgstorehandler.h"
#include "msginfodefs.h"
#include "conversationidpsconsts.h"
#include "debugtraces.h"

const XQCentralRepositorySettingsKey silenceModeKey(KCRUidProfileEngine.iUid, KProEngSilenceMode);

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::MsgNotifierPrivate
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
MsgNotifierPrivate::MsgNotifierPrivate(MsgNotifier* MsgNotifier) :
    q_ptr(MsgNotifier), mCvServer(NULL), iMsgStoreHandler(NULL),
    mSettingsManager(NULL),mPSUtils(NULL)
{
    QDEBUG_WRITE("MsgNotifierPrivate::MsgNotifierPrivate : Enter")

    initL();
    QDEBUG_WRITE("MsgNotifierPrivate::MsgNotifierPrivate : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::~MsgNotifierPrivate
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
MsgNotifierPrivate::~MsgNotifierPrivate()
{
    QDEBUG_WRITE("MsgNotifierPrivate::~MsgNotifierPrivate : Enter")
    if (mCvServer) {
        TRAP_IGNORE(mCvServer->RemoveConversationListChangeEventL(this));
        delete mCvServer;
        mCvServer = NULL;
    }

    if (iMsgStoreHandler) {
        delete iMsgStoreHandler;
        iMsgStoreHandler = NULL;
    }
    
    if(mPSUtils)
        {
        delete mPSUtils;
        }
    
    if(mSettingsManager)
        {
        delete mSettingsManager;
        }
    
    if(mSts)
        {
        delete mSts;
        mSts = NULL;
        }
    
    if(mVibra)
        {
        delete mVibra;
        mVibra = NULL;
        }
    QDEBUG_WRITE("MsgNotifierPrivate::~MsgNotifierPrivate : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::initL
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::initL()
{
    QDEBUG_WRITE("MsgNotifierPrivate::initL : Enter")

    mCvServer = CCSRequestHandler::NewL();
    mCvServer->RequestConversationListChangeEventL(this);
    iMsgStoreHandler = new MsgStoreHandler(this, mCvServer);
    updateUnreadIndications(true); 
    updateOutboxIndications();

    QT_TRYCATCH_LEAVING(mSettingsManager = new XQSettingsManager());
    
    // define property
    QT_TRYCATCH_LEAVING(mPSUtils = new XQPublishAndSubscribeUtils(*mSettingsManager));
    
    XQPublishAndSubscribeSettingsKey convIdKey(
            KMsgCVIdProperty, KMsgCVIdKey);
    bool success = mPSUtils->defineProperty(convIdKey, 
                            XQSettingsManager::TypeInt);
    
    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::initL "
                        "property creation ret value",success)
    
    // write -1 initially 
    success = mSettingsManager->writeItemValue(convIdKey,-1);
    
    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::initL "
                           "writing ret value",success)
    
    QT_TRYCATCH_LEAVING(mSts = new XQSystemToneService());
    
    mVibra = CHWRMVibra::NewL();
    
    QDEBUG_WRITE("MsgNotifierPrivate::initL : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::AddConversationList
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::AddConversationList(
                  const CCsClientConversation& aClientConversation)
{
    QCRITICAL_WRITE("MsgNotifierPrivate::AddConversationList : Enter")

    processListEntry(aClientConversation);
    updateUnreadIndications();

    QCRITICAL_WRITE("MsgNotifierPrivate::AddConversationList : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::DeleteConversationList
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::DeleteConversationList( 
	                const CCsClientConversation& /*aClientConversation*/)
{
    QDEBUG_WRITE("MsgNotifierPrivate::DeleteConversationList : Enter")
    updateUnreadIndications();
    QDEBUG_WRITE("MsgNotifierPrivate::DeleteConversationList : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::ModifyConversationList
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::ModifyConversationList(const CCsClientConversation& aClientConversation)
{
    QCRITICAL_WRITE("MsgNotifierPrivate::ModifyConversationList : Enter")

    processListEntry(aClientConversation);
    updateUnreadIndications();

    QCRITICAL_WRITE("MsgNotifierPrivate::ModifyConversationList : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::RefreshConversationList
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::RefreshConversationList()
{

}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::processListEntry
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::processListEntry(
        const CCsClientConversation& aClientConversation)
    {
    QCRITICAL_WRITE("MsgNotifierPrivate::processListEntry : Enter")
    
    CCsConversationEntry* convEntry = 
                                 aClientConversation.GetConversationEntry();
    
    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::processListEntry Direction",
                                                convEntry->ConversationDir())
    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::processListEntry New Entry Attribute",
                                                convEntry->IsAttributeSet(ECsAttributeNewEntryAdded))
    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::processListEntry Unread Attribute",
                                                convEntry->IsAttributeSet(ECsAttributeUnread))
   
    if((convEntry->ConversationDir() == ECsDirectionIncoming)
            && convEntry->IsAttributeSet(ECsAttributeNewEntryAdded) 
            && convEntry->IsAttributeSet(ECsAttributeUnread))
        {
        QDEBUG_WRITE("processListEntry : Processing data for Notification")
        
        NotificationData notifData;
        notifData.msvEntryId = convEntry->EntryId(); 
        notifData.mConversationId = aClientConversation.GetConversationEntryId();
        notifData.mMsgType = static_cast<int>(convEntry->GetType());
        
        HBufC* displayName = aClientConversation.GetDisplayName();
        HBufC* number =  convEntry->Contact();
        HBufC* descrp =  convEntry->Description();
        
        if(displayName)
            {
            notifData.mDisplayName = 
                                XQConversions::s60DescToQString(*displayName);
            }        
        if(number)
            {
            notifData.mContactNum =  XQConversions::s60DescToQString(*number);
            }
        if(descrp)
            {
            notifData.mDescription = XQConversions::s60DescToQString(*descrp);
            }
        
        QVariant silenceMode = mSettingsManager->readItemValue(silenceModeKey, XQSettingsManager::TypeInt);
        int silent = silenceMode.toInt();
        if(silent < 1)
            {
            //Play new message alert tone.
            mSts->playTone(XQSystemToneService::SmsAlertTone);
            }
        // Execute the vibra effect.
        if (mVibra) 
            {
            TInt err = KErrNone;
            TRAP(err,mVibra->StartVibraL(1000));
            }
        
        // check whether opened cv id and received 
        // cv id are same and show notification
        if( showNotification(notifData.mConversationId ))
            {
             q_ptr->displayNewMessageNotification(notifData);
             QDEBUG_WRITE("processListEntry : Notification display called")
            }
       
        }
    
    QCRITICAL_WRITE("MsgNotifierPrivate::processListEntry : Exit")
    }

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::updateUnreadIndications
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::updateUnreadIndications(bool bootup)
{
    QDEBUG_WRITE("MsgNotifierPrivate::updateIndicationsL : Enter")

    int unreadCount(0);
   if(bootup)
   {
       TRAP_IGNORE(unreadCount = iMsgStoreHandler->GetUnreadMessageCountL());
   }
   else
   {
       TRAP_IGNORE(unreadCount = mCvServer->GetTotalUnreadCountL());
   }

    QDEBUG_WRITE_FORMAT("MsgNotifierPrivate::"
        "updateIndications unreadCount = ",unreadCount );

    //activate or deactivate indications based on unread count
    q_ptr->updateUnreadIndications(unreadCount);

    QDEBUG_WRITE("MsgNotifierPrivate::updateIndicationsL : Exit")
}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::updateOutboxIndications
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::updateOutboxIndications()
{

    MsgInfo failedIndicatorData;
    MsgInfo pendingIndicatorData;
    TInt err = KErrNone;
    
    TRAP(err, iMsgStoreHandler->GetOutboxEntriesL(failedIndicatorData,pendingIndicatorData));

    q_ptr->updateOutboxIndications(failedIndicatorData);
    q_ptr->updateOutboxIndications(pendingIndicatorData);
}


// ----------------------------------------------------------------------------
// MsgNotifierPrivate::displayOutboxIndications
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------

void MsgNotifierPrivate::displayOutboxIndications(MsgInfo data)
	{
		q_ptr->updateOutboxIndications(data);
	}

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::displayFailedNote
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::displayFailedNote(MsgInfo info)
    {
    QDEBUG_WRITE("MsgNotifierPrivate::displayFailedNote start.")
            
    // check whether opened cv id and received 
    // cv id are same then dont show failed note
    if (!showNotification(info.mConversationId))
        {
        return;
        }

    //Even if name string is empty we shall add name into args
    QString nameString;

    info.mDisplayName.removeDuplicates();
    info.mDisplayName.sort();

    nameString.append(info.mDisplayName.at(0));
    for (int i = 1; i < info.mDisplayName.count(); ++i)
        {
        nameString.append(", ");
        nameString.append(info.mDisplayName.at(i));
        }

    // create request arguments
    QVariantList args;
    args << QVariant(nameString);
    args << info.mConversationId;
    args << info.mMessageType;

    // TODO: use XQAiwRequest
    XQServiceRequest snd("messaging.com.nokia.symbian.MsgErrorNotifier",
            "displayErrorNote(QVariantList)", false);

    snd << args;
    snd.send();
    
    QDEBUG_WRITE("MsgNotifierPrivate::displayFailedNote end.")
    }

// ----------------------------------------------------------------------------
// MsgNotifierPrivate::showNotification
// @see MsgNotifierPrivate.h
// ----------------------------------------------------------------------------
bool MsgNotifierPrivate::showNotification(int receivedMsgConvId)
{
    bool showNotification = false;
    
    RWsSession wsSession ;
    int err = wsSession.Connect();

    if ( KErrNone == err )
        {
        showNotification = true;
        TApaTaskList taskList(wsSession);
        TApaTask task = taskList.FindApp(KMsgAppUid); // find msgapp is running

        if (task.Exists())
            {
            TApaTask foregndtask = taskList.FindByPos(0); // foreground app
            // compare  window group id  
            // if application is in foregorund, then check the currently
            // opened conversation is same as received one.
            if (task.WgId() == foregndtask.WgId())
                {
                // get the current conversation ID
                XQPublishAndSubscribeSettingsKey convIdKey(KMsgCVIdProperty,
                        KMsgCVIdKey);
                QVariant value = mSettingsManager->readItemValue(convIdKey,
                        XQSettingsManager::TypeInt);

                int openedConvId = value.toInt();
                if (openedConvId == receivedMsgConvId)
                    {
                    showNotification = false;
                    QDEBUG_WRITE("processListEntry : Notification not shown")
                    }
                }
            }

        wsSession.Close();
        }
    return showNotification;
}
// ----------------------------------------------------------------------------
// MsgNotifierPrivate::PartialDeleteConversationList
// @see mcsconversationclientchangeobserver.h
// ----------------------------------------------------------------------------
void MsgNotifierPrivate::PartialDeleteConversationList(
        const CCsClientConversation& /*aClientConversation*/){/*empty implementation*/}


// ----------------------------------------------------------------------------
// MsgNotifierPrivate::ShowClass0Message
// @see msgnotifier_p.h
// ----------------------------------------------------------------------------

void MsgNotifierPrivate::ShowClass0Message(Class0Info& class0Info)
{
    QCRITICAL_WRITE("flashmsgnotifier BEGIN");
    QThreadPool::globalInstance()->start(new Class0SmsServiceTask(class0Info));
    QCRITICAL_WRITE("flashmsgnotifier END");
}




// ----------------------------------------------------------------------------
// Class0SmsServiceTask::Class0SmsServiceTask
// @see msgnotifier_p.h
// ----------------------------------------------------------------------------  
Class0SmsServiceTask::Class0SmsServiceTask(Class0Info& class0Info):
mClass0info(class0Info)
     {     
     }

// ----------------------------------------------------------------------------
// Class0SmsServiceTask::~Class0SmsServiceTask
// @see msgnotifier_p.h
// ----------------------------------------------------------------------------   
Class0SmsServiceTask::~Class0SmsServiceTask()
     {     
     }
     
// ----------------------------------------------------------------------------
// Class0SmsServiceTask::run
// @see msgnotifier_p.h
// ----------------------------------------------------------------------------   
void Class0SmsServiceTask::run()
     {
   	  QList<QVariant> args;
      QString serviceName("flashmsgnotifier");
      QString interfaceName("com.nokia.symbian.IFlashMsgNotifier");
      QString operation("displayFlashMsg(QByteArray)");
      XQAiwRequest* request;
      XQApplicationManager appManager;

      request = appManager.create(interfaceName, operation, false); //non-embedded

      if ( request == NULL )
      {
          QCRITICAL_WRITE("flashmsgnotifier request == NULL");
          return; 
      }

      QByteArray ba; 
      QDataStream stream(&ba, QIODevice::ReadWrite);
      stream << mClass0info.body;
      stream << mClass0info.address;
      stream << mClass0info.alias;
      stream << mClass0info.time;     
      stream << mClass0info.messageId;
       
      args << ba;
      request->setArguments(args);    
       
      if(!request->send())
      {
         QString lastErr;
          lastErr = request->lastErrorMessage();
          QDEBUG_WRITE_FORMAT("flashmsgnotifier launch failed", lastErr);
      }
       
      QCRITICAL_WRITE("flashmsgnotifier END");
      delete request;  
     }

//EOF
