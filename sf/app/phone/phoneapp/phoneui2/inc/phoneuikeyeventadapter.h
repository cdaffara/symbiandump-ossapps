/*!
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
* Description:  Adapter for converting Qt key event types to
*               Symbian types.
*
*/

#ifndef PHONEUIKEYEVENTADAPTER_H
#define PHONEUIKEYEVENTADAPTER_H

#include <QObject>
#include "mphonekeyeventhandler.h"

class QKeyEvent;

class PhoneUIKeyEventAdapter : public QObject
{
    Q_OBJECT

public:

    /*!
       Constructor
     */
    PhoneUIKeyEventAdapter (MPhoneKeyEventHandler &keyEventHandler, QObject *parent = 0);
    
    /*!
       Destructor
     */
    virtual ~PhoneUIKeyEventAdapter ();
    
    
public slots:

    /*!
        \fn void keyPressed (QKeyEvent *event)
    
        This method is called when keyPressEvent signal
        is emitted from view.
    */
    void keyPressed (QKeyEvent *event);
    
    /*!
        \fn void keyReleased (QKeyEvent *event)
    
        This method is called when keyReleaseEvent signal
        is emitted from view.
    */
    void keyReleased (QKeyEvent *event);

private:
 
    /*!
        \fn void forwardKeyEvent(TEventCode symbianEventCode,QKeyEvent *event)
    
        This method forwards key event for ui control.
    */
    void forwardKeyEvent(TEventCode symbianEventCode, QKeyEvent *event);
    
    /*!
        \fn void convertKeyCode(QKeyEvent *event)
    
        Returns symbian key code for Qt key event.
    */
    int convertKeyCode(QKeyEvent *event);
    
private:

    MPhoneKeyEventHandler &mHandler;

};

#endif // PHONEUIKEYEVENTADAPTER_H
