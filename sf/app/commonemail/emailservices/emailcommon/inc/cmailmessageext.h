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
* Description:  Message extension interface
*
*/

#ifndef CMAILMESSAGEEXT_H
#define CMAILMESSAGEEXT_H

#include "cemailextensionbase.h"

// interface UID value passed to CFSMailMessageBase::ExtensionL
const TUid KEmailMessageExtensionUid        = { 0x2001E288 };

/**
 * Message extension interface
 * 
 * @code
 * // below message is of type CFSMailMessageBase (or its derived type)
 * CEmailExtension* extension = message->ExtensionL( KEmailMessageExtensionUid );
 * CMailMessageExtension* mExt = reinterpret_cast<CMailMessageExtension*>( extension );
 * mExt->CommitL(CFSMailMessage&);
 * // Release extension when not needed any more.
 * message->ReleaseExtension( extension );
 * @endcode
 * 
 */
class CMailMessageExtension : public CEmailExtension
{
public:
    
    /**
     * Commits changes of the message to store.
     * @param aMessage a message to commit.
     */
    virtual void CommitL(CFSMailMessage& aMessage) = 0;

protected:    
    inline CMailMessageExtension();
};

inline CMailMessageExtension::CMailMessageExtension() :
         CEmailExtension( KEmailMessageExtensionUid )
    {
    }
         
#endif // CMAILMESSAGEEXT_H
