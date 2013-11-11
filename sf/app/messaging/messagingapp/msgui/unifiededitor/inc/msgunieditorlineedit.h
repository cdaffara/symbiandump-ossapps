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
* Description: Derieved HbLineEdit class for custom address editor
*
*/

#ifndef MSG_UNIFIED_EDITOR_LINEEDIT_H
#define MSG_UNIFIED_EDITOR_LINEEDIT_H

#include <HbLineEdit>
#include<QBasicTimer>


class MsgUnifiedEditorLineEdit : public HbLineEdit
{
    Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgUnifiedEditorLineEdit(const QString& label, QGraphicsItem* parent=0);
    
    /**
     * Destructor
     */
    ~MsgUnifiedEditorLineEdit();
    
    /**
     * seeker method for getting address list
     */
    QStringList addresses();

    /**
     * Flag to retain or override base-class behaviour
     * Needed because address & subject field classes derieve
     * from this class, but have different behaviour
     */
    void setDefaultBehaviour(bool defaultBehaviour = false);
    
    /**
     * seeker method for getting the text content held by editing field
     */
    QString content() const;
    
    /**
     * setter method to clear text content of the editing field
     */
    void clearContent();
    
    /**
     * set highlight on invalid contact
     * @param invalidStr, String to be highlighted
     */
    void highlightInvalidString(QString invalidStr);

signals:
    void contentsChanged(const QString& text);

public slots:
    void setText(const QString &text, bool underlined = true);

private slots:
    void selectionChanged(const QTextCursor &oldCursor, const QTextCursor& newCursor);
    void onContentsChanged();
    void aboutToShowContextMenu(HbMenu *contextMenu, const QPointF &pos);
    
protected: // from HbLineEdit
    void inputMethodEvent(QInputMethodEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void gestureEvent(QGestureEvent* event);
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent *event);
    void timerEvent (QTimerEvent *event);


private:
    void setHighlight(int currentPos);
    QString text() const;
    void handleTap();

private:
    QRegExp mLabelExpr;
    QString mLabel;
    QBasicTimer mSelectionSnapTimer;

    int mSelectionStart;
    int mSelectionEnd;
    bool mDefaultBehaviour;
    
#ifdef MSGUI_UNIT_TEST
    friend class TestMsgUnifiedEditorLineEdit;
#endif
};

#endif // MSG_UNIFIED_EDITOR_LINEEDIT_H
