/*
 * Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef NMAPIEMAILADDRESS_H
#define NMAPIEMAILADDRESS_H

#include <QExplicitlySharedDataPointer>
#include <nmapidef.h>

/*!
   Email address
 */

class QString;

namespace EmailClientApi
{

class NmApiEmailAddressPrivate;

class NMAPI_EXPORT NmApiEmailAddress
{
public:
    /*!
       Constructor for NmApiEmailAddress class
     */
    NmApiEmailAddress();
    virtual ~NmApiEmailAddress();

    /*!
       copying constructor for NmApiEmailAddress
     */
    NmApiEmailAddress(const NmApiEmailAddress &addr);

    /*!
       Assignment operator
     */
    NmApiEmailAddress &operator=(const NmApiEmailAddress &addr);

    /*!
       Compares data from \a addr
     */
    bool operator==(const NmApiEmailAddress &addr);

    /*
       getter for displayname
     */
    QString displayName() const;

    /*!
       getter for address
     */
    QString address() const;

    /*!
       setter for displayname 
     */
    void setDisplayName(const QString &displayName);

    /*!
       setter for address
     */
    void setAddress(const QString &address);
private:
    QExplicitlySharedDataPointer<NmApiEmailAddressPrivate> d;
};
}

#endif
