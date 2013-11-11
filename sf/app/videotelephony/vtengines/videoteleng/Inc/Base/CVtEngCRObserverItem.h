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
* Description:  Central repository observer class.
*
*/


#ifndef CVTENGCROBSITEM_H
#define CVTENGCROBSITEM_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "cenrepnotifyhandler.h"

// FORWARD DECLARATIONS
class MVtEngCRSettingObserver;

// CLASS DECLARATION

/**
*  Central repository listener for video telephony.
*
*  @lib videoteleng.lib
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS( CVtEngCRObserverItem ) : public CBase,
    public MCenRepNotifyHandlerCallback
    {
    public:  // Constructors and destructor.
        
        /**
        * Two-phased constructor.
        * @param aObserver the real observer to notify
        * @param aUid UID for this listener
        * @param aKey ID for this listener
        * @param aType type of information interested in
        * @return this CR observer
        */
        static CVtEngCRObserverItem* NewL( 
            MVtEngCRSettingObserver* aObserver,
            TUid aUid, TUint32 aKey, 
            CCenRepNotifyHandler::TCenRepKeyType aType );
        
        /**
        * Destructor.
        */
        virtual ~CVtEngCRObserverItem();

    public: // From MCenRepNotifyHandlerCallback.

        /**
        * Callback to notify changed integer setting.
        * @param aId changed id
        * @param aNewValue 
        */
        void HandleNotifyInt( TUint32 aId, TInt aNewValue );

        /**
        * Callback to notify changed real setting.
        * @param aId changed id
        * @param aNewValue 
        */
        void HandleNotifyReal( TUint32 aId, TReal aNewValue );

        /**
        * Callback to notify changed string setting.
        * @param aId changed id
        * @param aNewValue 
        */
        void HandleNotifyString( TUint32 aId, const TDesC16& aNewValue );

        /**
        * Callback to notify changed binary setting.
        * @param aId changed id
        * @param aNewValue 
        */
        void HandleNotifyBinary( TUint32 aId, const TDesC8& aNewValue );

        /**
        * Callback to notify changed setting.
        * @param aId changed id
        */
        void HandleNotifyGeneric( TUint32 aId);

        /**
        * Callback to notify of an occurred error.
        * @param aId changed id
        * @param aError 
        * @param aHandler which handler
        */
        void HandleNotifyError( TUint32 aId, TInt aError, 
            CCenRepNotifyHandler* aHandler );
        
        public: // New functions.
        
        /**
        * Checks if this one matches given UID and key.
        * @param aUID UID to use in comparison 
        * @param aKey key to use in comparison
        * @return ETrue if match
        */
        TBool Offer( const TUid aUid, const TUint32 aKey ) const;
        
        /**
        * Checks if this one matches given UID.
        * @param aUID UID to use in comparison 
        * @return ETrue if match
        */
        TBool Offer( const TUid aUid ) const;
        
        /**
        * Checks if this one matches given key.
        * @param aKey key to use in comparison 
        * @return ETrue if match
        */
        TBool Offer( const TUint32 aKey ) const;
        
        /**
        * Returns UID of this handler.
        * @return UID
        */
        TUid Uid() const;

        /**
        * Returns key of this handler.
        * @return key
        */
        TUint32 Key() const;
        
        /**
        * Returns notifier of this handler.
        * @return notifier
        */
        CCenRepNotifyHandler* Notifier() const;
        
        /**
        * Returns type of this handler.
        * @return type
        */
        CCenRepNotifyHandler::TCenRepKeyType Type() const;
        
        /**
        * Returns observer of this handler.
        * @return observer
        */
        MVtEngCRSettingObserver* Observer() const;
        
        /**
        * Sets CenRep notifier
        * @param notifier to set.
        */
        void SetCenRepNotifier( CCenRepNotifyHandler* aNotifier );
        
        
    private: // Constructor.

        /**
        * C++ default constructor.
        * @param aObserver the real observer to notify
        * @param aUid UID for this listener
        * @param aKey ID for this listener
        * @param aType type of information interested in
        */
        CVtEngCRObserverItem( MVtEngCRSettingObserver* Observer,
            TUid aUid, TUint32 aKey, 
            CCenRepNotifyHandler::TCenRepKeyType aType );

    private:    // Data

        // Callback for notifications.
        MVtEngCRSettingObserver* iObserver;

        // UID for this item.
        TUid                     iUid;

        // Key for this item.
        TUint32                  iKey;

        // Type of listener.
        CCenRepNotifyHandler::TCenRepKeyType iType;

        // Central repository notifier.
        CCenRepNotifyHandler*   iNotifier;
};
#endif      // CVTENGCROBSITEM_H
            
// End of File
