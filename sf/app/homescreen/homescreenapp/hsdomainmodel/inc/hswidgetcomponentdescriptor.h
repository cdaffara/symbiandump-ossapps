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

#ifndef HSWIDGETCOMPONENTDESCRIPTION_H
#define HSWIDGETCOMPONENTDESCRIPTION_H

#include <QMetaType>
#include <QString>

#include "hsdomainmodel_global.h"

class HSDOMAINMODEL_EXPORT HsWidgetComponentDescriptor
{
public:
    void setInstallationPath(const QString &path) { mInstallationPath = path; }
    QString installationPath() const { return mInstallationPath; }
    
    void setUri(const QString &uri) { mUri = uri; }
    QString uri() const { return mUri; }

    void setTitle(const QString &title) { mTitle = title; }
    QString title() const { return mTitle; }

    void setDescription(const QString &description) { mDescription = description; }
    QString description() const { return mDescription; }

    void setIconUri(const QString &iconUri) { mIconUri = iconUri; }
    QString iconUri() const { return mIconUri; }

    void setHidden(const QString &hidden) { mHidden = hidden; }
    QString hidden() const { return mHidden; }

    void setServiceXml(const QString &xml) { mServiceXml = xml; }
    QString serviceXml() const { return mServiceXml; }

    void setVersion(const QString &version) { mVersion = version; }
    QString version() const { return mVersion; }

    void setTranslationFilename(const QString &file) { mTranslationFilename = file; }
    QString translationFilename() const { return mTranslationFilename; }

    bool isValid();

private:
    QString mInstallationPath;
    QString mUri;
    QString mTitle;
    QString mDescription;
    QString mIconUri;
    QString mHidden;
    QString mServiceXml;
    QString mVersion;
    QString mTranslationFilename;
};

Q_DECLARE_METATYPE(HsWidgetComponentDescriptor)

#endif 
