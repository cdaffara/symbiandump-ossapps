/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Central repository proxy, which is used by all CR dependent.
*
*/



#ifndef CVTENGCRPROXY_H
#define CVTENGCRPROXY_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <centralrepository.h>

// FORWARD DECLARATIONS
class CVtEngCRObserverItem;
class CRepository;
class MVtEngCRSettingObserver;

// CLASS DECLARATION

/**
*  Central repository interface for video telephony.
*
*  @lib videoteleng.lib
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS( CVtEngCRProxy ) : public CBase
    {
    public: // Data structures.

        // Internal data structure for storing CR sessions.
        NONSHARABLE_CLASS( CCRSession ) : public CBase
            {
            public:
                /**
                * Destructor needed.
                */
                ~CCRSession();
           
            // CenRep session.
            CRepository*    iSession;
            // Uid attached to CenRep session.
            TUid            iUid;
            };

    public:  // Constructors and destructor.
        
        /**
        * Two-phased constructor.
        * @return instance of CVtEngCRProxy
        */
        static CVtEngCRProxy* NewL( );
        
        /**
        * Destructor.
        */
        virtual ~CVtEngCRProxy();

    public: // New functions

        /**
        * Sets integer value to Central Repository.
        * @param aUid used UID
        * @param aId which key to use
        * @param aVal a value to set
        * @return error code
        */  
        TInt Set( const TUid aUid, const TUint32 aId, TInt aVal );

        /**
        * Sets real value to Central Repository.
        * @param aUid used UID
        * @param aId which key to use
        * @param aVal a value to set
        * @return error code
        */  
        TInt Set( const TUid aUid, const TUint32 aId, const TReal& aVal );

        /**
        * Sets binary value to Central Repository.
        * @param aUid used UID
        * @param aId which key to use
        * @param aVal a value to set
        * @return error code
        */  
        TInt Set( const TUid aUid, const TUint32 aId, const TDesC8& aVal );

        /**
        * Sets string value to Central Repository.
        * @param aUid used UID
        * @param aId which key to use
        * @param aVal a value to set
        * @return error code
        */  
        TInt Set( const TUid aUid, const TUint32 aId, const TDesC16& aVal );
        
        /**
        * Gets integer value from Central Repository.
        * @param aUid used UID
        * @param aId which key to use
        * @param aVal a value in return
        * @return error code
        */
        TInt Get( const TUid aUid, const TUint32 aId, TInt& aVal );

        /**
        * Gets real value from Central Repository.
        * @param aUid used UID        
        * @param aId which key to use
        * @param aVal a value in return
        * @return error code
        */
        TInt Get( const TUid aUid, const TUint32 aId, TReal& aVal );

        /**
        * Gets binary value from Central Repository
        * @param aUid used UID        
        * @param aId which key to use
        * @param aVal a value in return
        * @return error code
        */
        TInt Get( const TUid aUid, const TUint32 aId, TDes8& aVal );
        
        /**
        * Gets string value from Central Repository.
        * @param aUid used UID        
        * @param aId which key to use
        * @param aVal a value in return
        * @return error code
        */
        TInt Get( const TUid aUid, const TUint32 aId, TDes16& aVal );

        /**
        * Add a new Cen Rep interested object.
        * @param aObserver callback interface
        */
        void RegisterInterestedL( CVtEngCRObserverItem* aObserver );

        /**
        * Removes an observer.
        * @param aObserver to be removed
        */
        void RemoveInterested( CVtEngCRObserverItem& aObserver );
        
    private: // New functions

        /**
        * Gets a Cen Rep session matching UID.
        * @param aUid UID to use in search
        * @param aCRSession found session or a new session
        * @return error code
        */
        TInt GetSessionL( const TUid aUid, CRepository*& aCRSession );    

        /**
        * Creates an id-CenRep paired session.
        * @param aUid the UID to use
        * @return new session or NULL 
        */    
        CCRSession* NewCRSessionL( TUid aUid );
    
        /**
        * Finds observer item.
        * @param aKey
        * @param aIndex
        * @return observer item or NULL 
        */    
        CVtEngCRObserverItem* FindItem( 
            const TUint32 aKey, TInt& aIndex ) const;

        /**
        * Removes all observers
        * 
        */
        void RemoveAllObservers();

        /**
        * Removes all Central repository instances
        * 
        */
        void RemoveSessions();

        /**
        * Maps a given UID to a certain central repository instance
        * @param aUid the UID to map
        * @return found CRepository item or NULL
        */
        CRepository* MapUidToCR( const TUid aUid );

        /**
        * Finds observer item.
        * @param aUid 
        * @param aKey
        * @param aIndex
        * @return observer item or NULL 
        */
        CVtEngCRObserverItem* FindItem( const TUid& aUid, const TUint32 aKey,
                TInt& aIndex ) const;

    private:

        /**
        * C++ default constructor.
        */
        CVtEngCRProxy();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
    
    private:    // Data

        // Storage for data observers.
        RPointerArray<CVtEngCRObserverItem> iObserverItems;

        // Array for central repository sessions.
        RPointerArray<CVtEngCRProxy::CCRSession> iCRSessions;
    };

#endif      // CVTENGCRPROXY_H
            
// End of File
