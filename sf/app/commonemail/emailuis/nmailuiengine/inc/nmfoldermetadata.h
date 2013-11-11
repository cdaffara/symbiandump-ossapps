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

#ifndef NMFOLDERMETADATA_H_
#define NMFOLDERMETADATA_H_

#include "nmuienginedef.h"
#include "nmcommon.h"

class NmFolderMetaDataPrivate;


class NMUIENGINE_EXPORT NmFolderMetaData
{
public:

    NmFolderMetaData();
    virtual ~NmFolderMetaData();
    QString name() const;
    void setName(const QString &name);
    NmId id() const;
    void setId(const NmId &id);

private:

    NmFolderMetaDataPrivate *d;
};


#endif /* NMFOLDERMETADATA_H_ */
