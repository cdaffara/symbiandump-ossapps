/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/

 
#ifndef CIRCACHEMGMT_H
#define CIRCACHEMGMT_H

#include <d32dbms.h>


const TInt KMAXHEADER = 255;

class CIRBrowseCatagoryItems;
class CIRBrowseChannelItems;
class CIRCacheCleanup;
class CIRHttpResponseData;
class CIRIsdsPreset;
class CIROTAUpdate;
class CIRSettings;
class MIRCacheObserver;

class CIRCacheMgmt : public CObject
    {
    
public:

    /**
    *CIRCacheMgmt::OpenL()
    *Standard two phased construction
    *calls ConstructL()
    *@param MIRCacheObserver &
    *@return CIRCacheMgmt*
    */
    IMPORT_C static CIRCacheMgmt* OpenL(MIRCacheObserver &aObserver);

    /**
    * Adds a cache observer
    *
    * @param aObserver The observer to be added
    */
    IMPORT_C void AddObserverL( MIRCacheObserver* aObserver );
    
    /**
    * Removes a cache observer
    *
    * @param aObserver The observer to be removed
    */
    IMPORT_C void RemoveObserver( MIRCacheObserver* aObserver );
  
    /**
    *CIRCacheMgmt::CheckCache()
    *API Exposed to Isds Client to check and get the cached items.
    *@ aType the type of items i.e category,channel or preset. aName is the 
    *file name(type,CategoryId,PresetId
    *@return TInt,1 for a cache hit,0 for cache miss,-1 for invalid cache
    *
    */
    IMPORT_C void CheckCacheL(TInt aType,const TDesC& aName,
        TBool aForceGet, TInt& aReturn);

    /**
    *CIRCacheMgmt::CheckValidity()
    *Checks the freshness of the cache
    *@param universal time of file creation
    *@return TBool 1 if fresh,0 if stale
    */ 
    TBool CheckValidity(const TTime &aCreateTime,TInt aTrustPeriod) const;

  
    /**
    *CIRCacheMgmt::CacheCategoryItems()
    *Caches the category array of data.By externalizing it into a file.
    *@param aPtrCategory the object array,aName the file name from which 
    *the information is to be written
    */ 
    IMPORT_C  void CacheCategoryItemsL(
        CArrayPtrFlat<CIRBrowseCatagoryItems>& aPtrCategory,
        const TDesC& aFilePath,const CIRHttpResponseData& aResponseHeaders);

    /**
    *CIRCacheMgmt::CacheChannelItems()
    *Caches the channel objects array of data.By externalizing it into a file.
    *@param aPtrChannel the object array,aName the file name from which 
    *the information is to be written
    */
    IMPORT_C  void CacheChannelItemsL(
        CArrayPtrFlat<CIRBrowseChannelItems>& aPtrChannel,
        const TDesC& aFilePath,const CIRHttpResponseData& aResponseHeaders);

    /**
    *CIRCacheMgmt::CachePresetItem()
    *Caches the preset objects .By externalizing it into a file.
    *Multiple presets can be cached.
    *@param aPreset preset object,aName the file name from which 
    *the information is to be written
    */
    IMPORT_C  void CachePresetItemL(
        CArrayPtrFlat<CIRIsdsPreset>& aPtrPresets,
        const TDesC& aName,const CIRHttpResponseData& aResponseHeaders);

    /**
    *Function : CIRCacheMgmt::CacheOtaInfoL()
    * Caches the OTA information  .By externalizing it into a db.
    *@param aOta otainfo object,aName the file name from which 
    *the information is to be written
    */
    IMPORT_C void CacheOtaInfoL(const CIROTAUpdate& aOta,const TDesC& aName,
        const CIRHttpResponseData& aResponseHeaders);
                
                
    ////////////////////////////////////////////////////////////////////////
    // CacheLogoL() API is added newly for logo cache management          //
    ////////////////////////////////////////////////////////////////////////              
    /**
    *CIRCacheMgmt::CacheLogoL()
    * Caches the logo data for a given logo url.
    *@param aData --- logo data
    *@param aUrl  --- logo url
    */                
    IMPORT_C void CacheLogoL(const TDesC8& aData, const TDesC& aUrl,
        const CIRHttpResponseData& aResponseHeaders);
                
                
    /**
    *CIRCacheMgmt::UpdateTrustPeriodL()
    *Modifies the Trust period that comes with the 304 
    *Not Modified responces from isds
    *@ aType the type of items i.e category,channel or preset.
    *aName is the file name(type,CategoryId,PresetId
    */    
    IMPORT_C void UpdateTrustPeriodL(TInt aType, const TDesC& aName,
        CIRHttpResponseData& aResponseHeaders);

    /**
    *CIRCacheMgmt::RemoveOtaInfoL() 
    *removes any cached ota response in case it is invalid
    */
    IMPORT_C void RemoveOtaInfoL();
    
    /**
    *CIRCacheMgmt::CheckSizeL()
    *checks the current size of cache against the max cache limit
    */  
    void CheckSizeL();
    /**
    *CIRCacheMgmt::RemoveOldUnusedDataL()
    *removes all unused data to bring the cache under the max cache size
    */  
    void RemoveOldUnusedDataL();
    /**
    *CIRCacheMgmt::CacheSize()
    *@return the cache size
    */  
    TInt CacheSize();
    
private:

    /**
    *CIRCacheMgmt::CIRCacheMgmt()
    *Standard C++ constructor
    *@param MIRCacheObserver &
    *@return NA
    *sets the trust period to 24hrs(default)
    */
    CIRCacheMgmt(MIRCacheObserver &aObserver);
    
    /**
    *CIRCacheMgmt::ConstructL()
    *Standard two phased construction
    *@param void
    *@return void
    *creates the folder paths if nonexistant
    */
    void ConstructL();
  
    /**
    *CIRCacheMgmt::~CIRCacheMgmt()
    *Standard C++ destructor
    *@param NA
    *@return NA
    */
    ~CIRCacheMgmt();
  
    /**
    *CIRCacheMgmt::CreateCacheTableL()
    *Creates the cache table
    *CacheTable
    * -----------------------------------------------------------------------------------------------
    *| RowIndex|DataType|DataId|TrustPeriod|LastModified  |LastAccessed|Created|ItemCount|CachedData|
    *------------------------------------------------------------------------------------------------
    *| auto    |TInt    |TDesC |TInt       |TDesC/DateTime|TTime       |Time   |TInt     | Streamed |
    *|increment|0,1,2 !0| !0   |  !0       |              |            |       |         | Data     |
    *------------------------------------------------------------------------------------------------
    */
    void CreateCacheTableL();
    
    /**
    *CIRCacheMgmt::CreateCacheIndexL()
    *creates the indices for cache table
    */
    void CreateCacheIndexL();
    
    void FetchCacheIfAvailableL(TInt aType,const TDesC& aId,
        TBool aForceGet,TInt& aReturnVal);
        
    void FetchCachedDataL(TInt aType,TInt aCountItems,RDbView& aCacheView);
    
    void CloseDb();
    
    TInt OpenCacheDb();
    
    TInt CreateDb();
    
    void CreateDbConditionalL();    
    /** 
    *CIRCacheMgmt::UpdateLastAccessedTime() 
    *updates the last accessed field to aid in deletion of old data
    *@param RDbView&,the view for which the update has to be made
    */
    void UpdateLastAccessedTimeL(RDbView &aCacheView);
  
public:
    //!Enumeration for type identification
    enum TIRTypes
      {
      ECatagory = 0,
      EChannels,
      EPresets,
      EOtaInfo,
      ELogo   //added for logo cache management
      };
      
    //!Enumeration for return types     
    enum TIRCacheStatus
      {
      ECacheNotValid = -1,
      ENotCached,
      ECacheUseable
      };
      
    //! trust period for cache(24hrs by default)
    TTimeIntervalSeconds iTrustPeriod;
    
    // Array of Pointers to PresetClass
    //used to return an array of cached presets to the UI
    CArrayPtrFlat<CIRIsdsPreset>* iPtrPreset;
    
    //! Array of Pointers to CategoryClass  
    CArrayPtrFlat<CIRBrowseCatagoryItems>* iPtrCategory;
    
    //! Array of Pointers to BrowseCategoryClass
    CArrayPtrFlat<CIRBrowseChannelItems>* iPtrChannel;
    
    //ota info holding object
    CIROTAUpdate* iOTA;
    //!Observer class for cache
    MIRCacheObserver& iCacheObserver;
    
    //To store the logo data which is retrieved from the cache
    RBuf8 iLogoData;
    
    //!time stamp string in HTTP format,used if cache is stale
    TTime iLastModified;
    
    //ETag header assaociated witht the logo data
    TBuf8< KMAXHEADER > iETag;
  
private:
    
    /** 
    * Array of cache observers
    */
    RPointerArray<MIRCacheObserver> iCacheObservers;
    
    RDbNamedDatabase iCacheDb;
    
    TBool iOpen;
    
    //! fileserver session
    RFs iFsSession;
    
    TFileName iDatabaseFileName;
    
    //central repository settings handle
    CIRSettings* iSettings;
    
    CIRCacheCleanup* iCleanup;
    
    TInt iCacheDbSize;
    
    };
    
#endif //CIRCACHEMGMT_H
