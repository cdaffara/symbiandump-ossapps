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
* Description:  HbMessageBox class declaration.
*
*/


#ifndef HBMESSAGEBOX_H
#define HBMESSAGEBOX_H

#include <qobject.h>
#include <qstring.h>
#include "hbdialog.h"
#include "hbaction.h"

class QGraphicsWidget;
class QGraphicsScene;
class QGraphicsItem;

class HbMessageBox : public HbDialog
{   
    Q_OBJECT

    Q_FLAGS(StandardButton StandardButtons)
    Q_ENUMS(MessageBoxType StandardButton)
    
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
    
    /**
    * Create new HbMessageBox and call emitDialogFinished after this one to finish the sequence.
     */
    void open( QObject* receiver = 0, const char* member = 0 );
    
    void emitDialogFinished( QObject* receiver, const char* member, int actionNum );

    void show();
    
    void setAttribute(int attribute);
    
    /** stubbed from HbMessgeBox */
/*    static void question(const QString &questionText,
                            QObject *receiver,
                            const char *member,
                            const QString &primaryButtonText = tr("Yes"),
                            const QString &secondaryButtonText = tr("No"),
                            QGraphicsWidget *headWidget = 0,
                            QGraphicsScene *scene = 0,
                            QGraphicsItem *parent = 0 );*/

    /** stubbed from HbMessgeBox */
    static void information(const QString &informationText,
        QGraphicsWidget *headWidget = 0,
        QGraphicsScene *scene = 0,
        QGraphicsItem *parent = 0);

    /** stubbed from HbMessgeBox */
    static void warning(const QString &warningText,
        QGraphicsWidget *headWidget = 0,
        QGraphicsScene *scene = 0,
        QGraphicsItem *parent = 0 );
    
    static void setStandardButtons(StandardButtons buttons);

    QList<HbAction*> actions() const
    {
        return mActions;
    }

signals:

    void finished(int action);
    
public:
    
    QList<HbAction*> mActions;
    
};

Q_DECLARE_OPERATORS_FOR_FLAGS(HbMessageBox::StandardButtons)

#endif // HBMESSAGEBOX_H
