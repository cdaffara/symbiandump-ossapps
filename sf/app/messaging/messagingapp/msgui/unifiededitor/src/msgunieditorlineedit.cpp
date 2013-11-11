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
 * Description:
 *
 */

#include "msgunieditorlineedit.h"
#include <HbTapGesture>
#include <HbMenu>
#include <QApplication>
#include <QClipboard>

const QRegExp expr("[,;\n]$");
const QRegExp sepAtEnd("; $");
const QRegExp sepAtMiddle("; ");

const QString replacementStr("; ");
const QString labelSeperator(": ");

const int fadedAlpha(125);
const int solidAlpha(255);

const int SNAP_DELAY = 350;

//Localization
#define LOC_PASTE hbTrId("txt_common_menu_paste")

MsgUnifiedEditorLineEdit::MsgUnifiedEditorLineEdit(const QString& label,QGraphicsItem *parent):
HbLineEdit(parent),
mSelectionStart(-1),
mSelectionEnd(-1),
mDefaultBehaviour(false)
{
    QString labelStr = label.trimmed();
    
    QTextCursor cursor(this->textCursor());
    QTextCharFormat colorFormat(cursor.charFormat());
   
    QColor fgColor = this->palette().color(QPalette::Text);     
    fgColor.setAlpha(fadedAlpha);
    colorFormat.setForeground(fgColor);
    cursor.insertText(labelStr , colorFormat);

    fgColor.setAlpha(solidAlpha);
    colorFormat.setForeground(fgColor);

    cursor.insertText(" ",colorFormat);    

    mLabelExpr.setPattern(QString("^"+labelStr+" $"));
    mLabel = labelStr+" ";
    
    moveCursor(QTextCursor::EndOfBlock);

    connect(this,SIGNAL(selectionChanged(QTextCursor,QTextCursor)),
            this,SLOT(selectionChanged(QTextCursor,QTextCursor)));
    connect(this, SIGNAL(contentsChanged()), this, SLOT(onContentsChanged()));
    
    connect(this,SIGNAL(aboutToShowContextMenu(HbMenu*,const QPointF &)),
            this,SLOT(aboutToShowContextMenu(HbMenu*,const QPointF &)));
    
}

MsgUnifiedEditorLineEdit::~MsgUnifiedEditorLineEdit()
{
}

void MsgUnifiedEditorLineEdit::inputMethodEvent(QInputMethodEvent *event)
{
    //let it go in default way.
    if(mDefaultBehaviour)
    {
        HbAbstractEdit::inputMethodEvent(event);
        event->accept();
        return;
    }

    if (!event->commitString().isEmpty() || event->replacementLength())
    {
        if (event->commitString().contains(expr))
        {
            if(this->text().isEmpty() || this->text().contains(sepAtEnd) || this->text().contains(mLabelExpr))
            {
                event->accept();
                return;
            }

            this->setCursorPosition(this->text().length());

            QString str = event->commitString();
            str.replace(expr, replacementStr);            

            event->setCommitString(str, event->replacementStart(), event->replacementLength());            
        }
        else if(this->hasSelectedText())
        {// all user inputs get appended at the end
            this->setCursorPosition(this->text().length());
        }
    }
    
    HbAbstractEdit::inputMethodEvent(event);
    event->accept();
}

void MsgUnifiedEditorLineEdit::keyPressEvent(QKeyEvent *event)
{
    QString str = event->text();

    if(event->key()== Qt::Key_Enter || event->key()== Qt::Key_Return)
    {
        if(mDefaultBehaviour)
        {
            HbAbstractEdit::keyReleaseEvent(event);
            event->accept();
            return;
        }
        if(this->text().isEmpty() || this->text().contains(sepAtEnd) || this->text().contains(mLabelExpr))
        {
            event->accept();
            return;
        }
        this->setCursorPosition(this->text().length());
        str = replacementStr;
        QKeyEvent eve(event->type(), Qt::Key_Any, event->modifiers(), str);
        HbAbstractEdit::keyPressEvent(&eve);
        event->accept();
        return;
    }

    if(event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete )
    {
        int pos = this->cursorPosition();
        bool pbkContact = true;

        if(!this->hasSelectedText())
        {
            this->setCursorPosition(pos-2);
            pbkContact = this->textCursor().charFormat().fontUnderline();
            this->setCursorPosition(pos);
        }

        QString text = this->text();
        text = text.left(pos);

        if(text.contains(mLabelExpr))
        {
            event->accept();
            return;
        }

        if(pbkContact)
        {
            //if already selected delete it.
            if(this->hasSelectedText())
            {
                // deleting phbkContact is an atomic operation
                // ensure that the signal is emitted only once
                disconnect(this, SIGNAL(contentsChanged()), 
                        this, SLOT(onContentsChanged()));
                HbLineEdit::keyPressEvent(event);
                event->accept();
                //delete seperator (i.e."; ").
                QKeyEvent eve(event->type(), Qt::Key_Delete, Qt::NoModifier);
                HbLineEdit::keyPressEvent(&eve);
                HbLineEdit::keyPressEvent(&eve);
                connect(this, SIGNAL(contentsChanged()), 
                        this, SLOT(onContentsChanged()));
                onContentsChanged();
            }
            else //make it selected
            {                
                this->setCursorPosition(pos-3);
                setHighlight(pos-3);
            }
        }
        else
        {
            QString str = text.right(2);
            if(str == replacementStr)
            {
                // deleting contact is an atomic operation
                // ensure that the signal is emitted only once
                disconnect(this, SIGNAL(contentsChanged()), 
                        this, SLOT(onContentsChanged()));
                //delete seperator (i.e."; ").
                QKeyEvent eve(event->type(), Qt::Key_Backspace, Qt::NoModifier);
                HbLineEdit::keyPressEvent(&eve);
                HbLineEdit::keyPressEvent(&eve);
                connect(this, SIGNAL(contentsChanged()), 
                        this, SLOT(onContentsChanged()));
                onContentsChanged();
            }
            else
            {
                HbLineEdit::keyPressEvent(event);
            }
            event->accept();
        }
        
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Left )
    {
        bool selectedText = this->hasSelectedText();

        //look ahead left.
        int pos = this->cursorPosition();

        QString text = this->text();
        text = text.left(pos);

        //no text other than label;
        if(text.contains(mLabelExpr))
        {
            event->accept();
            return;
        }

        //look for next seperator while going left.
        int newPos = text.lastIndexOf(sepAtMiddle);

        if(newPos < 0 && selectedText)
        {
            event->accept();
            return;
        }

        bool pbkContact = true;

        if(!selectedText)
        {
            this->setCursorPosition(pos-2);
            pbkContact = this->textCursor().charFormat().fontUnderline();
            this->setCursorPosition(pos);
        }
        else
        {
            this->setCursorPosition(newPos);
            pbkContact = this->textCursor().charFormat().fontUnderline();
            this->setCursorPosition(pos);
        }


        if(pbkContact && newPos >0)
        {

            setHighlight(newPos-1);
        }
        else
        {
            //move left, char by char. if seperator met jump over it.
            if( (newPos > 0 && selectedText) || (pos-2  == newPos))
            {
                this->setCursorPosition(newPos+1);
            }

            HbLineEdit::keyPressEvent(event);

        }
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Right)
    {
        bool selectedText = this->hasSelectedText();

        //look ahead.
        int pos = this->cursorPosition();
        this->setCursorPosition(pos+3);
        bool pbkContact = this->textCursor().charFormat().fontUnderline();
        this->setCursorPosition(pos);

        //look for next seperator.
        QString text = this->text();
        int newPos = text.indexOf(sepAtMiddle,pos+2);

        if(pbkContact && newPos >0)
        {
            this->setCursorPosition(newPos-1);
            setHighlight(newPos-1);
        }
        else
        {            
            int seperatorPos = text.indexOf(sepAtMiddle,pos);

            if(selectedText || seperatorPos == pos)
            {
                this->setCursorPosition(pos+1);
                this->deselect();
            }
            HbAbstractEdit::keyPressEvent(event);
        }
        event->accept();
        return;
    }

    if(!str.isEmpty())
    {
        if(mDefaultBehaviour)
        {
            HbAbstractEdit::keyPressEvent(event);
            event->accept();
            return;
        }
        if (str.contains(expr))
        {
            if(this->text().isEmpty() || this->text().contains(sepAtEnd) || this->text().contains(mLabelExpr))
            {
                event->accept();
                return;
            }

            // auto-complete the last incomplete word
            int contentLength = this->text().length();
            int pos = this->cursorPosition();
            QString incompleteWord(this->text().right(contentLength-(pos-1)));
            if(!incompleteWord.contains(sepAtMiddle))
            {
                this->setCursorPosition(this->text().length());
            }

            str.replace(expr, replacementStr);
            QKeyEvent eve(event->type(), event->key(), event->modifiers(), str);
            HbAbstractEdit::keyPressEvent(&eve);
        }
        else
        {
            HbAbstractEdit::keyPressEvent(event);
            event->accept();
            return;
        }
    }
}

void MsgUnifiedEditorLineEdit::handleTap()
{
    int currentPos = this->cursorPosition();

    QString txt = this->text();

    QString tempTxt = txt.left(currentPos+2);
    int seperatorPos = tempTxt.lastIndexOf(sepAtMiddle,currentPos);

    txt = txt.right(txt.length() - currentPos);
    int labelPos = txt.indexOf(labelSeperator);

    if(labelPos >= 0 )//pressed on label.
    {
        this->setCursorPosition(currentPos + labelPos + 2);
    }
    else if(seperatorPos == currentPos-1 || seperatorPos == currentPos)//pressed just on seperator.
    {
        this->setCursorPosition(seperatorPos+2);
    }
    else
    {
        this->setCursorPosition(currentPos+1);
        bool pbkContact = this->textCursor().charFormat().fontUnderline();
        if(pbkContact)
        {
            setHighlight(currentPos);
        }
    }

    this->update();
}

void MsgUnifiedEditorLineEdit::gestureEvent(QGestureEvent* event)
{
    //passing gesture event to base class.
    HbLineEdit::gestureEvent(event);
    
    
    if(HbTapGesture *tap = qobject_cast<HbTapGesture*>(event->gesture(Qt::TapGesture)))
    {
        //capturing gesture position, and map to local co-ordinates.
        QPointF pos = mapFromScene(tap->scenePosition());
        
        switch (tap->state()) 
        {
            case Qt::GestureFinished:
            {
                if (HbTapGesture::Tap == tap->tapStyleHint()) 
                {
                    handleTap();
                }
                break;
            }
            default:
                break;
        }
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MsgUnifiedEditorLineEdit::setText(const QString& text, bool underlined)
{

    if(!mDefaultBehaviour)
    {
        // atomic operation, ensure one signal only at the end
        disconnect(this, SIGNAL(contentsChanged()), this, SLOT(onContentsChanged()));

        //make sure previous text is complete.
        if(this->content().length() > 0)
        {
            QInputMethodEvent e;
            e.setCommitString(";");
            this->inputMethodEvent(&e);
        }
        this->setCursorPosition(this->text().length());

        QTextCursor cursor(this->textCursor());
        QTextCharFormat colorFormat(cursor.charFormat());
        if(underlined)
        {
            QColor fgColor = colorFormat.foreground().color();
            fgColor.setAlpha(fadedAlpha);
            colorFormat.setUnderlineColor(fgColor);
            colorFormat.setFontUnderline(true);        
        }
        cursor.insertText(text , colorFormat);
        colorFormat.setFontUnderline(false);

        cursor.insertText(replacementStr,colorFormat);
        connect(this, SIGNAL(contentsChanged()), this, SLOT(onContentsChanged()));
        onContentsChanged();
    }
    else
    {
       this->setCursorPosition(this->text().length());
       QTextCursor cursor(this->textCursor());
       cursor.insertText(text);
    }
    
    moveCursor(QTextCursor::EndOfBlock);
}

QStringList MsgUnifiedEditorLineEdit::addresses()
{
    QString text = this->content();
    QStringList list = text.split(replacementStr,QString::SkipEmptyParts);
    return list;
}

void MsgUnifiedEditorLineEdit::focusInEvent(QFocusEvent* event)
{    
    HbLineEdit::focusInEvent(event);
    this->setCursorVisibility(Hb::TextCursorVisible);
}

void MsgUnifiedEditorLineEdit::focusOutEvent(QFocusEvent* event)
{    
    HbLineEdit::focusOutEvent(event);
    this->setCursorVisibility(Hb::TextCursorHidden);
}

void MsgUnifiedEditorLineEdit::setHighlight(int currentPos)
{
    QString txt = this->text();    

    int endPos = qMax(txt.indexOf(sepAtMiddle,currentPos),
                      txt.indexOf(labelSeperator,currentPos));

    int startPos = qMax(txt.lastIndexOf(sepAtMiddle,currentPos),
                        txt.lastIndexOf(labelSeperator,currentPos));

    disconnect(this,SIGNAL(selectionChanged(QTextCursor,QTextCursor)),
               this,SLOT(selectionChanged(QTextCursor,QTextCursor)));

    //highlight if pbk contact.
    if(startPos > 0 && endPos > 0 && startPos != endPos)
    {
        this->setSelection(startPos + 2, endPos - startPos - 2);
        this->update();
    }
    else
    {
        this->deselect();
    }

    this->update();

    connect(this,SIGNAL(selectionChanged(QTextCursor,QTextCursor)),
            this,SLOT(selectionChanged(QTextCursor,QTextCursor)));
}

void MsgUnifiedEditorLineEdit::selectionChanged(const QTextCursor &oldCursor, const QTextCursor& newCursor)
{

    if(mSelectionSnapTimer.isActive())
    {
        mSelectionSnapTimer.stop();
    }

    if(newCursor.selectionStart() < mLabel.length())
    {
        this->setTextCursor(oldCursor);
        return;
    }

    if(!mDefaultBehaviour)
        {
        mSelectionStart  = newCursor.selectionStart();
        mSelectionEnd    = newCursor.selectionEnd();

        if(mSelectionStart == mSelectionEnd )
            {
            return;
            }
    
        mSelectionSnapTimer.start(SNAP_DELAY,this);
        }
}

void MsgUnifiedEditorLineEdit::timerEvent(QTimerEvent *event)
{
    //passing event to base class.
    HbLineEdit::timerEvent(event);

    if (event->timerId() == mSelectionSnapTimer.timerId())
    {
        mSelectionSnapTimer.stop();

        disconnect(this,SIGNAL(selectionChanged(QTextCursor,QTextCursor)),
                   this,SLOT(selectionChanged(QTextCursor,QTextCursor)));

        QString txt = this->text();

        int startPos = qMax(txt.lastIndexOf(sepAtMiddle,mSelectionStart),
                            txt.lastIndexOf(labelSeperator,mSelectionStart));

        int endPos = qMax(txt.indexOf(sepAtMiddle,mSelectionEnd),
                          txt.indexOf(labelSeperator,mSelectionEnd));

        if(endPos < 0 )
        {
            endPos = mSelectionEnd;
        }

        this->setSelection(startPos + 2, endPos - startPos - 2);

        connect(this,SIGNAL(selectionChanged(QTextCursor,QTextCursor)),
                this,SLOT(selectionChanged(QTextCursor,QTextCursor)));

        event->accept();
    }
}

void MsgUnifiedEditorLineEdit::setDefaultBehaviour(bool defaultBehaviour)
{
    mDefaultBehaviour = defaultBehaviour;
}

QString MsgUnifiedEditorLineEdit::text() const
{
    return HbLineEdit::text();
}

QString MsgUnifiedEditorLineEdit::content() const
{
    QString text = this->text();
    text.remove(mLabel);
    return text;
}

void MsgUnifiedEditorLineEdit::clearContent()
{
    // avoid getting updates during local editing
    disconnect(this, SIGNAL(contentsChanged()), this, SLOT(onContentsChanged()));
    
    int startPos = mLabel.length();
    this->setSelection(startPos, content().length());
    QKeyEvent eve(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    this->keyPressEvent(&eve);
    this->deselect();

    // re-connect signal to start getting updates
    connect(this, SIGNAL(contentsChanged()), this, SLOT(onContentsChanged()));
}

void MsgUnifiedEditorLineEdit::onContentsChanged()
{
    emit contentsChanged(content());
}

void MsgUnifiedEditorLineEdit::highlightInvalidString(QString invalidStr)
{
    // for only address editor
    if(!mDefaultBehaviour)
    {
        QString txtContent = this->text();
        int searchStartPos = mLabel.length();
        int startPos = txtContent.indexOf(invalidStr, searchStartPos);
        disconnect(this,SIGNAL(selectionChanged(QTextCursor,QTextCursor)),
                   this,SLOT(selectionChanged(QTextCursor,QTextCursor)));
        // if invalidStr found
        if(startPos > 0)
        {
            this->setSelection(startPos, invalidStr.length());
        }
        connect(this,SIGNAL(selectionChanged(QTextCursor,QTextCursor)),
                this,SLOT(selectionChanged(QTextCursor,QTextCursor)));
    }
}

void MsgUnifiedEditorLineEdit::aboutToShowContextMenu(HbMenu *contextMenu, const QPointF &pos)
{
    Q_UNUSED(pos)
    //clear all menu actions.
    contextMenu->clearActions();
    
    const QMimeData *mimedata = QApplication::clipboard()->mimeData();
    if(mimedata)
    {
        if(canInsertFromMimeData(mimedata))
        {
            contextMenu->addAction(LOC_PASTE,this,SLOT(paste()));
        }
    }

}
// eof
