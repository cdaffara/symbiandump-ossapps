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

#ifndef EMAILMRU_H_
#define EMAILMRU_H_

#include <QString>
#include "emailutilitiesdef.h"

class XQSettingsManager;


/*!
     Key to MRU repository
 */
static const qint32 emailMruRepositoryKey = 0x2003EA97;

/*!
     Where to find the cenrep-index of the newest saved entry
 */
static const qint32 currentIndexKey = 0;

/*!
     Where to find the number of entries
 */
static const qint32 countKey = 1;

/*!
     Key to contacts central repository
 */
static const qint32 contactsRepositoryKey = 0x2002FF54;

/*!
     Setting for name ordering.
 */
static const qint32 nameOrderKey = 0x1;

/*!
     How many entries are kept in MRU repository
 */
static const qint32 emailAddressHistorySize = 300;

class EMAIL_UTILITIES_EXPORT EmailMRU
{
public:

    EmailMRU();

    virtual ~EmailMRU();

    bool updateMRU(QString displayName, QString emailAddress);

    quint32 entryCount() const;

    bool getEntry(qint32 entryIndex, QString &displayName, QString &address);

    void reset();

    static int nameOrder();

private:

    quint32 getNextEntryIndex();

    bool writeCenRep(qint32 index, QString value);

    QString readCenRep(qint32 index);

    qint32 searchForAddress(QString address);

    static qint32 entryIndex(qint32 crKey);

    static qint32 addressKeyByEntryIndex(qint32 entryIndex);

    static qint32 nameKeyByEntryIndex(qint32 entryIndex);

    void moveEntry(qint32 oldEntryIndex, qint32 newEntryIndex);

    bool writeEntry(qint32 entryIndex, QString displayName, QString emailAddress);

    bool updateEntry(qint32 entryIndex, QString newDisplayName);

private:

    /*!
        Points to the latest saved entry
     */
    qint32 mLatestIndex;

    /*!
        Pointer to central repository manager
     */
    XQSettingsManager* mRepositoryManager;

    /*!
        Number of entries
     */
    quint32 mCount;

};


#endif // EMAILMRU_H_
