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

#include "ut_cntcache.h"
#include "cntcache.h"
#include "cntcache_p.h"

void TestCntCache::initTestCase()
{
    // get contact manager
    mContactManager = new QContactManager("symbian");

    // start with a clean database
    cleanDatabase();

    // TODO: This test needs to be run with the default provider, so when there is a mechanism
    //       to variate the info provider, use that mechanism here to select the default provider.

    // create contacts; twins with same image at indexes 0,5 and 2,3 (good for increasing branch coverage)
    mContacts[0] = createContact("Joe", "Doe", "1234567", "c:/data/images/image1.png");
    mContacts[1] = createContact("Marja", "Meikalainen", "7654321", "");
    mContacts[2] = createContact("Sven", "Dufva", "12121212", "c:/data/images/image2.png");
    mContacts[3] = createContact("Twin", "Dufva", "12121212", "c:/data/images/image2.png");
    mContacts[4] = createContact("", "", "123123", "");
    mContacts[5] = createContact("Twin", "Doe", "1234568", "c:/data/images/image1.png");
}

void TestCntCache::cleanupTestCase()
{
    // end with a clean database
    cleanDatabase();

    delete mContactManager;
    mContactManager = NULL;
}

/*
    Test case: Constructing and deleting the cache.
 */
void TestCntCache::construction()
{
    // test creation
    QPointer<CntCache> cache = CntCache::instance();
    QVERIFY(cache != NULL);
    QPointer<CntCacheThread> worker = cache->mWorker;
    QVERIFY(worker != NULL);

    // test singleton property
    QVERIFY(cache == CntCache::instance());

    // test data members
    QVERIFY(cache->mContactManager != NULL);
    QVERIFY(worker->mContactManager != NULL);
    QVERIFY(worker->mThumbnailManager != NULL);
    QVERIFY(worker->mInfoProviders.count() >= 1);

    // test deletion
    cache->onShutdown();
    QVERIFY(CntCache::mInstance == NULL);
}

/*
    Test case: Fetch contact info.

    Fetches all six contacts in two batches of four and two contacts. The contacts are
    divided up so that one pair of twins is in the same batch and the other pair of twins
    has one twin in each batch. This maximizes branch coverage.
 */
void TestCntCache::fetchContactInfo()
{
    CntContactInfo info[CntTestContacts];    // info for each of the contacts

    // create cache and attach a signal spy to it
    CntCache* cache = CntCache::instance();
    QSignalSpy spy(cache, SIGNAL(contactInfoUpdated(QContactLocalId)));

    // create list of ids
    QList<QContactLocalId> idList;
    for (int i = 0; i < CntTestContacts; ++i) {
        idList << mContacts[i].localId();
    }

    // fetch three of the contacts in rapid succession, one of them twice
    info[1] = cache->fetchContactInfo(1, idList);
    info[0] = cache->fetchContactInfo(0, idList);
    info[4] = cache->fetchContactInfo(4, idList);
    info[0] = cache->fetchContactInfo(0, idList);
    info[5] = cache->fetchContactInfo(5, idList);

    // check that names are ok
    QVERIFY(info[1].name() == mContacts[1].displayLabel());
    QVERIFY(info[0].name() == mContacts[0].displayLabel());
    QVERIFY(info[4].name() == mContacts[4].displayLabel());
    QVERIFY(info[5].name() == mContacts[5].displayLabel());

    // wait for possible signals to arrive (in the future, all info could be returned immediately)
    QTest::qWait(4000);
    spy.clear();

    // fetch all contacts -- they should be cached now by the read-ahead mechanism
    for (int i = 0; i < CntTestContacts; ++i) {
        info[i] = cache->fetchContactInfo(i, idList);
    }

    // confirm that no further signals from cache (i.e. they were all really cached)
    QTest::qWait(2000);
    QVERIFY(spy.count() == 0);

    // confirm that returned data equals created data 
    for (int i = 0; i < CntTestContacts; ++i) {
        QVERIFY(info[i].name() == mContacts[i].displayLabel());
        QVERIFY(info[i].text() == mContacts[i].detail<QContactPhoneNumber>().number());
    }

    // confirm that info cache contains correct data
    int cacheItemCount = 0;
    foreach (CntInfoCacheItem* item, cache->mInfoCache) {
        // find corresponding contact
        for (int i = 0; i < CntTestContacts; ++i) {
            if (mContacts[i].localId() == item->contactId) {
                QVERIFY(item->name == mContacts[i].displayLabel());
                QVERIFY(item->text == mContacts[i].detail<QContactPhoneNumber>().number());
                QVERIFY(item->icons[0].replace('\\', '/') == mContacts[i].detail<QContactAvatar>().imageUrl().toString().replace('\\', '/'));
                ++cacheItemCount;
            }
        }
    }
    QVERIFY(cacheItemCount == CntTestContacts);

    // confirm that icon cache contains correct data
    cacheItemCount = 0;
    foreach (CntIconCacheItem* item, cache->mIconCache) {
        // find corresponding contact
        for (int i = 0; i < CntTestContacts; ++i) {
            if (mContacts[i].detail<QContactAvatar>().imageUrl().toString().replace('\\','/') == item->iconName.replace('\\','/')) {
                QVERIFY(item->isFetched);
                QVERIFY(!item->icon.isNull());
                ++cacheItemCount;
            }
        }
    }
    QVERIFY(cacheItemCount == 2*2); // two images, both used by two contacts

    cache->onShutdown();
}

/*
    Test case: Clear cache.
 */
void TestCntCache::clearCache()
{
    CntCache* cache = CntCache::instance();

    QList<QContactLocalId> idList;
    for (int i = 0; i < CntTestContacts; ++i) {
        idList << mContacts[i].localId();
    }

    cache->fetchContactInfo(1, idList);
    cache->fetchContactInfo(3, idList);

    QVERIFY(cache->mInfoCache.count() == 2);
    QTest::qWait(3000);
    QVERIFY(cache->mIconCache.count() == 2);

    cache->clearCache();
    QVERIFY(cache->mInfoCache.count() == 0);    // icon cache needn't be cleared
    
    delete cache;
}

/* TODO: Test cases for overflowing info and icon requests (>30) -- IDs need not be real, just confirm
         that cancellation notifications eventually arrive */

/*  
    Helper function for cleaning the database.
 */
void TestCntCache::cleanDatabase()
{
    QList<QContactLocalId> ids = mContactManager->contactIds();
    QMap<int, QContactManager::Error> errorMapInit;
    mContactManager->removeContacts(ids, &errorMapInit);
    mContactManager->removeContact(mContactManager->selfContactId());
}

/*  
    Helper function for creating contacts.
 */
QContact TestCntCache::createContact(QString firstName, QString lastName, QString phoneNumber, QString imageName)
{
    QContact contact;

    if (!firstName.isEmpty() && !lastName.isEmpty()) {
        QContactName name;
        name.setFirstName(firstName);
        name.setLastName(lastName);
        contact.saveDetail(&name);
    }

    if (!phoneNumber.isEmpty()) {
        QContactPhoneNumber number;
        number.setNumber(phoneNumber);
        number.setContexts(QContactDetail::ContextHome);
        number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        contact.saveDetail(&number);
    }

    if (!imageName.isEmpty()) {
        QContactAvatar avatar;
        avatar.setImageUrl(imageName);
        contact.saveDetail(&avatar);
    }

    mContactManager->saveContact(&contact);

    return contact;
}
