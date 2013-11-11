/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     CMsgErrorWatcher declaration file
 *
 */
#ifndef MSGERRORWATCHER_H
#define MSGERRORWATCHER_H

#ifdef  MSGERRORWATCHER_DLL
#define MSGERRORWATCHER_EXPORT Q_DECL_EXPORT
#else
#define MSGERRORWATCHER_EXPORT Q_DECL_IMPORT
#endif

// INCLUDES
#include "msgerrorwatcher.hrh"
// SYSTEM INCLUDES
#include <QObject>
// CLASS DECLARATION
class CMsgErrorWatcherPrivate;

/**
 * Handles showing notes handled by the errowatcher
 */
class MSGERRORWATCHER_EXPORT MsgErrorWatcher: public QObject
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgErrorWatcher(QObject* parent = 0);

    /**
     * Destructor
     */
    ~MsgErrorWatcher();

public:
    
    /**
     *  Displays notifiction dialog
     *  @param errornoteid error id
     */
    void ShowNote(TMsgErrorNoteIds errornoteid);
    
private:

    /**
     * Object of private implementation.
     * Owned.
     */
    CMsgErrorWatcherPrivate* d_ptr;

};

#endif // MSGERRORWATCHER_H
//EOF
