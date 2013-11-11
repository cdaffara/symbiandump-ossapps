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
* Description:  mpxcollectionuihelper stub for testing mpmpxframeworkwrapper
*
*/

#ifndef MMPXCOLLECTIONUIHELPER_H
#define MMPXCOLLECTIONUIHELPER_H

#include <mpxmediageneraldefs.h>
#include <mpxcommonframeworkdefs.h>
#include <mpxmediageneraldefs.h>
#include <QString.h>


// FORWARD DECLARATIONS
class CMPXCollectionPath;
class CMPXMedia;
class MMPXCHelperObserver;

class MMPXCollectionUiHelper
{
public:
    
    // Stub functions
    MMPXCollectionUiHelper();
    ~MMPXCollectionUiHelper();
    
    void DeleteL( CMPXCollectionPath& aPath,
                          MMPXCHelperObserver* aObserver );
    CMPXCollectionPath* MusicMenuPathL();
    CMPXCollectionPath* MusicAllSongsPathL();
    CMPXCollectionPath* MusicPlaylistPathL(); 
    void Close();
    void IncAddL( const CMPXMedia& aMedia,
                          MMPXCHelperObserver* aObserver=0,
                          const TInt aSize=0 );
    void RenameL( const CMPXMedia& aMedia,
                              MMPXCHelperObserver* aObserver );
    void ReorderPlaylistL(const TMPXItemId& aPlaylistId,
                                      const TMPXItemId& aSongId,
                                      TUint aOriginalOrdinal,
                                      TUint aNewOrdinal,
                                      MMPXCHelperObserver* aObserver);
    void Cancel();
    
public:
    
    TBool           iIncAdd;
    TBool           iDelete;
    TBool           iCancel;
    CMPXMedia       *iMedia;
    TBool           iValidRename;
    TBool           iValidReorder;
    TMPXItemId      iPlaylistId;
    QString         iRenameTitle;

};


#endif // MMPXCOLLECTIONUIHELPER_H
