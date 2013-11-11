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
 * Description: Message service application main window. 
 *
 */

#ifndef __MSG_SERVICE_WINDOW_H__
#define __MSG_SERVICE_WINDOW_H__

#include <hbmainwindow.h>

// FORWARD DECLARATIONS
class MsgViewInterface;
class MsgStoreHandler;
class MsgServiceViewManager;
class MsgShareUiInterface;
class MsgSendInterface;
class MsgUriHandlerInterface;

class MsgServiceWindow : public HbMainWindow
    {
    Q_OBJECT
    
public:
    /**
     * Constructor
     */
    MsgServiceWindow();
    
    /**
     * Destructor
     */
    ~MsgServiceWindow();

private slots:

    /**
     * Calls activity handler save drafts contents
     */
    void saveDraftContents(); 
    
protected:

    /**
     * Key press event handler
     * @see QGraphicsItem
     */
    void keyPressEvent(QKeyEvent *event);

private:
    /**
     * View interface object
     * Owned
     */
    MsgSendInterface* mSendInterface;
    
    /**
     * View interface object
     * Owned
     */
    MsgViewInterface*  mViewInterface;
    
    /**
     * View interface object
     * Owned
     */
    MsgShareUiInterface*  mShareUiInterface;

    /**
     * URI handler interface object
     * Owned
     */
    MsgUriHandlerInterface*  mUriInterface;
    
    /**
     * Message store handler
     * Owned.
     */
    MsgStoreHandler* mStoreHandler;
    
    /**
     * View manager
     * Owned.
     */
    MsgServiceViewManager* mViewManager;
    
    };

#endif // __MSG_SERVICE_WINDOW_H__ 
