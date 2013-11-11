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

#ifndef NMFRAMEWORKADAPTERHEADERS_H_
#define NMFRAMEWORKADAPTERHEADERS_H_

// Qt
#include <QtPlugin>
#include <QByteArray>
#include <QTextCodec>
#include <QRegExp>
#include <QStringList>
#include <QTimer>
#include <QPointer>

#include <xqsharablefile.h>

// Symbian
#include <utf.h> 

// nmailuiengine 
#include <nmoperation.h>
#include <nmstoreenvelopesoperation.h>
#include <nmmessagesendingoperation.h>
#include <nmaddattachmentsoperation.h>
#include <nmmessagecreationoperation.h>
#include <nmdataplugininterface.h>
#include <nmapplicationstateinterface.h>

// nmailbase
#include <nmmailbox.h>
#include <nmcommon.h>
#include <nmmessageenvelope.h>
#include <nmmessage.h>
#include <nmconverter.h>

// emailcommon
#include <nmcleanuputils.h>
#include <cmailboxstateext.h>

// nmframeworkadapter
#include "nmframeworkadapter.h"
#include "nmfwaaddattachmentsoperation.h"
#include "nmfwaremoveattachmentoperation.h"
#include "nmfwamessagecreationoperation.h"
#include "nmfwaforwardmessagecreationoperation.h"
#include "nmfwareplymessagecreationoperation.h"
#include "nmfwamessagefetchingoperation.h"
#include "nmfwamessagepartfetchingoperation.h"
#include "nmfwamessagepartsfetchingoperation.h"
#include "nmfwamessagesendingoperation.h"
#include "nmfwastoreenvelopesoperation.h"
#include "nmfwastoremessageoperation.h"
#include "nmmailboxsearchobserver.h"
#include "nmfwaremovedraftmessageoperation.h"
#include "nmfwadeletemailboxoperation.h"

// fs email
#include <CFSMailCommon.h>
#include <CFSMailClient.h>
#include <CFSMailRequestObserver.h>
#include <CFSMailFolder.h>
#include <CFSMailBox.h>
#include <MFSMailBoxSearchObserver.h>
#include <MFSMailIterator.h>




#endif /* NMFRAMEWORKADAPTERHEADERS_H_ */
