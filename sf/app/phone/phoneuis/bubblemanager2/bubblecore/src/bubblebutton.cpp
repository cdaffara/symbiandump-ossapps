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

#include <hbevent.h>
#include <hbframeitem.h>
#include <hbiconitem.h>
#include <hbcolorscheme.h>

#include "bubblebutton.h"

BubbleButton::BubbleButton(QGraphicsItem *parent)
    : HbPushButton(parent), mButtonType(DefaultButton)
{
}

BubbleButton::~BubbleButton()
{
}

BubbleButton::BubbleButtonType BubbleButton::buttonType() const
{
    return mButtonType;
}

void BubbleButton::setButtonType(BubbleButtonType type)
{
    mButtonType = type;
}

bool BubbleButton::sceneEvent(QEvent *event)
{
    bool result = HbPushButton::sceneEvent(event);

    if (event->type() == QEvent::UngrabMouse) {
        if (isVisible() && isDown()) {
            // this is needed in situation, where
            // longpress launches a dialog (vmbx)
            // and button release event goes to
            // dialog (HbPopup grabs mouse).
            setDown(false);
            emit clicked();
            emit released();
        }
    }

    return result;
}

void BubbleButton::updatePrimitives()
{
    HbPushButton::updatePrimitives();

    HbFrameItem* frame =
        qgraphicsitem_cast<HbFrameItem*>(HbWidget::primitive(
            QLatin1String("background")));

    if (!frame) {
        return;
    }

    QString graphicsName;
    QColor iconColor; // CSS doesn't work with changing icon color

    if (isDown()) {
        if (buttonType()==GreenButton) {
            graphicsName = QLatin1String("qtg_fr_btn_green_pressed");
            iconColor = HbColorScheme::color(
                QLatin1String("qtc_callhandling_answer_pressed"));
        } else if (buttonType()==RedButton) {
            graphicsName = QLatin1String("qtg_fr_btn_red_pressed");
            iconColor = HbColorScheme::color(
                QLatin1String("qtc_callhandling_reject_pressed"));
        } else {
            iconColor = HbColorScheme::color(
                QLatin1String("qtc_button_pressed"));
        }
    } else {
        if (buttonType()==GreenButton) {
            graphicsName =
                QLatin1String("qtg_fr_btn_green_normal");
            iconColor = HbColorScheme::color(
                QLatin1String("qtc_callhandling_answer_normal"));
        } else if (buttonType()==RedButton) {
            graphicsName = QLatin1String("qtg_fr_btn_red_normal");
            iconColor = HbColorScheme::color(
                QLatin1String("qtc_callhandling_reject_normal"));
        } else {
            iconColor = HbColorScheme::color(
                QLatin1String("qtc_button_normal"));
        }
    }

    if (graphicsName.length()) {
        frame->frameDrawer().setFrameGraphicsName(graphicsName);
    }

    // set icon color
    HbIconItem* iconItem =
        qgraphicsitem_cast<HbIconItem*>(HbWidget::primitive("icon"));

    if (iconItem && iconColor.isValid()) {
        iconItem->setColor(iconColor);
    }
}
