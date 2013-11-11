/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: HbLabel stub for testing MpMtpInfoLink
*
*/

#include <QGraphicsSceneMouseEvent>

#include "stub/inc/hblabel.h"


/*!
    Constructs the label
 */
HbLabel::HbLabel() 
{
    mText = QString();
    mHtmlText = QString();
    mTextWrapping = Hb::TextNoWrap;
}

/*!
    Label destructor.
 */
HbLabel::~HbLabel ()
{
}

/*!
    Sets the text wrapping mode to \a mode.
    \param mode - wrapping mode

    \sa Hb::TextWrapping
 */
void HbLabel::setTextWrapping(Hb::TextWrapping mode)
{
    mTextWrapping = mode;
}

/*!
    \return the label's current text wrapping mode.
    Default value is NoWrap.

    \sa setTextWrapping()
 */
Hb::TextWrapping HbLabel::textWrapping() const
{
    return mTextWrapping;
}

/*!
    Sets the label contents to plain text containing \a text. Any previous content is cleared.
    \param text - the plain text that is shown in the label.

    \sa setHtml()
 */
void HbLabel::setPlainText(const QString &text)
{
    mText = text;
}

/*!
    Sets the label contents to html text containing \a text. Any previous content is cleared.
    \param text - the html text that is shown in the label.

    \sa setPlainText()
 */
void HbLabel::setHtml(const QString &text)
{
    mHtmlText = text;
}

/*!
    Plain text accessor. Returns empty string if not set.
 */
QString HbLabel::plainText() const
{
    return mText;
}

/*!
    Rich text text accessor. Returns empty string if not set.
 */
QString HbLabel::html() const
{
    return mHtmlText;
}

/*!
    Fake widget rectangle accessor.
 */
QRectF HbLabel::boundingRect()
{
    return QRectF(10, 10, 10, 10);
}

