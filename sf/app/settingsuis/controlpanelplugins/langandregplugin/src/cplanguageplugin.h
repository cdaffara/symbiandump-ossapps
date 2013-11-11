/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef	CPLANGUAGEPLUGIN_H
#define	CPLANGUAGEPLUGIN_H

#include <QObject.h>
#include <cpplugininterface.h>
#include "cplanguageplugin_global.h"

class CpLanguagePlugin : public QObject, public CpPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CpPluginInterface)
public:
    CpLanguagePlugin();
    virtual ~CpLanguagePlugin();
    virtual QList<CpSettingFormItemData*> createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
public:
    static void updateEntryItem();
};

#endif // CPLANGUAGEPLUGIN_H

//End of File
