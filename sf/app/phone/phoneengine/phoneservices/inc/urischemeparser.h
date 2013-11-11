/*!
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
* Description: Base class for URI Scheme parsers.
*
*/

#ifndef URISCHEMEPARSER_H
#define URISCHEMEPARSER_H

#include <QObject>
#include <QString>

class UriSchemeParser : public QObject
{
    Q_OBJECT

public:
    UriSchemeParser(QObject *parent = 0);
    ~UriSchemeParser();
    
    virtual bool isSupportedUriScheme(const QString &uri) const = 0;
    virtual bool parseUri(
        const QString &uri,
        QString &phoneNumber,
        QString &dtmfString) const = 0;

protected:
    bool parsePhoneDialString(
        const QString &uri,
        QString &phoneNumber,
        QString &dtmfString) const;
};

#endif // URISCHEMEPARSER_H
