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
* Description: 
*
*/
#include <QObject>
#include <QPointF>

class HbMenu;
class QAbstractItemModel;
class QAction;
class HbMainWindow;
class GlxSettingInterface;


//Grid view option menu
enum {
   GlxGridViewSend,
   GlxGridViewSlideShow,
   GlxGridViewAddToAlbum,
   GlxGridViewRemoveFromAlbum,
   GlxGridViewDelete,
   GlxGridView3DEffect
};

//Fullscreen view option menu
enum {
   GlxFullScreenViewSend,
   GlxFullScreenViewSlideShow,
#ifndef __WINSCW__
   GlxFullScreenViewUseImage,
   GlxFullScreenViewMenuRotate,
   GlxFullScreenViewCrop,
#endif
   GlxFullScreenViewAddToAlbum,
   GlxFullScreenViewRemoveoAlbum
};

class GlxMenuManager : public QObject
{
Q_OBJECT

public :
	GlxMenuManager( HbMainWindow* mainWindow );
	~GlxMenuManager();
    void createMarkingModeMenu( HbMenu* menu );
    void ShowItemSpecificMenu( qint32 viewId,QPointF pos );
    void setModel( QAbstractItemModel *model ) { mModel = model ; }
    void addMenu( qint32 viewId, HbMenu* menu );
    void removeMenu( qint32 viewId, HbMenu* menu );
    void disableAction( HbMenu* menu,bool disable );
    
signals :
    void commandTriggered( qint32 commandId );
    
private:
    void CreateGridMenu( HbMenu* menu );
    void CreateListMenu( HbMenu* menu );
    void CreateFullscreenMenu( HbMenu* menu );
    void createSlideShowMenu( HbMenu* menu );
    void setAllActionVisibility( QList<QAction*> actionList, bool visible );
    int  viewSubState();

private slots:
    void menuItemSelected();
    void updateGridMenu();
    void updateFullscreenMenu();
    void closeContextMenu();
    
private:
    QAbstractItemModel *mModel; //It should point to current view model
	HbMainWindow* mMainWindow;
	HbMenu *mContextMenu;
	HbMenu* mSubMenu;
	HbMenu* m3DEffectSubMenu;
	HbMenu* mRotateSubMenu;
	HbMenu* mUseImgSubMenu;
	GlxSettingInterface *mSettings;
};
