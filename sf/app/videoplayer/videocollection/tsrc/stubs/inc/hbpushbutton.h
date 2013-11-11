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
* Description:  stub hbpushbutton 
* 
*/

#ifndef HBPUSHBUTTON_H
#define HBPUSHBUTTON_H

#include "hbwidget.h"
#include "qgraphicsitem.h"
#include "hbicon.h"

class HbPushButton : public HbWidget
{
    Q_OBJECT

public:
    HbPushButton( QGraphicsItem *parent = 0 ) : HbWidget(parent) {}
    
    HbPushButton( const QString &text, QGraphicsItem *parent = 0 ) : HbWidget(parent) {
        Q_UNUSED(text);
    }
    HbPushButton( const HbIcon &icon, const QString &text, QGraphicsItem *parent = 0 ) : HbWidget(parent) {
        Q_UNUSED(icon);
        Q_UNUSED(text);
    }
    virtual ~HbPushButton( ) {}

    QString text( ) const
    {
        return mText;
    }

    void setIcon( const HbIcon &icon )
    {
        mIcon = icon;
    }
    
    HbIcon icon( ) const
    {
        return mIcon;
    }

public slots:
    void setText( const QString &text ) {
        Q_UNUSED(text);
    }

public:
    QString mText;
    HbIcon mIcon;
};

#endif // HBPUSHBUTTON_H
