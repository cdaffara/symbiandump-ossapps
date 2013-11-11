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

#ifndef HSWIDGETPOSITIONINGONORIENTATIONCHANGE_H
#define HSWIDGETPOSITIONINGONORIENTATIONCHANGE_H

#include <QList>
#include <QRectF>
#include "hsutils_global.h"

class HSUTILS_EXPORT HsWidgetPositioningOnOrientationChange
{
public:
    virtual ~HsWidgetPositioningOnOrientationChange() {}
    
    virtual QList<QRectF> convert(const QRectF &fromRect, 
                                  const QList<QRectF> &fromGeometries,
                                  const QRectF &toRect) = 0;

    static void setInstance(HsWidgetPositioningOnOrientationChange *instance);
    static HsWidgetPositioningOnOrientationChange *instance();

private:
    static HsWidgetPositioningOnOrientationChange *mInstance;
};

class HSUTILS_EXPORT HsAdvancedWidgetPositioningOnOrientationChange : public HsWidgetPositioningOnOrientationChange
{
public:
    QList<QRectF> convert(const QRectF &fromRect, 
                          const QList<QRectF> &fromGeometries,
                          const QRectF &toRect);
};

#endif // HSWIDGETPOSITIONINGONORIENTATIONCHANGE_H
