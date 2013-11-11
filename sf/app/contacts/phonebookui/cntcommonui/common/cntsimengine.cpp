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

#include <QString.h>

#include "cntglobal.h"
#include "CntSimEngine.h"
#include "cntimportviewcallback.h"
#include "cntdebug.h"

#include <hbview.h>
#include <QCoreApplication>
#include <QStandardItemModel>
#include <hbextendedlocale.h>
#include <hbstringutil.h>
#include <hbparameterlengthlimiter.h>
#include <hbdialog.h>
#include <hbnotificationdialog.h>
#include <hbdevicenotificationdialog.h>

const int KTimerValue = 1; // used as 1 msec timer for saving ADN contacts from SIM   

CntSimEngine::CntSimEngine(CntImportViewCallback& aImportViewCallback, HbView& aView)
: mImportViewCallback(aImportViewCallback),
    mView(aView),
    mAdnSimUtility(0),
    mSdnSimUtility(0),
    mContactSimManagerADN(0),
    mContactSimManagerSDN(0),    
    mFetchRequestADN(0),
    mFetchRequestSDN(0),
    mTimer(0),
    mAbsEngine(0),
    mTimerId(0),
    mSaveCount(0),
    mAdnStoreEntries(0),
    mSdnStoreEntries(0),
    mAdnEntriesPresent(false),
    mFetchIsDone(false),
    mAdnStorePresent(false),
    mSdnStorePresent(false),
    mSimPresent(false),
    mSimError(false),
    mWaitingForAdnCache(false),
    mImportInProgress(false)
{

}

CntSimEngine::~CntSimEngine()
{
    CNT_ENTRY
    delete mFetchRequestADN;
    delete mFetchRequestSDN;
    delete mAdnSimUtility;
    delete mSdnSimUtility;
    CNT_EXIT
}

void CntSimEngine::init(CntAbstractEngine& aAbsEngine)
{
    CNT_ENTRY
    mAbsEngine = &aAbsEngine;
    mContactSymbianManager = &mAbsEngine->contactManager(SYMBIAN_BACKEND);
     
    // Sim Utility info fetch
     int getSimInfoError(0);
     int storesError(0);
     int getSdnSimInfoError(0);
     
     
     //ADN store
     int error = -1;
     mAdnSimUtility = new CntSimUtility(CntSimUtility::AdnStore, error);
     
     if (error != 0) 
     {
         delete mAdnSimUtility; 
         mAdnSimUtility = 0;
         mSimError = true;
         //mSimError = true;
     }
     else
     {
         // check what stores are there
         CntSimUtility::AvailableStores stores = mAdnSimUtility->getAvailableStores(storesError);
         if(!storesError)
         {
             mSimPresent = stores.SimPresent;
             mAdnStorePresent = stores.AdnStorePresent; 
             mSdnStorePresent = stores.SdnStorePresent;
         }
         
         //check there are ADN contacts 
         if (mAdnStorePresent)
         {
             CntSimUtility::SimInfo simInfo = mAdnSimUtility->getSimInfo(getSimInfoError);
             if (getSimInfoError == KErrNone)
             {
                 // sim entries are present
                 mAdnStoreEntries = simInfo.usedEntries;
                 if (mAdnStoreEntries > 0) 
                 {
                      mAdnEntriesPresent = true;
                 }
             }
             else
             {
                 if (getSimInfoError == KErrNotReady)
                 {
                     //ADN store is not accessible, e.g. because of active FDN
                     //Or ADN cache is not ready yet, so wait for this  
                     
                     mAdnStorePresent = true;
                     mAdnEntriesPresent = true;
                     mWaitingForAdnCache = true;
                     connect(mAdnSimUtility, SIGNAL(getSimInfoAndUpdateUI(CntSimUtility::CacheStatus&, int)), this, SLOT(getSimInfoAndUpdateUI(CntSimUtility::CacheStatus&, int)));
                     if (!mAdnSimUtility->notifyAdnCacheStatus()) {
                         mAdnStorePresent = false;
                         mAdnEntriesPresent = false;
                         mWaitingForAdnCache = false;
                     }
                 }
                 else
                 {
                     simInfoErrorMessage(getSimInfoError);
                 }
             }
         }
     }
     
     //SDN store
      if (mSdnStorePresent && !mWaitingForAdnCache)
      {
         int sdnError = -1;
         mSdnStorePresent = false;
         if (!mSdnSimUtility) 
         {
             mSdnSimUtility = new CntSimUtility(CntSimUtility::SdnStore, sdnError);
             if (sdnError != 0) 
             {
                 delete mSdnSimUtility; 
                 mSdnSimUtility = 0;
             }
         }
         if (mSdnSimUtility)
         {
             //get number of SDN contacts 
             CntSimUtility::SimInfo sdnSimInfo = mSdnSimUtility->getSimInfo(getSdnSimInfoError);
             if (!getSdnSimInfoError)
             {
                 // sim entries are present
                 mSdnStoreEntries = sdnSimInfo.usedEntries;
                 if (mSdnStoreEntries > 0)
                 {
                     mSdnStorePresent = true;    
                 }
             }
         }
      }
      CNT_EXIT
     // end SDN store
}

void CntSimEngine::checkEntries(QStringList& simList, QStandardItem& importSimItem)
{
    CNT_ENTRY
    QString simImport(hbTrId("txt_phob_dblist_import_from_sim"));
    QString simNoContacts(hbTrId("txt_phob_dblist_import_from_1_val_no_sim_contacts"));
    QString simNoCard(hbTrId("txt_phob_dblist_import_from_ovi_val_no_sim_card"));

    // check if SIM card is NOT there 
    if (mSimError)
    {
        simList << simImport;
        importSimItem.setEnabled(false);
    }
    else if (!(mAdnStorePresent || mSdnStorePresent))
    {
        // Both stores are not present
        // disable the QStandardItem
        if (!mSimPresent) 
        {
            simList << simImport << simNoCard;
        }
        else
        {
            simList << simImport << simNoContacts;
        }
        importSimItem.setEnabled(false);
    }
    else if (!(mAdnEntriesPresent || mSdnStorePresent))
    {
        // Sim card is present
        // No ADN entries are there
        // no SDN entries are there
        simList << simImport << simNoContacts;
        importSimItem.setEnabled(false);
    }
    else
    {   
        // SIM card is present
        //ADN entries or SDN entries are there
        simList << simImport;
        int error = 0;
        QDateTime date = mAdnSimUtility->getLastImportTime(error);
        if (error == 0) {
            HbExtendedLocale locale = HbExtendedLocale::system();
            QString dateStr = locale.format(date.date(), r_qtn_date_usual);
            QString dateStrLocaleDigits = HbStringUtil::convertDigits(dateStr); 
            QString dateStrFull = 
                HbParameterLengthLimiter(hbTrId("txt_phob_dblist_import_from_1_val_updated_1")).arg(dateStrLocaleDigits);
            simList << dateStrFull;
        }
    }
    CNT_EXIT
}

bool CntSimEngine::startSimImport()
{
    CNT_ENTRY
    bool started = false;
    mImportInProgress = true;
    mSaveCount = 0;

    delete mFetchRequestADN;
    mContactSimManagerADN = &mAbsEngine->contactManager(SIM_BACKEND_ADN);
    mFetchRequestADN = new QContactFetchRequest;
    mFetchRequestADN->setManager(mContactSimManagerADN);   
    
    delete mFetchRequestSDN;
    mContactSimManagerSDN = &mAbsEngine->contactManager(SIM_BACKEND_SDN);
    mFetchRequestSDN = new QContactFetchRequest;
    mFetchRequestSDN->setManager(mContactSimManagerSDN);        
        
    if (mWaitingForAdnCache)
    {
        //show progress dialog and wait when ADN cache is ready
        started = true;
    }
    else
    {
        if(mAdnStorePresent)
        {
            connect(mFetchRequestADN, SIGNAL(resultsAvailable()), this, SLOT(importADNFetchResults()));
        }
      
        if(mSdnStorePresent)
        {
            connect(mFetchRequestSDN, SIGNAL(resultsAvailable()), this, SLOT(importSDNFetchResults()));
        }
        
        if(mAdnEntriesPresent > 0 && mContactSimManagerADN->error() == QContactManager::NoError) 
        {
            // ADN contacts are there, start fetch
            mFetchRequestADN->start();
            started = true;
        }
        else if (mSdnStorePresent && mContactSimManagerSDN->error() == QContactManager::NoError)        
        {
            //start fetch of SDN contacts since ADN contacts are not there
            mFetchRequestSDN->start();
            started = true;
        }
    }
    CNT_EXIT
    return started;
}  


void CntSimEngine::stopSimImport()
{
    CNT_ENTRY    
    if(mImportInProgress)
    {
        emit closePopup();            
        mImportInProgress = false;
        if (mFetchIsDone)
        {
            // indicates that timer has been started
            killTimer(mTimerId);
        }
        mSaveSimContactsList.clear(); 
        mSaveSimContactsListSDN.clear(); 
        
        showSimImportResults();
        
        if (mFetchRequestADN != NULL)
        {
            mFetchRequestADN->cancel();
        }
        if (mFetchRequestSDN != NULL)
        {
            mFetchRequestSDN->cancel();
        }
        
        // save import time
        int error = 0;
        mAdnSimUtility->setLastImportTime(error);
        
        //update sim import row with last import time
        if (error == 0)
        {
            QDateTime date = mAdnSimUtility->getLastImportTime(error);
            if (error == 0)
            {
                QString simImport(hbTrId("txt_phob_dblist_import_from_sim"));
                
                HbExtendedLocale locale = HbExtendedLocale::system();
                QString dateStr = locale.format(date.date(), r_qtn_date_usual);
                QString dateStrLocaleDigits = HbStringUtil::convertDigits(dateStr);
                QString dateStrFull = 
                    HbParameterLengthLimiter(hbTrId("txt_phob_dblist_import_from_1_val_updated_1")).arg(dateStrLocaleDigits);
                //simList << dateStrFull;
                
                mImportViewCallback.setListBoxItemText(simImport, dateStrFull);
            }
        }
    }
    
    CNT_EXIT
}

void CntSimEngine::timerEvent(QTimerEvent *event)
{
    CNT_ENTRY
    Q_UNUSED(event);
    if (!(mSaveCount >= mSaveSimContactsList.count())) // while mSaveCount is less than or equal to mSaveSimContactsList.count()
    {
        QContact unSavedContact = mSaveSimContactsList.at(mSaveCount);
        mContactSymbianManager->saveContact(&unSavedContact);
        mSaveCount++;
        //use another counter to check number of actual saved if saveContact() fails
    }
    else
    {
        // saving complete
        killTimer(mTimerId);
        mSaveSimContactsList.clear();
        
        // ADN contacts fetching and saving is done
        // Now start SDN fetching and save
        fetchSDNContacts();
    }
    CNT_EXIT
}


void CntSimEngine::importADNFetchResults()
{
    CNT_ENTRY
    //save import time
    int error = 0;
    mAdnSimUtility->setLastImportTime(error);

    QList<QContact> simContactsList = mFetchRequestADN->contacts();
    if (simContactsList.isEmpty())
    {
        // No sim contacts, jump to fetching SDN contacts
        fetchSDNContacts();
    }
    else
    {
        // save ADN contacts
    
        int count = simContactsList.count();
        foreach(QContact contact, simContactsList) 
        {
            if (contact.localId() > 0) 
            {
            //delete local id before saving to different storage
               QScopedPointer<QContactId> contactId(new QContactId());
               contactId->setLocalId(0);
               contactId->setManagerUri(QString());
               contact.setId(*contactId);
    
               // custom label contains name information, save it to the first name 
               QList<QContactDetail> names = contact.details(QContactName::DefinitionName);
               if (names.count() > 0) 
               {
                   QContactName name = static_cast<QContactName>(names.at(0));
                   name.setFirstName(name.customLabel());
                   name.setCustomLabel(QString());
                   contact.saveDetail(&name);
               }
                 
               //update phone numbers to contain default subtype
               QList<QContactDetail> numbers = contact.details(QContactPhoneNumber::DefinitionName);
               for (int i = 0; i < numbers.count(); i++) 
               {
                   QContactPhoneNumber number = static_cast<QContactPhoneNumber>(numbers.at(i));
                   number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                   contact.saveDetail(&number);
               }
               
               //remove sync target details, it's read-only. 
                QList<QContactDetail> syncTargets = contact.details(QContactSyncTarget::DefinitionName);
                for (int j = 0; j < syncTargets.count(); j++) 
                {
                    QContactSyncTarget syncTarget = static_cast<QContactSyncTarget>(syncTargets.at(j));
                    contact.removeDetail(&syncTarget);
                }
               
               contact.setType(QContactType::TypeContact);
               setPreferredDetails(contact);
               mSaveSimContactsList.append(contact);
                
            }
        }
        // save the list synchronously because async cancelling of save request is  
        // not supported in symbian backend at the time of this implementation
        
        if (!(mSaveSimContactsList.isEmpty()))
        {
            // indicates that there is one or more sim contact that has been fetched
            // create a timer and start saving synchronously one by one
            // when cancel is pressed, kill the timer
            
            mTimerId = startTimer(KTimerValue); // starting a 1 msec timer
            mFetchIsDone = true;
            mSaveCount = 0;    
        }
    }
    CNT_EXIT
}

void CntSimEngine::importSDNFetchResults()
{
    CNT_ENTRY
    //save import time
    int error = 0;
    mAdnSimUtility->setLastImportTime(error);

    QList<QContact> simContactsListSDN = mFetchRequestSDN->contacts();
    if (simContactsListSDN.isEmpty())
    {
        //No sdn contacts present
        showSimImportResults();
        mSaveSimContactsListSDN.clear(); 
        emit closePopup();
        mImportInProgress = false;
        // Importing finished, go back to NamesView
        emit showNamesView();
        //showPreviousView();
    }
    else
    {
    // SAVE SDN CONTACTS
        int count = simContactsListSDN.count();
        foreach(QContact contact, simContactsListSDN) 
        {
            if (contact.localId() > 0) 
            {
            //delete local id before saving to different storage
               QScopedPointer<QContactId> contactId(new QContactId());
               contactId->setLocalId(0);
               contactId->setManagerUri(QString());
               contact.setId(*contactId);
    
               //custom label contains name information, save it to the first name 
              QList<QContactDetail> names = contact.details(QContactName::DefinitionName);
              if (names.count() > 0) 
              {
                  QContactName name = static_cast<QContactName>(names.at(0));
                  name.setFirstName(name.customLabel());
                  name.setCustomLabel(QString());
                  contact.saveDetail(&name);
              }
                
              //update phone numbers to contain default subtype
              QList<QContactDetail> numbers = contact.details(QContactPhoneNumber::DefinitionName);
              for (int i = 0; i < numbers.count(); i++) 
              {
                  QContactPhoneNumber number = static_cast<QContactPhoneNumber>(numbers.at(i));
                  number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                  contact.saveDetail(&number);
              }
              
              //remove sync target details, it's read-only. 
              QList<QContactDetail> syncTargets = contact.details(QContactSyncTarget::DefinitionName);
              for (int j = 0; j < syncTargets.count(); j++) 
              {
                  QContactSyncTarget syncTarget = static_cast<QContactSyncTarget>(syncTargets.at(j));
                  contact.removeDetail(&syncTarget);
              }
              
               contact.setType(QContactType::TypeContact);
               setPreferredDetails(contact);
               mSaveSimContactsListSDN.append(contact);
                
            }
        }
        // save the list synchronously because async cancelling of save request is  
        // not supported in symbian backend at the time of this implementation
        
        if (!(mSaveSimContactsListSDN.isEmpty()))
        {
            // indicates that there is one or more SDN sim contact that has been fetched
            QMap<int, QContactManager::Error> errorMap;
            mContactSymbianManager->saveContacts(&mSaveSimContactsListSDN,&errorMap);
            // check number of contacts really saved
            mSaveCount = mSaveCount + mSaveSimContactsListSDN.count();
        }
        
        // no more SDN contacts to fetch
        showSimImportResults();
        mSaveSimContactsListSDN.clear(); 
        emit closePopup();
        mImportInProgress = false;
        // Importing finished, go back to NamesView
        emit showNamesView();
        //showPreviousView();
    }   
    CNT_EXIT
}

void CntSimEngine::setPreferredDetails(QContact& aContact )
{
    CNT_ENTRY
    QList<QContactPhoneNumber> numberList( aContact.details<QContactPhoneNumber>() );
    //set preferred number for call if there is only one phone number
    if ( aContact.preferredDetail("call").isEmpty() && numberList.count() == 1 )
    {
        aContact.setPreferredDetail( "call", numberList.first() );
    }
    //set preferred number for message if there is only one mobile phone number
    if ( aContact.preferredDetail("message").isEmpty() && numberList.count() == 1 )
    {      
        aContact.setPreferredDetail( "message", numberList.first() );
    }
    //set preferred number for message if there is only one email address
    QList<QContactEmailAddress> emailList( aContact.details<QContactEmailAddress>() );
    if ( aContact.preferredDetail("email").isEmpty() && emailList.count() == 1 )
    {      
        aContact.setPreferredDetail( "email", emailList.first() );
    }
    CNT_EXIT
}

void CntSimEngine::showSimImportResults() const
{   
    CNT_ENTRY
    QString results = hbTrId("txt_phob_dpophead_ln_contacts_imported").arg(mSaveCount).arg(mAdnStoreEntries + mSdnStoreEntries);
    HbNotificationDialog::launchDialog(results);
    CNT_EXIT
}

void CntSimEngine::fetchSDNContacts()
{
    CNT_ENTRY
   if (mSdnStorePresent && mContactSimManagerSDN->error() == QContactManager::NoError)
    {
        mFetchRequestSDN->start();
    }
    else
    {
        // no SDN contacts to fetch
        showSimImportResults();
        emit closePopup();
        mImportInProgress = false;
        // Importing finished, go back to NamesView
        
        //showPreviousView();
        emit showNamesView();
    }
    CNT_EXIT
}


void CntSimEngine::getSimInfoAndUpdateUI(CntSimUtility::CacheStatus& cacheStatus, int error)
{
    CNT_ENTRY
    mWaitingForAdnCache = false;
    //update ADN store info...
    if (error != KErrNone || cacheStatus == CntSimUtility::ECacheFailed)
    {
        mAdnStorePresent = false;
        mAdnEntriesPresent = false;
        mSdnStorePresent = false;
    }
    else
    {
        //check if there are ADN contacts
        mAdnStorePresent = true;
        mAdnEntriesPresent = false;
        int getSimInfoError = -1;
        CntSimUtility::SimInfo simInfo = mAdnSimUtility->getSimInfo(getSimInfoError);
        if (!getSimInfoError)
        {
            // sim entries are present
            mAdnStoreEntries = simInfo.usedEntries;
            if (mAdnStoreEntries > 0) 
            { 
                mAdnEntriesPresent = true;
            }
        }
        
        //get number of SDN contacts
        int sdnError = -1;
        mSdnStorePresent = false;
        if (!mSdnSimUtility)
        {
            mSdnSimUtility = new CntSimUtility(CntSimUtility::SdnStore, sdnError);
            if (sdnError != 0) 
            {
                delete mSdnSimUtility; 
                mSdnSimUtility = 0;
            }
        }
        if (mSdnSimUtility)
        {
            int getSdnSimInfoError = 0;
            CntSimUtility::SimInfo sdnSimInfo = mSdnSimUtility->getSimInfo(getSdnSimInfoError);
            if (!getSdnSimInfoError)
            {
                mSdnStoreEntries = sdnSimInfo.usedEntries;
                if (mSdnStoreEntries > 0)
                {
                    mSdnStorePresent = true;    
                }
            }
        }
    }
    
    //dismiss progress dialog, if there are no SIM contacts 
    if(!mAdnEntriesPresent && !mSdnStorePresent)
    {
        emit closePopup();
        showSimImportResults();
    }
    
    //disable sim import item, if there are no SIM contacts 
    if(!mAdnEntriesPresent && !mSdnStorePresent)
    {
        QString simImport(hbTrId("txt_phob_dblist_import_from_sim"));
        QString simNoContacts(hbTrId("txt_phob_dblist_import_from_1_val_no_sim_contacts"));
        
        mImportViewCallback.setListBoxItemText(simImport, simNoContacts);
        mImportViewCallback.setListBoxItemEnabled(false);     
    }
    
    //start SIM contacts import, if user tapped "SIM import"
//    if(mImportSimPopup != NULL && mImportSimPopup->isActive())
//    {
        if (!startSimImport())
        {
            //dismiss wait note
            emit closePopup();
            mImportInProgress = false;
            //and show error note
            simInfoErrorMessage(KErrAccessDenied);
        }
 //   }
        CNT_EXIT
}

void CntSimEngine::simInfoErrorMessage(int infoError)
{
    CNT_ENTRY
    Q_UNUSED(infoError);
    QString errorMessage;
    errorMessage.append(hbTrId("txt_phob_info_sim_card_error"));
    HbNotificationDialog::launchDialog(errorMessage);
    mSimError = true;
    CNT_EXIT
}

bool CntSimEngine::userCancelsImport(QString& aResults)
{   
    CNT_ENTRY
    aResults = "";
    
    bool boolResult = mImportInProgress && !mSimError;
    
    if(boolResult)
    {
        aResults = hbTrId("txt_phob_dpophead_ln_contacts_imported").arg(mSaveCount).arg(mAdnStoreEntries + mSdnStoreEntries);
    }
    mImportInProgress = false;
    CNT_EXIT
    return boolResult;
}




