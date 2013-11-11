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



#ifndef GLXVIEW_H_
#define GLXVIEW_H_


//Includes
#include <hbview.h>

//User Includes
#include "glxuistd.h"


//Qt/Orbit forward declartion
class QAbstractItemModel;
class QItemSelectionModel;

class GlxView :public HbView
{
Q_OBJECT
    
public :
    GlxView( qint32 id, QGraphicsItem *parent = 0 );
    virtual void activate() = 0;
    virtual void deActivate() = 0;
    virtual void setModel( QAbstractItemModel *model ) = 0;

    virtual bool compare ( qint32 id );
    virtual void addToolBar( HbToolBar *toolBar ) { Q_UNUSED(toolBar) }
    virtual void enableMarking() { }
    virtual void disableMarking() { }
    virtual void handleUserAction( qint32 commandId ) { Q_UNUSED(commandId) }
    virtual QItemSelectionModel * getSelectionModel() { return NULL ; }
    virtual void resetView() {}
    /*
     * model - Model to retreive the initial set up data
     * preView - previous view pointer to retrieve the mode , state information 
     */
    virtual void initializeView( QAbstractItemModel *model, GlxView *preView ) 
    {
        Q_UNUSED( model )
        Q_UNUSED( preView )
    }
     
    inline qint32 viewId() { return mId;}
    QSize screenSize();
    QRect screenGeometry(); 
    virtual QGraphicsItem * getAnimationItem( GlxEffect transtionEffect ); 
    virtual ~GlxView() {   }   
	
signals:
    void actionTriggered( qint32 id );
    void itemSpecificMenuTriggered( qint32,QPointF );
    
private :
	qint32 mId;	
};

#endif /* GLXVIEW_H_ */
