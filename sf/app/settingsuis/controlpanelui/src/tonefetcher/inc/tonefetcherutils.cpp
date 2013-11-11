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
 *     The source file for tone fetcher utilities.
 *     
 */

#include "tonefetcherutils.h"
#include <QDir>
#include <QChar>

QString ToneFetcherUtils::normalizeSeperator(const QString &path)
{
    QString standardpath( path );
    QChar c('/');
    QChar c1('\\');
    if (standardpath.contains(c, Qt::CaseSensitive)) {
        standardpath.replace(c, QDir::separator());
    }
    if (standardpath.contains(c1, Qt::CaseSensitive)) {
        standardpath.replace(c1, QDir::separator());
    }        
    return standardpath;
}
