/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  stub HbMessageBox class declaration.
*
*/


#ifndef HBMESSAGEBOX_H
#define HBMESSAGEBOX_H

#include <QObject>
#include <QString>
#include <QGraphicsItem>

#include <hbaction.h>

class HbMessageBox : public QObject 
{
    Q_OBJECT

public:

    enum MessageBoxType {
        MessageTypeInformation,
        MessageTypeQuestion,
        MessageTypeWarning
    };

    enum StandardButton {
        NoButton           = 0x00000000,
        Ok                 = 0x00000400,
        Save               = 0x00000800,
        Open               = 0x00001000,
        Yes                = 0x00002000,
        Continue           = 0x00004000,
        Delete             = 0x00008000,
        No                 = 0x00010000,
        Retry              = 0x00020000,
        Close              = 0x00040000,
        Cancel             = 0x00080000,
        Help               = 0x00100000,
        Apply              = 0x00200000,
        Reset              = 0x00400000
    };    

    Q_DECLARE_FLAGS(StandardButtons, StandardButton)

    HbMessageBox(MessageBoxType type = MessageTypeInformation, QGraphicsItem *parent = 0);
    HbMessageBox(const QString &text, MessageBoxType type = MessageTypeInformation, QGraphicsItem *parent = 0);
    ~HbMessageBox();
    
    void show();
    
    /**
     * Create new HbMessageBox and call emitDialogFinished after this one to finish the sequence.
     */
    void open( QObject* receiver = 0, const char* member = 0 );
    
    void emitDialogFinished( QObject* receiver, const char* member, int actionNum );
    
    void setAttribute(int attribute);
    
    /**
     * saves provided text to mLatestTxt
     */
    static void warning(QString text)
    {
        mLatestTxt = text;
    }
    
    /**
     * saves provided text to mLatestTxt
     */
    static void information(QString text)
    {
        mLatestTxt = text;
    }
    
    static void setStandardButtons(StandardButtons buttons);

    QList<HbAction*> actions() const
    {
        return mActions;
    }
    
    /**
     * saved text
     */
    static QString mLatestTxt;
    
    static int mType;
    static int mAttribute;
    static int mOpenCallCount;
    static int mShowCallCount;
    
    QList<HbAction*> mActions;
    
signals:

    void finished(int);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(HbMessageBox::StandardButtons)

#endif // HBMESSAGEBOX_H
