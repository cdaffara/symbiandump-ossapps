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
* Description: Data container & constants for "phonebookservices" Qt Highway services.
*
*/

#ifndef CNTSERVICESCONTACT_H_
#define CNTSERVICESCONTACT_H_

#include <QList>
#include <QString>
#include <xqserviceipcmarshal.h>

//Action filter definitions:

/// Action filter that DOES NOT filter away based on supported action.
const QString KCntActionAll("all");
/// Action filter for showing only contacts that support calling.
const QString KCntActionCall("call");
/// Action filter for showing only contacts that suppport SMS sending.
const QString KCntActionSms("sms");
// Action filter for showing only contacts that support emailing.
const QString KCntActionEmail("email");


/// Return values from editing service
const int KCntServicesReturnValueContactSaved       = 1;
const int KCntServicesReturnValueContactDeleted     = -1;
const int KCntServicesReturnValueContactNotModified = 0;
const int KCntServicesTerminated = -2;

/**
 * A data container class to be used with "phonebookservices" Qt Highway services.
 */
class CntServicesContact
{
public:
    CntServicesContact() : mContactId(-1) {};
    virtual ~CntServicesContact() {};

    QString mDisplayName;
    QString mPhoneNumber;
    QString mEmailAddress;
    int mContactId; //should be changed to QContactUniqueId

    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);
};

template <typename Stream> inline void CntServicesContact::serialize(Stream &s) const
{
    s << mDisplayName;
    s << mPhoneNumber;
    s << mEmailAddress;
    s << mContactId;
}

template <typename Stream> inline void CntServicesContact::deserialize(Stream &s)
{
    s >> mDisplayName;
    s >> mPhoneNumber;
    s >> mEmailAddress;
    s >> mContactId;
}

typedef QList<CntServicesContact> CntServicesContactList;

//-------------------------------------------------------------------
// These are DEPRECATED and will be removed soon:

/// @deprecated
const QString KCntFilterDisplayAll("all");
/// @deprecated
const QString KCntFilterDisplayFavorites("favorites");

/// @deprecated
const QString KCntNoSelectionMode("No Selection");
/// @deprecated
const QString KCntSingleSelectionMode("SingleSelect");
/// @deprecated
const QString KCntMultiSelectionMode("MultiSelect");

//-------------------------------------------------------------------

Q_DECLARE_USER_METATYPE(CntServicesContact)
Q_DECLARE_USER_METATYPE_NO_OPERATORS(CntServicesContactList)

#endif /* CNTSERVICESCONTACT_H_ */
