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
#ifndef SIMUTILITY_H
#define SIMUTILITY_H

#include <qglobal.h>
#include <QObject>
#include <QDateTime>

#include <etelmm.h>
#include <rmmcustomapi.h>
#include <secuisecuritysettings.h> 
#include <secui.h>

#include "simutilityglobal.h"
#include "asyncworker.h"

/*!
 * SimUtility provides additional functionality for SIM contacts
 * handling which QContactManager doesn't support.
 */
class SIMUTILITYLIB_EXPORT CntSimUtility : public QObject
{
    Q_OBJECT

private:
    enum ActiveRequest {
         ENoActiveRequest = 0,
         EGetInfo,
         EGetAvailableStores,
         EGetCacheStatus
    };
    
public:
    enum StoreType {
        AdnStore = 0,
        SdnStore,
        FdnStore
    };
    
    struct SimInfo {
        int totalEntries;
        int usedEntries;
        int maxNumLength;
        int maxTextLength;
        int maxSecondNames;
        int maxTextLengthSecondName;
        int maxAdditionalNumbers;
        int maxNumLengthAdditionalNumber;
        int maxTextLengthAdditionalNumber;
        int maxGroupNames;
        int maxTextLengthGroupName;
        int maxEmailAddr;
        int maxTextLengthEmailAddr;
        
        SimInfo() {
            totalEntries = 0;
            usedEntries = 0;
            maxNumLength = 0;
            maxTextLength = 0;
            maxSecondNames = 0;
            maxTextLengthSecondName = 0;
            maxAdditionalNumbers = 0;
            maxNumLengthAdditionalNumber = 0;
            maxTextLengthAdditionalNumber = 0;
            maxGroupNames = 0;
            maxTextLengthGroupName = 0;
            maxEmailAddr = 0;
            maxTextLengthEmailAddr = 0;
        };
    };
    
    struct AvailableStores {
        bool SimPresent;
        bool AdnStorePresent;
        bool SdnStorePresent;
        bool FdnStorePresent;
        bool OnStorePresent;
        
        AvailableStores() {
            SimPresent = false;
            AdnStorePresent = false;
            SdnStorePresent = false;
            FdnStorePresent = false;
            OnStorePresent = false;
        };
    };
    
    enum CacheStatus {
        ECacheReady,
        ECacheFailed
    };
    
public:
	CntSimUtility(StoreType type, int& error, QObject *parent = 0);
	~CntSimUtility();
	
	//sync requests
	SimInfo getSimInfo(int& error);
	AvailableStores getAvailableStores(int& error);
	bool verifyPin2Code();
	bool isFdnActive();
	int setFdnStatus(bool activated);
	QDateTime getLastImportTime(int& error);
	void setLastImportTime(int& error);
	
	//async request
	bool startGetSimInfo();
	bool startGetAvailableStores();
	bool notifyAdnCacheStatus();
	
public:
    void RequestCompleted(int error);
	
signals:
    void simInfoReady(CntSimUtility::SimInfo& simInfo, int error);
    void availableStoresReady(CntSimUtility::AvailableStores& availableStores, int error);
    void adnCacheStatusReady(CntSimUtility::CacheStatus& cacheStatus, int error);
	
private: 
    void ParseServiceTable(AvailableStores* availableStores) const;
    bool isSimInserted() const;

private:
    RTelServer m_etelServer;
    RMobilePhone m_etelPhone;
    RMobilePhoneBookStore m_etelStore;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5 m_etelStoreInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg m_etelStoreInfoPckg;
    RMobilePhone::TMobilePhoneServiceTableV1 m_serviceTable;
    RMobilePhone::TMobilePhoneServiceTableV1Pckg m_serviceTablePckg;
    RMobilePhone::TMobilePhoneServiceTable m_serviceTableType;
    RMmCustomAPI m_customPhone;
    TName m_etelStoreNameCached;

    AsyncWorker* m_asyncWorker;
    int m_activeRequest;
    CSecuritySettings* m_securitySettings;
};

#endif
