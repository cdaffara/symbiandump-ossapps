/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Inline methods for CMmsMMBoxMessageHeaders
*
*/



inline TInt CMmsMMBoxMessageHeaders::MmsStore() const
    {
    return iMmsStore;
    }

inline void CMmsMMBoxMessageHeaders::SetMmsStore ( TInt aMmsStore )
    {
    iMmsStore = aMmsStore;
    }

inline TInt CMmsMMBoxMessageHeaders::MmsMMState() const
    {
    return iMmsMMState;
    }

inline void CMmsMMBoxMessageHeaders::SetMMState( TInt aMmsMMState )
    {
    iMmsMMState = aMmsMMState;
    }

inline TInt CMmsMMBoxMessageHeaders::MmsStored() const
    {
    return iMmsStored;
    }

inline void CMmsMMBoxMessageHeaders::SetMmsStored( TInt aMmsStored )
    {
    iMmsStored = aMmsStored;
    }

inline TInt CMmsMMBoxMessageHeaders::MmsStoreStatus() const
    {
    return iMmsStoreStatus;
    }

inline void CMmsMMBoxMessageHeaders::SetMmsStoreStatus( TInt aMmsStoreStatus )
    {
    iMmsStoreStatus = aMmsStoreStatus;
    }

inline CDesC8Array& CMmsMMBoxMessageHeaders::ContentLocationList() const 
    {
    return *iContentLocationArray;
    }

inline RPointerArray<CMmsMMBoxFlags>& CMmsMMBoxMessageHeaders::KeywordArray()
    {
    return iKeywordArray; //lint !e1536
    }


// End of File
