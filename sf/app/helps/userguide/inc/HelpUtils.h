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

#ifndef HELPUTILS_H
#define HELPUTILS_H

#include <QString>

class HelpUtils
{
public:
	static void loadHtmlFromZipFile(const QString& path, const QString& htmlFile, QString& htmlContent);
	static int launchApplication(const QString& appUid);
	static bool suppportFeatureID(int featureID);
	static QString rootPath();
	static QString UILocaleFromQtToSymbian();
	static Qt::Orientation defaultOrientation();
	static Qt::SortOrder sortOrder();
	static int findStr(const QString& strFrom, const QString& strToFind);
	static int compareStr(const QString& str1, const QString& str2);
};

#endif //HELPUTILS_H
