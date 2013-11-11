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
* Description: Provides services to parse CTI URIs.
*
*/

#ifndef CTIURISCHEMEPARSER_H
#define CTIURISCHEMEPARSER_H

#include <QObject>
#include <QString>
#include "urischemeparser.h"

class CtiUriSchemeParser : public UriSchemeParser
{
    Q_OBJECT

public:
    CtiUriSchemeParser(QObject *parent = 0);
    ~CtiUriSchemeParser();
    
public:
    bool isSupportedUriScheme(const QString &uri) const;
    bool parseUri(
        const QString &uri,
        QString &phoneNumber,
        QString &dtmfString) const;
    
private:
    bool validateUri(const QString &uri) const;
    void removeVisualSeparators(QString &uri) const;
    void convertToneInputToPostDialDtmfString(QString &uri) const;
};

#endif // CTIURISCHEMEPARSER_H
