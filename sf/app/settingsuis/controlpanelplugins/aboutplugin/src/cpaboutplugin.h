/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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
#ifndef	CPABOUTPLUGIN_H
#define	CPABOUTPLUGIN_H

#include <qobject.h>
#include <cpplugininterface.h>

#include "cpabout_global.h"

class CpAboutPlugin : public QObject, public CpPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CpPluginInterface)
    
public:
    CpAboutPlugin();
    virtual ~CpAboutPlugin();
    virtual QList<CpSettingFormItemData*> createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
        
    ABOUT_TEST_FRIEND_CLASS(TestAboutPlugin)
};

#endif //CPABOUTPLUGIN_H
