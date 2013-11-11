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
* Description: Time label for oled digital clock.
*
*/

#include "snsroledtimelabel.h"
#include "snsrcolors.h"

#include <hbevent.h>
#include <QPainter>



/*!
    \class SnsrOledTimeLabel
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver oled digital clock's time label
 */

/*!
    Constructs a new SnsrOledTimeLabel.
    \param parent Parent object.
 */

SnsrOledTimeLabel::SnsrOledTimeLabel(QGraphicsItem *parent)
    : HbTextItem(parent)
{
    setTextColor(SnsrColors::PowerSaveModeWidgetColor);
    
    setMinimumLines( 1 );
    setMaximumLines( 1 );
    setAlignment(Qt::AlignCenter);
    
    // as we do own painting, must disable the flag that is enabled
    // by default due to performance optimatizations.
    setFlag(QGraphicsItem::ItemHasNoContents,false);
}

SnsrOledTimeLabel::~SnsrOledTimeLabel()
{
}

/*!
    \reimp
    We want to draw only the outlines of the time label text and thus
    text is drawn here with QPainterPath and pen (no brush) which seems to be the
    only way to accomplish this. 
    Base class's Paint method must not be called!
    We use base class only to get the bounding rect inside which to draw the text
    (boundingRect has been adjusted to current text, font, alignment etc.)
 */
void SnsrOledTimeLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    // TODO: changes needed for mirrored layout and/or other than latin digits??
    
    painter->save();   //saves the painter state.
    
    QPainterPath path;
    
    QRectF boundingRect = this->boundingRect();   
    QString timeText = this->text();
    QFont font = this->font();

    // As painterPath takes the left end of the text's baseline as 
    // a parameter we must calculate that point from boundingRect's bottom left 
    // so that font's descent (and baseline (=1)) are taken into account. 
    // We want to center the text horizontally too. The logic
    // is basically the same as used inside the base class for drawing the text
    // within boundingRect.
    QPointF leftEndOfBaseline(0,0);  // for painterPath
    QFontMetrics fontMetrics(font);
    int descent = fontMetrics.descent();
    int width = fontMetrics.width(timeText);   
    leftEndOfBaseline.setY(boundingRect.bottomLeft().y() - descent -1);
    leftEndOfBaseline.setX((boundingRect.width() - width) / 2 ); // centerH
    
    path.addText(leftEndOfBaseline, font, timeText);  
    
    painter->setFont(font);

    QPen newPen = painter->pen();
    newPen.setWidth( 1 ); // TODO: is this width ok?
    newPen.setColor(this->textColor());
    painter->setPen(newPen);
    
    painter->setBrush(QBrush(Qt::NoBrush));

  //  painter->setLayoutDirection ( layoutDirection() ); // TODO: needed or not?
    
    painter->setRenderHint(QPainter::Antialiasing, true); // TODO: use or not?
    
    painter->drawPath(path);
    
    // Must restore the painter or the whole UI will be messed up!
    painter->restore();

}

