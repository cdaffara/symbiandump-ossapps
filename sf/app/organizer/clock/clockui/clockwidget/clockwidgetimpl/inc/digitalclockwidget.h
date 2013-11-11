/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DigitalClockWidget
*
*/

#ifndef DIGITALCLOCKWIDGET_H
#define DIGITALCLOCKWIDGET_H

// System includes
#include <HbWidget>
#include <QPointer>

// User includes
#include "clockwidgetdefines.h"

// Forward declarations
class QTimer;
class HbLabel;
class HbTextItem;

class DigitalClockWidget : public HbWidget
{
    Q_OBJECT

public:
     DigitalClockWidget(bool useAmPm = true, QGraphicsItem *parent = 0);
    ~DigitalClockWidget();

signals:
    void clockTapped();

public slots:
    void tick();
    void setAmPm(bool useAmPm);

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);

private:
    void createPrimitives();

public:
    void updatePrimitives();

private:
    QPointer<QTimer>        mTimer;
    QPointer<HbTextItem>    mClockLabel;
    QPointer<HbTextItem>    mAmPmLabel;
    bool                    mUseAmPm;
};

#endif // DIGITALCLOCKWIDGET_H
