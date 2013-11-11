/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  utility class
 *
 */
#ifndef CPLANGUAGEPLUGINUTIL_H
#define CPLANGUAGEPLUGINUTIL_H

#include <QStringList>
#include <QList>
#include <hbinputlanguage.h>

class CpLanguagePluginUtil
{
public:
    static QStringList localizedLanguageNames(const QStringList &languageNames);
    static QStringList localizedRegionNames(const QStringList &regionNames);
    static QStringList localizedInputLanguageNames(const QList<HbInputLanguage> &inputLanguages);
    static bool isChineseVariant();
private:
    CpLanguagePluginUtil();
    Q_DISABLE_COPY(CpLanguagePluginUtil)
};

#endif

//End of File
