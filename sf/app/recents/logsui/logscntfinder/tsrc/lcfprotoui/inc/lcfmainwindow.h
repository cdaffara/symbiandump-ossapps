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
#ifndef LCFMAINWINDOW_H
#define LCFMAINWINDOW_H


#include <QObject>
#include <hbmainwindow.h>


class LcfMainWindow : public HbMainWindow
    {
    Q_OBJECT
    
public:
    
    LcfMainWindow();
    virtual ~LcfMainWindow();
    
protected: // From HbMainWindow
    
    void keyPressEvent( QKeyEvent *event );
    
signals:
        
    void callKeyPressed();

    
    };


#endif //LCFMAINWINDOW_H
