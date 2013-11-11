/*!
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
* Description:  Base class for bubble handlers.
*
*/

#ifndef BUBBLEHANDLER_H_
#define BUBBLEHANDLER_H_

#include <QObject>
#include "bubblewidgetmanager.h"

class BubbleHeader;
class HbAction;
class QGraphicsWidget;

class BubbleHandler : public QObject
    {
    Q_OBJECT

public:
    explicit BubbleHandler(
        BubbleWidgetManager& widgetManager,
        BubbleWidgetManager::View view,
        BubbleWidgetManager::Container container,
        QObject* parent = 0);

    virtual ~BubbleHandler();

    virtual void reset() = 0;
    virtual void readBubbleHeader( const BubbleHeader& header ) = 0;
    virtual QGraphicsWidget* graphicsWidgetForAction(
        HbAction* action ) const = 0;
    virtual void updateTimerDisplayNow() = 0;
    virtual void conferenceMemberRemoved(int bubbleId);
    virtual void conferenceRemoved();

protected:
    QGraphicsWidget* widget(BubbleWidgetManager::Widget widget) const;

signals:
    void headerSelected(int bubbleId);

private:
    BubbleWidgetManager& mWidgetManager;
    BubbleWidgetManager::View mView;
    BubbleWidgetManager::Container mContainer;    
    };

#endif /* BUBBLEHANDLER_H_ */
