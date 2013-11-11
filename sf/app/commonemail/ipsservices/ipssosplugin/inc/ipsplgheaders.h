/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains headers needed in plugin
*
*/

#ifndef IPSPLGHEADERS_H
#define IPSPLGHEADERS_H

// System includes
// <qmail>
#include <nmcommonheaders.h>
// </qmail>
#include <e32cmn.h>
#include <e32def.h>
#include <e32base.h>
#include <e32property.h>

#include <f32file.h>

// <qmail> eikenv, eikrutil, coemain includes removed

#include <charconv.h>

// cenrep
#include <centralrepository.h>
// ecom
#include <implementationproxy.h>
// mtm
// <qmail> MTMStore include removed
#include <mtmuibas.h>
// mtcl
#include <mtclbase.h>
#include <mtclreg.h>
// msv
#include <msvstd.hrh>
#include <msvapi.h>     // Messaging framework
#include <msvuids.h>
#include <msvstd.h>
// miut
#include <miutset.h>    // MTM identifiers
#include <miuthdr.h>    // TMsvEmailEntry
#include <miut_err.h>   // KErrImapServerBusy
#include <miutmsg.h>
#include <miutconv.h>
// <qmail>
#include <timrfc822datefield.h> // KUidMsvEmailTextEntry, KUidMsvEmailHtmlEntry 
// </qmail>
// imap
#include <imapset.h>
#include <imapcmds.h>
// pop
#include <pop3set.h>
#include <popcmtm.h>
// smtp
#include <smtpset.h>

#include <cmsvattachment.h>           // CMsvAttachment
#include <mmsvattachmentmanager.h>    // MMsvAttachmentManager
// cfsmail
//<cmail>
#include <CFSMailCommon.h>
#include <CFSMailMessage.h>
#include <CFSMailFolder.h>
// sendui
#include <SendUiConsts.h>
#include <SenduiMtmUids.h>

// <qmail> fsmailserverconst include removed
// mfsmail
#include "MFSMailBoxSearchObserver.h"
#include "MFSMailEventObserver.h"
//</cmail>
// alwaysonline
#include <AlwaysOnlineManagerClient.h>

#include <CoreApplicationUIsSDKCRKeys.h>

//<cmail> defines.h is not used and neither can be found in the environment.
//#include <defines.h>
//</cmail>

#include <cemailaccounts.h>

#include <in_sock.h>

// Internal headers

// base
#include "ipsplgsosbaseplugin.h"
#include "ipsplgbaseoperation.h"
#include "ipsplgsosbaseplugin.hrh"
#include "ipsplgcommon.h"

// smtp
#include "ipsplgsmtpservice.h"
#include "ipsplgsmtpoperation.h"
#include "ipsplgsmtpoperation.h"


// msg
#include "ipsplgmsgmapper.h"
#include "ipsplgmsgkey.h"
#include "ipsplgmsgiterator.h"      // CIpsPlgMsgIterator
#include "ipsplgmsgswap.h"        // TIpsPlgMsgSwap


// imap4
#include "ipsplgimap4connectop.h"
#include "ipsplgimap4plugin.h"
#include "ipsplgimap4connectop.h"
#include "ipsplgimap4populateop.h"
#include "ipsplgimap4fetchattachmentop.h"
#include "ipsplgimap4moveremoteop.h"
#include "ipsplgimap4fetchattachmentop.h"
#include "ipsplgimap4connectop.h"
#include "ipsplgconnectandrefreshfolderlist.h"


// pop3
#include "ipsplgpop3plugin.h"
#include "ipsplgpop3connectop.h"
#include "ipsplgpop3fetchoperation.h"
#include "ipsplgpop3connectop.h"


//<cmail>
//</cmail>

// search
#include "ipsplgsearch.h"
#include "ipsplgsearchop.h"
#include "ipsplgtextsearcher.h"

// <qmail> setting-related includes removed
// mrulist
#include "ipsplgmrulist.h"


// events
#include "ipsplgeventtimer.h"
#include "ipsplgeventhandler.h"
#include "ipsplgsyncstatehandler.h"

// observers
#include "MFSMailRequestObserver.h"
#include "ipsplgsettingsobserver.h"
#include "ipsplgpropertywatcher.h"


// panics
#include "ipsplgpanic.h"


// operations
#include "ipsplgtimeroperation.h"
// <qmail> removing #include "ipsplgoperationwait.h"
#include "ipsplgonlineoperation.h"
// <qmail>
#include "ipsplgcreatemessageoperation.h"
#include "ipsplgcreateforwardmessageoperation.h"
#include "ipsplgcreatereplymessageoperation.h"
#include "ipsplgnewchildpartfromfileoperation.h"
#include "ipsplgremovechildpartoperation.h"
#include "ipsplgmailstoreroperation.h"
#include "ipsplgmessagepartstoreroperation.h"
// </qmail>
#include "ipsplgdisconnectop.h"
#include "ipsplgservicedeletionop.h"
// <qmail> name changed
#include "ipsplgdeleteoperation.h"
// </qmail>
#include "ipsplgdeletelocal.h"
#include "ipsplgsingleopwatcher.h"

#endif //IPSPLGHEADERS_H
