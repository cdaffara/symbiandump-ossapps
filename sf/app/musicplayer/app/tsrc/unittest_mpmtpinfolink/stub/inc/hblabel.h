/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  HbLabel stub for testing MpMtpInfoLink
*
*/

#ifndef HBLABEL_H
#define HBLABEL_H

#include <QGraphicsView>

namespace Hb
{
/*!
    A type definition for text wrapping
*/
    enum TextWrapping
    {
        TextNoWrap = 0,
        TextWordWrap = 1,
        TextWrapAnywhere = 3
    };
}

class HbLabel : public QObject
{

public:
    explicit HbLabel ();
    virtual ~HbLabel ();

    QString plainText() const;
    QString html() const;

    void setTextWrapping(Hb::TextWrapping mode);
    Hb::TextWrapping textWrapping() const;

    void setPlainText(const QString &text);
    void setHtml(const QString &text);
    QRectF boundingRect();

private:
    QString mText;
    QString mHtmlText;
    Hb::TextWrapping mTextWrapping;
};


#endif // HBLABEL_H
