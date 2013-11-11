/*
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
* Description: Message editor body text field
*
*/

#ifndef NMEDITORTEXTEDIT_H_
#define NMEDITORTEXTEDIT_H_

#include <hbtextedit.h>
#include "nmailuiwidgetsdef.h"

class NMAILUIWIDGETS_EXPORT NmEditorTextEdit : public HbTextEdit
{
Q_OBJECT

public:
    NmEditorTextEdit(QGraphicsItem *parent = 0);
    virtual ~NmEditorTextEdit();

    void setCustomTextColor(const QPair<bool, QColor> &customColor);
    void setCustomTextColor(bool useCustom, const QColor& color = Qt::black);
    QPair<bool, QColor> customTextColor() const;

    bool hasInputFocus() const;

    QRectF rectForCursorPosition() const;
    
    void moveCursor(QTextCursor::MoveOperation op, QTextCursor::MoveMode mode);

public slots:
    void updateCustomTextColor();

private:
    QPair<bool, QColor> mCustomTextColor;//!<This property keeps info about using customTextColor and color to use. 
};

#endif /* NMEDITORTEXTEDIT_H_ */
