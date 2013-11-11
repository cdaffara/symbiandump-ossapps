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
* Description:
*
*/

#ifndef HSWIDGETCOMPONENTPARSER_H
#define HSWIDGETCOMPONENTPARSER_H

#include <QObject>
#include <QXmlStreamReader>
#include "hswidgetcomponentdescriptor.h"

class HsWidgetComponentParser : public QObject
{
    Q_OBJECT
    
public:
    HsWidgetComponentParser(const QString &aComponentManifest, QObject *parent=0);
    ~HsWidgetComponentParser();
    HsWidgetComponentDescriptor widgetComponentDescriptor();
    bool error();
private:
    void parseFile(const QString &aComponentManifest);
    void parseWidgetManifestTag();
    void parseWidgetProviderTag();
    
    void parseVersion1_0();

    void parseUri();
    void parseIcon();
    void parseTitle();
    void parseDescription();
    void parseHidden();
    void parseServiceXml();
    void parseTranslationFilename();
    
private:
    QXmlStreamReader mParser;
    HsWidgetComponentDescriptor mComponentDescriptor;
    bool mError;
};

#endif // HSWIDGETCOMPONENTPARSER_H
