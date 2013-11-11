/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common email base object
*
*/

#include "emailtrace.h"

//<cmail>
#include "CFSMailRequestHandler.h"
#include "CFSMailMessageBase.h"
#include "cmailmessageext.h"
//</cmail>

//<qmail>
#include <nmcommonheaders.h>
#include "nmmessageenvelope.h"
#include "nmconverter.h"
//</qmail>

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailMessageBase::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessageBase * CFSMailMessageBase::NewLC( TFSMailMsgId aMessageId )
{
  NM_FUNCTION;
  
  CFSMailMessageBase* message = new (ELeave) CFSMailMessageBase();
  CleanupStack:: PushL(message);
  message->ConstructL(aMessageId);
  return message;
} 

// -----------------------------------------------------------------------------
// CFSMailMessageBase::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessageBase * CFSMailMessageBase::NewL( TFSMailMsgId aMessageId )
{
  NM_FUNCTION;
  
  CFSMailMessageBase* message =  CFSMailMessageBase::NewLC(aMessageId);
  CleanupStack:: Pop(message);
  return message;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::CFSMailMessageBase
// -----------------------------------------------------------------------------
CFSMailMessageBase::CFSMailMessageBase(): iSender(NULL)
{
    NM_FUNCTION;
    
// <qmail> unnecessary member initialization removed: iSubject, iFlags </qmail>
    // set request handler pointer
  iRequestHandler = static_cast<CFSMailRequestHandler*>(Dll::Tls());
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ConstructL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ConstructL( TFSMailMsgId aMessageId )
{
    NM_FUNCTION;
    
// <qmail>
    // Construction of shared data object
    iNmPrivateMessageEnvelope = new NmMessageEnvelopePrivate();

  // typedef long int TInt32 -> typedef unsigned int quint32
    iNmPrivateMessageEnvelope->mMessageId.setId32((quint32)aMessageId.Id() );

  // typedef unsigned int TUint -> typedef unsigned int quint32
    iNmPrivateMessageEnvelope->mMessageId.setPluginId32((quint32)aMessageId.PluginId().iUid);

    // construct the CFSMailAddress object and connect it with NmAddress private data 
    iSender = CFSMailAddress::NewL(iNmPrivateMessageEnvelope->mSender);
// </qmail>
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessageBase::ConstructL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ConstructL(
    const NmMessageEnvelope &aMessageEnvelope )
{
    NM_FUNCTION;

    iNmPrivateMessageEnvelope = aMessageEnvelope.d;
}
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailMessageBase::~CFSMailMessageBase
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessageBase::~CFSMailMessageBase()
{
    NM_FUNCTION;
    
  if(iSender)
    {
    delete iSender;
    iSender = NULL;
    }
// <qmail> unnecessary member destruction removed: iSubject, iToRecipients, iCcRecipients, iBccRecipients </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetMessageId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::GetMessageId(  ) const
{
    NM_FUNCTION;
    
// <qmail>
    //For message  
    TFSMailMsgId id = TFSMailMsgId(iNmPrivateMessageEnvelope->mMessageId);
    
    //For message part
    if(id.IsNullId()){
        id = TFSMailMsgId(iNmPrivateMessageEnvelope->mFolderId);
    }
  
    return id;
// </qmail>
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetMessageId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetMessageId( const TFSMailMsgId aMessageId )
{
    NM_FUNCTION;
    
    // typedef long int TInt32 -> typedef unsigned int quint32
    iNmPrivateMessageEnvelope->mMessageId.setId32( aMessageId.Id() );

    // typedef unsigned int TUint -> typedef unsigned int quint32
    iNmPrivateMessageEnvelope->mMessageId.setPluginId32(
            (unsigned int)aMessageId.PluginId().iUid);
}
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetFolderId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::GetFolderId( ) const
{
    NM_FUNCTION;
    
// <qmail>
    return TFSMailMsgId(iNmPrivateMessageEnvelope->mFolderId);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetFolderId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetFolderId( const TFSMailMsgId aFolderId )
{
    NM_FUNCTION;
    
// <qmail>
    iNmPrivateMessageEnvelope->mFolderId = NmConverter::mailMsgIdToNmId(aFolderId);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::GetMailBoxId( ) const
{
    NM_FUNCTION;
    
// <qmail>
    return NmId(iNmPrivateMessageEnvelope->mMailboxId);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetMailBoxId( const TFSMailMsgId aMailBoxId )
{
    NM_FUNCTION;
    
// <qmail>
    iNmPrivateMessageEnvelope->mMailboxId = NmConverter::mailMsgIdToNmId(aMailBoxId);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetSender
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetSender(CFSMailAddress* aSender)
{
    NM_FUNCTION;
    
// <qmail>
    // store sender
    if (iSender)
        {
        delete iSender;
        iSender = NULL;
        }
    if (!aSender) 
        {
        TRAP_IGNORE(iSender = CFSMailAddress::NewL());
        }
    else 
        {
        iSender = aSender;
        }   
    iNmPrivateMessageEnvelope->mSender = iSender->GetNmAddress();
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetSender
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress* CFSMailMessageBase::GetSender() const
{
    NM_FUNCTION;
    
  return iSender;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetToRecipients
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailAddress> CFSMailMessageBase::GetToRecipients()
{
// <qmail>
    NM_FUNCTION;
    
    return NmConverter::toRPointerArray(
        iNmPrivateMessageEnvelope->mToRecipients);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetCCRecipients
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailAddress> CFSMailMessageBase::GetCCRecipients()
{
// <qmail>
    NM_FUNCTION;
    
    return NmConverter::toRPointerArray(
        iNmPrivateMessageEnvelope->mCcRecipients);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetBCCRecipients
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailAddress> CFSMailMessageBase::GetBCCRecipients()
{
// <qmail>
    NM_FUNCTION;
    
    return NmConverter::toRPointerArray(
        iNmPrivateMessageEnvelope->mBccRecipients);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::AppendToRecipient
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::AppendToRecipient(CFSMailAddress* aRecipient)
{
    NM_FUNCTION;
    
// <qmail>
    if (aRecipient) 
        {
        iNmPrivateMessageEnvelope->mToRecipients.append(aRecipient->GetNmAddress());
        delete aRecipient;//ownership transferred to us so we can delete.
        aRecipient = NULL;
        }
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::AppendCCRecipient
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::AppendCCRecipient(CFSMailAddress* aRecipient )
{
    NM_FUNCTION;
    
// <qmail>
    if (aRecipient) 
        {
        iNmPrivateMessageEnvelope->mCcRecipients.append(
            aRecipient->GetNmAddress());
        delete aRecipient;//ownership transferred to us so we can delete.
        aRecipient = NULL;
        }
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::AppendBCCRecipient
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::AppendBCCRecipient( CFSMailAddress* aRecipient )
{
    NM_FUNCTION;
    
// <qmail>
    if (aRecipient)
        {
        iNmPrivateMessageEnvelope->mBccRecipients.append(
            aRecipient->GetNmAddress());
        delete aRecipient;//ownership transferred to us so we can delete.
        aRecipient = NULL;
        }
// </qmail>      
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ClearToRecipients
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ClearToRecipients( )
{
    NM_FUNCTION;
    
// <qmail>
    iNmPrivateMessageEnvelope->mToRecipients.clear();
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ClearCcRecipients
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ClearCcRecipients( )
{
    NM_FUNCTION;
    
// <qmail>
    iNmPrivateMessageEnvelope->mCcRecipients.clear();
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ClearBccRecipients
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ClearBccRecipients( )
{
    NM_FUNCTION;
    
// <qmail>
    iNmPrivateMessageEnvelope->mBccRecipients.clear();
// </qmail>
}

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetSubject
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailMessageBase::GetSubject() const
{
    NM_FUNCTION;
    
// <qmail>
    iSubjectPtr.Set(reinterpret_cast<const TUint16*> (iNmPrivateMessageEnvelope->mSubject.utf16()),
        iNmPrivateMessageEnvelope->mSubject.length());
    return iSubjectPtr;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetDate
// -----------------------------------------------------------------------------
EXPORT_C TTime CFSMailMessageBase::GetDate() const
{
    NM_FUNCTION;
    
// <qmail>
    return NmConverter::toTTime(iNmPrivateMessageEnvelope->mSentTime);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetDate
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetDate( const TTime aDate )
{
    NM_FUNCTION;
    
// <qmail>
    iNmPrivateMessageEnvelope->mSentTime = NmConverter::toQDateTime(aDate);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetSubject
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetSubject(const TDesC& aSubject)
{
    NM_FUNCTION;
    
// <qmail>
    QString qtSubject = QString::fromUtf16(aSubject.Ptr(), aSubject.Length());
    iNmPrivateMessageEnvelope->mSubject = qtSubject;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetFlags
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessageBase::GetFlags( ) const
{
    NM_FUNCTION;
    
// <qmail>
    return (TInt)iNmPrivateMessageEnvelope->flags();
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetFlag
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetFlag(const TInt aFlag)
{
    NM_FUNCTION;
    
// <qmail>
    iNmPrivateMessageEnvelope->setFlags((NmMessageFlags)aFlag, true);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ResetFlag
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ResetFlag(const TInt aFlag)
{
    NM_FUNCTION;
    
// <qmail>
    iNmPrivateMessageEnvelope->setFlags((NmMessageFlags)aFlag, false);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::IsFlagSet
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailMessageBase::IsFlagSet(const TInt aFlag) const
{
    NM_FUNCTION;
    
// <qmail>
    TBool result = EFalse;
    if (iNmPrivateMessageEnvelope->isFlagSet((NmMessageFlag)aFlag)) 
        {
        result = ETrue;
        }
    return result;
// <qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetReplyToAddress
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetReplyToAddress(CFSMailAddress* aReplyToAddress)
{
    NM_FUNCTION;
    
  // store sender
  if (iReplyTo)
    {
    delete iReplyTo;
    }
  iReplyTo = aReplyToAddress;

}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetReplyToAddress
// -----------------------------------------------------------------------------
EXPORT_C const CFSMailAddress& CFSMailMessageBase::GetReplyToAddress()
{
    NM_FUNCTION;
    
    return *iReplyTo;
}

EXPORT_C CFSMailRequestHandler& CFSMailMessageBase::RequestHandler( )
{
    NM_FUNCTION;
    
    return *iRequestHandler;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetNmMessageEnvelope
// -----------------------------------------------------------------------------
EXPORT_C NmMessageEnvelope* CFSMailMessageBase::GetNmMessageEnvelope()
{
    NM_FUNCTION;
    
    return new NmMessageEnvelope(iNmPrivateMessageEnvelope);
}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ReleaseExtension
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ReleaseExtension( CEmailExtension* aExtension )
    {
    NM_FUNCTION;
    
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(
		iNmPrivateMessageEnvelope->mMessageId ) )
        {
        // If plugin has created the extension, let it handle destruction.
        plugin->ReleaseExtension( aExtension );
        }
    else
        {
        CExtendableEmail::ReleaseExtension( aExtension );
        }
    }
    
// -----------------------------------------------------------------------------
// CFSMailMessageBase::ExtensionL
// -----------------------------------------------------------------------------
EXPORT_C CEmailExtension* CFSMailMessageBase::ExtensionL( 
    const TUid& aInterfaceUid )
    {
    NM_FUNCTION;
    
    CEmailExtension* ext = CExtendableEmail::ExtensionL( aInterfaceUid );
    // didn't find already created instance, try now
    if ( !ext )
        {
        // check that plugin supports requested extension.
// <qmail>
        if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(
			iNmPrivateMessageEnvelope->mMessageId ) )
// </qmail>
            {
            // request extension from plugin, leaves if not supported
            ext = plugin->ExtensionL( aInterfaceUid );    
            }

        }
    return ext;
    }
