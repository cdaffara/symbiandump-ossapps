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
* Description: mpxcollectionutility stub for testing mpmpxframeworkwrapper
*
*/


#ifndef MMPXCOLLECTIONUTILITY_H
#define MMPXCOLLECTIONUTILITY_H

#include <mpxcollectionframeworkdefs.h>
#include <mpxcommonframeworkdefs.h>
#include <mpxcollectionobserver.h>
#include <mpxattribute.h>
#include <mpxsearchcriteria.h>
#include <mpxattributespecs.h>

#include <badesca.h>


//Forward declarations
class CMPXCollectionPath;
class MMPXCollectionFindObserver;
class CMPXFilter;

class MMPXCollection
{
public:
    
    // Stub functions interface
    virtual void OpenL(TMPXOpenMode aMode=EMPXOpenDefault) = 0;   
    virtual void OpenL(TInt aIndex,TMPXOpenMode aMode=EMPXOpenDefault) = 0;   
    virtual void OpenL(const CMPXCollectionPath& aPath,
                       TMPXOpenMode aMode=EMPXOpenDefault) = 0;
    virtual CMPXCollectionPath* PathL() = 0;
    virtual void BackL() = 0;
    virtual void CancelRequest() = 0;
    virtual CMPXMedia* FindAllL(const CMPXSearchCriteria& aCriteria,
                                const TArray<TMPXAttribute>& aAttrs) = 0;
    virtual void FindAllL(const CMPXSearchCriteria& aCriteria,
                          const TArray<TMPXAttribute>& aAttrs,
                          MMPXCollectionFindObserver& aObs) = 0;
    virtual void MediaL(const CMPXCollectionPath& aPath,
                        const TArray<TMPXAttribute>& aAttrs,
                        CMPXAttributeSpecs* aSpecs=NULL,
                        CMPXFilter* aFilter=NULL) = 0;
};

class MMPXCollectionUtility : public MMPXCollection
{
public:

    // Test utility functions
    void setPlaylists( const CMPXMedia& entries );

    // Stub functions
    static MMPXCollectionUtility* NewL( MMPXCollectionObserver* aObs = NULL,
                                        const TUid& aModeId = KMcModeDefault);
    MMPXCollectionUtility();
    ~MMPXCollectionUtility();
    MMPXCollection& Collection();
    TUid CollectionIDL(const TArray<TUid>& aUids);
    void Close();
    
    //From MMPXCollection
    void OpenL(TMPXOpenMode aMode=EMPXOpenDefault);
    void OpenL(TInt aIndex,TMPXOpenMode aMode=EMPXOpenDefault);
    void OpenL(const CMPXCollectionPath& aPath, TMPXOpenMode aMode=EMPXOpenDefault);
    CMPXCollectionPath* PathL();
    void BackL();
    void CancelRequest();
    CMPXMedia* FindAllL(const CMPXSearchCriteria& aCriteria,
                        const TArray<TMPXAttribute>& aAttrs);
    void FindAllL(const CMPXSearchCriteria& aCriteria,
                  const TArray<TMPXAttribute>& aAttrs,
                  MMPXCollectionFindObserver& aObs);
    void MediaL(const CMPXCollectionPath& aPath,
                const TArray<TMPXAttribute>& aAttrs,
                CMPXAttributeSpecs* aSpecs=NULL,
                CMPXFilter* aFilter=NULL);

public:
 
    TBool          iBack;
    TBool          iOpen;
    TBool          iMedia;
    TInt           iOpenCount;
    TInt           iCountPath;
    int            iIndex;
    CMPXMedia      *iPlaylists; //Owned
    CMPXMediaArray *iAlbumSongs; //Owned
    CMPXMedia      *iAsynchFindResult; //Owned
};





#endif      // MMPXCOLLECTIONUTILITY_H

