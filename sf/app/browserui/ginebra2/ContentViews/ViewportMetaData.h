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

#ifndef ViewportMetaData_h
#define ViewportMetaData_h

#include <QFlags>
#include <QObject>
#include <QRect>

namespace GVA {

struct ParsedViewportData{
    QString m_width;
    QString m_height;
    qreal m_minScale;
};

class ViewportMetaData {
public:
    enum {
        ValueUndefined = -1
    };

    enum ScaleLimit {
        UserDefinedMinumumScale = 0x1,
        UserDefinedMaximumScale = 0x2,
        UserDefinedInitialScale = 0x4,
    };
    Q_DECLARE_FLAGS(ScaleLimits, ScaleLimit)

    ViewportMetaData();
    ViewportMetaData(const ViewportMetaData&);
    ViewportMetaData& operator=(const ViewportMetaData&);
    ~ViewportMetaData();

    void adjustViewportData(const QRect& clientRect);
    void updateViewportData(const QSize& size, const QRect& clientRect);
    void orientationChanged(const QRect& newClientRect);
    bool isLayoutNeeded();
    void reset() {initialize();}

    void setFlag(ScaleLimit scaleLimit, bool enable)
    {
        if (enable)
            m_scaleLimits |= scaleLimit;
        else
            m_scaleLimits &= !scaleLimit;
    }

    bool getFlag(ScaleLimit scaleLimit)
    {
        return m_scaleLimits.testFlag(scaleLimit);
    }

protected:
    void initialize();
    void adjustZoomValues(const QRect& clientRect);
    bool isUserSpecifiedWidth();
    bool isUserSpecifiedHeight();

public:
    qreal m_initialScale;
    qreal m_minimumScale;
    qreal m_maximumScale;
    int m_width;
    int m_height;
    bool m_userScalable;
    ScaleLimits m_scaleLimits;
    bool m_isValid;
    ParsedViewportData m_specifiedData;
};//ViewportMetaData

Q_DECLARE_OPERATORS_FOR_FLAGS(ViewportMetaData::ScaleLimits)

} //namespace GVA

#endif //ViewportMetaData_h
