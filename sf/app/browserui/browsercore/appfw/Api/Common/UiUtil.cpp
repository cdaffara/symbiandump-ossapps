/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include <QRegExp>
#include "UiUtil.h"

#define GOOGLE_SEARCH_ENGINE "http://www.google.com/search?q="

namespace WRT {


QString UiUtil::removeScheme(const QString & str ) 
{

    QUrl url(str);
    QString scheme=url.scheme();
    QString urlStr = str;

    if (scheme == "http" || scheme == "https") {
        urlStr.remove(0, scheme.length() + 3); //remove "scheme://"
    }
    return urlStr;

}

QString UiUtil::loadFiletoString(const QString &name)
{
    QFile file(name);
    file.open(QFile::ReadOnly);
    QString fileString = QLatin1String(file.readAll());
    file.close();
    return fileString;
}

QUrl UiUtil::searchUrl(const QString &string)
{
	QString urlStr = string.trimmed();
	QUrl url(QLatin1String(GOOGLE_SEARCH_ENGINE) + urlStr, QUrl::TolerantMode);
    if(url.isValid())
		return url;
    else
        return QUrl();
}

QUrl UiUtil::guessUrlFromString(const QString &string)
{
    QString urlStr = string.trimmed();
    QRegExp schemaTest(QLatin1String("^[a-zA-Z]+\\:.*"));

    // Check if it looks like a qualified URL. Try parsing it and see.
    bool hasSchema = schemaTest.exactMatch(urlStr);
    bool hasSpaces = urlStr.contains(" ");
    if (hasSchema) {
        if(!hasSpaces){
			QUrl url(urlStr, QUrl::TolerantMode);
			if (url.isValid())
				return url;
		}
		else{
			if(urlStr.contains("https://")){
				urlStr.remove("https://" ,Qt::CaseInsensitive);
			}
			else if(urlStr.contains("http://")){
				urlStr.remove("http://" ,Qt::CaseInsensitive);
			}
			else if(urlStr.contains("rtsp://")){
				urlStr.remove("rtsp://" ,Qt::CaseInsensitive);
			}
			QUrl url(QLatin1String(GOOGLE_SEARCH_ENGINE) + urlStr, QUrl::TolerantMode);
			if(url.isValid())
				return url;
		}
    }
    // Might be a file.
    if (hasSchema) {
        if (QFile::exists(urlStr))
            return QUrl::fromLocalFile(urlStr);
    }  
    // Might be a shorturl - try to detect the schema.
    if (!hasSchema) {
        int dotIndex = urlStr.indexOf(QLatin1Char('.'));
        if (dotIndex != -1 && !hasSpaces) {
            QUrl url;
            urlStr.endsWith(".") ? ( url.setUrl(QLatin1String("http://") + urlStr + QLatin1String("com"), QUrl::TolerantMode) ) : ( url.setUrl(QLatin1String("http://") + urlStr, QUrl::TolerantMode) );
            if (url.isValid()){
                return url;
            }
    //--Condition for Character DOT(.)--
            else {
                QUrl url(QLatin1String(GOOGLE_SEARCH_ENGINE) + urlStr, QUrl::TolerantMode);
                if(url.isValid())
                    return url;
            }
        }
    //--The string parameter is simple text and a search should be performed. Like for Special Character :\ etc.--
		else {
            QUrl url(QLatin1String(GOOGLE_SEARCH_ENGINE) + urlStr, QUrl::TolerantMode);
            if(url.isValid())
                return url;
        }
    }

    // Fall back to QUrl's own tolerant parser.
    return QUrl(string, QUrl::TolerantMode);
}

}
