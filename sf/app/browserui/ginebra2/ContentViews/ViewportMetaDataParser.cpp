/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ViewportMetaDataParser.h"

namespace GVA {

ViewportMetaDataParser::ViewportMetaDataParser(const QRect& clientRect)
            : m_clientRect(clientRect)
{}

ViewportMetaDataParser::~ViewportMetaDataParser()
{}

ViewportMetaData ViewportMetaDataParser::parse(const QString& viewportParams)
{
    ViewportMetaData viewportMetaData = processArguments(viewportParams);
    viewportMetaData.adjustViewportData(m_clientRect);
    return viewportMetaData;
}


//W A R N I N G
//FOLLOWING CODE HAS BEEN TAKEN FROM WebCore/dom/Document.cpp
//NEED TO REMOVE WHEN QtWebkit provides viewport param API

// Though isspace() considers \t and \v to be whitespace, Win IE doesn't.
bool ViewportMetaDataParser::isSeparator(QChar c)
{
    return c == ' '
            || c == '\t'
            || c == '\n'
            || c == '\r'
            || c == '='
            || c == ','
            || c == '\0'
            || c == ';';
}

ViewportMetaData ViewportMetaDataParser::processArguments(const QString& features)
{
    int keyBegin, keyEnd;
    int valueBegin, valueEnd;
    ViewportMetaData viewportMetaData;

    int i = 0;
    int length = features.length();
    QString buffer = features.toLower();

    while (i < length) {
        // skip to first non-separator, but don't skip past the end of the string
        while (isSeparator(buffer[i])) {
            if (i >= length)
                break;
            i++;
        }
        keyBegin = i;

        // skip to first separator
        while (!isSeparator(buffer[i]))
            i++;
        keyEnd = i;

        // skip to first '=', but don't skip past a ',' or the end of the string
        while (buffer[i] != '=') {
            if (buffer[i] == ',' || i >= length)
                break;
            i++;
        }
        // skip to first non-separator, but don't skip past a ',' or the end of the string
        while (isSeparator(buffer[i])) {
            if (buffer[i] == ',' || i >= length)
                break;
            i++;
        }
        valueBegin = i;

        // skip to first separator
        while (!isSeparator(buffer[i]))
            i++;
        valueEnd = i;

        Q_ASSERT(i <= length);

        QString keyString = buffer.mid(keyBegin, keyEnd - keyBegin);
        QString valueString = buffer.mid(valueBegin, valueEnd - valueBegin);
        setViewportFeature(keyString, valueString, viewportMetaData);
    }
    return viewportMetaData;
}

void ViewportMetaDataParser::setViewportFeature(const QString& keyString, const QString& valueString, ViewportMetaData& viewportMetaData)
{
    float value = ViewportMetaData::ValueUndefined;

    if (QString::compare(valueString, "device-width", Qt::CaseInsensitive) == 0)
        value = m_clientRect.width();
    else if (QString::compare(valueString, "device-height", Qt::CaseInsensitive) == 0)
        value = m_clientRect.height();
    else if (QString::compare(valueString, "default", Qt::CaseInsensitive) == 0) {
        // This allows us to distinguish the omission of a key from asking for the default value.
        value = -2;
    }
    else if (valueString.length()) {
        // listing a key with no value is shorthand for key=default
        value = valueString.toDouble();
    }

    if (keyString == "initial-scale") {
        viewportMetaData.m_initialScale = value;
        viewportMetaData.setFlag(ViewportMetaData::UserDefinedInitialScale,true);
    }
    else if (keyString == "minimum-scale") {
        viewportMetaData.m_minimumScale = value;
        viewportMetaData.m_specifiedData.m_minScale = value;
        viewportMetaData.setFlag(ViewportMetaData::UserDefinedMinumumScale,true);
    }
    else if (keyString == "maximum-scale") {
        viewportMetaData.m_maximumScale = value;
        viewportMetaData.setFlag(ViewportMetaData::UserDefinedMaximumScale,true);
    }
    else if (keyString == "user-scalable") {
        if (QString::compare(valueString, "yes", Qt::CaseInsensitive) == 0)
            viewportMetaData.m_userScalable = true;
        else
            viewportMetaData.m_userScalable = false;
    }
    else if (keyString == "width") {
        viewportMetaData.m_width = value;
        viewportMetaData.m_specifiedData.m_width = valueString;
    }
    else if (keyString == "height") {
        viewportMetaData.m_height = value;
        viewportMetaData.m_specifiedData.m_height = valueString;
    }
}

} //namespace GVA
