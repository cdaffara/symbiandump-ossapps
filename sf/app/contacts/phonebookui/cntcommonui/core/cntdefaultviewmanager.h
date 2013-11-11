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
#ifndef CNTDEFAULTVIEWMANAGER_H_
#define CNTDEFAULTVIEWMANAGER_H_

#include <cntabstractviewmanager.h>
#include <hbnamespace.h>
#include "cntglobal.h"

class HbMainWindow;
class CntAbstractViewFactory;
class CntAbstractView;
class CntViewNavigator;
class CntSimUtility;
class CntAbstractEngine;

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class QTPBK_EXPORT CntDefaultViewManager : public QObject, public CntAbstractViewManager
{
    Q_OBJECT
    
public:
    CntDefaultViewManager( HbMainWindow* aWindow );
    ~CntDefaultViewManager();
    
    // ownership transferred
    void setViewFactory( CntAbstractViewFactory* aFactory );
    void setViewNavigator( CntViewNavigator* aNavigator );
    
    CntAbstractEngine& engine();
    
public: // From CntAbstractViewManager
    void changeView( const CntViewParameters aArgs );
    void back( const CntViewParameters aArgs, bool toRoot = false );
    
public:
    int currentViewId();
    
private slots:
    void removeCurrentView();
    void deleteOldView();
    void switchView( const CntViewParameters aArgs, QFlags<Hb::ViewSwitchFlag> flags );
    
protected slots:
    virtual void closeApp();
    
private:
    void cleanup();
        
private:
    CntAbstractViewFactory*      mFactory;
    CntAbstractView*             mCurrent;
    CntAbstractView*             mOldView;
    QMap<int, CntAbstractView*>  mDefaults;
    CntViewNavigator*            mNavigator;
    CntViewParameters            mArgs;
    
    QList<QContactManager*>      mBackends;
    HbMainWindow*                mMainWindow;
    
    CntSimUtility* mSimUtility;
    CntAbstractEngine* mEngine;
    friend class T_CntDefaultViewManager;
};
#endif /* CNTDEFAULTVIEWMANAGER_H_ */
