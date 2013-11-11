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
* Description: MpDetailsView stub for testing MpDetailsViewPlugin
*
*/

#ifndef MPDETAILSVIEW_H
#define MPDETAILSVIEW_H

//includes
#include <QGraphicsWidget>

class MpDetailsView : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit MpDetailsView();
    virtual ~MpDetailsView();
    
    void initializeView();
    void activateView();
    void deactivateView();

    void emitCommand(int cmd);
    void resetCounters();
    
signals:
    void command(int cmd);

public:
    int mInitializeViewCount;
    int mActivateViewCount;
    int mDeactivateViewCount;
    
};

#endif    // MPDETAILSVIEW_H
