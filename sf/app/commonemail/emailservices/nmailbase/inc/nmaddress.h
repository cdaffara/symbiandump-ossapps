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

#ifndef NMADDRESS_H_
#define NMADDRESS_H_

#include <QExplicitlySharedDataPointer>

#include "nmbasedef.h"


class NMBASE_EXPORT NmAddressPrivate : public QSharedData
{
public:
    NmAddressPrivate();
    virtual ~NmAddressPrivate();

public:
    QString mDisplayName;
    QString mAddress;
};

class NMBASE_EXPORT NmAddress
{
public:
    NmAddress();
    NmAddress(const QString &displayName, const QString &address);
    NmAddress(const QString &address);
    NmAddress(const NmAddress &nmAddress);
    NmAddress(QExplicitlySharedDataPointer<NmAddressPrivate> nmPrivateAddress);
    NmAddress &operator=(const NmAddress &nmAddress);
    virtual ~NmAddress();

    bool operator==(const NmAddress &otherAddress) const;
    bool operator!=(const NmAddress &otherAddress) const;

    void setDisplayName(const QString &displayName);
    QString displayName() const;
    void setAddress(const QString &address);
    QString address() const;

private:
    // Data object shared between Symbian and QT data types.
    QExplicitlySharedDataPointer<NmAddressPrivate> d;

    friend class CFSMailAddress;
};

#endif /* NMADDRESS_H_ */





