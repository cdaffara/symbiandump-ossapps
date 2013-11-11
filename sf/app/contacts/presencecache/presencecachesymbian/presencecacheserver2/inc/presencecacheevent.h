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
#ifndef PRESENCECACHEEVENT_H_
#define PRESENCECACHEEVENT_H_

#include <e32base.h>


class CPresenceCacheEvent : public CBase
    {
    public:
                
        static CPresenceCacheEvent* NewL( HBufC8* aMsg );
                
        TPtrC8 Message();
                
        virtual ~CPresenceCacheEvent();
        
        TBool NotifySent();
        
        void SetNotifySent();
        
    private:
        
        CPresenceCacheEvent( );
                
        void ConstructL( HBufC8* aMsg );

        
    private:  // Data
        
        HBufC8*                             iMessage;
        TBool                               iNotifySent;
    };

#endif // PRESENCECACHEEVENT_H_
