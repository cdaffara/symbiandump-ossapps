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
* Description: HgMediawall stub for testing mpcollectioncontainers
*
*/

#ifndef HGMEDIAWALL_H
#define HGMEDIAWALL_H

//includes
#include <hbglobal.h>
#include <hbnamespace.h>
#include <hbwidget.h>

class HgWidget : public HbWidget
{
    Q_OBJECT

public:

    enum ScrollBarPolicy {
        ScrollBarAsNeeded = Qt::ScrollBarAsNeeded,
        ScrollBarAlwaysOff = Qt::ScrollBarAlwaysOff,
        ScrollBarAlwaysOn = Qt::ScrollBarAlwaysOn,
        ScrollBarAutoHide
    };

    explicit HgWidget ( QGraphicsItem *parent = 0 );
    virtual ~HgWidget ();

    void setScrollBarPolicy( ScrollBarPolicy policy );
    virtual void setModel(QAbstractItemModel *model);
    void scrollTo(const QModelIndex &index);
    QModelIndex currentIndex() const;
    QSizeF itemSize() const;
    void setDefaultImage(QImage defaultImage);
    
signals:

    void scrollingEnded();

};

class HgMediawall : public HgWidget
{
    Q_OBJECT

public:

    enum LabelPosition {
        PositionNone = 0,
        PositionAboveImage,
        PositionBelowImage
    };

    explicit HgMediawall( QGraphicsItem *parent = 0 );
    virtual ~HgMediawall();

    void setTitlePosition( LabelPosition position );
    void setDescriptionPosition( LabelPosition position );
    void setTitleFontSpec( const HbFontSpec &fontSpec );
    void setDescriptionFontSpec( const HbFontSpec &fontSpec );

    void enableReflections( bool enabled );
    bool reflectionsEnabled() const;

public:

    bool        mReflectionEnabled;

};

#endif    // HGMEDIAWALL_H
