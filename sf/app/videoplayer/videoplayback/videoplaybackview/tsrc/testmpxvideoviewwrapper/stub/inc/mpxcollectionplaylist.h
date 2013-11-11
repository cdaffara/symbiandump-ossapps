/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Encapsulates a 'playlist' from a collection
*
*/


#ifndef CMPXCOLLECTIONPLAYLIST_H
#define CMPXCOLLECTIONPLAYLIST_H

#include <s32strm.h>


class CMPXCollectionPlaylist
{
    public:
        static CMPXCollectionPlaylist* NewL();

        virtual ~CMPXCollectionPlaylist();

    public:
        TInt Count() const;

    private:
        CMPXCollectionPlaylist();

        void ConstructL();

    public:
        TInt iCount;
};

#endif // CMPXCOLLECTIONPLAYLIST_H
