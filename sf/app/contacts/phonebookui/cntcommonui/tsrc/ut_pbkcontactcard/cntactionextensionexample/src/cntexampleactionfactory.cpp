/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#define ACTIONFACTORYPLUGINTARGET cntexampleactionsplugin
#endif
#ifndef ACTIONFACTORYPLUGINNAME
#define ACTIONFACTORYPLUGINNAME SymbianActionFactory
#endif

#include "cntexampleactionfactory.h"
#include "cntexampleaction.h"
#include "cntexample2action.h"
#include "cntexamplenodetailaction.h"
#include "cntexampledynamicaction.h"

#include <qcontactonlineaccount.h>
#include <qcontactdetailfilter.h>

#define makestr(x) (#x)
#define makename(x) makestr(x)


//Factory class
MobExampleActionFactory::MobExampleActionFactory() 
{
    // Next two actions are different, but for same service. These actions can be used
    // also for other services, by changing "Test service". In real plugin all these actions
    // would have been created once for each service. In this example third action is created for
    // different service for testing purposes.
    actionList.append(new MobExampleAction("staticaction1", "Test service"));
    actionList.append(new MobExample2Action("staticaction2", "Test service"));
    // This action is for second service.
    actionList.append(new MobExampleNoDetailAction("nodetailaction", "Test service 2"));
    
    // create several variants of one action and change it behaviour in run time
    MobExampleDynamicAction* nonUiItem = new MobExampleDynamicAction("dynamicaction", "Test service 2");
    nonUiItem->setDefinitionName(QContactOnlineAccount::DefinitionName);
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactOnlineAccount::DefinitionName);
    nonUiItem->setFilter(filter);
    nonUiItem->setIcon(QIcon());
    nonUiItem->setTitleField("");
    nonUiItem->setTitleFieldDetail("");
    actionList.append(nonUiItem);
    
    MobExampleDynamicAction* emptyValueField = new MobExampleDynamicAction("dynamicaction2", "Test service 2");
    emptyValueField->setDefinitionName(QContactOnlineAccount::DefinitionName);
    QContactDetailFilter filter2;
    filter2.setDetailDefinitionName(QContactOnlineAccount::DefinitionName);
    emptyValueField->setFilter(filter2);
    emptyValueField->setIcon(QIcon());
    emptyValueField->setTitleFieldDetail(QContactOnlineAccount::FieldServiceProvider);
    emptyValueField->setValueField("");
    actionList.append(emptyValueField);
}

MobExampleActionFactory::~MobExampleActionFactory()
{
	 while (!actionList.isEmpty())
	     delete actionList.takeFirst();
}

QString MobExampleActionFactory::name() const
{
    return QString(makename(ACTIONFACTORYPLUGINNAME));
}


QList<QContactActionDescriptor> MobExampleActionFactory::actionDescriptors() const
{
   QList<QContactActionDescriptor> descriptorList; 
   
   //loop through all the actions and add the descriptor to the list
   for (int i = 0; i < actionList.size(); i++)
   {
	   descriptorList << actionList.at(i)->actionDescriptor();   
   }
   
   return descriptorList;
}

QContactAction* MobExampleActionFactory::instance(const QContactActionDescriptor& descriptor) const
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

QVariantMap MobExampleActionFactory::actionMetadata(const QContactActionDescriptor& descriptor) const
{
    QVariantMap map;
    
    //loop through the actions and return the one that supports the descriptor
    for (int i = 0; i < actionList.size() && map.isEmpty(); i++)
    {
        if (actionList.at(i)->actionDescriptionSupported(descriptor))
        {
            map = actionList.at(i)->metaData();
        }    
    }
  
    return map;

}

Q_EXPORT_PLUGIN2(ACTIONFACTORYPLUGINTARGET, MobExampleActionFactory);
