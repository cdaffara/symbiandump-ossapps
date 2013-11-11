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

#ifndef RADIONOTIFICATIONDATA_H
#define RADIONOTIFICATIONDATA_H

// System includes
#include <QVariant>

class RadioNotificationData
{
public:

    RadioNotificationData() : mType( 0 ) {}
    RadioNotificationData( int type, const QVariant& data ) :
        mType( type ),
        mData( data )
    {}

    virtual ~RadioNotificationData() {}

    #ifndef BUILD_WIN32
    template <typename Stream> void serialize( Stream& stream ) const;
    template <typename Stream> void deserialize( Stream& stream );
    #endif // BUILD_WIN32

    int         mType;
    QVariant    mData;

};

#ifndef BUILD_WIN32

Q_DECLARE_USER_METATYPE(RadioNotificationData)

template <typename Stream> void RadioNotificationData::serialize( Stream& stream ) const
{
    stream << mType;
    stream << mData;
}

template <typename Stream> void RadioNotificationData::deserialize( Stream& stream )
{
    stream >> mType;
    stream >> mData;
}

Q_IMPLEMENT_USER_METATYPE(RadioNotificationData)

#else

Q_DECLARE_METATYPE( RadioNotificationData )

#endif // BUILD_WIN32
#endif // RADIONOTIFICATIONDATA_H
