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

#include "ViewportMetaData.h"

static const int DefaultViewportWidth = 640;
static const int DefaultViewportHeight = 640;
static const int MinViewportWidth = 200;
static const int MaxViewportWidth = 10000;
static const int MinViewportHeight = 200;
static const int MaxViewportHeight = 10000;
static const qreal DefaultMinScale = 0.2;
static const qreal DefaultMaxScale = 5.;

namespace GVA {

ViewportMetaData::ViewportMetaData()
{
    initialize();
}

ViewportMetaData::ViewportMetaData(const ViewportMetaData& other)
{
    m_initialScale = other.m_initialScale;
    m_minimumScale = other.m_minimumScale;
    m_maximumScale = other.m_maximumScale;
    m_width = other.m_width;
    m_height = other.m_height;
    m_userScalable = other.m_userScalable;
    m_isValid = other.m_isValid;
    m_scaleLimits = other.m_scaleLimits;
    m_specifiedData.m_width = other.m_specifiedData.m_width;
    m_specifiedData.m_height = other.m_specifiedData.m_height;
    m_specifiedData.m_minScale = other.m_specifiedData.m_minScale;
}

ViewportMetaData& ViewportMetaData::operator=(const ViewportMetaData& other)
{
    m_initialScale = other.m_initialScale;
    m_minimumScale = other.m_minimumScale;
    m_maximumScale = other.m_maximumScale;
    m_width = other.m_width;
    m_height = other.m_height;
    m_userScalable = other.m_userScalable;
    m_isValid = other.m_isValid;
    m_scaleLimits = other.m_scaleLimits;
    m_specifiedData.m_width = other.m_specifiedData.m_width;
    m_specifiedData.m_height = other.m_specifiedData.m_height;
    m_specifiedData.m_minScale = other.m_specifiedData.m_minScale;

    return *this;
}

ViewportMetaData::~ViewportMetaData()
{}

void ViewportMetaData::adjustViewportData(const QRect& clientRect)
{
    //Data updated from viewport tag
    m_isValid = true;

    //Adjust viewport dimensions
    m_width = qBound(MinViewportWidth, m_width, MaxViewportWidth);
    m_height = qBound(MinViewportHeight, m_height, MaxViewportHeight);

    //Aspect ratio
    qreal aspectRation = (qreal)clientRect.width() / clientRect.height();

    if (m_width != DefaultViewportWidth && m_height == DefaultViewportHeight) {
        //Width has been specified. Adjust height, min scale and max scale
        m_height = m_width * (1 / aspectRation);
    } else if (m_width == DefaultViewportWidth && m_height != DefaultViewportHeight) {
        //Height has been specified. Adjust width, min scale and max scale
        m_width = m_height * aspectRation;
    } else {
        //Putting below code under seperate 'else' to make it readable!
        m_height = m_width * (1 / aspectRation);
    }

    //Adjust zoom limits
    adjustZoomValues(clientRect);
}

void ViewportMetaData::updateViewportData(const QSize& newContentSize, const QRect& clientRect)
{
    //If still viewport tag has not been parsed
    //Do not update values.
    if(!m_isValid)
        return;

    //Update with viewport dimensions
    m_width = qBound(MinViewportWidth, newContentSize.width(), MaxViewportWidth);
    m_height = qBound(MinViewportHeight, newContentSize.height(), MaxViewportHeight);

    //Adjust zoom limits
    adjustZoomValues(clientRect);
}

void ViewportMetaData::orientationChanged(const QRect& newClientRect)
{
    //If still viewport tag has not been parsed
    //Do not update values.
    if(!m_isValid)
        return;

    //Aspect ratio
    qreal aspectRation = (qreal)newClientRect.width() / newClientRect.height();

    //Update with viewport dimensions
    if (m_specifiedData.m_width.length()) {
        if (QString::compare(m_specifiedData.m_width, "device-width", Qt::CaseInsensitive) == 0)
            m_width = newClientRect.width();
        else if(QString::compare(m_specifiedData.m_width, "device-height", Qt::CaseInsensitive) == 0)
            m_width = newClientRect.height();

        m_height = m_width * (1 / aspectRation);
    }

    //Check if width has not bee specified.
    if (!m_specifiedData.m_width.length()) {
        if (QString::compare(m_specifiedData.m_height, "device-width", Qt::CaseInsensitive) == 0)
            m_height = newClientRect.width();
        else if(QString::compare(m_specifiedData.m_height, "device-height", Qt::CaseInsensitive) == 0)
            m_height = newClientRect.height();

        m_width = m_height * aspectRation;
    }

    //Update with bounds
    m_width = qBound(MinViewportWidth, m_width, MaxViewportWidth);
    m_height = qBound(MinViewportHeight, m_height, MaxViewportHeight);
}

bool ViewportMetaData::isLayoutNeeded()
{
    if (!isUserSpecifiedWidth() || !isUserSpecifiedHeight())
        return true;
    return false;
}
void ViewportMetaData::initialize()
{
    m_initialScale = ValueUndefined;
    m_minimumScale = DefaultMinScale;
    m_maximumScale = DefaultMaxScale;
    m_width = DefaultViewportWidth;
    m_height = DefaultViewportHeight;
    m_userScalable = true;
    m_isValid = false;

    //Clear user defined scales
    setFlag(UserDefinedMinumumScale, false);
    setFlag(UserDefinedMaximumScale, false);
    setFlag(UserDefinedInitialScale, false);
}

void ViewportMetaData::adjustZoomValues(const QRect& clientRect)
{
    qreal fitToWidthZoom = (qreal)clientRect.width() / m_width;
    fitToWidthZoom = qBound(DefaultMinScale, fitToWidthZoom, DefaultMaxScale);

    m_maximumScale = qBound(fitToWidthZoom, m_maximumScale, DefaultMaxScale);

    //Adjust minimum-sclae
    if (getFlag(UserDefinedMinumumScale)) {
        m_minimumScale = m_specifiedData.m_minScale;
        m_minimumScale = qBound(fitToWidthZoom, m_minimumScale, m_maximumScale);
    }
    else
        m_minimumScale = fitToWidthZoom;

    //Adjust initial-scale
    if (getFlag(UserDefinedInitialScale))
        m_initialScale = qBound(m_minimumScale, m_initialScale, m_maximumScale);
    else
        m_initialScale = m_minimumScale;

    //Turn off zooming if min and max zoom are same
    if (m_minimumScale == m_maximumScale)
        m_userScalable = false;
}


//FIX ME : Merge below functions to single. Now in Hurry!!
bool ViewportMetaData::isUserSpecifiedWidth()
{
    if (m_specifiedData.m_width.length()) {
        if (!QString::compare(m_specifiedData.m_width, "device-width", Qt::CaseInsensitive)
            || !QString::compare(m_specifiedData.m_width, "device-height", Qt::CaseInsensitive))
            return false;
    }
    return true;
}

bool ViewportMetaData::isUserSpecifiedHeight()
{
    if (m_specifiedData.m_height.length()) {
        if (!QString::compare(m_specifiedData.m_height, "device-width", Qt::CaseInsensitive)
            || !QString::compare(m_specifiedData.m_height, "device-height", Qt::CaseInsensitive))
            return false;
    }
    return true;
}

}//namespace GVA

