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

#include "nmailuiwidgetsheaders.h"

static const int NmLabelIndentUsePreviousValue = 0;

/*
#include "nmhtmllineedit.h"
*/
/*!
    Constructor
 */
NmHtmlLineEdit::NmHtmlLineEdit(QGraphicsItem *parent) :
    HbLineEdit(parent),
    mIndent(0)
{
    NM_FUNCTION;

    // Disable scrolling so that baunch effect works correctly
    HbAbstractEdit::setScrollable(false);
    HbAbstractEdit::scrollArea()->setScrollDirections(0);
}

/*!
    Destructor
 */
NmHtmlLineEdit::~NmHtmlLineEdit()
{
    NM_FUNCTION;
}

/*!
    Replaces the document used in the editor.

    This allows
 */
void NmHtmlLineEdit::setDocument(QTextDocument *document)
{
    NM_FUNCTION;
    
    HbAbstractEdit::setDocument(document);
}

QTextDocument *NmHtmlLineEdit::document() const
{
    NM_FUNCTION;
    
    return HbAbstractEdit::document();
}

void NmHtmlLineEdit::setTextCursor(const QTextCursor &cursor)
{
    NM_FUNCTION;
    
    HbAbstractEdit::setTextCursor(cursor);
}

QTextCursor NmHtmlLineEdit::textCursor() const
{
    NM_FUNCTION;
    
    return HbAbstractEdit::textCursor();
}

QString NmHtmlLineEdit::toHtml() const
{
    NM_FUNCTION;
    
    return HbAbstractEdit::toHtml();
}

void NmHtmlLineEdit::setHtml(const QString &text)
{
    NM_FUNCTION;
    
    HbAbstractEdit::setHtml(text);
    // Resetting the indent is called because setting new content lose old indention set. 
    setIndentForLabel(NmLabelIndentUsePreviousValue);    
}

QString NmHtmlLineEdit::toPlainText () const
{
    NM_FUNCTION;
    
    return HbAbstractEdit::toPlainText();
}

void NmHtmlLineEdit::setPlainText (const QString &text)
{
    NM_FUNCTION;
    
    HbAbstractEdit::setPlainText(text);
    // Resetting the indent is called because setting new content lose old indention set. 
    setIndentForLabel(NmLabelIndentUsePreviousValue);    
}

/*!
 *  Returns true if this widget has the input focus.
 */
bool NmHtmlLineEdit::hasInputFocus() const
{
    NM_FUNCTION;

    bool ret = false;
    
    HbInputMethod* inputMethod = HbInputMethod::activeInputMethod();
    
    if (inputMethod) {
        HbInputFocusObject *focusObject = inputMethod->focusObject();
        if (focusObject) {
            ret = (focusObject->object() == this);
        }
    }
    
    return ret; 
}


/*!
 *  Returns the rectangle for the cursor including the predictive text area.  
 *  HbAbstractEditPrivate::ensurePositionVisible() uses the same algorithm.
 */
QRectF NmHtmlLineEdit::rectForCursorPosition() const
{
    NM_FUNCTION;
    
    int cursorPos = cursorPosition();
    QRectF rect = rectForPosition(cursorPos);
    QTextDocument *doc = document();
    if (doc) {
        rect.adjust(0, -doc->documentMargin(), 0, doc->documentMargin());
        const QTextBlock block = doc->findBlock(cursorPos);
        QTextLayout *blockLayout = block.layout();
        if (block.isValid() && blockLayout) {
            if (blockLayout->preeditAreaText().length()) {
                // Adjust cursor rect so that predictive text will be also visible
                rect.adjust(0, 0, boundingRect().width() / 2, 0);
            }
        }
    }

    return rect;
}

/*!
    Set margin for the first line. This will create empty space
    into begining of the edit field to reserve enough space for the
    label text e.g. 'To:'.
 */
void NmHtmlLineEdit::setIndentForLabel(const qreal indent)
{
    NM_FUNCTION;
	
    // Check that should we use previous value or set new value.
    // Previous value is used when edit field content is changed.
    if (indent != NmLabelIndentUsePreviousValue) {
        mIndent = indent;
    }
    QTextCursor cursor(this->document());
    cursor.select(QTextCursor::BlockUnderCursor);
    QTextBlockFormat format;
    format.setTextIndent(mIndent);
    cursor.mergeBlockFormat(format);
}

