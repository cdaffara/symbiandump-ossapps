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
* Description: Custom button
*
*/

#ifndef BUBBLEBUTTON_H
#define BUBBLEBUTTON_H

#include "hbpushbutton.h"

class BubbleButton : public HbPushButton
{
    Q_OBJECT

    Q_PROPERTY(BubbleButtonType buttonType READ buttonType WRITE setButtonType)
    Q_ENUMS(BubbleButtonType)

public:
    enum BubbleButtonType {
        DefaultButton = 0x1,
        GreenButton   = 0x2,
        RedButton     = 0x3  
    };

public:
    explicit BubbleButton(QGraphicsItem *parent = 0 );
    virtual ~BubbleButton( );

    BubbleButtonType buttonType() const;
    void setButtonType(BubbleButtonType type);

    void updatePrimitives();

protected:
    bool sceneEvent(QEvent *event);

private:
    BubbleButtonType mButtonType;
};

#endif // BUBBLEBUTTON_H
