/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    MMF ROP plugin specific settings model.*
*/



// Version : %version: 6 %




#ifndef MPSETTINGSMODELFORROP_H
#define MPSETTINGSMODELFORROP_H

//  INCLUDES
#include    <e32base.h> 
#include    <centralrepository.h>
#include    <mmf/common/mmfcontroller.h>
#include    "mpsettingsmodel.h"

class CMPRopSettingItem;
class CMPSettingsRopConfigParser;
class CCommsDatabase;
class CApUtils;

// CLASS DECLARATION

/**
*  CMPSettingsModelForROP
*  MMF ROP plugin specific settings model.
*
*  @lib MPSettEngine.dll
*  @since 2.0
*/
class CMPSettingsModelForROP : public CMPSettingsModel
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMPSettingsModelForROP* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMPSettingsModelForROP();

    public: // Functions from base classes

        /**
        * From MMPSettingsModel, provides uid of the MMF controller,
        * which settings this class handles
        * @since 2.0
        * @return MMF controller UID
        */
        const TUid MMFControllerID();
        
        /**
        * From MMPSettingsModel,
        * @since 2.0
        */
        void LoadSettingsL(TInt aConfigVersion);

        /**
        * From MMPSettingsModel, 
        * @since 2.0
        */
        void StoreSettingsL();

        /**
        * From MMPSettingsModel, sets video contrast
        * @since 2.0
        * @param aContrast Contrast level
        * @return Error value.
        */
        TInt SetVideoContrast(TInt aContrast);

        /**
        * From MMPSettingsModel, returns video contrast level.
        * @since 2.0
        * @param aContrast Contrast level:
        * @return Error value.
        */
        TInt GetVideoContrast(TInt& aContrast);

        /**
        * From MMPSettingsModel, sets proxy mode..
        * @since 2.0
        * @param aMode Proxy mode.
        * @return Error value.
        */
        TInt SetProxyMode(TInt aMode);
        
        /**
        * From MMPSettingsModel, returns proxy mode.
        * @since 2.0
        * @param aMode Proxy mode:
        *         0:    Disabled
        *         1:    Enabled
        * @return Error value.
        */
        TInt GetProxyMode(TInt& aMode);

        /**
        * From MMPSettingsModel, sets Proxy host name.
        * @since 2.0
        * @param aHostName Proxy host name.
        * @return Error value.
        */
        TInt SetProxyHostNameL(const TDesC& aHostName);
        
        /**
        * From MMPSettingsModel, returns proxy host name.
        * @since 2.0
        * @param aHostName Proxy host name.
        * @return Error value.
        */
        TInt GetProxyHostName(TDes& aHostName);

        /**
        * From MMPSettingsModel, sets proxy port number.
        * @since 2.0
        * @param aPort Proxy port number.
        * @return Error value.
        */
        TInt SetProxyPort(TInt aPort);
        
        /**
        * From MMPSettingsModel, returns proxy port number..
        * @since 2.0
        * @return Integer: Proxy port number.
        * @return Error value.
        */
        TInt GetProxyPort(TInt& aPort);
        
        /**
        * From MMPSettingsModel, sets default access point.
        * @since 2.0
        * @param aApId Access point ID.
        * @return Error value.
        */
        TInt SetDefaultAp(TUint32 aApId);

        /**
        * From MMPSettingsModel, returns default access point ID.
        * @since 2.0
        * @param aApId Access point ID.
        * @return Error value.
        */
        TInt GetDefaultAp(TUint32& aApId);

        /**
        * Sets bandwidth control mode.
        * @since 2.0
        * @param aMode bandwidth control mode.
        * @return Error value.
        */
        TInt SetBandwidthControlMode(TInt aMode);
        
        /**
        * Returns bandwidth control mode.
        * @since 2.0
        * @param aMode bandwidth control mode:
        *           0: Use dynamic bw control if possible
        *           1: Use static bw control
        * @return Error value.
        */
        TInt GetBandwidthControlMode(TInt& aMode);

        /**
        * From MMPSettingsModel, sets maximum bandwidth.
        * @since 2.0
        * @param aMaxBw Maximum bandwidth.
        * @param aBearer Bearer, which max bw is set.
        * @return Error value.
        */
        TInt SetMaxBandwidth(TInt aMaxBw, TDataBearer aBearer);
        
        /**
        * From MMPSettingsModel, returns maximum bandwidth.
        * @since 2.0
        * @param aMaxBw Maximum bandwidth.
        * @param aBearer Bearer, which max bw is enquired.
        * @return Error value.
        */
        TInt GetMaxBandwidth(TInt& aMaxBw, TDataBearer aBearer);

        /**
        * From MMPSettingsModel, sets connection timeout.
        * @since 2.0
        * @param aTimeout Timeout in minutes.
        * @return Error value.
        */
        TInt SetConnectionTimeout(TInt aTimeout);
        
        /**
        * From MMPSettingsModel, returns connection timeout.
        * @since 2.0
        * @param aTimeout Timeout in minutes.
        * @return Error value.
        */
        TInt GetConnectionTimeout(TInt& aTimeout);
        
        /**
        * From MMPSettingsModel, sets server timout.
        * @since 2.0
        * @param aTimeout Timeout in minutes.
        * @return Error value.
        */
        TInt SetServerTimeout(TInt aTimeout);
        
        /**
        * From MMPSettingsModel, returns server timeout.
        * @since 2.0
        * @param aTimeout Timeout in minutes.
        * @return Error value.
        */
        TInt GetServerTimeout(TInt& aTimeout);

        /**
        * From MMPSettingsModel, sets minimum UDP port number.
        * @since 2.0
        * @param aPort minimum port number.
        * @return Error value.
        */
        TInt SetMinUDPPort(TInt aPort);

        /**
        * From MMPSettingsModel, returns minimum UDP port number.
        * @since 2.0
        * @param aPort minimum UDP port number in minutes.
        * @return Error value.
        */
        TInt GetMinUDPPort(TInt& aPort);

        /**
        * From MMPSettingsModel, sets maximum UDP port number.
        * @since 2.0
        * @param aPort maximum port number.
        * @return Error value.
        */
        TInt SetMaxUDPPort(TInt aPort);

        /**
        * From MMPSettingsModel, returns maximum UDP port number.
        * @since 2.0
        * @param aPort maximum UDP port number in minutes.
        * @return Error value.
        */
        TInt GetMaxUDPPort(TInt& aPort);

        /**
        * Provides MMF controller version information.
        * @since 2.0
        * @param aVersion Controller version number.
        * @return Error value.
        */
        TInt GetControllerVersionInfo(TDes& aVersion);
            
        /**
        * Provides MMF controller's build date.
        * @since 2.0
        * @param aBldDate Controller build date.
        * @return Error value.
        */
        TInt GetControllerBuildDate(TDes& aBldDate);

        /**
        * Provides additional information about MMF controller.
        * @since 2.0
        * @param aAdditionalInfo Additional controller information.
        * @return Error value.
        */
        TInt GetControllerAdditionalInfo(TDes& aAdditionalInfo);

        /**
        * Sets demand bandwidth factor.
        * @since 2.1
        * @param aFactor Demand bandwidth factor.
        * @return Error value.
        */
        TInt SetDemandBwFactor(TInt aFactor);

        /**
        * Returns demand factor bandwidth.
        * @since 2.1
        * @param aFactor Demand bandwidth factor.
        * @return Error value.
        */
        TInt GetDemandBwFactor(TInt& aFactor); 

        /**
        * Sets sustainable bandwidth.
        * @since 2.1
        * @param aSustainBw Sustainable bandwidth.
        * @param aBearer Bearer, which max bw is set.
        * @return Error value.
        */
        TInt SetSustainBandwidth(TInt aSustainBw, TDataBearer aBearer);
        
        /**
        * Returns sustainable bandwidth.
        * @since 2.1
        * @param aSustainBw Sustainable bandwidth.
        * @param aBearer Bearer, which max bw is enquired.
        * @return Error value.
        */
        TInt GetSustainBandwidth(TInt& aSustainBw, TDataBearer aBearer);

        /**
        * Returns an array of the fixed sustainable bandwidth values.
        * @since 2.1
        * @param aBwArray Fixed sustain bandwidth values are copied to this array.
        * @param aBearer Bearer, which fixed sustain bw values is enquired.
        * @return Error value.
        */
        TInt GetSustainBwPresetsL(RArray<TInt>& aBwArray, TDataBearer aBearer);

        /**
        * Returns an array of the fixed max bandwidth values.
        * @since 2.1
        * @param aBwArray Fixed max bandwidth values are copied to this array.
        * @param aBearer Bearer, which fixed max bw values is enquired.
        * @return Error value.
        */
        TInt GetMaxBwPresetsL(RArray<TInt>& aBwArray, TDataBearer aBearer);

     public:  // value to / from MediaPlayer Repository
	/**
        * Returns video repeat mode.
        * @since 2.0
        * @return ETrue:    On
        *         EFalse:   Off
        */
        TBool IsVideoRepeatOnL();

        /**
        * Sets video repeat mode. Deprecated.
        * @since 2.0
        * @param aRepeat Repeat mode.
        */
        void SetVideoRepeatL(const TBool aRepeat);

		/**
        * Returns default view mode. Deprecated.
        * @since 3.2
        * @return ETrue:    On  (Normal-screen view)
        *         EFalse:   Off (Full-screen view)
        */
        TBool IsDefaultViewOnL();

        /**
        * Sets default view mode.
        * @since 3.2
        * @param aView Default view mode.
        */
        void SetDefaultViewL(const TBool aView);

		/**
        * Returns if Rocker Keys Feature is supported
        * @since 3.2
        * @return ETrue:    - Rocker Keys feature is supported
        *         EFalse:   - Rocker Keys feature is not supported
        */
        TBool IsRockerKeysSupportedL();

        /**
        * Sets rocker keys mode.
        * @since 3.2
        * @param aRockerKeys rocker keys mode.
        */
        void SetRockerKeysL(const TBool aRockerKeys);

		/**
        * Returns Rocker Keys mode
        * @since 3.2
        * @return ETrue:    Show (Rocker Keys are shown)
        *         EFalse:   Hide (Rocker Keys are hidden)
        */
		TBool ShowRockerKeysL();

	/**
        * Sets Media Player auto disconnection time.
        * @since 2.0
        * @param aTime Auto disconnection time.
        */
        void SetAutoDisconTimeL(const TInt aTime);

        /**
        * Returns Media Player auto disconnection time.
        * @since 2.0
        * @return 0:    Unlimited time
        *         1-99: Time in minutes
        */
        TInt AutoDisconTimeL();


    private:

        /**
        * C++ default constructor.
        */
        CMPSettingsModelForROP();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
        * Function for setting an integer value to iItems
        * array according to item id.
        */
        TInt SetIntegerValue(TInt aId, TInt aValue);

        /**
        * Function for getting an integer value from iItems
        * array according to item id.
        */
        TInt GetIntegerValue(TInt aId, TInt& aValue);

        /**
        * Function for getting an string value from iItems
        * array according to item id.
        */
        TInt GetStringValue(TInt aId, TDes& aString);

        /**
        * Resets aArray and populates it with values from iIntArray.
        * aId is the id of an item from iItems, which iIntArray
        * is copied.
        */
        TInt GetArrayL(TInt aId, RArray<TInt>& aArray);

        /**
        * Finds item from iItems according to it's id.
        */
        TInt GetItem(TInt aId, CMPRopSettingItem*& aItem);

        /**
        * Locates the resource file searching from y: to a: and then z: last.
        *
        * @param aFileName Full path to resource file.
        * @param aFs File server session.
        */
        void LocateResourceFileL( TFileName& aFileName, RFs& aFs );
    
        /**
         * Gets WAP id from IAP id. Leaves with KErrNotFound if no record with given 
         * IAP id is found.
         * 
         * @return WAP id matching the given IAP id.
         */
        TUint32 WapIdFromIapIdL( TUint32 aIapId );

        /**
         * Gets WAP id from IAP id. Leaves with KErrNotFound if no record with given 
         * IAP id is found.
         * 
         * @return WAP id matching the given IAP id.
         */
        TUint32 IapIdFromWapIdL( TUint32 aWapId );        
        
    private: // new
           
	void InitializeCentralRepositoryL();
	void UninitializeCentralRepository();


    private:    // Data

        //Central repository objects
        CRepository* iClient;
		CRepository* iMediaPlayerClient;

        CArrayPtr<CMPRopSettingItem>* iItems;
        CMPSettingsRopConfigParser* iParser;

        HBufC8* iROPSettings;
        HBufC8* iROPHeader;

        RMMFController iMMFController;      
        CCommsDatabase* iCommDb;
        CApUtils* iApUtils;
    };

#endif      // MPSETTINGSMODELFORROP_H   
            
// End of File
