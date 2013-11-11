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
 *     The header file for mde tone fetcher.
 *     
 */

#ifndef CTONESELECTION_H
#define CTONESELECTION_H

#include <e32base.h>
#include <mdesession.h>
#include <mdequery.h>
#include <mdelogiccondition.h>
#include <mdeconstants.h>
#include <mdeobjectquery.h>
#include <mdccommon.h>
#include <mdeitem.h>
#include <mdeobject.h>

// FORWARD DECLARATIONS
class MToneSelectionWatcher;
class CToneSelection;

/**
* CMFActiveCaller
*
* CMFActiveCaller is used for generating a call from active scheduler.
* Typical use is to start some operation after a short delay.
*/
NONSHARABLE_CLASS (CMFActiveCaller) : public CActive
    {
    public:
        static CMFActiveCaller* NewL( CToneSelection* aObserver );
        virtual ~CMFActiveCaller();

    private:
        CMFActiveCaller( CToneSelection* aObserver );
        void ConstructL();

    public:
        void Start( TInt aMilliseconds );
        void Stop();
        void Request();

    private:
        void RunL();
        void DoCancel();
        
    private:
        // timer
        RTimer iTimer;

        // observer that gets called
        CToneSelection* iObserver;
    };

// CLASS DECLARATION
/**
 *  This class is used for quering tones from mde.
 *
 */
class CToneSelection : public CBase, 
                       public MMdESessionObserver,
                       public MMdEQueryObserver,
                       public MMdEObjectObserver,
                       public MMdEObjectPresentObserver                            
    {
    public:
        enum TStorageType
            {
            EPhoneMemory = 0, ERomStorage, EMassStorage, EMemoryCard
            };
        
        enum TQueryAttribute
            {
            EAttrMediaType = 20, // integer
            EAttrFileSize, // integer
            EAttrStorageType, // integer
            EAttrMediaFileId, // integer
            EAttrFileName, // string
            EAttrFullName, // string
            EAttrSongName, // string
            EAttrArtist, // string
            EAttrAlbum, // string
            EAttrGenre, // string
            EAttrComposer
            };
    
    public:
        static CToneSelection* NewL( MToneSelectionWatcher *aWatcher );
        static CToneSelection* NewLC( MToneSelectionWatcher *aWatcher );
        
        virtual ~CToneSelection();
        //iTimer's callback function.
        void ChangeObject();
        void QueryTonesL();   
        void SetAttr( int attr, int value );
        TBool QueryReady();
    
    private:   
        CToneSelection(  MToneSelectionWatcher *aWatcher );
        void ConstructL(); 
        void ExcludeMusicPropertiesL( CMdELogicCondition& aCondition );    
        void LeaveIfSessionClosedL();
        CMdEPropertyDef& PropertyDefL(TInt aAttr);
        CMdEPropertyDef& PropertyDefL(CMdESession* aSession, TInt aAttr);
    
    private:
        // from MMdESessionObserver
        void HandleSessionOpened( CMdESession& aSession, TInt aError );
        void HandleSessionError( CMdESession& aSession, TInt aError );
    
    private:
        // from MMdEQueryObserver (mdequery.h)
        void HandleQueryNewResults( CMdEQuery& aQuery, TInt aFirstNewItemIndex,
                TInt aNewItemCount );
        void HandleQueryCompleted( CMdEQuery& aQuery, TInt aError );    
    private:
        // from MMdEObjectObserver
        void HandleObjectNotification( CMdESession& aSession,
                TObserverNotificationType aType,
                const RArray<TItemId>& aObjectIdArray );
    
    private:
        // from MMdEObjectPresentObserver
        void HandleObjectPresentNotification( CMdESession& aSession,
                TBool aPresent, const RArray<TItemId>& aObjectIdArray);
        void AddObjectObserverL();
        void HandleObjectChanged();
    private:
    
        MToneSelectionWatcher* iToneSelectionWatcher;
    
        // session to metadata engine
        CMdESession* iSession;
        
        CMdENamespaceDef* iDefNS; 
    
        // metadata query
        CMdEObjectQuery* iQuery;
        TBool iIsQuerying;
    
        // used for saving the quering result.
        RPointerArray<TDesC> iResultArray;
    
        // is metadata session open
        TBool iSessionOpen;
    
        // max audio file file size
        TInt iMaxFileSize;
        
        TInt iMediaFileCounter;
        // for generating active object calls
        CMFActiveCaller* iObjectNotificationCaller;
        
    };
#endif /* CTONESELECTION_H */

