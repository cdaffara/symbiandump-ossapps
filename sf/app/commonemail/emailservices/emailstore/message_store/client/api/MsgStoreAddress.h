/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store address.
*
*/



#ifndef __MSG_STORE_ADDRESS_H__
#define __MSG_STORE_ADDRESS_H__

#include <e32base.h>
//<cmail>
#include "MsgStoreTypes.h"
//</cmail>

/** Simple convinent on-stack class to encapsulate the {email address, display name} pair used
 *  as address fields (From, Sender, To, Cc, Bcc) in a message.
 */    
class RMsgStoreAddress
    {
    public:
        /**
         * Close() must be called before this object goes out of scope
         */
        IMPORT_C void Close();
        
        RBuf    iEmailAddress;
        RBuf    iDisplayName;
    };

/** Simple convinent on-heap class to encapsulate the {email address, display name} pair used
 *  as address fields (From, Sender, To, Cc, Bcc) in a message.
 * 
 *  This class is created because on-stack class RMsgStoreAddress does not work very well
 *  when the caller is expecting an ARRAY of ADDRESS to be returned.
 */    
class CMsgStoreAddress : public CBase
    {
    public:
        IMPORT_C static CMsgStoreAddress* NewL( const TDesC& aEmailAddress, const TDesC& aDisplayName );
        IMPORT_C virtual ~CMsgStoreAddress();
        
        IMPORT_C const TDesC& EmailAddress();
        IMPORT_C const TDesC& DisplayName();
        
    private:
        CMsgStoreAddress( );
        void ConstructL( const TDesC& aEmailAddress, const TDesC& aDisplayName );

    private:
        RBuf    iEmailAddress;
        RBuf    iDisplayName;
    };

#endif  // __MSG_STORE_ADDRESS_H__
