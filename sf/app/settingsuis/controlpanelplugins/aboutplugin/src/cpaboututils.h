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

#ifndef CPABOUTUTILS_H
#define CPABOUTUTILS_H

#include <QString>
#include <QChar>
#include <HbTextEdit>

//for Qt
const QChar lineBreak = '\n';
const QString htmlLineBreak = "<br>";
const QString doubleHtmlLineBreak = "<br><br>";
const QString htmlLinkStart("<a href=\"link\">");
const QString htmlLinkEnd("</a>");
const TInt sysUtilVersionTextLength = 128;
//for Symbian
_LIT(KEol, "\n");

class CpAboutUtils
{
public:
    static HbTextEdit *createTextEdit();
    static QString contentString(const QString &prefixString, int index);
    static QString linkHtmlContent(const QString &textContent);
    static QString findAndReplaceWithLink(const QString &string);
    static QString preprocessText(const QString &string);
    static QString getPhoneModel();
    static QString getProductRelease();
    static QString getSoftwareVersion();
    static QString getPhoneType();
};

#endif /* CPABOUTUTILS_H */
