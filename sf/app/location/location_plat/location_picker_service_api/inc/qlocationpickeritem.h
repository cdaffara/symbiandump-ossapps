/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: QLocationPickerItem class
*
*/


#ifndef QLOCATIONPICKERITEM_H
#define QLOCATIONPICKERITEM_H

#include <QString>
#include <xqserviceipcmarshal.h>

/** class defines location type
  */
class QLocationPickerItem
{
public:
    QLocationPickerItem():mIsValid(false),
    	                  mLatitude(200.0),
    	                  mLongitude(200.0)
     {};
    ~QLocationPickerItem() {};

    // copy contructor
    inline QLocationPickerItem(const QLocationPickerItem &l)
    {
        mIsValid = l.mIsValid;
        mName = l.mName;
        mStreet = l.mStreet;
        mPostalCode = l.mPostalCode;
        mCity = l.mCity;
        mState = l.mState;
        mCountry = l.mCountry;
        mLatitude = l.mLatitude;
        mLongitude = l.mLongitude;
    };

    // assignment operator
    inline QLocationPickerItem &operator=(const QLocationPickerItem &l)
    {
        mIsValid = l.mIsValid;
        mName = l.mName;
        mStreet = l.mStreet;
        mPostalCode = l.mPostalCode;
        mCity = l.mCity;
        mState = l.mState;
        mCountry = l.mCountry;
        mLatitude = l.mLatitude;
        mLongitude = l.mLongitude;
        return *this;
    };

    bool mIsValid; // if the current object is valid
    QString mName; // location name
    QString mStreet; // location street
    QString mPostalCode; // location postal code
    QString mCity; // location city
    QString mState; // location state/region
    QString mCountry; // location country
    double mLatitude; // latitude
    double mLongitude; // longitude

    // functions to pass data between application
    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);
};

template <typename Stream> inline void QLocationPickerItem::serialize(Stream &s) const
{
    s << mIsValid;
    s << mName;
    s << mStreet;
    s << mPostalCode;
    s << mCity;
    s << mState;
    s << mCountry;
    s << mLatitude;
    s << mLongitude;
}

template <typename Stream> inline void QLocationPickerItem::deserialize(Stream &s)
{
    s >> mIsValid;
    s >> mName;
    s >> mStreet;
    s >> mPostalCode;
    s >> mCity;
    s >> mState;
    s >> mCountry;
    s >> mLatitude;
    s >> mLongitude;
}


Q_DECLARE_USER_METATYPE(QLocationPickerItem)

#endif // QLOCATIONPICKERITEM_H
