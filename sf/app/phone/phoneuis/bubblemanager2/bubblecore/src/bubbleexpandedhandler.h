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
* Description:  Expanded call bubble handler.
*
*/

#ifndef BUBBLEEXPANDEDHANDLER_H_
#define BUBBLEEXPANDEDHANDLER_H_

#include "bubblehandler.h"

class BubbleHeader;
class HbAction;
class BubbleButton;
class BubbleImageWidget;
class BubbleHeadingWidget;

class BubbleExpandedHandler : public BubbleHandler
    {
    Q_OBJECT

public:
    explicit BubbleExpandedHandler(
            BubbleWidgetManager& widgetManager,
            BubbleWidgetManager::View view,
            BubbleWidgetManager::Container container,
            QObject* parent = 0);
    virtual ~BubbleExpandedHandler();

protected:
    void reset();

    void readBubbleHeader( const BubbleHeader& header );

    void setButtons(const QList<HbAction*>& actions);

    QGraphicsWidget* graphicsWidgetForAction( HbAction* action ) const;

    void updateTimerDisplayNow();

protected: //data
    const BubbleHeader* mHeader;

    BubbleHeadingWidget* mHeading;

    BubbleImageWidget* mImage;

    BubbleButton* mButtonCenter;
    BubbleButton* mButtonLeft;
    BubbleButton* mButtonRight;
    };

#endif /* BUBBLEEXPANDEDHANDLER_H_ */
