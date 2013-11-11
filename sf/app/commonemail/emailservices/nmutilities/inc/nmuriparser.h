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
* Description: A utility class for parsing URI strings
*
*/
#ifndef NMURIPARSER_H_
#define NMURIPARSER_H_

#include <QObject>
#include <QStringList>

#include "emailutilitiesdef.h"

/*!
    \class NmUriParser
    \brief A helper class for parsing URI strings
*/
class EMAIL_UTILITIES_EXPORT NmUriParser
{
public:

    NmUriParser();
    ~NmUriParser();

    bool extractData(const QString &data);
    void resetData();
    
    QString subject() const;
    QStringList toAddresses() const;
    QStringList ccAddresses() const;
    QStringList bccAddresses() const;
    QString bodyText() const;
    
private:
    
    void addAddressesToList(QString &addresses, QStringList &list);

private: // Data

    QString mSubject;
    QStringList mToAddresses;
    QStringList mCcAddresses;
    QStringList mBccAddresses;
    QString mBodyText;
};
#endif /* NMURIPARSER_H_*/
