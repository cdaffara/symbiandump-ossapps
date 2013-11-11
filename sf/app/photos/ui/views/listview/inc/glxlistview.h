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

#ifndef GLXLISTVIEW_H
#define GLXLISTVIEW_H

//User Includes
#include "glxview.h"

//Qt/Orbit forward declarations
class HbListView;
class HbMainWindow;
class HbAbstractViewItem;
class HbDocumentLoader;
class HbGroupBox;

class GlxListView : public GlxView
{
    Q_OBJECT
    
public :
    GlxListView(HbMainWindow *window);
    virtual ~GlxListView();
    void activate() ;
    void deActivate();
    void setModel(QAbstractItemModel *model);
    void addToolBar( HbToolBar *toolBar );
    void initializeView( QAbstractItemModel *model, GlxView *preView );
    QGraphicsItem * getAnimationItem( GlxEffect transtionEffect );

public slots:
    void itemSelected(const QModelIndex &  index);
    void populated();
    void showAlbumCount();
    
private slots:
	void indicateLongPress( HbAbstractViewItem *item, QPointF coords );
	void setVisvalWindowIndex();
	
private:
	void addViewConnection();
	void removeViewConnection();
	void createListView();
	/*
	* This loads the docml and retrives the widgets/layout information from the 
	* docml corresponding to the present orentation 
	*/	
	void loadListView();

private:
	HbListView         *mListView;
	HbView             *mView;
	HbGroupBox         *mAlbumCount;
	HbMainWindow       *mWindow;  //no ownership
	QAbstractItemModel *mModel ;
	HbDocumentLoader   *mDocLoader; //Docml loader to load the widgets from docml
	bool               mIsLongPress;
};

#endif /* GLXLISTVIEW_H */
