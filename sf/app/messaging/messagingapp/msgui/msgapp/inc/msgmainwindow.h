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
 * Main window of the messaging application. Activates the default view.
 *
 */

#ifndef MSG_MAIN_WINDOW_H
#define MSG_MAIN_WINDOW_H

// INCLUDES
#include <qobject.h>
#include <hbmainwindow.h>

// FORWARD DECLARATIONS
class MsgViewManager;
class MsgServiceInterface;
class MsgSendServiceInterface;

/**
 * Main window of the messaging application. Activates the
 * default view which is the messaging list view.
 */
class MsgMainWindow : public HbMainWindow
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgMainWindow(bool serviceRequest,
                   QWidget *parent = 0);

    /**
     * Destructor
     */
    virtual ~MsgMainWindow();
    
    /**
     * Returns view manager.
     */
    MsgViewManager* viewManager();
    
protected:

    /**
     * Key press event handler
     * @see QGraphicsItem
     */
    void keyPressEvent(QKeyEvent *event);
    
private:
    /**
     * This initializes the member objects
     */
    void initialize(bool serviceRequest);
    
private:
    MsgServiceInterface* mMsgSI;
    MsgSendServiceInterface* mMsgSendSI;
    MsgViewManager* mViewManager;
    
    /**
     * Unit Testing
     */
    #ifdef MSGUI_UNIT_TEST
      friend class TestMsgMainWindow;
    #endif
    
};

#endif // MSG_MAIN_WINDOW_H

// EOF
