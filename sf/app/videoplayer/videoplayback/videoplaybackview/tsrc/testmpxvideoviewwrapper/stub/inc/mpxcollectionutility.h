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
* Description:  Interface to collections
*
*/


#ifndef MMPXCOLLECTIONUTILITY_H
#define MMPXCOLLECTIONUTILITY_H

#include <e32cmn.h>

class CMPXCollectionPath;
class MMPXCollectionObserver;

class MMPXCollection
{
    public:
        virtual TUid UidL() const = 0;

        virtual CMPXCollectionPath* PathL() = 0;

    public:
        TUid iUid;
};

class MMPXCollectionUtility
{
    public:
        static MMPXCollectionUtility* NewL( MMPXCollectionObserver* aObs );

        virtual MMPXCollection& Collection() = 0;

        virtual void Close() = 0;
};

class CMPXCollectionUtility : public MMPXCollectionUtility,
                              public MMPXCollection
{
    public:
        static CMPXCollectionUtility* NewL( MMPXCollectionObserver* aObs );

        ~CMPXCollectionUtility();

    private:
        CMPXCollectionUtility( MMPXCollectionObserver* aObs );

        void ConstructL();

    public:
        MMPXCollection& Collection();

        void Close();

        TUid UidL() const;

        CMPXCollectionPath* PathL();
};

#endif //MMPXCOLLECTIONUTILITY_H

// End of file
