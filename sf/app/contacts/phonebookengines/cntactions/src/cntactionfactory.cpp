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
#ifndef ACTIONFACTORYPLUGINTARGET
#define ACTIONFACTORYPLUGINTARGET cntactionplugin
#endif
#ifndef ACTIONFACTORYPLUGINNAME
#define ACTIONFACTORYPLUGINNAME SymbianActionFactory
#endif

#include "cntactionfactory.h"
#include "cntcallaction.h"
#include "cntvideocallaction.h"
#include "cntmessageaction.h"
#include "cntemailaction.h"
#include "cntbrowseraction.h"

#define makestr(x) (#x)
#define makename(x) makestr(x)


//Factory class
CntActionFactory::CntActionFactory() 
{
	actionList.append(new CntCallAction());
	actionList.append(new CntVideoCallAction());
    actionList.append(new CntMessageAction());
    actionList.append(new CntEmailAction());
    actionList.append(new CntBrowserAction());
}

CntActionFactory::~CntActionFactory()
{
	 while (!actionList.isEmpty())
	     delete actionList.takeFirst();
}

QString CntActionFactory::name() const
{
    return QString(makename(ACTIONFACTORYPLUGINNAME));
}


QList<QContactActionDescriptor> CntActionFactory::actionDescriptors() const
{
   QList<QContactActionDescriptor> descriptorList; 
   
   //loop through all the actions and add the descriptor to the list
   for (int i = 0; i < actionList.size(); i++)
   {
	   descriptorList << actionList.at(i)->actionDescriptor();   
   }
   
   return descriptorList;
}

QContactAction* CntActionFactory::instance(const QContactActionDescriptor& descriptor) const
{
    QContactAction *action(0);
	
    //loop through the actions and return the one that supports the descriptor
    for (int i = 0; i < actionList.size() && action == 0; i++)
    {
    	if (actionList.at(i)->actionDescriptionSupported(descriptor)){
    		//create a new heap object of the action
    		action = actionList.at(i)->clone();
    	}    
    }
  
    return action;
}

QVariantMap CntActionFactory::actionMetadata(const QContactActionDescriptor& descriptor) const
{
    Q_UNUSED(descriptor);
    
    return QVariantMap();
}

Q_EXPORT_PLUGIN2(ACTIONFACTORYPLUGINTARGET, CntActionFactory);
