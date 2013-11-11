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
* Description:  Track List Widget  stub for testing mpmediawallview
*
*/
#ifndef MPTRACKLISTWIDGET_H
#define MPTRACKLISTWIDGET_H

#include <hbwidget.h>

class HbListView;

class MpTrackListWidget : public HbWidget
{
    Q_OBJECT
  
public:
    
    MpTrackListWidget( QGraphicsItem *parent = 0 );
    virtual ~MpTrackListWidget();
    HbListView *list(); 

};

#endif // MPTRACKLISTWIDGET_H

// EOF
