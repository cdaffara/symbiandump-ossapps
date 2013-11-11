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

#ifndef IRSERVICEDATA_H
#define IRSERVICEDATA_H

#include <QVariant>
#include <xqserviceipcmarshal.h>

class IrServiceData
{
public:

    IrServiceData() : mType(-1) {}
    IrServiceData(int aType, const QVariant &aData) :
        mType(aType),
        mData(aData)
    {}

    virtual ~IrServiceData() {}

    int        mType;
    QVariant   mData;
    
    template <typename Stream> void serialize(Stream &aStream) const;
    template <typename Stream> void deserialize(Stream &aStream);
};

template <typename Stream> inline void IrServiceData::serialize(Stream &aStream) const
{
    aStream << mType;
    aStream << mData;
}

template <typename Stream> inline void IrServiceData::deserialize(Stream &aStream)
{
    aStream >> mType;
    aStream >> mData;
}


typedef QList<IrServiceData> IrServiceDataList;

Q_DECLARE_USER_METATYPE(IrServiceData)
Q_DECLARE_USER_METATYPE_NO_OPERATORS(IrServiceDataList)


#endif // IRSERVICEDATA_H
