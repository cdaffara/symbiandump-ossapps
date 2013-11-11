/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Profile container class
*
*/



#ifndef CNSMLDMSYNCPROFILE_H
#define CNSMLDMSYNCPROFILE_H

//  INCLUDES
#include <e32base.h>
#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include "nsmldmdef.h"

//#include "NSmlDMSyncApp.h"


/**
* CNSmlDMSyncProfile 
*
* CNSmlDMSyncProfile is used for getting/setting sync profile settings.
*/
NONSHARABLE_CLASS (CNSmlDMSyncProfile) : public CBase
    {
	enum TOpenMode
		{
	    EOpenRead,
	    EOpenReadWrite
		};
		
    public:
        /**
        * Two-phased constructor.
        * @param aParam
        */
        static CNSmlDMSyncProfile* NewLC( const TInt aApplicationId,
                                          RSyncMLSession* aSyncSession);

        /**
        * Two-phased constructor.
        * @param aParam
        */
        static CNSmlDMSyncProfile* NewL( const TInt aApplicationId,
                                         RSyncMLSession* aSyncSession );

	    /**
        * Destructor.
        */
        virtual ~CNSmlDMSyncProfile();

    private:
        /**
        * C++ default constructor.
        * @param aParam
        */
	    CNSmlDMSyncProfile( const TInt aApplicationId,
	                        RSyncMLSession* aSyncSession );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();
	    
    public:
        /**
		* Open profile
        * @param aProfileId Profile id to open
        * @param aOpenMode Opening mode
		* @return None
        */
        void OpenL( TInt aProfileId, TInt aOpenMode );
        
        /**
		* Create profile
        * @param None.
		* @return Profile ID.
        */
        TInt CreateL();
        
        /**
		* Create copy profile
        * @param aProfileId Profile that is used for copying
		* @return None
        */
        void CreateCopyL( TInt aProfileId );
        
        /**
		* Save profile
        * @param None
		* @return None
        */
        void SaveL();
        
	public:
        
        /**
		* Set name for profile
        * @param aText name for profile
		* @return None
        */
        void SetNameL(const TDesC& aText);
        
        /**
		* Get profile name
        * @param aText name for profile
		* @return None
        */
        void GetName( TDes& aText );
        
        /**
		* Set creator id for the profile
        * @param aCreatorId creator id
		* @return None
        */
        void SetCreatorId(TInt aCraetorId);
        
        /**
		* Get creator id
        * @param None
		* @return creator id
        */
        TInt CreatorId();
        
        /**
		* Get profile id
        * @param None
		* @return Currently open profile id
        */
        TInt ProfileId();
        
        /**
		* Check if delete is allowed
        * @param None
		* @return ETrue if allowed
        */
        TBool DeleteAllowed();
        
        
        TBool ProfileLocked();
        /**
		* Has profile been synced
        * @param None
		* @return ETrue if synced
        */
		TBool IsSynced();
        
        /**
		* Last sync time
        * @param None
		* @return Time of sync
        */
		TTime LastSync();
		
		/**
		* Last Success sync time
        * @param None
		* @return Time of sync
        */
		TTime LastSuccessSync();

    // settings that connection dialog displays
    public:
        /**
		* Set server id
        * @param aText server id
		* @return None
        */
        void SetServerIdL(const TDesC& aText);
        
        /**
		* Get server id
        * @param aText server id
		* @return NOne
        */
        void GetServerId(TDes& aText);
        
        /**
		* Get server password
        * @param aText server password
		* @return None
        */
        void GetServerPassword( TDes& aText );
        
        /**
		* Set server password
        * @param aText server password
		* @return None
        */
        void SetServerPasswordL( const TDesC& aText );
        
        /**
		* Get protocol version
        * @param None
		* @return Version
        */
        TInt ProtocolVersion();
        
        /**
		* Set Protocol version
        * @param aProtocolVersion protocol version
		* @return None
        */
        void SetProtocolVersionL( TInt aProtocolVersion );
        
        /**
		* Get bearer type
        * @param None
		* @return bearer type
        */
        TInt BearerType();
        
        /**
		* Set bearer type
        * @param aId bearer type
		* @return None
        */
        void SetBearerTypeL( TInt aId );
        
        /**
		* Get accesspoint
        * @param None
		* @return accespoint
        */
        TInt AccessPointL();
        
        /**
		* Set accesspoint
        * @param aId accespoint id
		* @return None
        */
        void SetAccessPointL(const TInt aId);
        
        /**
		* Get host address
        * @param aText host address
        * @param aPort host port
		* @return None
        */
        void GetHostAddress( TDes& aText, TInt& aPort );
        
        /**
		* set host address
        * @param aText host address
        * @param aPort host 
		* @return None
        */
        void SetHostAddressL( const TDesC& aText, const TInt aPort );
        
        
        /**
         * converts backslash in host address to forward slash 
         * @param aUrl host address with out backslash
         * @param aModifiedUrl host address with out backslash
         * @return None
         */
        void convertBackslashinUrl( const TDesC& aUrl, TDes& aModifiedUrl );
        
        
        /**
		* Get port number 
        * @param None
		* @return port number
        */
        TInt PortNumber();
        
        /**
		* Set port number
        * @param aPortNumber port number
		* @return None
        */
        void SetPortNumber( TInt aPortNumber );
        
        /**
		* Get user name
        * @param aText user name
		* @return None
        */
        void GetUserName( TDes& aText );
        
        /**
		* Set user name
        * @param aText user name
		* @return None
        */
        void SetUserNameL( const TDesC& aText );
        
        /**
		* Get user password
        * @param aText user password
		* @return None
        */
        void GetPassword( TDes& aText );
        
        /**
		* Set user password
        * @param aText user password
		* @return None
        */
        void SetPasswordL( const TDesC& aText );
        
        /**
		* Set server alert state
        * @param aState server alert state
		* @return None
        */        
        void SetSASyncStateL( TInt aState );
        
        /**
		* Get Server alert state
        * @param None
		* @return server alert state
        */
        TInt SASyncState();
        
        /**
		* Set http used state
        * @param aEnable ETrue if http authentication needs to be used
		* @return None
        */
        void SetHttpUsedL( TBool aEnable );
        
        /**
		* Check whether http authentication is used
        * @param None
		* @return ETrue if http authentication is used
        */
        TBool HttpUsedL();
        
        /**
		* Get http user name
        * @param aText user name
		* @return None
        */        
        void GetHttpUserNameL( TDes& aText );
        
        /**
		* Set http user name
        * @param aText user name
		* @return None
        */
        void SetHttpUserNameL( const TDesC& aText );
        
        /**
		* Get http password
        * @param aText password
		* @return None
        */
        void GetHttpPasswordL( TDes& aText );
        
        /**
		* Set http password
        * @param aText password
		* @return None
        */
        void SetHttpPasswordL( const TDesC& aText );
 
               
    public:
       
        /**
		* Return latest history job
        * @param None
		* @return CSyncMLHistoryJob*
        */        
        const CSyncMLHistoryJob* LatestHistoryJob();
        
        /**
		* Open connection
        * @param None
		* @return None
        */
        void OpenConnection();
        
        /**
		* Open history log
        * @param None
		* @return None
        */
        void OpenHistoryLog();

        /**
		* Close history log
        * @param None
		* @return None
        */        
        void CloseHistoryLog();

        /**
		* Return DevMan profile
        * @param None
		* @return RSyncMLDevManProfile&
        */
        RSyncMLDevManProfile& Profile();
        
        /**
		* Return SyncML Session
        * @param None
		* @return RSyncMLSession&
        */
        RSyncMLSession& Session();
        
    public:
        /**
		* Get connection name
        * @param aText name
        * @param aPropertyPos position at the array
		* @return None
        */
        void GetConnectionPropertyNameL( TDes8& aText, TInt aPropertyPos );
        
    private:
    
        /**
		* Copy values from one profile to another
        * @param aSource source profile
		* @return None
        */    
        void CopyValuesL( CNSmlDMSyncProfile* aSource );

	private:
	    // profile id
        TInt                    iProfileId;
        // Log open
        TBool                   iHistoryLogOpen;
        // Connection open
        TBool                   iConnectionOpen;
        // Port number
        TInt                    iPortNumber;
   		// needed for setting creator id
		TInt                    iApplicationId;
		// long buffer for string handling 
		TBuf<KBufSize255>       iBuf;
		// long buffer for string handling 
		TBuf8<KBufSize255>      iBuf8;
   		// sync session
		RSyncMLSession*         iSyncSession;
		// sync profile
		RSyncMLDevManProfile    iProfile;
		// history log
		RSyncMLHistoryLog       iHistoryLog;
		// sync connection
		RSyncMLConnection       iConnection;
    };

#endif  // CNSMLDMSYNCPROFILE_H

// End of file
