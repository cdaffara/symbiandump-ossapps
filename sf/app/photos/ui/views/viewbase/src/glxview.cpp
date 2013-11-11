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
* Description:   ?Description
*
*/

//include
#include <hbmainwindow.h>
#include <hbinstance.h>

//User Includes
#include "glxview.h"


GlxView::GlxView(qint32 id, QGraphicsItem *parent ) : HbView(parent), mId(id)
{ 
    
}

bool GlxView::compare (qint32 id)
{ 
    return mId == id;
}

QSize GlxView::screenSize()
{
    HbMainWindow *window = hbInstance->allMainWindows().first();
    QSize deviceSize = HbDeviceProfile::current().logicalSize();
    QSize screenSize = ( window->orientation() == Qt::Vertical ) ? QSize( deviceSize.width(), deviceSize.height() ) 
                                                                   : QSize( deviceSize.height(), deviceSize.width() )  ;
    return screenSize;
}

QRect GlxView::screenGeometry()
{
    HbMainWindow *window = hbInstance->allMainWindows().first();
    QSize size = screenSize();
    QRect rect ( window->geometry().x(), window->geometry().y(), size.width(), size.height()  );
    return rect;
}

QGraphicsItem * GlxView::getAnimationItem( GlxEffect transtionEffect ) 
{
    Q_UNUSED(transtionEffect)
    return NULL; 
}
