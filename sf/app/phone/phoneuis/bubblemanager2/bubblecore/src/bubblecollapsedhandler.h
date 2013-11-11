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
* Description:  Collapsed call bubble handler.
*
*/

#ifndef BUBBLECOLLAPSEDHANDLER_H_
#define BUBBLECOLLAPSEDHANDLER_H_

#include "bubblehandler.h"
#include "bubblewidgetmanager.h"

class BubbleHeader;
class HbAction;
class BubbleHeadingWidget;

class BubbleCollapsedHandler : public BubbleHandler
    {
    Q_OBJECT

public:
    explicit BubbleCollapsedHandler(
            BubbleWidgetManager& widgetManager,
            BubbleWidgetManager::View view,
            BubbleWidgetManager::Container container,
            QObject* parent = 0);
    virtual ~BubbleCollapsedHandler();

protected:
    void reset();

    void readBubbleHeader(const BubbleHeader& header);

    QGraphicsWidget* graphicsWidgetForAction( HbAction* action ) const;
    
    void updateTimerDisplayNow();

protected slots:
    void handleBubbleClicked();

protected: //data
    const BubbleHeader* mHeader;
    BubbleHeadingWidget* mHeading;
    };

#endif /* BUBBLECOLLAPSEDHANDLER_H_ */
