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
#include "debugtraces.h"

#include <QDateTime>

#include <csmsemailfields.h>
//USER INCLUDES
#include <xqconversions.h>
#include "convergedmessageaddress.h"
#include "convergedmessage.h"

#include "unismsdataplugin_p.h"

#define KSmsMtmUidValue  0x1000102C
const TUid KSmsMtmUid =
{KSmsMtmUidValue};
_LIT(KUnixEpoch, "19700000:000000.000000");

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::~UniSMSDataPluginPrivate
// @see header
//---------------------------------------------------------------
UniSMSDataPluginPrivate::~UniSMSDataPluginPrivate()
{
    q_ptr = NULL;
    delete iSmsClient;
    delete iMtmReg;
    delete iMSession;
}

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::UniSMSDataPluginPrivate
// @see header
//---------------------------------------------------------------
UniSMSDataPluginPrivate::UniSMSDataPluginPrivate(UniSMSDataPlugin* plugin) :
    iMSession(NULL),
    iMtmReg(NULL),
    iSmsClient(NULL),
    q_ptr(plugin)
{   
   
    iMSession = CMsvSession::OpenSyncL(*this);
    done = EFalse;
}

void UniSMSDataPluginPrivate::initL()
{

    iMtmReg = CClientMtmRegistry::NewL(*iMSession);
    TMsvSelectionOrdering ordering;

    CMsvEntry* root = CMsvEntry::NewL(*iMSession,
                                      KMsvRootIndexEntryId,
                                      ordering);
    CleanupStack::PushL(root);
 
    TMsvId firstId;
    TRAPD(err, TSmsUtilities::ServiceIdL(*root,firstId));

    if (err == KErrNotFound)
    {
        TMsvEntry entry;
        entry.iMtm = KUidMsgTypeSMS;
        entry.iType = KUidMsvServiceEntry;
        entry.SetReadOnly(EFalse);
        entry.SetVisible(EFalse);
        entry.iDate.UniversalTime();
        entry.iDetails.Set(_L("SMS Service"));

        root->SetEntryL(KMsvRootIndexEntryId);
        root->CreateL(entry);
    }
    
    CleanupStack::PopAndDestroy(root);
    iSmsClient = static_cast<CSmsClientMtm*> (iMtmReg->NewMtmL(KSmsMtmUid));
    done = ETrue;
    iMessageId = 0;
}

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::reset
// @see header
//---------------------------------------------------------------
void UniSMSDataPluginPrivate::reset()
{
}

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::setMessageId
// @see header
//---------------------------------------------------------------
int UniSMSDataPluginPrivate::setMessageId(int mId)
{
    TInt error = KErrNone;
    TRAP(error, setMessageIdL(mId));
    return error;

}

void UniSMSDataPluginPrivate::setMessageIdL(int mId)
{
    if (done == EFalse)
    {
        initL();
    }
    if (iMessageId != mId)
    {
        iMessageId = (TMsvId) mId;
        iSmsClient->SwitchCurrentEntryL(iMessageId);
        iSmsClient->LoadMessageL();
    }

}

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::body
// @see header
//---------------------------------------------------------------
void UniSMSDataPluginPrivate::body(QString& aBodyText)
{
    CRichText& textBody = iSmsClient->Body();
    TInt len = textBody.DocumentLength();
    HBufC* buf = HBufC::NewL(len);
    TPtr bufPtr = buf->Des();
    textBody.ExtractSelectively(bufPtr, 0, len, CPlainText::EExtractAll);
    aBodyText = XQConversions::s60DescToQString(*buf);
    delete buf;
}

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::messageSize
// @see header
//---------------------------------------------------------------
qint32 UniSMSDataPluginPrivate::messageSize()
{
    return iSmsClient->Entry().Entry().iSize;
}

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::toRecipientList
// @see header
//---------------------------------------------------------------
void UniSMSDataPluginPrivate::toRecipientList(
                                              ConvergedMessageAddressList& mAddressList)
{
    TPtrC name;
    TPtrC address;
    // Get the recipient list from the client mtm
    const CMsvRecipientList& smsRecipients = iSmsClient->AddresseeList();

    for (TInt id = 0; id < smsRecipients.Count(); ++id)
    {
        extractNameAndAddress(smsRecipients[id], name, address);
        // populate address
        ConvergedMessageAddress
                * messageAddress =
                        new ConvergedMessageAddress(XQConversions::s60DescToQString(address),
                                                    XQConversions::s60DescToQString(name));
        mAddressList.append(messageAddress);
    }

    const CSmsEmailFields& emailFields = iSmsClient->SmsHeader().EmailFields();
    // check if it is a EOS
    if (emailFields.HasAddress())
    {
        const MDesCArray& emailRecipients = emailFields.Addresses();
        for (TInt id = 0; id < emailRecipients.MdcaCount(); ++id)
        {
            extractNameAndAddress(emailRecipients.MdcaPoint(id), name, address);
            ConvergedMessageAddress
                    * messageAddress =
                            new ConvergedMessageAddress(XQConversions::s60DescToQString(address),
                                                        XQConversions::s60DescToQString(name));
            mAddressList.append(messageAddress);
        }
    }

}

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::fromAddress
// @see header
//---------------------------------------------------------------
void UniSMSDataPluginPrivate::fromAddress(QString& messageAddress)
{
    CPlainText* pText = CPlainText::NewL();
    CleanupStack::PushL(pText);

    CSmsHeader* smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *pText);
    CleanupStack::PushL(smsHeader);

    CMsvEntry *cEntry = CMsvEntry::NewL(iSmsClient->Session(),
                                        iMessageId,
                                        TMsvSelectionOrdering());
    CleanupStack::PushL(cEntry);

    CMsvStore* store = cEntry->ReadStoreL();
    CleanupStack::PushL(store);

    smsHeader->RestoreL(*store);

    messageAddress
            = XQConversions::s60DescToQString(smsHeader->FromAddress());
    CleanupStack::PopAndDestroy(4, pText);
}

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::timeStamp
// @see header
//---------------------------------------------------------------
int UniSMSDataPluginPrivate::timeStamp()
{
    QDateTime retTimeStamp;
    TTime timeStamp = iSmsClient->Entry().Entry().iDate;
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    timeStamp.SecondsFrom(unixEpoch, seconds);
    return seconds.Int();
}

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::session
// @see header
//---------------------------------------------------------------
CMsvSession* UniSMSDataPluginPrivate::session()
{
    return iMSession;
}

//---------------------------------------------------------------
// UniSMSDataPluginPrivate::extractNameAndAddress
// @see header
//---------------------------------------------------------------
void UniSMSDataPluginPrivate::extractNameAndAddress(const TDesC& aMsvAddress,
                                                    TPtrC& aName,
                                                    TPtrC& aAddress)
{
    // For address information separation (start)
    const TUint KMsgSmsAddressStartChar('<');

    // For address information separation (end)
    const TUint KMsgSmsAddressEndChar('>');

    TInt addressStart = aMsvAddress.LocateReverse(KMsgSmsAddressStartChar);
    TInt addressEnd = aMsvAddress.LocateReverse(KMsgSmsAddressEndChar);

    if (addressStart != KErrNotFound && addressEnd != KErrNotFound
            && addressEnd > addressStart)
    {
        // verified address, will be used as selected from contacts manager
        aName.Set(aMsvAddress.Ptr(), addressStart);
        aAddress.Set(aMsvAddress.Mid(addressStart + 1).Ptr(), (addressEnd
                - addressStart) - 1);
        if (!aAddress.Length())
        {
            aAddress.Set(aName);
            aName.Set(KNullDesC); // empty string
        }
    }
    else
    {
        // unverified string, will be used as entered in the header field
        aName.Set(KNullDesC); // empty string
        aAddress.Set(aMsvAddress.Ptr(), aMsvAddress.Length()); // a whole string to address
    }

    if (aName.CompareF(aAddress) == 0)
    {
        aName.Set(KNullDesC); // empty string
    }
}


//---------------------------------------------------------------
// UniSMSDataPluginPrivate::HandleSessionEventL
// @see header
//---------------------------------------------------------------
void UniSMSDataPluginPrivate::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                                  TAny* /*aArg1*/,
                                                  TAny* /*aArg2*/, TAny* /*aArg3*/)
{
// do nothing
}


