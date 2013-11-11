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
#include "cplanguagepluginutil.h"
#include <QLocale>
#include <hblocaleutil.h>
#include "cplanguagepluginlog.h"
#include <hbinputsettingproxy>

QStringList CpLanguagePluginUtil::localizedLanguageNames(const QStringList &languageNames)
{
    QStringList localizedLanguages;
    
    CPLANG_LOG("=================All languages=======================");
    foreach(const QString &language,languageNames) {
        CPLANG_LOG("Language: " + language + "\tLocalised:" + HbLocaleUtil::localisedLanguageName(language));
        localizedLanguages << HbLocaleUtil::localisedLanguageName(language);
    }
    CPLANG_LOG("======================================================");
    
    return localizedLanguages;
}

QStringList CpLanguagePluginUtil::localizedRegionNames(const QStringList &regionNames)
{
    QStringList localizedRegions;
    
    CPLANG_LOG("=================All regions=======================");
    foreach(const QString &region,regionNames) {
        CPLANG_LOG("Region: " + region + "\tLocalised:" + HbLocaleUtil::localisedRegionName(region));
        localizedRegions << HbLocaleUtil::localisedRegionName(region);
    }
    CPLANG_LOG("===================================================");
    return localizedRegions;
}

QStringList CpLanguagePluginUtil::localizedInputLanguageNames(const QList<HbInputLanguage> &inputLanguages)
{
    QStringList localizedInputLanguages;
    
    CPLANG_LOG("=================All Input languages=======================");
    foreach (HbInputLanguage inputLanguage, inputLanguages) {
        CPLANG_LOG("Input language:" + inputLanguage.asString() + "\tLocalised:" + inputLanguage.localisedName());
        if (inputLanguage == HbInputLanguage()) {
            localizedInputLanguages << hbTrId("txt_cp_setlabel_secondary_writing_language_val_non");
        }
        else {
            localizedInputLanguages << inputLanguage.localisedName();
        }
    }
    CPLANG_LOG("============================================================");
    
    return localizedInputLanguages;
}

bool CpLanguagePluginUtil::isChineseVariant()
{    
    QStringList supportedLanguages = HbLocaleUtil::supportedLanguages();
    foreach(const QString &language, supportedLanguages) {
        if (QLocale(language).language() == QLocale::Chinese) {
            CPLANG_LOG("Chinese Variant.");
            return true;
        }
    }
    return false;
}

//End of File

