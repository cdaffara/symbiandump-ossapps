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
* Description: MpAlbumCoverWidget stub for testing mpmediawallview
*
*/

#ifndef MPALBUMCOVERWIDGET_H
#define MPALBUMCOVERWIDGET_H

//includes
#include <QObject>
#include <HbWidget>
#include <QGraphicsItem>

class HbIcon;

class MpAlbumCoverWidget : public HbWidget
{
    Q_OBJECT

public:

    explicit MpAlbumCoverWidget( QGraphicsItem *parent = 0 );
    virtual ~MpAlbumCoverWidget();
    void setIcon(const HbIcon &icon );
    void setDefaultIcon( const HbIcon &icon );
};

#endif    // MPALBUMCOVERWIDGET_H
