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
 * Description:  CalenDayContentWidget class definition.
 *
 */

#ifndef CALENDAYCONTENTWIDGET_H_
#define CALENDAYCONTENTWIDGET_H_

// System includes
#include <HbWidget>

// User includes
#include "calendaycommonheaders.h"

// Forward declarations
class QGraphicsLinearLayout;

class CalenDayContentWidget : public HbWidget
{
Q_OBJECT

public:
    /*!
     \enum CalenWidgetPosition
     \brief Enumeration identifies widget position if queue.
     */
    enum CalenWidgetPosition
    {
        ECalenLastWidget = 0,
        ECalenFirstWidget
    };

public:
    CalenDayContentWidget(QGraphicsItem *parent = 0);
    ~CalenDayContentWidget();

    void add(HbWidget *item, CalenWidgetPosition where = ECalenLastWidget);
    HbWidget *take(CalenWidgetPosition which);
    void remove(CalenWidgetPosition which);

signals:
    void widgetsRelayoutFinished(CalenScrollDirection scrollTo);
    void scrollPositionChanged(const QPointF &newPos);

public slots:
    void relayoutWidgets(CalenScrollDirection scrollTo);
    void widgetScrolled(const QPointF &newPos);

private:
    QGraphicsLinearLayout *mLayout; //!< Linear layout for day views

    QList<HbWidget *> mWidgets; //!< List of widget instances
};

#endif /* CALENDAYCONTENTWIDGET_H_ */
