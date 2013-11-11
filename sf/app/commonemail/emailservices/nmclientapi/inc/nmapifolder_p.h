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

#ifndef NMAPIFOLDER_P_H
#define NMAPIFOLDER_P_H

#include <nmapicommon.h>


namespace EmailClientApi
{

class NmApiFolderPrivate : public QSharedData
{
public:
    NmApiFolderPrivate() 
    {
        
    }
    virtual ~NmApiFolderPrivate()
    {
        
    }

    quint64 id;
    QString name;
    EmailClientApi::NmApiEmailFolderType folderType;
    quint64 parentId;
    QList<quint64> childFolderIds;
};

}

#endif /* NMAPIFOLDER_P_H */
