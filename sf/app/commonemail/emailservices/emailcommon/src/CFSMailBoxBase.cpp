/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common base mailbox object
*
*/

#include "emailtrace.h"

// <qmail>
#include <nmcommonheaders.h>
// </qmail>

#include "CFSMailBoxBase.h"
// <qmail>
#include "CFSMailAddress.h"
#include "mmrinfoprocessor.h"
// </qmail>
#include "nmmailbox.h"

//<qmail> Cmail fix task applied using QString methods (AknTextUtils n/a in 10.1)
const int KCharCount = 5;
static const QChar KCharsToReplace[KCharCount] = { '\r','\n','\t',0x2028,0x2029 };
//</qmail>

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailBoxBase::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBoxBase* CFSMailBoxBase::NewLC(TFSMailMsgId aMailBoxId)
{
    NM_FUNCTION;
    
    CFSMailBoxBase* api = new (ELeave) CFSMailBoxBase();
    CleanupStack:: PushL(api);
    api->ConstructL(aMailBoxId);
    return api;
} 

// -----------------------------------------------------------------------------
// CFSMailBoxBase::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBoxBase* CFSMailBoxBase::NewL(TFSMailMsgId aMailBoxId)
{
    NM_FUNCTION;
    
    CFSMailBoxBase* api =  CFSMailBoxBase::NewLC(aMailBoxId);
    CleanupStack:: Pop(api);
    return api;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::CFSMailBoxBase
// -----------------------------------------------------------------------------
CFSMailBoxBase::CFSMailBoxBase() : CExtendableEmail()
{
    NM_FUNCTION;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::~CFSMailBoxBase
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBoxBase::~CFSMailBoxBase()
{
    NM_FUNCTION;
    
    if( iMailBoxName )
        {
  		delete iMailBoxName;
        }
	iMailBoxName = NULL;

    if( iBrId )
        {
  		delete iBrId;
        }
	iBrId = NULL;

    if( iOwnMailAddress )
        {
  		delete iOwnMailAddress;
        }
	iOwnMailAddress = NULL;	

    if( iMRInfoProcessor )
        {
        delete iMRInfoProcessor;
        }
    iMRInfoProcessor = NULL;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::ConstructL
// -----------------------------------------------------------------------------
void CFSMailBoxBase::ConstructL(const TFSMailMsgId aMailBoxId)
{
    NM_FUNCTION;
    
// <qmail> Removed iMailboxId </qmail>
    // prepare null empty descriptor
    iMailBoxName = HBufC::NewL(1);
    iMailBoxName->Des().Copy(KNullDesC());

    iBrId = HBufC::NewL(1);
    iBrId->Des().Copy(KNullDesC());

// <qmail>
    // Creating QT side object of the mailbox class.
    iNmPrivateMailbox = new NmMailboxPrivate();
    // Initialize QT data: pluginId and id. These need conversions between QT
    // and Symbian data types. Name not set here as it is null.

    // typedef long int TInt32 -> typedef unsigned int quint32
    // NOTE: This cast will loose negative values: range [0,max] is identical
    // but range [min,0] is different between these data types.
    // -> Assumption: iUid should never contain negative value.
    iNmPrivateMailbox->mId.setId32( aMailBoxId.Id() );

    // typedef unsigned int TUint -> typedef unsigned int quint32
    iNmPrivateMailbox->mId.setPluginId32((unsigned int)aMailBoxId.PluginId().iUid);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::GetId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailBoxBase::GetId() const
{
    NM_FUNCTION;
    
// <qmail>
    return TFSMailMsgId(iNmPrivateMailbox->mId);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::GetName
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailBoxBase::GetName() const
{
    NM_FUNCTION;
    
// <qmail>
    iTextPtr.Set( 
        reinterpret_cast<const TUint16*> (iNmPrivateMailbox->mName.utf16()),
        iNmPrivateMailbox->mName.length());
    return iTextPtr;
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::SetName
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetName(const TDesC& aMailBoxName)
{
    NM_FUNCTION;
    
// <qmail>
    QString qtName = QString::fromUtf16(aMailBoxName.Ptr(), aMailBoxName.Length());
    // Fix from Cmail 9.2 rewritten with QString methods
    for (int i = 0; i < KCharCount; i++) {
        QChar chr = KCharsToReplace[i];
        qtName = qtName.replace(chr, ' ');
    }
    iNmPrivateMailbox->mName = qtName;
// </qmail>
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CFSMailBoxBase::SetRCLInfo
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetRCLInfo(const TUid aProtocolUid, const TUint aAccountUid)
{
    NM_FUNCTION;
    
	iProtocolUid = aProtocolUid;
	iAccountUid = aAccountUid;
}
// CFSMailBoxBase::SetSettingsUid
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetSettingsUid(const TUid aUid)
	{
    NM_FUNCTION;
    
		iSettingsUid = aUid;
	}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::MRInfoProcessorL
// -----------------------------------------------------------------------------
EXPORT_C MMRInfoProcessor& CFSMailBoxBase::MRInfoProcessorL()
	{
    NM_FUNCTION;
    
	return *iMRInfoProcessor;
	}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::IsMRInfoProcessorSet
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailBoxBase::IsMRInfoProcessorSet()
	{
    NM_FUNCTION;
    
	if(iMRInfoProcessor)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::SetMRInfoProcessorL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetMRInfoProcessorL(MMRInfoProcessor* aMRInfoProcessor)
	{
    NM_FUNCTION;
    
	if(iMRInfoProcessor)
	{
		delete iMRInfoProcessor;
	}
	iMRInfoProcessor = aMRInfoProcessor;
	}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::OwnMailAddress
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress& CFSMailBoxBase::OwnMailAddress()
{
    NM_FUNCTION;
    
	return *iOwnMailAddress;
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::SetOwnMailAddress
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBoxBase::SetOwnMailAddressL(CFSMailAddress* aOwnMailAddress)
{
    NM_FUNCTION;
// <qmail>
    if (!aOwnMailAddress)
        {
        User::Leave(KErrArgument);
        }
// </qmail>
    if(iOwnMailAddress)
        {
        delete iOwnMailAddress;
        }
    iOwnMailAddress = aOwnMailAddress;
    
    // set also branding id based on domain name
    TPtrC ptr = OwnMailAddress().GetEmailAddress();
    TInt index = ptr.Locate('@') + 1;
    if(index > 0 && ptr.Length() > 0)
    {
        if(iBrId)
        {
            delete iBrId;
            iBrId = NULL;
        }
        ptr.Set( ptr.Right( ptr.Length() - index ) );        
        iBrId = HBufC::New(ptr.Length());
        iBrId->Des().Copy(ptr);
    }
// <qmail>
    iNmPrivateMailbox->mAddress = aOwnMailAddress->GetNmAddress();
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailBoxBase::BrandingId
// -----------------------------------------------------------------------------
TDesC& CFSMailBoxBase::BrandingId( )
{
    NM_FUNCTION;
    
    return *iBrId;
}

// <qmail> Removed SetMailBoxId </qmail>

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailBoxBase::GetNmMailbox
// -----------------------------------------------------------------------------
EXPORT_C NmMailbox* CFSMailBoxBase::GetNmMailbox()
{
    NM_FUNCTION;
    
    NmMailbox* nmmailbox = new NmMailbox(iNmPrivateMailbox);
    return nmmailbox;
}
// </qmail>
