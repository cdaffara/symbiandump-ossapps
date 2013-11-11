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
* Description: Inline methods for class CFSMailPlugin.
*
*/


#include <ecom.h>

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
//
inline CFSMailPlugin::CFSMailPlugin()
    {

    }

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
//
inline CFSMailPlugin::~CFSMailPlugin()
    {
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::Close
// -----------------------------------------------------------------------------
//
inline void CFSMailPlugin::Close()
    {
    REComSession::FinalClose();
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::ListImplementationsL
// -----------------------------------------------------------------------------
inline void CFSMailPlugin::ListImplementationsL(
    RPointerArray<CImplementationInformation>& aInfo)
    {
    REComSession::ListImplementationsL(KFSMailPluginInterface, aInfo);
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::NewL
// -----------------------------------------------------------------------------
//
inline CFSMailPlugin* CFSMailPlugin::NewL(TUid aUid)
    {
    CFSMailPlugin* plugin = static_cast<CFSMailPlugin*>(
        REComSession::CreateImplementationL(
            aUid, _FOFF( CFSMailPlugin, iDtor_ID_Key ) ) );
    return plugin;
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::CreateForwardMessageL
// -----------------------------------------------------------------------------
inline void CFSMailPlugin::CreateForwardMessageL( const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aOriginalMessageId*/,
    MFSMailRequestObserver& /*aOperationObserver*/,
    const TInt /*aRequestId*/,
    const TDesC& /*aHeaderDescriptor*/ )
    {
    User::Leave( KErrFSMailPluginNotSupported );
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::CreateReplyMessageL
// -----------------------------------------------------------------------------
inline void CFSMailPlugin::CreateReplyMessageL( const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aOriginalMessageId*/,
    const TBool /*aReplyToAll*/,
    MFSMailRequestObserver& /*aOperationObserver*/,
    const TInt /*aRequestId*/,
    const TDesC& /*aHeaderDescriptor*/ )
    {
    User::Leave( KErrFSMailPluginNotSupported );
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::SendMessageL
// -----------------------------------------------------------------------------
inline void CFSMailPlugin::SendMessageL(
        CFSMailMessage& /*aMessage*/,
        MFSMailRequestObserver& /*aOperationObserver*/,
        const TInt /*aRequestId*/ )
    {
    User::Leave( KErrFSMailPluginNotSupported );
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::NewChildPartFromFileL
// -----------------------------------------------------------------------------
inline void CFSMailPlugin::NewChildPartFromFileL( const TFSMailMsgId& /*aMailBoxId*/,
                                     const TFSMailMsgId& /*aParentFolderId*/,
                                     const TFSMailMsgId& /*aMessageId*/,
                                     const TFSMailMsgId& /*aParentPartId*/,
                                     const TDesC& /*aContentType*/,
                                     const TDesC& /*aFilePath*/,
                                     MFSMailRequestObserver& /*aOperationObserver*/,
                                     const TInt /*aRequestId*/ )
    {
    User::Leave( KErrFSMailPluginNotSupported );
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::RemoveChildPartL
// -----------------------------------------------------------------------------
inline void CFSMailPlugin::RemoveChildPartL( const TFSMailMsgId& /*aMailBoxId*/,
                               const TFSMailMsgId& /*aParentFolderId*/,
                               const TFSMailMsgId& /*aMessageId*/,
                               const TFSMailMsgId& /*aParentPartId*/,
                               const TFSMailMsgId& /*aPartId*/,
                               MFSMailRequestObserver& /*aOperationObserver*/,
                               const TInt /*aRequestId*/)
    {
    User::Leave( KErrFSMailPluginNotSupported );
    }

// -----------------------------------------------------------------------------
// CFSMailPlugin::DeleteMessagesByUidL
// -----------------------------------------------------------------------------
inline void CFSMailPlugin::DeleteMessagesByUidL( const TFSMailMsgId& /*aMailBoxId*/,
                                   const TFSMailMsgId& /*aFolderId*/,
                                   const RArray<TFSMailMsgId>& /*aMessages*/,
                                   MFSMailRequestObserver& /*aOperationObserver*/,
                                   const TInt /*aRequestId*/ )
    {
    User::Leave( KErrFSMailPluginNotSupported );
    }
// </qmail>

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailPlugin::GetSignatureL
// -----------------------------------------------------------------------------
inline HBufC* CFSMailPlugin::GetSignatureL( const TFSMailMsgId& /*aMailBoxId*/ )
    {
    return NULL;
    }
// </qmail>

