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
* Description:   VideoDetailsLabel class definition
*
*/

#ifndef _VIDEODETAILSLABEL_H
#define _VIDEODETAILSLABEL_H

// SYSTEM INCLUDES
#include <hblabel.h>

// FORWORD DECLARATIONS
class QGraphicsSceneMouseEvent;

/**
 * This class is a custom layout widget for Contact Card layout.
 */
class VideoDetailsLabel : public HbLabel
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit VideoDetailsLabel(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~VideoDetailsLabel();

public:

    void mousePressEvent ( QGraphicsSceneMouseEvent *event );
    
    void click();

signals:

    /**
     * Signal emitted when widget is clicked.
     */
    void clicked(bool);

private:

    /**
     * Initialization function.
     */
    void init();
};

// EOF
#endif
