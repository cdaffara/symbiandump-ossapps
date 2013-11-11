// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef _PHBKSYNCLOG_H_
#define _PHBKSYNCLOG_H_

/**
 *  @file
 *
 *  Logging macros for Phonebook Sync. These use the Comms Debug / Flogger V2
 *  Framework.
 *
 *  @internalComponent
 */

#include <comms-infras/commsdebugutility.h>

#ifdef _DEBUG

_LIT8(KPhBkSyncSubSystem, "PhBkSync");
_LIT8(KPhBkSyncPlugin,  "Plugin");
_LIT8(KPhBkSyncClient,  "Client");
_LIT8(KPhBkSyncCommon,  "Common");
_LIT8(KPhBkSyncSession, "Session");
_LIT8(KPhBkSyncServer,  "Server");
_LIT8(KPhBkSyncParams,  "Params");
_LIT8(KPhBkSyncIniFile, "IniFile");
_LIT8(KPhBkSyncEngine,  "Engine");
_LIT8(KPhBkSyncActive,  "Active");

#define LOGPLUGIN1(A)                 __FLOG_STATIC0(KPhBkSyncSubSystem,KPhBkSyncPlugin,(A))
#define LOGPLUGIN2(A,B)               __FLOG_STATIC1(KPhBkSyncSubSystem,KPhBkSyncPlugin,(A),(B))
#define LOGPLUGIN3(A,B,C)             __FLOG_STATIC2(KPhBkSyncSubSystem,KPhBkSyncPlugin,(A),(B),(C))
#define LOGPLUGIN4(A,B,C,D)           __FLOG_STATIC3(KPhBkSyncSubSystem,KPhBkSyncPlugin,(A),(B),(C),(D))
#define LOGPLUGIN5(A,B,C,D,E)         __FLOG_STATIC4(KPhBkSyncSubSystem,KPhBkSyncPlugin,(A),(B),(C),(D),(E))
#define LOGPLUGIN6(A,B,C,D,E,F)       __FLOG_STATIC5(KPhBkSyncSubSystem,KPhBkSyncPlugin,(A),(B),(C),(D),(E),(F))
#define LOGPLUGIN7(A,B,C,D,E,F,G)     __FLOG_STATIC6(KPhBkSyncSubSystem,KPhBkSyncPlugin,(A),(B),(C),(D),(E),(F),(G))
#define LOGPLUGIN8(A,B,C,D,E,F,G,H)   __FLOG_STATIC7(KPhBkSyncSubSystem,KPhBkSyncPlugin,(A),(B),(C),(D),(E),(F),(G),(H))
#define LOGPLUGINTIMESTAMP()		  { \
									  TTime  now; \
									  now.UniversalTime(); \
									  TDateTime dateTime(now.DateTime()); \
									  __FLOG_STATIC4(KPhBkSyncSubSystem, KPhBkSyncPlugin, \
									  _L8("TimeStamp=%02d:%02d:%02d.%06d"), \
									  dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()); \
									  }

#define LOGCLIENT1(A)                 __FLOG_STATIC0(KPhBkSyncSubSystem,KPhBkSyncClient,(A))
#define LOGCLIENT2(A,B)               __FLOG_STATIC1(KPhBkSyncSubSystem,KPhBkSyncClient,(A),(B))
#define LOGCLIENT3(A,B,C)             __FLOG_STATIC2(KPhBkSyncSubSystem,KPhBkSyncClient,(A),(B),(C))
#define LOGCLIENT4(A,B,C,D)           __FLOG_STATIC3(KPhBkSyncSubSystem,KPhBkSyncClient,(A),(B),(C),(D))
#define LOGCLIENT5(A,B,C,D,E)         __FLOG_STATIC4(KPhBkSyncSubSystem,KPhBkSyncClient,(A),(B),(C),(D),(E))
#define LOGCLIENT6(A,B,C,D,E,F)       __FLOG_STATIC5(KPhBkSyncSubSystem,KPhBkSyncClient,(A),(B),(C),(D),(E),(F))
#define LOGCLIENT7(A,B,C,D,E,F,G)     __FLOG_STATIC6(KPhBkSyncSubSystem,KPhBkSyncClient,(A),(B),(C),(D),(E),(F),(G))
#define LOGCLIENT8(A,B,C,D,E,F,G,H)   __FLOG_STATIC7(KPhBkSyncSubSystem,KPhBkSyncClient,(A),(B),(C),(D),(E),(F),(G),(H))
#define LOGCLIENTTIMESTAMP()		  { \
									  TTime  now; \
									  now.UniversalTime(); \
									  TDateTime dateTime(now.DateTime()); \
									  __FLOG_STATIC4(KPhBkSyncSubSystem, KPhBkSyncClient, \
									  _L8("TimeStamp=%02d:%02d:%02d.%06d"), \
									  dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()); \
									  }

#define LOGCOMMON1(A)                 __FLOG_STATIC0(KPhBkSyncSubSystem,KPhBkSyncCommon,(A))
#define LOGCOMMON2(A,B)               __FLOG_STATIC1(KPhBkSyncSubSystem,KPhBkSyncCommon,(A),(B))
#define LOGCOMMON3(A,B,C)             __FLOG_STATIC2(KPhBkSyncSubSystem,KPhBkSyncCommon,(A),(B),(C))
#define LOGCOMMON4(A,B,C,D)           __FLOG_STATIC3(KPhBkSyncSubSystem,KPhBkSyncCommon,(A),(B),(C),(D))
#define LOGCOMMON5(A,B,C,D,E)         __FLOG_STATIC4(KPhBkSyncSubSystem,KPhBkSyncCommon,(A),(B),(C),(D),(E))
#define LOGCOMMON6(A,B,C,D,E,F)       __FLOG_STATIC5(KPhBkSyncSubSystem,KPhBkSyncCommon,(A),(B),(C),(D),(E),(F))
#define LOGCOMMON7(A,B,C,D,E,F,G)     __FLOG_STATIC6(KPhBkSyncSubSystem,KPhBkSyncCommon,(A),(B),(C),(D),(E),(F),(G))
#define LOGCOMMON8(A,B,C,D,E,F,G,H)   __FLOG_STATIC7(KPhBkSyncSubSystem,KPhBkSyncCommon,(A),(B),(C),(D),(E),(F),(G),(H))
#define LOGCOMMONTIMESTAMP()		  { \
									  TTime  now; \
									  now.UniversalTime(); \
									  TDateTime dateTime(now.DateTime()); \
									  __FLOG_STATIC4(KPhBkSyncSubSystem, KPhBkSyncCommon, \
									  _L8("TimeStamp=%02d:%02d:%02d.%06d"), \
									  dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()); \
									  }
                                     
#define LOGSESSION1(A)                __FLOG_STATIC0(KPhBkSyncSubSystem,KPhBkSyncSession,(A))
#define LOGSESSION2(A,B)              __FLOG_STATIC1(KPhBkSyncSubSystem,KPhBkSyncSession,(A),(B))
#define LOGSESSION3(A,B,C)            __FLOG_STATIC2(KPhBkSyncSubSystem,KPhBkSyncSession,(A),(B),(C))
#define LOGSESSION4(A,B,C,D)          __FLOG_STATIC3(KPhBkSyncSubSystem,KPhBkSyncSession,(A),(B),(C),(D))
#define LOGSESSION5(A,B,C,D,E)        __FLOG_STATIC4(KPhBkSyncSubSystem,KPhBkSyncSession,(A),(B),(C),(D),(E))
#define LOGSESSION6(A,B,C,D,E,F)      __FLOG_STATIC5(KPhBkSyncSubSystem,KPhBkSyncSession,(A),(B),(C),(D),(E),(F))
#define LOGSESSION7(A,B,C,D,E,F,G)    __FLOG_STATIC6(KPhBkSyncSubSystem,KPhBkSyncSession,(A),(B),(C),(D),(E),(F),(G))
#define LOGSESSION8(A,B,C,D,E,F,G,H)  __FLOG_STATIC7(KPhBkSyncSubSystem,KPhBkSyncSession,(A),(B),(C),(D),(E),(F),(G),(H))
#define LOGSESSIONTIMESTAMP()		  { \
									  TTime  now; \
									  now.UniversalTime(); \
									  TDateTime dateTime(now.DateTime()); \
									  __FLOG_STATIC4(KPhBkSyncSubSystem, KPhBkSyncSession, \
									  _L8("TimeStamp=%02d:%02d:%02d.%06d"), \
									  dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()); \
									  }
                                     
#define LOGSERVER1(A)                 __FLOG_STATIC0(KPhBkSyncSubSystem,KPhBkSyncServer,(A))
#define LOGSERVER2(A,B)               __FLOG_STATIC1(KPhBkSyncSubSystem,KPhBkSyncServer,(A),(B))
#define LOGSERVER3(A,B,C)             __FLOG_STATIC2(KPhBkSyncSubSystem,KPhBkSyncServer,(A),(B),(C))
#define LOGSERVER4(A,B,C,D)           __FLOG_STATIC3(KPhBkSyncSubSystem,KPhBkSyncServer,(A),(B),(C),(D))
#define LOGSERVER5(A,B,C,D,E)         __FLOG_STATIC4(KPhBkSyncSubSystem,KPhBkSyncServer,(A),(B),(C),(D),(E))
#define LOGSERVER6(A,B,C,D,E,F)       __FLOG_STATIC5(KPhBkSyncSubSystem,KPhBkSyncServer,(A),(B),(C),(D),(E),(F))
#define LOGSERVER7(A,B,C,D,E,F,G)     __FLOG_STATIC6(KPhBkSyncSubSystem,KPhBkSyncServer,(A),(B),(C),(D),(E),(F),(G))
#define LOGSERVER8(A,B,C,D,E,F,G,H)   __FLOG_STATIC7(KPhBkSyncSubSystem,KPhBkSyncServer,(A),(B),(C),(D),(E),(F),(G),(H))
#define LOGSERVERTIMESTAMP()		  { \
									  TTime  now; \
									  now.UniversalTime(); \
									  TDateTime dateTime(now.DateTime()); \
									  __FLOG_STATIC4(KPhBkSyncSubSystem, KPhBkSyncServer, \
									  _L8("TimeStamp=%02d:%02d:%02d.%06d"), \
									  dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()); \
									  }
                                     
#define LOGPARAMS1(A)                 __FLOG_STATIC0(KPhBkSyncSubSystem,KPhBkSyncParams,(A))
#define LOGPARAMS2(A,B)               __FLOG_STATIC1(KPhBkSyncSubSystem,KPhBkSyncParams,(A),(B))
#define LOGPARAMS3(A,B,C)             __FLOG_STATIC2(KPhBkSyncSubSystem,KPhBkSyncParams,(A),(B),(C))
#define LOGPARAMS4(A,B,C,D)           __FLOG_STATIC3(KPhBkSyncSubSystem,KPhBkSyncParams,(A),(B),(C),(D))
#define LOGPARAMS5(A,B,C,D,E)         __FLOG_STATIC4(KPhBkSyncSubSystem,KPhBkSyncParams,(A),(B),(C),(D),(E))
#define LOGPARAMS6(A,B,C,D,E,F)       __FLOG_STATIC5(KPhBkSyncSubSystem,KPhBkSyncParams,(A),(B),(C),(D),(E),(F))
#define LOGPARAMS7(A,B,C,D,E,F,G)     __FLOG_STATIC6(KPhBkSyncSubSystem,KPhBkSyncParams,(A),(B),(C),(D),(E),(F),(G))
#define LOGPARAMS8(A,B,C,D,E,F,G,H)   __FLOG_STATIC7(KPhBkSyncSubSystem,KPhBkSyncParams,(A),(B),(C),(D),(E),(F),(G),(H))
#define LOGPARAMSTIMESTAMP()		  { \
									  TTime  now; \
									  now.UniversalTime(); \
									  TDateTime dateTime(now.DateTime()); \
									  __FLOG_STATIC4(KPhBkSyncSubSystem, KPhBkSyncParams, \
									  _L8("TimeStamp=%02d:%02d:%02d.%06d"), \
									  dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()); \
									  }
                                     
#define LOGINIFILE1(A)                __FLOG_STATIC0(KPhBkSyncSubSystem,KPhBkSyncIniFile,(A))
#define LOGINIFILE2(A,B)              __FLOG_STATIC1(KPhBkSyncSubSystem,KPhBkSyncIniFile,(A),(B))
#define LOGINIFILE3(A,B,C)            __FLOG_STATIC2(KPhBkSyncSubSystem,KPhBkSyncIniFile,(A),(B),(C))
#define LOGINIFILE4(A,B,C,D)          __FLOG_STATIC3(KPhBkSyncSubSystem,KPhBkSyncIniFile,(A),(B),(C),(D))
#define LOGINIFILE5(A,B,C,D,E)        __FLOG_STATIC4(KPhBkSyncSubSystem,KPhBkSyncIniFile,(A),(B),(C),(D),(E))
#define LOGINIFILE6(A,B,C,D,E,F)      __FLOG_STATIC5(KPhBkSyncSubSystem,KPhBkSyncIniFile,(A),(B),(C),(D),(E),(F))
#define LOGINIFILE7(A,B,C,D,E,F,G)    __FLOG_STATIC6(KPhBkSyncSubSystem,KPhBkSyncIniFile,(A),(B),(C),(D),(E),(F),(G))
#define LOGINIFILE8(A,B,C,D,E,F,G,H)  __FLOG_STATIC7(KPhBkSyncSubSystem,KPhBkSyncIniFile,(A),(B),(C),(D),(E),(F),(G),(H))
#define LOGINIFILETIMESTAMP()		  { \
									  TTime  now; \
									  now.UniversalTime(); \
									  TDateTime dateTime(now.DateTime()); \
									  __FLOG_STATIC4(KPhBkSyncSubSystem, KPhBkSyncIniFile, \
									  _L8("TimeStamp=%02d:%02d:%02d.%06d"), \
									  dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()); \
									  }
                                     
#define LOGENGINE1(A)                 __FLOG_STATIC0(KPhBkSyncSubSystem,KPhBkSyncEngine,(A))
#define LOGENGINE2(A,B)               __FLOG_STATIC1(KPhBkSyncSubSystem,KPhBkSyncEngine,(A),(B))
#define LOGENGINE3(A,B,C)             __FLOG_STATIC2(KPhBkSyncSubSystem,KPhBkSyncEngine,(A),(B),(C))
#define LOGENGINE4(A,B,C,D)           __FLOG_STATIC3(KPhBkSyncSubSystem,KPhBkSyncEngine,(A),(B),(C),(D))
#define LOGENGINE5(A,B,C,D,E)         __FLOG_STATIC4(KPhBkSyncSubSystem,KPhBkSyncEngine,(A),(B),(C),(D),(E))
#define LOGENGINE6(A,B,C,D,E,F)       __FLOG_STATIC5(KPhBkSyncSubSystem,KPhBkSyncEngine,(A),(B),(C),(D),(E),(F))
#define LOGENGINE7(A,B,C,D,E,F,G)     __FLOG_STATIC6(KPhBkSyncSubSystem,KPhBkSyncEngine,(A),(B),(C),(D),(E),(F),(G))
#define LOGENGINE8(A,B,C,D,E,F,G,H)   __FLOG_STATIC7(KPhBkSyncSubSystem,KPhBkSyncEngine,(A),(B),(C),(D),(E),(F),(G),(H))
#define LOGENGINETIMESTAMP()		  { \
									  TTime  now; \
									  now.UniversalTime(); \
									  TDateTime dateTime(now.DateTime()); \
									  __FLOG_STATIC4(KPhBkSyncSubSystem, KPhBkSyncEngine, \
									  _L8("TimeStamp=%02d:%02d:%02d.%06d"), \
									  dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()); \
									  }
                                     
#define LOGACTIVE1(A)                 __FLOG_STATIC0(KPhBkSyncSubSystem,KPhBkSyncActive,(A))
#define LOGACTIVE2(A,B)               __FLOG_STATIC1(KPhBkSyncSubSystem,KPhBkSyncActive,(A),(B))
#define LOGACTIVE3(A,B,C)             __FLOG_STATIC2(KPhBkSyncSubSystem,KPhBkSyncActive,(A),(B),(C))
#define LOGACTIVE4(A,B,C,D)           __FLOG_STATIC3(KPhBkSyncSubSystem,KPhBkSyncActive,(A),(B),(C),(D))
#define LOGACTIVE5(A,B,C,D,E)         __FLOG_STATIC4(KPhBkSyncSubSystem,KPhBkSyncActive,(A),(B),(C),(D),(E))
#define LOGACTIVE6(A,B,C,D,E,F)       __FLOG_STATIC5(KPhBkSyncSubSystem,KPhBkSyncActive,(A),(B),(C),(D),(E),(F))
#define LOGACTIVE7(A,B,C,D,E,F,G)     __FLOG_STATIC6(KPhBkSyncSubSystem,KPhBkSyncActive,(A),(B),(C),(D),(E),(F),(G))
#define LOGACTIVE8(A,B,C,D,E,F,G,H)   __FLOG_STATIC7(KPhBkSyncSubSystem,KPhBkSyncActive,(A),(B),(C),(D),(E),(F),(G),(H))
#define LOGACTIVETIMESTAMP()		  { \
									  TTime  now; \
									  now.UniversalTime(); \
									  TDateTime dateTime(now.DateTime()); \
									  __FLOG_STATIC4(KPhBkSyncSubSystem, KPhBkSyncActive, \
									  _L8("TimeStamp=%02d:%02d:%02d.%06d"), \
									  dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()); \
									  }
                                     
#else  // _DEBUG

#define LOGPLUGIN1(A)
#define LOGPLUGIN2(A,B)
#define LOGPLUGIN3(A,B,C)
#define LOGPLUGIN4(A,B,C,D)
#define LOGPLUGIN5(A,B,C,D,E)
#define LOGPLUGIN6(A,B,C,D,E,F)
#define LOGPLUGIN7(A,B,C,D,E,F,G)
#define LOGPLUGIN8(A,B,C,D,E,F)
#define LOGPLUGINTIMESTAMP()

#define LOGCLIENT1(A)
#define LOGCLIENT2(A,B)
#define LOGCLIENT3(A,B,C)
#define LOGCLIENT4(A,B,C,D)
#define LOGCLIENT5(A,B,C,D,E)
#define LOGCLIENT6(A,B,C,D,E,F)
#define LOGCLIENT7(A,B,C,D,E,F,G)
#define LOGCLIENT8(A,B,C,D,E,F)
#define LOGCLIENTTIMESTAMP()

#define LOGCOMMON1(A)
#define LOGCOMMON2(A,B)
#define LOGCOMMON3(A,B,C)
#define LOGCOMMON4(A,B,C,D)
#define LOGCOMMON5(A,B,C,D,E)
#define LOGCOMMON6(A,B,C,D,E,F)
#define LOGCOMMON7(A,B,C,D,E,F,G)
#define LOGCOMMON8(A,B,C,D,E,F,G,H)
#define LOGCOMMONTIMESTAMP()
                                     
#define LOGSESSION1(A)
#define LOGSESSION2(A,B)
#define LOGSESSION3(A,B,C)
#define LOGSESSION4(A,B,C,D)
#define LOGSESSION5(A,B,C,D,E)
#define LOGSESSION6(A,B,C,D,E,F)
#define LOGSESSION7(A,B,C,D,E,F,G)
#define LOGSESSION8(A,B,C,D,E,F,G,H)
#define LOGSESSIONTIMESTAMP()
                                     
#define LOGSERVER1(A)
#define LOGSERVER2(A,B)
#define LOGSERVER3(A,B,C)
#define LOGSERVER4(A,B,C,D)
#define LOGSERVER5(A,B,C,D,E)
#define LOGSERVER6(A,B,C,D,E,F)
#define LOGSERVER7(A,B,C,D,E,F,G)
#define LOGSERVER8(A,B,C,D,E,F,G,H)
#define LOGSERVERTIMESTAMP()
                                     
#define LOGPARAMS1(A)
#define LOGPARAMS2(A,B)
#define LOGPARAMS3(A,B,C)
#define LOGPARAMS4(A,B,C,D)
#define LOGPARAMS5(A,B,C,D,E)
#define LOGPARAMS6(A,B,C,D,E,F)
#define LOGPARAMS7(A,B,C,D,E,F,G)
#define LOGPARAMS8(A,B,C,D,E,F,G,H)
#define LOGPARAMSTIMESTAMP()
                                     
#define LOGINIFILE1(A)
#define LOGINIFILE2(A,B)
#define LOGINIFILE3(A,B,C)
#define LOGINIFILE4(A,B,C,D)
#define LOGINIFILE5(A,B,C,D,E)
#define LOGINIFILE6(A,B,C,D,E,F)
#define LOGINIFILE7(A,B,C,D,E,F,G)
#define LOGINIFILE8(A,B,C,D,E,F,G,H)
#define LOGINIFILETIMESTAMP()
                                     
#define LOGENGINE1(A)
#define LOGENGINE2(A,B)
#define LOGENGINE3(A,B,C)
#define LOGENGINE4(A,B,C,D)
#define LOGENGINE5(A,B,C,D,E)
#define LOGENGINE6(A,B,C,D,E,F)
#define LOGENGINE7(A,B,C,D,E,F,G)
#define LOGENGINE8(A,B,C,D,E,F,G,H)
#define LOGENGINETIMESTAMP()
                                     
#define LOGACTIVE1(A)
#define LOGACTIVE2(A,B)
#define LOGACTIVE3(A,B,C)
#define LOGACTIVE4(A,B,C,D)
#define LOGACTIVE5(A,B,C,D,E)
#define LOGACTIVE6(A,B,C,D,E,F)
#define LOGACTIVE7(A,B,C,D,E,F,G)
#define LOGACTIVE8(A,B,C,D,E,F,G,H)
#define LOGACTIVETIMESTAMP()
                                     
#endif	//_DEBUG

#endif  // _PHBKSYNCLOG_H_
