/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef IRSETTINGSUTILITY_H
#define IRSETTINGSUTILITY_H

#include <e32cmn.h>

class CDictionaryStore;

NONSHARABLE_CLASS( IRSettingsUtility )
    {

public:

    static void WriteIntL( CDictionaryStore& aDictionary, TUid aUid, TInt aValue );
    static void WriteInt64L( CDictionaryStore& aDictionary, TUid aUid, TInt64 aValue );
    static void WriteUintL( CDictionaryStore& aDictionary, TUid aUid, TUint aValue );
    static void WriteBoolL( CDictionaryStore& aDictionary, TUid aUid, TBool aValue );
    static void WriteStringL( CDictionaryStore& aDictionary, TUid aUid, const TDesC& aValue );

    static TInt ReadIntL( CDictionaryStore& aDictionary, TUid aUid );
    static TInt64 ReadInt64L( CDictionaryStore& aDictionary, TUid aUid );
    static TUint ReadUintL( CDictionaryStore& aDictionary, TUid aUid );
    static TBool ReadBoolL(  CDictionaryStore& aDictionary, TUid aUid );
    static HBufC* ReadStringL( CDictionaryStore& aDictionary, TUid aUid );

    };

#endif // IRSETTINGSUTILITY_H
