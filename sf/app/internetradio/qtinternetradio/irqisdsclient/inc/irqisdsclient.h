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
*
*/

#ifndef IRQISDSCLIENT_H
#define IRQISDSCLIENT_H

#include <QObject>
#include <e32base.h>
#include <badesca.h>
#include <QList> 
#include <QMutex>
#include "irqenums.h" 
#include "irqisdsclientexporter.h"

class IRQAbstractIsdsClientImpl;

class IRQIsdsClientImpl;
class IRQFavoritesDB;
class IRQPreset;
class IRQBrowseCategoryItem;
class IRQChannelItem;

enum IRQSycPresetStatus
{
    EIRQIsdsSycPresetNoChange,
    EIRQIsdsSycPresetRemoved,
    EIRQIsdsSycPresetChanged
};

/**
 *This class is being used by the UI to send request and get data from low layer
 *All interfaces are QT-supported
 */
class IRQISDSCLIENT_DLL_EXPORT IRQIsdsClient : public QObject
{
Q_OBJECT

public:

    /**
     *IRQIsdsClient::IRQIsdsClientInterfaceIDs()
     *enum to indicate which the type of category   
     */
    enum IRQIsdsClientInterfaceIDs
    {
        EGenre, ELanguages, ECountries, Ehotpicks
    };

    /**
     *IRQIsdsClient::openInstance() 
     *to get an instance of the IRQIsdsClient. 
     *@return IRQIsdsClient *
     */
    static IRQIsdsClient *openInstance();

    /**
     *IRQIsdsClient::CloseInstance() 
     *close the instance.
     *@return IRQIsdsClient *
     */
    void closeInstance();

    /**
     *IRQIsdsClient::IsdsSearchRequestL()
     *Issue a search request to the isds server
     *@param QString, the requested search string
     **/
    void isdsSearchRequest(const QString& aIsdsSearchString);

    /**
     *IRQIsdsClient::IsdsCategoryRequest()
     *Send the category request by the category type
     *@param IRQIsdsClientInterfaceIDs, the requested category type
     **/
    void isdsCategoryRequest(
            IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType, bool& aCache);
    
    /**
     *IRQIsdsClient::isdsIsCategoryCached()
     *to check wether the request data is cached. Different from the above
     *function, this function will not send the http request if the data is 
     *not cached in DB. Normally, we call this function to check the cache.
     **/
    bool isdsIsCategoryCached(IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType);
    
    /**
     *IRQIsdsClient::isdsIsChannelCached()
     *to check wether the request data is cached. Different from the followed
     *function, this function will not send the http request if the data is 
     *not cached in DB. Normally, we call this function to check the cache.
     **/
    bool isdsIsChannelCached(int aIndex);

    /**
     *IRQIsdsClient::IsdsChannelRequestL()
     *Send the channels request by the channel index in the specify category
     *@param int, the requested index from UI view   
     **/
    void isdsChannelRequest(int aIndex, bool& aCache);

    /**
     *IRQIsdsClient::IsdsListenRequestL()
     *issue a listen request to the isds client. 
     *@param int,bool, 
     *if we set the aHistoryBool to false, the aCurrentIndex means the current song's index
     *in the current list.
     *if we set the aHistoryBool to true,  the aCurrentIndex means the channel ID you expect 
     *to listen, for sometimes, the channel ID is saved in the DB so we could get it directly
     **/
    void isdsListenRequest(int aCurrentIndex, bool aHistoryBool =
            false);

    /**
     *IRQIsdsClient::IsdsListenRequestL()
     *to syncronize presets
     *@param int,QString, the preset id and the last modified tag for the preset   
     **/
    int isdsSyncPreset(int aPresetId, const QString& aIfModifySince, IRQFavoritesDB *aFavPresets);

    /**
     *IRQIsdsClient::IsdsCancelRequest()
     *Cacel the request sent by the UI.
     *@param None
     **/
    void isdsCancelRequest();

    /**
     *IRQIsdsClient::IsdsGetCurrentPlayingIndex()
     *get the playing song's index 
     *@param None
     **/
    int isdsGetCurrentPlayingIndex();

    /**
     *IRQIsdsClient::IsdsIsCategoryBanner()
     *to see wether category view has a banner.
     *@param None
     **/
    bool isdsIsCategoryBanner();

    /**
     *IRQIsdsClient::IsdsIsChannelBanner()
     *to see wether channel view has a banner.
     *@param None
     **/
    bool isdsIsChannelBanner();

    /**
     *IRQIsdsClient::IsdsLogoDownSendRequestL()
     *the api is called from the UI(nowplaying view) to download logo.
     *@param None
     **/
    void isdsLogoDownSendRequest(IRQPreset* aPreset, int aNPVReq = 1,
            int aXValue = 0, int aYValue = 0);

    /**
     *IRQIsdsClient::isdsIsLogoCached()
     *to check wether the preset's logo is cached or not.      
     **/
    bool isdsIsLogoCached(IRQPreset* aPreset, int aXValue = 0, int aYValue = 0);

    /**
     *IRQIsdsClient::isdsLogoDownCancelTransaction()
     *the api is called to cancel the current transaction
     *@param None
     **/
    void isdsLogoDownCancelTransaction();

    /**
     * IRQIsdsClient::isdsLogoDownIsRunning()
     *  To know the status of downloading logo
     */
    bool isdsLogoDownIsRunning() const;

    /**
     * IRQIsdsClient::IsdsLogoDownCheckCacheLogoL()
     * takes the url as a parameter and the return value indicates wether the 
     * logo is in the cache. If the logo is in the cache, the logo data will 
     * be saved in an internal member of the logodown engine and we can use 
     * the next function to get it from the engine. 
     * this API is called form the search results view.
     * @param QString: the url of the img, int: the status for getting
     */
    void isdsLogoDownCheckCacheLogo(const QString& aURL, int& aStatus);

    /**
     * IRQIsdsClient::IsdsLogoDownSendCacheLogo()
     * get the cache logo from the logodown engine. The return value should 
     * be changed to be QT value in future 
     * @param None
     */
    TDesC8& isdsLogoDownSendCacheLogo();
    
    /**
     * IRQIsdsClient::isdsPostLog()
     * send the log file to the isds server
     * @param None
     */
    void isdsPostLog(const QString& aFileName);
    
    /*
     * IRQIsdsClient::isdsGetIRID()
	 * get the irid of the application
     */
    void isdsGetIRID();

    /*
     * IRQIsdsClient::isdsGetBrowseBanner()
     * get the browse advertisement url and clickthrough url
     */
    void isdsGetBrowseBanner(QString& aBannerUrl, QString& aClickThroughUrl);

    /*
     * IRQIsdsClient::isdsMultiSearch()
     * the new interface of multi search
     */
    void isdsMultSearch(QString aGenreID, QString aCountryID, QString aLanguageID, QString aSearchText);

    bool isdsIsConstructSucceed() const;
    
    signals:
    /**
     *IRQIsdsClient::categoryItemsChanged()
     *A signal to notify UI that the category items data is changed
     *@param QList<IRQBrowseCategoryItem *>, the data list sent to UI model
     **/
    void categoryItemsChanged(QList<IRQBrowseCategoryItem *> *aPushItemsList);
    /**
     *IRQIsdsClient::channelItemsChanged()
     *A signal to notify UI that the channel items data is changed
     *@param QList<QIRChannelItem *>, the data list sent to UI model
     **/
    void channelItemsChanged(QList<IRQChannelItem *> *aPushItemsList);
    /**
     *IRQIsdsClient::operationException()
     *A signal to notify UI that some inner error occurs     
     *@param IRQError, the error type notified
     **/
    void operationException(IRQError aError);

    /**
     *IRQIsdsClient::presetResponse()
     *A signal to notify UI that the preset data is changed     * 
     *@param QIRPreset*, the data sent to UI model
     **/
    void presetResponse(IRQPreset *aPreset);

    /**
     *IRQIsdsClient::presetResponse()
     *A signal to notify UI that the preset data is changed     * 
     *@param QIRPreset* IRQSycPresetStatus, the data sent to UI model
     **/
    void syncPresetResult(IRQSycPresetStatus aStatus, IRQPreset *aPreset);

    /**
     *IRQIsdsClient::presetLogoDownloaded()
     *A signal to notify UI that the preset logo requested is downloaded from server 
     *@param None
     **/
    void presetLogoDownloaded(IRQPreset* aPreset);

    /**
     *IRQIsdsClient::presetLogoDownloadError()
     *A signal to notify UI that the preset logo downloading is error      
     *@param None
     **/
    void presetLogoDownloadError();
    
    /*
     * IRQIsdsClient::iridReceived()
     * A signal to notify UI that the irid is received
     */
    void iridReceived(QString aIRID);
    
    
    
private:

    /*
     * Pointer to IRQIsdsClient instance
     */
	static IRQIsdsClient *mInstance;
	
    /*
     * Mutex lock for instance singleton
     */
    static QMutex mMutex;
	
    /*
     * the pointer of IRQAbstractIsdsClientImpl type point to IRQIsdsClientImpl of the IRQIsdsClient
     */
	IRQAbstractIsdsClientImpl *d_ptr;

    /**
     *iSingletonInstances
     * the count of reference of the object
     */
    static int mRef;

    IRQIsdsClient();
    /**
     *IRQIsdsClient::~IRQIsdsClient()
     *default c++ function
     **/
    ~IRQIsdsClient();

	friend class IRQIsdsClientImpl;
};

#endif
