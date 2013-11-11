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

#ifndef NMMAILBOXMETADATA_H_
#define NMMAILBOXMETADATA_H_

#include <QMetaType>
#include "nmuienginedef.h"
#include "nmcommon.h"

class NmMailboxMetaDataPrivate;

class NMUIENGINE_EXPORT NmMailboxMetaData
{
public:

    NmMailboxMetaData();
    virtual ~NmMailboxMetaData();
    QString name() const;
    void setName(const QString &name);
    NmId id() const;
    void setId(const NmId &id);
    NmId IconId() const;
    void setIconId(const NmId &id);
    QString address() const;
    void setAddress(const QString &address);

private:

    NmMailboxMetaDataPrivate *d;
};

Q_DECLARE_METATYPE(NmMailboxMetaData*)

#endif /* NMMAILBOXMETADATA_H_ */
