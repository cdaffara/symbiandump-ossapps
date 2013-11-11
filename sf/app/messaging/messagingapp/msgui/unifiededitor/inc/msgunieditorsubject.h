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
 */

#ifndef MSG_UNIFIED_EDITOR_SUBJECT_H
#define MSG_UNIFIED_EDITOR_SUBJECT_H

#include <msgunieditorbasewidget.h>

#include "convergedmessage.h"

class HbIconItem;
class UniEditorGenUtils;
class MsgUnifiedEditorLineEdit;

class MsgUnifiedEditorSubject : public MsgUnifiedEditorBaseWidget
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgUnifiedEditorSubject(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgUnifiedEditorSubject();

    /**
     * to set priority.
     * @param priority MsgPriority {High,Normal,Low}
     */
    void setPriority(ConvergedMessage::Priority priority);
    
    /**
     * returns content of subject field.
     */
    QString text();

    /**
     * return the priority of this message
     */
    ConvergedMessage::Priority priority();
    
    /**
     * get size of content in bytes
     */
    int subjectSize();

    /**
     * set the text on the subject field
     * @param text
     */
    void setText(const QString& text);
    
    /**
     * To set focus on editable field.
     */
    void setFocus();

signals:
    /**
     * Emitted when subject/priority content is added or removed
     */
    void contentChanged();
    
private slots:
    /**
     * called when contentsChanged signal is emitted by the line edit
     */
    void onContentsChanged(const QString&);
    
private:
    /**
     * Check if email over sms is supported. If yes, subject is a valid sms field
     * @return true, if email over sms is supported.
     */
    bool subjectOkInSms();

private:
    
    /**
     * line edit to input subject.
     */
    MsgUnifiedEditorLineEdit* mSubjectEdit;
    
    /**
     * icon item to display priority.
     */
    HbIconItem* mPriorityIcon;

    /**
     * priority of the message
     */
    ConvergedMessage::Priority mPriority;
    
    /**
     * general utilities
     */
    UniEditorGenUtils* mGenUtils;
    
    /**
     * Holds the previous buffer inside subject field
     */
    QString mPrevBuffer;
    
#ifdef MSGUI_UNIT_TEST
    friend class TestMsgUnifiesEditorSubject;
#endif
    };

#endif //MSG_UNIFIED_EDITOR_SUBJECT_H
