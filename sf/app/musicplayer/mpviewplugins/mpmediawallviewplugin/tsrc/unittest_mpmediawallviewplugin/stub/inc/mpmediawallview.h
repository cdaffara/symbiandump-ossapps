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
* Description:  Stub Music Player Media Wall view.
*
*/

#ifndef MPMEDIAWALLVIEW_H
#define MPMEDIAWALLVIEW_H

#include <QGraphicsWidget>

class MpMediaWallView : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit MpMediaWallView();
    virtual ~MpMediaWallView();
    
    void initializeView();
    void activateView();
    void deactivateView();
    void resetCounters();

signals:
    void command( int aCommand );

public:
    int mInitializeViewCount;
    int mActivateViewCount;
    int mDeactivateViewCount;


//    Q_DISABLE_COPY(MpMediaWallView)
};


#endif //MPMEDIAWALLVIEW_H
