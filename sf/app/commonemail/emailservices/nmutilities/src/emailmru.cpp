/*
 * Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Object for accessing most recent used email addresses and
 * display names.
 *
 */

#include "emailtrace.h"
#include "emailmru.h"

#include <XQSettingsManager>
#include <XQCentralRepositoryUtils>
#include <XQCentralRepositorySearchCriteria>

/*!
    Constructor
 */

// ID 0, Last saved ID.
// ID 1, Count of MRU Items.

EmailMRU::EmailMRU()
{
    NM_FUNCTION;

    mRepositoryManager = new XQSettingsManager();

    mLatestIndex = readCenRep(currentIndexKey).toInt();
    mCount = readCenRep(countKey).toInt();
}

/*!
    Destructor
 */
EmailMRU::~EmailMRU()
{
    NM_FUNCTION;

    QString entryIndex;
    entryIndex.setNum(mLatestIndex);

    QString count;
    count.setNum(mCount);

    writeCenRep(currentIndexKey, entryIndex);
    writeCenRep(countKey, count);

    delete mRepositoryManager;
}

/*!
    Add entry to MRU database
    \param displayName
    \param emailAddress
    \return success, was the update operation successful
 */
bool EmailMRU::updateMRU(QString displayName, QString emailAddress)
{
    NM_FUNCTION;

    bool success = false;
    qint32 foundEntryIndex = searchForAddress(emailAddress);
    if (foundEntryIndex) {
        success = updateEntry(foundEntryIndex, displayName);
    }

    else {
        qint32 entryIndex = getNextEntryIndex();
        success = writeEntry(entryIndex, displayName, emailAddress);
        mCount++;
        if (mCount > emailAddressHistorySize) {
            mCount = emailAddressHistorySize;
        }
    }

    return success;
}

/*!
    Get the number of entries
    \return number of entries
 */
quint32 EmailMRU::entryCount() const
{
    return mCount;
}

/*!
    Get entry details
    \param entryIndex entry index to look for
    \param displayName will contain displayname of entry if found
    \param address will contain address of entry if found
    \return success
 */
bool EmailMRU::getEntry(qint32 entryIndex, QString &displayName, QString &address)
{
    NM_FUNCTION;

    bool success = false;

    if ((entryIndex >= 1) && (mCount > 0)) {
        displayName = readCenRep(nameKeyByEntryIndex(entryIndex));
        address = readCenRep(addressKeyByEntryIndex(entryIndex));
        success = true;

        if (address.size()==0) {
            success = false;
        }
    }

    return success;
}

/*!
    Resets repository
 */
void EmailMRU::reset()
{
    NM_FUNCTION;

    XQCentralRepositoryUtils utils(*mRepositoryManager);
    utils.resetRepository(emailMruRepositoryKey);
    mLatestIndex = 0;
    mCount = 0;
}

/*!
    Writes to central repository
    \param index where to write
    \param value what to write
    \return success, whether write operation succeeded
 */
bool EmailMRU::writeCenRep(qint32 index, QString value)
{
    NM_FUNCTION;

    XQCentralRepositorySettingsKey writeKey(emailMruRepositoryKey, index);
    bool success = mRepositoryManager->writeItemValue(writeKey, value);
    if (!success) {
        XQCentralRepositoryUtils utils(*mRepositoryManager);
        success = utils.createKey(writeKey, value);
    }

    return success;
}

/*!
    Reads from central repository
    \param index to read
    \return value
 */
QString EmailMRU::readCenRep(qint32 index)
{
    NM_FUNCTION;

    XQCentralRepositorySettingsKey readKey(emailMruRepositoryKey, index);
    return mRepositoryManager->readItemValue(readKey, XQSettingsManager::TypeString).value<QString> ();
}

/*!
    Translates entry indexes into real central repository indexes
    \return central repository index
 */
quint32 EmailMRU::getNextEntryIndex()
{
    NM_FUNCTION;

    mLatestIndex++;

    if (mLatestIndex > emailAddressHistorySize) {
        mLatestIndex = 1;
    }

    return mLatestIndex;
}

/*!
    Translates central repository indexes into entry index
    \param realIndex central repository index
    \return entry index
 */
qint32 EmailMRU::entryIndex(qint32 crKey)
{
    NM_FUNCTION;

    qint32 entryIndex = 0;

    if (crKey%2 == 0){
        entryIndex = crKey/2;
    }
    else {
        entryIndex = (crKey-1)/2;
    }

    return entryIndex;
}

/*!
    Translates entry index into cenrep email address index
    \param entryIndex
    \return cenrep email address index
 */
qint32 EmailMRU::addressKeyByEntryIndex(qint32 entryIndex)
{
    return entryIndex*2+1;
}

/*!
    Translates entry index into cenrep displayname index
    \param entryIndex
    \return cenrep displayname index
 */
qint32 EmailMRU::nameKeyByEntryIndex(qint32 entryIndex)
{
    return entryIndex*2;
}

/*!
    Checks if MRU list already contains the address
    \param address to search
    \return entry index of found address, zero if not found
 */
qint32 EmailMRU::searchForAddress(QString address)
{
    NM_FUNCTION;

    quint32 partialKey(0x00000000);
    quint32 bitMask(0x00000000);
    XQCentralRepositorySearchCriteria criteria(emailMruRepositoryKey, partialKey, bitMask);
    criteria.setValueCriteria(address);

    // Find the keys.
    XQCentralRepositoryUtils utils(*mRepositoryManager);
    QList<XQCentralRepositorySettingsKey> foundKeys = utils.findKeys(criteria);

    qint32 index = 0;
    // We should only get one key as a result.
    if (foundKeys.count() == 1) {
        index = foundKeys[0].key();
    }
    return entryIndex(index);
}

/*!
    Moves entry, will overwrite the newEntryIndex
    \param oldEntryIndex entry to be moved
    \param newEntryIndex where to move
 */
void EmailMRU::moveEntry(qint32 oldEntryIndex, qint32 newEntryIndex)
{
    NM_FUNCTION;

    // Save the values to be moved
    QString name = readCenRep(nameKeyByEntryIndex(oldEntryIndex));
    QString address = readCenRep(addressKeyByEntryIndex(oldEntryIndex));

    // Overwrite values into new entry
    writeEntry(newEntryIndex, name, address);
}

/*!
    Updates displayname and sets entry as newest
    \param entryIndex to update
    \param newDisplayName
    \return success was update operation successfull
 */
bool EmailMRU::updateEntry(qint32 entryIndex, QString newDisplayName)
{
    NM_FUNCTION;

    // Save address
    QString address = readCenRep(addressKeyByEntryIndex(entryIndex));

    // Move all entries following the one to be updated
    // until we go past the newest entry
    for (qint32 i = entryIndex, j = 0; i != mLatestIndex;) {
        j = i+1;
        if (j > emailAddressHistorySize) j = 1;

        moveEntry(j, i);

        i++;
        if (i > emailAddressHistorySize) i = 1;
    }

    // Write the updated entry as the newest entry
    return writeEntry(mLatestIndex, newDisplayName, address);
}

/*!
    Writes entry. Overwrites any existing data at give index
    \param entryIndex to write
    \param displayName
    \param emailAddress
    \return success was update operation successfull
 */
bool EmailMRU::writeEntry(qint32 entryIndex, QString displayName, QString emailAddress)
{
    NM_FUNCTION;

    bool nameSuccess = false;
    bool addressSuccess = false;

    nameSuccess = writeCenRep(nameKeyByEntryIndex(entryIndex), displayName);
    if (nameSuccess) {
        addressSuccess = writeCenRep(addressKeyByEntryIndex(entryIndex), emailAddress);
    }

    // Writing name succeeded but writing address not, we destroy the name
    if (!addressSuccess) {
        XQCentralRepositoryUtils utils(*mRepositoryManager);
        XQCentralRepositorySettingsKey deleteKey(emailMruRepositoryKey, nameKeyByEntryIndex(entryIndex));
        utils.deleteKey(deleteKey);
    }

    return (nameSuccess & addressSuccess);
}

/*!
    Gets first name / last name order from cenrep.
    defined in cntuids.h.
    \return name order.
 */
int EmailMRU::nameOrder()
{
    XQSettingsManager *repositoryManager = new XQSettingsManager();
    XQCentralRepositorySettingsKey readKey(contactsRepositoryKey,
                                           nameOrderKey);

    int nameOrder = repositoryManager->readItemValue(readKey,
                                                     XQSettingsManager::TypeInt).toInt();

    delete repositoryManager;

    return nameOrder;
}

