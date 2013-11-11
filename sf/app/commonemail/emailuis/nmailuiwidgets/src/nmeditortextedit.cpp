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

#include "nmailuiwidgetsheaders.h"

static const QString FILE_PATH_CSS_DEFAULT = ":nmeditortexteditblack.css";
static const QString FILE_PATH_CSS_SECONDARY = ":nmeditortexteditblue.css";
static const QString FILE_PATH_WIDGETML = ":nmeditortextedit.widgetml";

/*!
    Constructor
*/
NmEditorTextEdit::NmEditorTextEdit(QGraphicsItem *parent) :
    HbTextEdit(parent)
{
    NM_FUNCTION;
    
    HbStyleLoader::registerFilePath(FILE_PATH_WIDGETML);
    HbStyleLoader::registerFilePath(FILE_PATH_CSS_DEFAULT);

    mCustomTextColor = QPair<bool, QColor>(false, Qt::black);
    
    // Disable HbTextEdit scrolling. Background scroll area will handle scrolling.
    setScrollable(false);
    scrollArea()->setScrollDirections(0);

    // set background colour to plain white
    QPixmap whitePixmap(10,10);
    whitePixmap.fill(Qt::white);
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem (whitePixmap);
    setBackgroundItem(pixmapItem); 
    
    // Create custom palette based on the current one
    QPalette testPalette = QApplication::palette();

    // Sets the selection background colour
    testPalette.setColor(QPalette::Active, QPalette::Highlight, Qt::cyan);
    testPalette.setColor(QPalette::Inactive, QPalette::Highlight, Qt::cyan);

    // Sets the link and visited link colours
    testPalette.setColor(QPalette::Active, QPalette::Link, Qt::darkBlue);
    testPalette.setColor(QPalette::Inactive, QPalette::Link, Qt::darkBlue);
    testPalette.setColor(QPalette::Active, QPalette::LinkVisited, Qt::darkMagenta);
    testPalette.setColor(QPalette::Inactive, QPalette::LinkVisited, Qt::darkMagenta);

    // Update custom palette for this widget
    setPalette(testPalette);
}

/*!
    Destructor
*/
NmEditorTextEdit::~NmEditorTextEdit()
{
    NM_FUNCTION;
    
    HbStyleLoader::unregisterFilePath(FILE_PATH_WIDGETML);
    if (mCustomTextColor.first) {
		HbStyleLoader::unregisterFilePath(FILE_PATH_CSS_SECONDARY);    
    }
    else {
		HbStyleLoader::unregisterFilePath(FILE_PATH_CSS_DEFAULT);    
    }
}

/*!
	This slot applies custom text color for user - entered text
    It does not affect the text originally inserted into editor
 */
void NmEditorTextEdit::updateCustomTextColor()
{
    NM_FUNCTION;
    
    if (mCustomTextColor.first) {
        QTextCursor tcursor = textCursor();
        QTextCharFormat fmt;
        int pos = tcursor.position();
        if (pos > 0) {
            // If there isn't any user-made selection - apply custom color
            if (!tcursor.hasSelection() && !tcursor.hasComplexSelection()) {
                // Select last added char and set its color to custom
                if (tcursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1)
                    && tcursor.hasSelection()) {
                    fmt = tcursor.charFormat();
                    fmt.setForeground(mCustomTextColor.second);
                    tcursor.mergeCharFormat(fmt);
                }
            }
        }
        else {
            fmt = tcursor.charFormat();
            fmt.setForeground(mCustomTextColor.second);
            tcursor.mergeCharFormat(fmt);
            setTextCursor(tcursor);
        }
    }
}

/*!
   Sets flag is custom text color should be used and sets the custom color.
   
   Function does not affect the color of existing content, only text that will be entered later.
 */
void NmEditorTextEdit::setCustomTextColor(const QPair<bool, QColor> &customColor)
{
    NM_FUNCTION;
    
    mCustomTextColor = customColor;
}

/*!
    Reimplemented function \sa NmEditorTextEdit::setCustomTextColor(const QPair<bool, QColor> &customColor).
    
    \arg info about using of custom color
    \arg color to be used as custom. If \arg useCustom is set to false then color is not changed
 */
void NmEditorTextEdit::setCustomTextColor(bool useCustom, const QColor& color)
{   
    NM_FUNCTION;
    
    if (!mCustomTextColor.first && useCustom) {
        HbStyleLoader::unregisterFilePath(FILE_PATH_CSS_DEFAULT);    
        HbStyleLoader::registerFilePath(FILE_PATH_CSS_SECONDARY);

        mCustomTextColor.first = useCustom;
        mCustomTextColor.second = color;
    }		
}

/*!
 *  Return current custom color and if it must be used.
 *  
 *  \return Current custem color.
 */
QPair<bool, QColor> NmEditorTextEdit::customTextColor() const
{
    NM_FUNCTION;
    
    return mCustomTextColor;
}

/*!
 *  Returns true if this widget has the input focus.
 */
bool NmEditorTextEdit::hasInputFocus() const
{
    NM_FUNCTION;

    bool ret = false;
    
    HbInputMethod *inputMethod = HbInputMethod::activeInputMethod();
    
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
QRectF NmEditorTextEdit::rectForCursorPosition() const
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
 *  Sets the visible cursor position
 */

void NmEditorTextEdit::moveCursor(QTextCursor::MoveOperation op, QTextCursor::MoveMode mode)
{
    NM_FUNCTION;
    HbAbstractEdit::moveCursor(op, mode);
}

