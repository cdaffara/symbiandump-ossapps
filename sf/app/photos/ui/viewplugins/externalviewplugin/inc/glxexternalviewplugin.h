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


#ifndef GLXEXTERNALVIEWPLUGIN_H
#define GLXEXTERNALVIEWPLUGIN_H

#include <QObject>
#include <glxstatemanager.h>
#include <qviewplugin.h>

class HbView;
class HbPushButton;
class QGraphicsGridLayout;

class GlxExternalViewPlugin : public QViewPlugin
{
    Q_OBJECT
    //Q_INTERFACES( QViewPlugin )

public:
    /**
     * Constructor.
     *
     */
    GlxExternalViewPlugin();

    /**
     * Destructor.
     *
     */
    virtual ~GlxExternalViewPlugin();

    /**
     * Interface method to create view.
     *
     */
    QGraphicsWidget* getView();

    void createView(){};
    
    /**
     * Interface method to destroy view.
     *
     */
    void destroyView();

    /**
     * Interface method to activate view.
     *
     * @param aParam Parameter required to activate the view.
     */
    void activateView();
    
    /**
     * Interface method to deactivate view.
     *
     */
    void deactivateView() ;
  
public slots: 
    void back(){};
    void externalCommand(int cmdId);
    void orientationChange( Qt::Orientation orientation );
      
private:

    HbView* iView;

    HbPushButton* collectionButton;
    HbPushButton* imageButton;
    
    QGraphicsGridLayout* iLayout;
	GlxStateManager mStateMgr;
    
};


#endif //GLXEXTERNALVIEWPLUGIN_H
