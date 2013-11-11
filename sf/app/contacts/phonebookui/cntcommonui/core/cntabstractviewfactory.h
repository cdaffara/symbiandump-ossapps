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
#ifndef CNTABSTRACTVIEWFACTORY_H_
#define CNTABSTRACTVIEWFACTORY_H_

#include <QObject>
#include "cntglobal.h"

class CntAbstractView;
class HbMainWindow;
class CntExtensionManager;

class CntAbstractViewFactory
{

public:
    virtual ~CntAbstractViewFactory() {}
    virtual CntAbstractView* createView( int aViewId ) = 0;
};

/*!
 * Default implementation of the view factory
 * 
 */
class QTPBK_EXPORT CntDefaultViewFactory : public CntAbstractViewFactory
{
    
public:
    CntDefaultViewFactory( CntExtensionManager& aExtManager );
    ~CntDefaultViewFactory();

public:
    CntAbstractView* createView( int aId );
    
private:
    CntAbstractView* createPluginView( int aId );

private:
    CntExtensionManager& mExtensionManager;
};
#endif /* CNTABSTRACTVIEWFACTORY_H_ */
