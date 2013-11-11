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
 * Description: caservice_p.h
 *
 */

#ifndef CASERVICE_PRIVATE_H
#define CASERVICE_PRIVATE_H

#include <QList>
#include <caentry.h>

class QString;
class CaQuery;
class CaServicePrivate
{

public:
    ~CaServicePrivate() {
        clearAll();
    }
    
    QList< QSharedPointer<CaEntry> > mCreatedEntries;
    QList< QSharedPointer<CaEntry> > mReturnedEntries;
    int mEntryToRemove;
    QSharedPointer<CaEntry> mUpdatedEntry;
    CaQuery* mQueryPassedToGetList;
    
    void AddEntriesForUpdate();
    void clearAll();    
    static CaServicePrivate& instance() {
        static CaServicePrivate caServicePrivate;
        return caServicePrivate;
    }
    
private:
    
    explicit CaServicePrivate(): mUpdatedEntry (NULL), mQueryPassedToGetList (NULL)
    {
    }    
};

#endif //CASERVICE_PRIVATE_H
