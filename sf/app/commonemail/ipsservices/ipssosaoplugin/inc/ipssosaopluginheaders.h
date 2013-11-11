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
*
*/

#ifndef IPSSOSAOPLUGINHEADERS_H_
#define IPSSOSAOPLUGINHEADERS_H_

// Symbian
#include <e32base.h>
#include <ecom/implementationproxy.h>
#include <e32property.h>
#include <AlwaysOnlineManagerCommon.h>
#include <AlwaysOnlineManagerClient.h>
#include <cemailaccounts.h>
#include <imapcmds.h>
#include <miutset.h>
#include <impcmtm.h>
#include <msvapi.h>
#include <SendUiConsts.h>
#include <msvids.h>
#include <msvuids.h>
#include <cmmanager.h>
#include <pop3cmds.h>
#include <popcmtm.h>
#include <smtpset.h>

// qt
#include <qlist.h>

// qt extensions
#include <xqsettingsmanager.h>
#include <xqcentralrepositoryutils.h>
#include <xqcentralrepositorysearchcriteria.h>

// emailcommon
#include "CFSMailCommon.h"

// emaildebug
#include "emailtrace.h"

// nmailbase
#include "nmcommon.h"

// nmipssettings
#include "nmipssettingitems.h"
#include "ipssettingkeys.h"

// ipssosplugin
#include "ipsplgsosbaseplugin.hrh"
#include "ipsplgcommon.h"
#include "ipsplgsmtpoperation.h"
#include "ipsplgimap4populateop.h"

// ipssosaoplugin
#include "IpsSosAOBaseAgent.h"
#include "IpsSosAOEMNResolver.h"
#include "IpsSosAoExtendedSettingsManager.h"
#include "IpsSosAOImapAgent.h"
#include "IpsSosAOImapPopLogic.h"
#include "IpsSosAOMboxLogic.h"
#include "IpsSosAOPlugin.hrh"
#include "IpsSosAOPluginEComInterface.h"
#include "IpsSosAOPluginTimer.h"
#include "IpsSosAOPopAgent.h"
#include "IpsSosAOSchedulerUtils.h"
#include "IpsSosAOSettingsHandler.h"
#include "IpsSosAOSmtpAgent.h"
#include "IpsSosAOSmtpSendWatcher.h"

#endif /* IPSSOSAOPLUGINHEADERS_H_ */
