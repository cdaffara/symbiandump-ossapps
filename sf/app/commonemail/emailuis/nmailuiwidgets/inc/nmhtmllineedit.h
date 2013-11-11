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
* Description: Message editor header widget
*
*/

#ifndef NMHTMLLINEEDIT_H_
#define NMHTMLLINEEDIT_H_

#include <hblineedit.h>
#include"nmailuiwidgetsdef.h"

class NMAILUIWIDGETS_EXPORT NmHtmlLineEdit : public HbLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString plainText READ toPlainText WRITE setPlainText USER true)
public:
    NmHtmlLineEdit(QGraphicsItem *parent=0);
    virtual ~NmHtmlLineEdit();

    void setDocument(QTextDocument *document);
    QTextDocument *document() const;

    void setTextCursor(const QTextCursor &cursor);
    QTextCursor textCursor() const;

    QString toPlainText() const;
    QString toHtml() const;
    
    bool hasInputFocus() const;

    QRectF rectForCursorPosition() const;
    void setIndentForLabel(const qreal indent);
    
public slots:
    void setPlainText(const QString &text);
    void setHtml(const QString &text);

private:
    qreal mIndent;
};

#endif /* NMHTMLLINEEDIT_H_ */
