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
* Description:  mpx collection utility stub for unit testing
*
*/

// Version : %version: 1 %

#ifndef MPXCOLLECTIONUTILITYIMP_STUB_H_
#define MPXCOLLECTIONUTILITYIMP_STUB_H_

#include <mpxcollectionutility.h>
#include <mpxcollectionobserver.h>
#include <mpxmessageobserver.h>

NONSHARABLE_CLASS( CMPXCollectionUtility ) : public MMPXCollectionUtility,
                                             public MMPXMessageObserver,
                                             public MMPXCollection

{
    public:
        /**
        * Two-phased constructor
        *
        * @param aObs observer
        * @param aModeId collection mode id
        * @return object of constructed
        */
        static CMPXCollectionUtility* NewL( const TUid& aModeId, MMPXCollectionObserver* aObs );

        /**
        * Destructor
        */
        ~CMPXCollectionUtility();

        void AddRefCount();

        //void AddTestObserverL( MAppUiEngineUnitTestObserver* aObserver );
        //void RemoveTestObserverL();

    private:
        /**
        * C++ constructor
        */
        CMPXCollectionUtility( MMPXCollectionObserver* aObs );

        /**
        * Second phase constructor
        *
        * @param aModeId collection mode id
        */
        void ConstructL( const TUid& aModeId );

    public:

        //------------------------------------------------------------------------------------------
        //  MMPXCollectionUtility Implementation
        //------------------------------------------------------------------------------------------
        MMPXCollection& Collection();
        TUid CollectionIDL( const TArray<TUid>& aUids );
        void Close();

        //------------------------------------------------------------------------------------------
        //  MMPXCollection Implementation
        //------------------------------------------------------------------------------------------
        void OpenL( TMPXOpenMode aMode = EMPXOpenDefault );
        void OpenL( TInt aIndex, TMPXOpenMode aMode = EMPXOpenDefault );
        void OpenL( const CMPXCollectionPath& aPath, TMPXOpenMode aMode = EMPXOpenDefault );
        void OpenL( TInt aIndex,
                    const TArray<TMPXAttribute>& aAttrs,
                    TMPXOpenMode aMode = EMPXOpenDefault );
        void OpenL( const CMPXCollectionPath& aPath,
                    const TArray<TMPXAttribute>& aAttrs,
                    TMPXOpenMode aMode = EMPXOpenDefault );
        void OpenL( const TArray<TUid>& aUids, TMPXOpenMode aMode = EMPXOpenDefault );
        void OpenL( const TUid& aUid, TMPXOpenMode aMode = EMPXOpenDefault );
        void SetFilterL( CMPXFilter* aFilter );
        CMPXFilter* FilterL();
        TUid UidL() const;
        CMPXCollectionPath* PathL();
        void BackL();
        TBool IsRemote( const CMPXCollectionPath& aPath );
        void CancelRequest();
        void AddL( const CMPXMedia& aNewProperties );
        void RemoveL( const CMPXCollectionPath& aPath, MMPXCollectionRemoveObserver* aObs = NULL );
        void RemoveL( const CMPXMedia& aProperties );
        void SetSyncL( const CMPXMedia& aMedia );
        void SetL( const CMPXMedia& aMedia );
        void FindAllL( const CMPXMedia& aMedia,
                       const TArray<TMPXAttribute>& aAttrs,
                       MMPXCollectionFindObserver& aObs );
        CMPXMedia* FindAllL( const CMPXMedia& aMedia, const TArray<TMPXAttribute>& aAttrs );
        void MediaL( const CMPXCollectionPath& aPath,
                     const TArray<TMPXAttribute>& aAttrs,
                     CMPXAttributeSpecs* aSpecs = NULL,
                     CMPXFilter* aFilter = NULL );
        void NotifyL( TMPXCollectionBroadCastMsg aMsg, TInt aData );
        void CommandL( TMPXCollectionCommand aCmd, TInt aData = 0 );
        void CommandL( CMPXCommand& aCmd );
        void GetSupportedTypesL( RPointerArray<CMPXCollectionType>& aArray );
        void GetSupportedCapabilitiesL( TCollectionCapability& aCapability );
        TUid CollectionIDL( TUid& aCollection );
        void AddSubscriptionL( const CMPXSubscription& aSubscription );
        void RemoveSubscriptionL( const CMPXSubscription& aSubscription );
        void ClearSubscriptionsL();

        //------------------------------------------------------------------------------------------
        //  MMPXMessageObserver Implementation
        //------------------------------------------------------------------------------------------
        void MessageReceived( TInt aMsgData, TInt aError );

    private:
        MMPXCollectionObserver*         iObserver;
        CMPXCollectionPath*             iCollectionPath;
        //MAppUiEngineUnitTestObserver*   iTestObserver;

        TInt                            iRefCount;
};

#endif /*MPXCOLLECTIONUTILITYIMP_STUB_H_*/
