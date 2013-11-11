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
* Description: List view for the autofill items
*
*/

#ifndef NMAUTOFILLLISTVIEW_H
#define NMAUTOFILLLISTVIEW_H

#include <hblistview.h>

class QGraphicsItem;
class QGestureEvent;


class NmAutoFillListView : public HbListView
{
public:
    NmAutoFillListView(QGraphicsItem *parent = NULL);
public slots:
    void orientationChanged(Qt::Orientation orientation);
};

#endif // NMAUTOFILLLISTVIEW_H
