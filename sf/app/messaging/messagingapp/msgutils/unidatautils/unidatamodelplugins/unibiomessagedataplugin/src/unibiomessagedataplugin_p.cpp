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
 * Description: 
 *
 */

//SYSTEM INCLUDES
#include <e32cmn.h>
#include <mtclreg.h>
#include <mtclbase.h>
#include <msvids.h>
#include <txtetext.h>
#include <txtrich.h>
#include <txtfmlyr.h>
#include <smsclnt.h>

#include <bioscmds.h>
#include <biocmtm.h>
#include "debugtraces.h"

#include <QDateTime>
#include <xqconversions.h>
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include "convergedmessage.h"
#include "convergedmessageaddress.h"
#include "unibiomessagedataplugin_p.h"

#define KBioUidValue  0x10001262
const TUid KBioUidValueUid =
{KBioUidValue};
_LIT(KUnixEpoch, "19700000:000000.000000");

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::~UniBioMessageDataPluginPrivate
// @see header
//---------------------------------------------------------------
UniBioMessageDataPluginPrivate::~UniBioMessageDataPluginPrivate()
{
    q_ptr = NULL;
    
    if(attachmentProcessed == EFalse && iAttachmentCount == 1 && iMsvEntry)
    {
       TRAP_IGNORE( CMsvStore* store = iMsvEntry->EditStoreL();
        CleanupStack::PushL(store);
        MMsvAttachmentManagerSync& attachMan = store->AttachmentManagerExtensionsL();
        attachMan.RemoveAttachmentL(0);
        CleanupStack::PopAndDestroy(););
    }
    
    if (iMsvEntry) {
        delete iMsvEntry;
    }

    delete iBioClientMtm;
    delete iMtmReg;
    delete iMSession;

}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::UniBioMessageDataPluginPrivate
// @see header
//---------------------------------------------------------------
UniBioMessageDataPluginPrivate::UniBioMessageDataPluginPrivate(UniBioMessageDataPlugin* plugin) :
    q_ptr(plugin), iMSession(NULL), iMtmReg(NULL), iBioClientMtm(NULL),
            iMsvEntry(NULL),iAttachmentCount(0)
{
    iMSession = CMsvSession::OpenSyncL(*this);
    done = EFalse;
}




void UniBioMessageDataPluginPrivate::initL()
{
    iMtmReg = CClientMtmRegistry::NewL(*iMSession);
    iBioClientMtm = (CBIOClientMtm*) iMtmReg->NewMtmL(KBioUidValueUid);

    attachmentProcessed = EFalse;
    done = ETrue;

}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::reset
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPluginPrivate::reset()
{
}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::setMessageId
// @see header
//---------------------------------------------------------------
int UniBioMessageDataPluginPrivate::setMessageId(int mId)
{
    TInt error = KErrNone;
    TRAP(error, setMessageIdL(mId));
    return error;
}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::setMessageIdL
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPluginPrivate::setMessageIdL(int mId)
{
    if (done == EFalse)
    {
        initL();
    }
    if (iMessageId != mId)
    {
        attachmentProcessed = EFalse;
    }

    iMessageId = (TMsvId) mId;
    iBioClientMtm->SwitchCurrentEntryL(iMessageId);

    iMsvEntry = CMsvEntry::NewL(iBioClientMtm->Session(),
                                iMessageId,
                                TMsvSelectionOrdering());

    iBioClientMtm->LoadMessageL();
}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::body
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPluginPrivate::body(QString& aBodyText)
{

    CRichText& textBody = iBioClientMtm->Body();
    TInt len = textBody.DocumentLength();
    HBufC* buf = HBufC::NewL(len);
    TPtr bufPtr = buf->Des();
    textBody.ExtractSelectively(bufPtr, 0, len, CPlainText::EExtractAll);
    aBodyText = XQConversions::s60DescToQString(*buf);
    delete buf;
}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::messageSize
// @see header
//---------------------------------------------------------------
qint32 UniBioMessageDataPluginPrivate::messageSize()
{
    return iBioClientMtm->Entry().Entry().iSize;
}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::toRecipientList
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPluginPrivate::toRecipientList(
                                               ConvergedMessageAddressList& mAddressList)
{
    TPtrC name;
    TPtrC address;

    CPlainText* pText = CPlainText::NewL();
    CleanupStack::PushL(pText);

    CSmsHeader* smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *pText);
    CleanupStack::PushL(smsHeader);

    CMsvStore* store = iMsvEntry->ReadStoreL();
    CleanupStack::PushL(store);

    smsHeader->RestoreL(*store);

    for (TInt id = 0; id < smsHeader->Recipients().Count(); ++id)
    {
        CSmsNumber* rcpt = smsHeader->Recipients()[id];
        name.Set(rcpt->Name());
        address.Set(rcpt->Address());

        ConvergedMessageAddress
                * messageAddress =
                        new ConvergedMessageAddress(XQConversions::s60DescToQString(address),
                                                    XQConversions::s60DescToQString(name));
        mAddressList.append(messageAddress);
    }
    CleanupStack::PopAndDestroy(3, pText);

}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::fromAddress
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPluginPrivate::fromAddress(QString& messageAddress)
{
    CPlainText* pText = CPlainText::NewL();
    CleanupStack::PushL(pText);

    CSmsHeader* smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *pText);
    CleanupStack::PushL(smsHeader);

    CMsvStore* store = iMsvEntry->ReadStoreL();
    CleanupStack::PushL(store);

    smsHeader->RestoreL(*store);

    messageAddress
            = XQConversions::s60DescToQString(smsHeader->FromAddress());
    CleanupStack::PopAndDestroy(3, pText);
}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::timeStamp
// @see header
//---------------------------------------------------------------
int UniBioMessageDataPluginPrivate::timeStamp()
{
    QDateTime retTimeStamp;
    TTime timeStamp = iBioClientMtm->Entry().Entry().iDate;
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    return seconds.Int();
}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::attachmentList
// @see header
//---------------------------------------------------------------
RFile UniBioMessageDataPluginPrivate::attachmentL()
{
    if (attachmentProcessed) {

        CMsvStore* store1 = iMsvEntry->ReadStoreL();
        CleanupStack::PushL(store1);
        MMsvAttachmentManager& attachMan = store1->AttachmentManagerL();
        RFile file;
		
        if (iAttachmentCount > 0)
        {
            file = attachMan.GetAttachmentFileL(0);
        }
            
        CleanupStack::PopAndDestroy(store1);
        return file;
    }

    RFile file;
    
    if (iMsvEntry->Entry().SendingState() != KMsvSendStateNotApplicable
        || iMsvEntry->Entry().SendingState() != KMsvSendStateUnknown) {

        CMsvStore* store = iMsvEntry->ReadStoreL();
        CleanupStack::PushL(store);
        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();

        iAttachmentCount = attachMan.AttachmentCount();

        if (iAttachmentCount > 0) {
            file = attachMan.GetAttachmentFileL(0);
        }
        CleanupStack::PopAndDestroy(store);

    }

    if (iAttachmentCount == 0) {
        CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
        CleanupStack::PushL(selection);

        selection->AppendL(iMessageId);

        TBuf8<1> aParameter;
        CMsvOperationActiveSchedulerWait* wait = CMsvOperationActiveSchedulerWait::NewLC();

        CMsvOperation* operation = iBioClientMtm->InvokeAsyncFunctionL(KBiosMtmParse, *selection,
            aParameter, wait->iStatus);

        wait->Start();

        CMsvStore* store = iMsvEntry->ReadStoreL();
        CleanupStack::PushL(store);
        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();

        iAttachmentCount = attachMan.AttachmentCount();
		
        if(iAttachmentCount) {
         file = attachMan.GetAttachmentFileL(0);
		}

        delete operation;
        CleanupStack::PopAndDestroy(3, selection);
        attachmentProcessed = ETrue;
    }
    return file;

}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::attachmentCount
// @see header
//---------------------------------------------------------------

int UniBioMessageDataPluginPrivate::attachmentCount()
{
    if (!attachmentProcessed) {
        RFile file = attachmentL();
        file.Close();
    }

    return (int) iAttachmentCount;
}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::messagePriority
// @see header
//---------------------------------------------------------------

MsgPriority UniBioMessageDataPluginPrivate::messagePriority()
{

    MsgPriority priority = (MsgPriority) iMsvEntry->Entry().Priority();
    return priority;
}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::session
// @see header
//---------------------------------------------------------------
CMsvSession* UniBioMessageDataPluginPrivate::session()
{
    return iMSession;
}

//---------------------------------------------------------------
// UniBioMessageDataPluginPrivate::HandleSessionEventL
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPluginPrivate::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                                  TAny* /*aArg1*/,
                                                  TAny* /*aArg2*/, TAny* /*aArg3*/)
{
// do nothing
}


