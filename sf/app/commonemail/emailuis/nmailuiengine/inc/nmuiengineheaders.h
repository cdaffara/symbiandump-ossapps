/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMUIENGINEHEADERS_H_
#define NMUIENGINEHEADERS_H_

// Qt
#include <QList>
#include <QPluginLoader>
#include <QDir>
#include <QApplication>
#include <QLocale>
#include <QTimer>
#include <QPointer>

#include <xqsharablefile.h>

// Symbian
#include <e32base.h>

// Orbit
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbnamespace.h>
#include <hbicon.h>
#include <hbiconanimationmanager.h>
#include <hbmenu.h>
#include <hbindicator.h>

// nmailuiengine
#include "nmuienginedef.h"
#include "nmuiengine.h"
#include "nmdatamanager.h"
#include "nmmailboxlistmodel.h"
#include "nmfolderlistmodel.h"
#include "nmmessagelistmodel.h"
#include "nmdatapluginfactory.h"
#include "nmdataplugininterface.h"
#include "nmmailboxlistmodelitem.h"
#include "nmmessagelistmodelitem.h"
#include "nmmailboxmetadata.h"
#include "nmfoldermetadata.h"
#include "nmicons.h"
#include "nmbaseclientplugin.h"
#include "nmoperation.h"
#include "nmstoreenvelopesoperation.h"
#include "nmmessagesendingoperation.h"
#include "nmaddattachmentsoperation.h"
#include "nmmessagecreationoperation.h"
#include "nmmessagecreationoperation.h"
#include "nmapplicationstateinterface.h"

// nmailui
#include "nmactionrequest.h"
#include "nmactionresponse.h"
#include "nmactionobserver.h"
#include "nmaction.h"

// nmailbase
#include "nmcommon.h"
#include "nmmailbox.h"
#include "nmmessage.h"
#include "nmmessagepart.h"
#include "nmmessageenvelope.h"
#include "nmfolder.h"


#endif /* NMUIENGINEHEADERS_H_ */
