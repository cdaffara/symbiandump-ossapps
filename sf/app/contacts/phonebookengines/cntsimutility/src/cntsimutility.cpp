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
#include "cntsimutility.h"
#include <mmtsy_names.h>
#include <startupdomainpskeys.h>
#include <e32property.h>
#include <centralrepository.h>

// Number of keys used in central repository to keep latest import date
// from SIM card: 5 IMSI strings and 5 dates. 
const int KCenRepKeysNumber = 10; 
const int KIMSISize = 15;
const int KCenRepUid = 0x200315A8;

CntSimUtility::CntSimUtility(StoreType type, int& error, QObject *parent)
    : QObject(parent),
    m_etelStoreInfoPckg(m_etelStoreInfo),
    m_serviceTablePckg(m_serviceTable),
    m_asyncWorker(NULL),
    m_activeRequest(ENoActiveRequest),
    m_securitySettings(NULL)
{
#ifdef __WINS__
    error = KErrNotSupported;
    return;
#endif

    error = m_etelServer.Connect();
    if (error == KErrNone) {
        error = m_etelServer.LoadPhoneModule(KMmTsyModuleName);
    }
    if (error == KErrNone) {
        RTelServer::TPhoneInfo info;
        error = m_etelServer.GetPhoneInfo(0, info);
        if (error == KErrNone) {
            error = m_etelPhone.Open(m_etelServer, info.iName);
        }
    }
    if (error == KErrNone) {
        // open specified Etel store
        if (type == AdnStore) {
            error = m_etelStore.Open(m_etelPhone, KETelIccAdnPhoneBook);
        }
        else if (type == SdnStore) {
            error = m_etelStore.Open(m_etelPhone, KETelIccSdnPhoneBook);
        }
        else if (type == FdnStore) {
            error = m_etelStore.Open(m_etelPhone, KETelIccFdnPhoneBook);
        }
        else {
            error = KErrNotSupported;
        }
    }
    
    if (error == KErrNone) {
        error = m_customPhone.Open(m_etelPhone);
    }
    
    if (error == KErrNone) {
        TRAP(error, 
                m_asyncWorker = new (ELeave)AsyncWorker(this);
                TSecUi::InitializeLibL();
                m_securitySettings = CSecuritySettings::NewL();
            );
    }
}

CntSimUtility::~CntSimUtility()
{
    m_etelStore.Close();
    m_etelPhone.Close();
    m_etelServer.Close();

    delete m_asyncWorker;
    delete m_securitySettings;
    TSecUi::UnInitializeLib();
}

CntSimUtility::SimInfo CntSimUtility::getSimInfo(int& error)
{
    TRequestStatus requestStatus;
    m_etelStore.GetInfo(requestStatus, (TDes8&)m_etelStoreInfoPckg);
    User::WaitForRequest(requestStatus);
    error = requestStatus.Int();
    SimInfo results;
    
    if (error == KErrNone) {
        results.totalEntries = m_etelStoreInfo.iTotalEntries;
        results.usedEntries = m_etelStoreInfo.iUsedEntries;
        results.maxNumLength = m_etelStoreInfo.iMaxNumLength;
        results.maxTextLength = m_etelStoreInfo.iMaxTextLength;
        results.maxSecondNames = m_etelStoreInfo.iMaxSecondNames;
        results.maxTextLengthSecondName = m_etelStoreInfo.iMaxTextLengthSecondName;
        results.maxAdditionalNumbers = m_etelStoreInfo.iMaxAdditionalNumbers;
        results.maxNumLengthAdditionalNumber = m_etelStoreInfo.iMaxNumLengthAdditionalNumber;
        results.maxTextLengthAdditionalNumber = m_etelStoreInfo.iMaxTextLengthAdditionalNumber;
        results.maxGroupNames = m_etelStoreInfo.iMaxGroupNames;
        results.maxTextLengthGroupName = m_etelStoreInfo.iMaxTextLengthGroupName;
        results.maxEmailAddr = m_etelStoreInfo.iMaxEmailAddr;
        results.maxTextLengthEmailAddr = m_etelStoreInfo.iMaxTextLengthEmailAddr;
    }
    return results;
}

CntSimUtility::AvailableStores CntSimUtility::getAvailableStores(int& error)
{
    AvailableStores availableStores;
    if (!isSimInserted()) {
        error = KErrNone;
        return availableStores;
    }
    
    m_serviceTableType = RMobilePhone::ESIMServiceTable;
    unsigned long int iccCaps;
    TRequestStatus requestStatus;
    
    int result = m_etelPhone.GetIccAccessCaps(iccCaps);
    if (result == KErrNone && (iccCaps & RMobilePhone::KCapsUSimAccessSupported)) {
        m_serviceTableType = RMobilePhone::EUSIMServiceTable;
    }

    m_etelPhone.GetServiceTable(requestStatus, m_serviceTableType, m_serviceTablePckg);
    User::WaitForRequest(requestStatus);
    error = requestStatus.Int();
    
    if (error == KErrNone) {
        //parse service table to find what stores are supported
         ParseServiceTable(&availableStores);
    }
    return availableStores;
}

bool CntSimUtility::verifyPin2Code()
{
    bool verified = false;
    TRAP_IGNORE(verified = m_securitySettings->AskPin2L(););
    return verified;
}

bool CntSimUtility::isFdnActive()
{
    RMobilePhone::TMobilePhoneFdnStatus fdnStatus;
    (void)m_etelPhone.GetFdnStatus(fdnStatus);
    if (fdnStatus == RMobilePhone::EFdnActive ||
        fdnStatus == RMobilePhone::EFdnPermanentlyActive) {
        return true;
        }
    return false;
}

int CntSimUtility::setFdnStatus(bool activated)
{
    RMobilePhone::TMobilePhoneFdnSetting fdnStatus = RMobilePhone::EFdnSetOff;
    if (activated) {
        fdnStatus = RMobilePhone::EFdnSetOn;
    }
    TRequestStatus status;
    m_etelPhone.SetFdnSetting(status, fdnStatus);
    User::WaitForRequest(status);
    return status.Int();
}

QDateTime CntSimUtility::getLastImportTime(int& error)
{
    error = KErrNone;
    if (!isSimInserted()) {
        error = KErrAccessDenied;
        return QDateTime();
    }
    
    // get IMSI
    TBuf<KIMSISize> imsiBuf;
    TRequestStatus reqStatus;
    m_etelPhone.GetSubscriberId(reqStatus, imsiBuf);
    User::WaitForRequest(reqStatus);
    if (reqStatus.Int() != KErrNone) {
        error = reqStatus.Int();
        return QDateTime();
    }
    
    CRepository* cenrep = NULL;
    TRAPD(err, cenrep = CRepository::NewL(TUid::Uid(KCenRepUid)));
    if (err != KErrNone) {
        error = err;
        return QDateTime();
    }
    
    // find current IMSI in the stored values
    int lastImportDate = 0;
    for (int i = 1; i <= KCenRepKeysNumber; i+=2) {
        TBuf<KIMSISize> storedImsiBuf;
        if (cenrep->Get(i, storedImsiBuf) == KErrNone) {
            if (storedImsiBuf.Compare(imsiBuf) == 0) {
                if (cenrep->Get(i+1, lastImportDate) == KErrNone) {
                    break;
                }
            }
        }
    }
    delete cenrep;
    cenrep = NULL;
    
    if (lastImportDate == 0) {
        error = KErrNotFound;
        return QDateTime();
    }
    
    QDateTime retLastImportDate;
    retLastImportDate.setTime_t(lastImportDate);
    return retLastImportDate;
}

void CntSimUtility::setLastImportTime(int& error)
{
    error = KErrNone;
    if (!isSimInserted()) {
        error = KErrAccessDenied;
        return;
    }
    
    // get IMSI
    TBuf<KIMSISize> imsiBuf;
    TRequestStatus reqStatus;
    m_etelPhone.GetSubscriberId(reqStatus, imsiBuf);
    User::WaitForRequest(reqStatus);
    if (reqStatus.Int() != KErrNone) {
        error = reqStatus.Int();
        return;
    }
    
    CRepository* cenrep = NULL;
    TRAPD(err, cenrep = CRepository::NewL(TUid::Uid(KCenRepUid)));
    if (err != KErrNone) {
        error = err;
        return;
    }
     
    // find current IMSI in the stored values
    int lastImportDate = QDateTime::currentDateTime().toTime_t();
    bool importDateUpdated = false;
    for (int i = 1; i <= KCenRepKeysNumber; i+=2) {
        TBuf<KIMSISize> storedImsiBuf;
        if (cenrep->Get(i, storedImsiBuf) == KErrNone) {
            if (storedImsiBuf.Compare(imsiBuf) == 0) {
                //current IMSI already stored, update import date
                if (cenrep->Set(i+1, lastImportDate) != KErrNone) {
                    error = KErrAccessDenied;
                }
                importDateUpdated = true;
                break;
            }
        }
    }
    
    if (!importDateUpdated) {
        int oldestDate = 0;
        int key = 0;
        // contacts were not imported from this SIM before,
        // find oldest import date and replace it
        for (int j = 2; j <= KCenRepKeysNumber; j+=2) {
            int date;
            if (cenrep->Get(j, date) == KErrNone) {
                if (date >= oldestDate ) {
                    oldestDate = date;
                    key = j;
                }
            }
        }
    
        if (key == 0) {
            error = KErrAccessDenied;
        }
        else {
            if (cenrep->Set(key, lastImportDate) != KErrNone) {
                error = KErrAccessDenied;
            }
            if (cenrep->Set(key-1, imsiBuf) != KErrNone) {
                error = KErrAccessDenied;
            }
        }
    }
    
    delete cenrep;
    cenrep = NULL;
}

bool CntSimUtility::startGetSimInfo()
{
    if(m_asyncWorker->IsActive()) {
        return false;
    }
    
    m_etelStore.GetInfo(m_asyncWorker->iStatus, (TDes8&)m_etelStoreInfoPckg);
    m_asyncWorker->SetActive();
    m_activeRequest = EGetInfo;
    return true;
}

bool CntSimUtility::startGetAvailableStores()
{
    if(m_asyncWorker->IsActive()) {
        return false;
    }
    
    if (!isSimInserted()) {
        AvailableStores availableStores;
        emit availableStoresReady(availableStores, KErrNone);
        return true;
    }
    
    m_serviceTableType = RMobilePhone::ESIMServiceTable;
    unsigned long int iccCaps;
    TRequestStatus requestStatus;
    
    int result = m_etelPhone.GetIccAccessCaps(iccCaps);
    if (result == KErrNone && (iccCaps & RMobilePhone::KCapsUSimAccessSupported)) {
        m_serviceTableType = RMobilePhone::EUSIMServiceTable;
    }
    m_etelPhone.GetServiceTable(m_asyncWorker->iStatus, m_serviceTableType, m_serviceTablePckg);
    m_asyncWorker->SetActive();
    m_activeRequest = EGetAvailableStores;
    return true;
}

bool CntSimUtility::notifyAdnCacheStatus()
{
    if(m_asyncWorker->IsActive()) {
        return false;
    }
    CacheStatus cacheStatus;
    
    //check current cache status
    TRequestStatus requestStatus;
    RMmCustomAPI::TPndCacheStatus pndStatus;
    TName storeName;
    storeName.Copy(KETelIccAdnPhoneBook);
    m_customPhone.GetPndCacheStatus(requestStatus, pndStatus, storeName);
    User::WaitForRequest(requestStatus);
    if (requestStatus.Int() != KErrNone) {
        return false;
    }
    
    if (pndStatus == RMmCustomAPI::ECacheReady ||
        pndStatus == RMmCustomAPI::ECacheNotUsed) {
        cacheStatus = ECacheReady;
        emit adnCacheStatusReady(cacheStatus, KErrNone);
        return true;
        }
    else if (pndStatus == RMmCustomAPI::ECacheFailed) {
        cacheStatus = ECacheFailed;
        emit adnCacheStatusReady(cacheStatus, KErrNone);
        return true;    
        }
    else if (pndStatus == RMmCustomAPI::ECacheNotReady) {
        //wait for cache notification
        m_customPhone.NotifyPndCacheReady(m_asyncWorker->iStatus, m_etelStoreNameCached);
        m_asyncWorker->SetActive();
        m_activeRequest = EGetCacheStatus;
        return true;
    }
    else {
        return false;
    }
}

void CntSimUtility::RequestCompleted(int error)
{
    if (m_activeRequest == EGetInfo) {
        SimInfo results;
        if (error == KErrNone) {
            results.totalEntries = m_etelStoreInfo.iTotalEntries;
            results.usedEntries = m_etelStoreInfo.iUsedEntries;
            results.maxNumLength = m_etelStoreInfo.iMaxNumLength;
            results.maxTextLength = m_etelStoreInfo.iMaxTextLength;
            results.maxSecondNames = m_etelStoreInfo.iMaxSecondNames;
            results.maxTextLengthSecondName = m_etelStoreInfo.iMaxTextLengthSecondName;
            results.maxAdditionalNumbers = m_etelStoreInfo.iMaxAdditionalNumbers;
            results.maxNumLengthAdditionalNumber = m_etelStoreInfo.iMaxNumLengthAdditionalNumber;
            results.maxTextLengthAdditionalNumber = m_etelStoreInfo.iMaxTextLengthAdditionalNumber;
            results.maxGroupNames = m_etelStoreInfo.iMaxGroupNames;
            results.maxTextLengthGroupName = m_etelStoreInfo.iMaxTextLengthGroupName;
            results.maxEmailAddr = m_etelStoreInfo.iMaxEmailAddr;
            results.maxTextLengthEmailAddr = m_etelStoreInfo.iMaxTextLengthEmailAddr;
        }
        emit simInfoReady(results, error);
    }//EGetInfo
    else if (m_activeRequest == EGetAvailableStores) {
        AvailableStores availableStores;
        if (error == KErrNone) {
            //parse service table to find what stores are supported
            ParseServiceTable(&availableStores);
        }
        emit availableStoresReady(availableStores, error);
    }//EGetAvailableStores
    else if (m_activeRequest == EGetCacheStatus) {
        if ( m_etelStoreNameCached.Compare(KETelIccAdnPhoneBook) == 0) {
            //ADN cache is ready
            CacheStatus cacheStatus;
            if (error != KErrNone) {
                cacheStatus = ECacheFailed;
                emit adnCacheStatusReady(cacheStatus, error);
            }
            else {
                cacheStatus = ECacheReady;
                emit adnCacheStatusReady(cacheStatus, error);
            }
        }
        else {
            //another store is cached, continue listening for ADN cache
            notifyAdnCacheStatus();
        }
    }//EGetCacheStatus
    
    if (!m_asyncWorker->IsActive()) {
        m_activeRequest = ENoActiveRequest;
    }
}

void CntSimUtility::ParseServiceTable(AvailableStores* availableStores) const
{
    availableStores->SimPresent = true;
    if (m_serviceTableType == RMobilePhone::EUSIMServiceTable) {
        //ADN store is always present if SIM card is inserted
        availableStores->AdnStorePresent = true;
        
        if (m_serviceTable.iServices1To8 & RMobilePhone::KUstSDN ) {
            availableStores->SdnStorePresent = true;
        }
        else {
            availableStores->SdnStorePresent = false;
        }
        
        if (m_serviceTable.iServices1To8 & RMobilePhone::KUstFDN) {
            availableStores->FdnStorePresent = true;
        }
        else {
            availableStores->FdnStorePresent = false;
        }
        
        if (m_serviceTable.iServices17To24 & RMobilePhone::KUstMSISDN) {
            availableStores->OnStorePresent = true;
        }
        else {
            availableStores->OnStorePresent = false;
        }
    }
    else if (m_serviceTableType == RMobilePhone::ESIMServiceTable) {
        //ADN store is always present if SIM card is inserted
        availableStores->AdnStorePresent = true;
        
        if (m_serviceTable.iServices17To24 & RMobilePhone::KSstSDN) {
            availableStores->SdnStorePresent = true;
        }
        else {
            availableStores->SdnStorePresent = false;
        }
        
        if (m_serviceTable.iServices1To8 & RMobilePhone::KSstFDN) {
            availableStores->FdnStorePresent = true;
        }
        else {
            availableStores->FdnStorePresent = false;
        }
        
        if (m_serviceTable.iServices9To16 & RMobilePhone::KSstMSISDN) {
            availableStores->OnStorePresent = true;
        }
        else {
            availableStores->OnStorePresent = false;
        }
    }
}
    
bool CntSimUtility::isSimInserted() const
{
    bool result = false;
    
    RProperty property;   
    int simStatus = KErrNotFound;
    
    int ret = property.Get(KPSUidStartup, KPSSimStatus, simStatus);
    if (simStatus == ESimUsable && ret == KErrNone) {
        result = true;
        }
    
    property.Close();
    return result;
}
