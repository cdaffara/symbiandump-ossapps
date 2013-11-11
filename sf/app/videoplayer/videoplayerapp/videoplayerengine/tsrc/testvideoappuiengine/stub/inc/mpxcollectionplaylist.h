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
* Description:  mpx collection playlist class stub for unit testing
*
*/

// Version : %version:  1 %

#ifndef MPXCOLLECTIONPLAYLIST_H_
#define MPXCOLLECTIONPLAYLIST_H_

#include <e32base.h>

class CMPXCollectionPath;
//class MMPXCollectionPlaylistObserver;

class CMPXCollectionPlaylist : public CBase
{
    public:
    		CMPXCollectionPlaylist();
    		~CMPXCollectionPlaylist();

        //CMPXCollectionPlaylist* NewL(const CMPXCollectionPath& aPath,
        //                             MMPXCollectionPlaylistObserver* aPlObs = NULL);
                                     
        static CMPXCollectionPlaylist* NewL(const CMPXCollectionPath& aPath);                                     

        void SetSingleItemPlaylist();
        
        void SetToFirst();
        
        void SetAutoPlay(TBool aAutoPlay);
        
        void SetPreInitPlugin(TBool aPreInit);
                                     
};
#endif /*MPXCOLLECTIONPLAYLIST_H_*/
