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
* Description: TEL URI parser.
*
*/

#ifndef TELURISCHEMEPARSER_H
#define TELURISCHEMEPARSER_H

#include <QObject>
#include <QString>
#include "urischemeparser.h"

class TelUriSchemeParser : public UriSchemeParser
{
    Q_OBJECT
public:
    TelUriSchemeParser(QObject *parent = 0);
    ~TelUriSchemeParser();
    
public:
    bool isSupportedUriScheme(const QString &uri) const;
    bool parseUri(
        const QString &uri,
        QString &phoneNumber,
        QString &dtmfString) const;
};

#endif // TELURISCHEMEPARSER_H
