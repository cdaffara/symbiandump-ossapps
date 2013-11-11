/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  
 *
 */
#ifndef SEARCH_UTILITY_H
#define SEARCH_UTILITY_H

#include <QStringList>

class SearchUtility
    {
public:
    static QStringList drives();
    static QStringList pluginDirs();
    static QStringList SearchcfgDirs();

private:
    SearchUtility();
    };

#endif //SEARCH_UTILITY_H
