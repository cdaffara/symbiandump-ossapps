/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: CmManagerShim stub
 */

#ifndef CMMANAGER_SHIM_H
#define CMMANAGER_SHIM_H

#include <qstring.h>
#include <qhash.h>

class CmConnectionMethodShim;

namespace CMManagerShim {
    enum ConnectionMethodAttribute {
        CmName,
        CmIapId,
        CmId,
    };
}


class CmManagerShim
{

public: 

    /*!
     *  Constructor.
     */
    CmManagerShim();

    /*!
     *  Destructor.
     */
    ~CmManagerShim();
    
    /*!
     * Returns the connection method queried by its ID.
     * 
     * @param[in] connectionMethodId Id of the connection method.
     * @return Found connection method instance.
     */
    CmConnectionMethodShim *connectionMethod(uint connectionMethodId) const;
    
    /*!
     * Returns the list of connection methods that do not belong to any 
     * destination.
     * 
     * @param[out] cmArray On return it is the array of connection method ids.
     * @param[in] legacyOnly When set to true only Legacy connection method
     *                       ids (connection methods which do not belong to
     *                       any destination) will be returned
     */
    void connectionMethod(
        QList<uint> &cmArray,
        bool legacyOnly = true) const;
    
    
    void clear();
    
public: // data
    
    QHash<uint, CmConnectionMethodShim*> mConnectionMethods;
    
    static int mConnectionMethodThrowsAtId;
    static int mDestructorCount;
};

#endif /* CMMANAGER_SHIM_H */
