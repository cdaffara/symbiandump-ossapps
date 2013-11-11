/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMUIHEADERS_H_
#define NMUIHEADERS_H_

// Qt
#include <QApplication>
#include <QTimer>
#include <QtGlobal>
#include <QDateTime>
#include <QTranslator>
#include <QGraphicsProxyWidget>
#include <QMouseEvent>
#include <QTextDocument>
#include <QGraphicsLinearLayout>
#include <QWebFrame>
#include <QStackedLayout>
#include <QGraphicsSceneMouseEvent>
#include <QUrl>
#include <QDir>
#include <QDesktopServices>
#include <QWebElement>
#include <QWebPage>
#include <QPluginLoader>
#include <QCoreApplication>
#include <QAbstractItemModel>
#include <QNetworkDiskCache>
#include <QWebSettings>
#include <QPointer>
#include <QTapGesture>
#include <QPinchGesture>
#include <QInputContext>
#include <QFont>
#include <QFontMetrics>

// Orbit
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbtreeview.h>
#include <hbtreeviewitem.h>
#include <hbdocumentloader.h>
#include <hblabel.h>
#include <hbtextitem.h>
#include <hbanchorlayout.h>
#include <hbicon.h>
#include <hbframedrawer.h>
#include <hbframeitem.h>
#include <hbdeviceprofile.h>
#include <hbinputeditorinterface.h>
#include <hbstackedlayout.h>
#include <hbtextedit.h>
#include <hbtoolbar.h>
#include <hbgroupbox.h>
#include <hbpushbutton.h>
#include <hbprogressdialog.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbmessagebox.h>
#include <hbnotificationdialog.h>
#include <hbstyleloader.h>
#include <hbiconitem.h>
#include <hbextendedlocale.h>
#include <hbcolorscheme.h>
#include <hbdeviceprogressdialog.h>
#include <hbdevicemessagebox.h>
#include <hbtransparentwindow.h>
#include <hbdialog.h>
#include <hbmessagebox.h>
#include <hbtoolbarextension.h>
#include <hbinstance.h>
#include <hbeffect.h>
#include <hbmodeliterator.h>
#include <hbscrollarea.h>
#include <hbinputvkbhost.h>
#include <hbindicator.h>
#include <hbformatdialog.h>
#include <hbshrinkingvkbhost.h>
#include <hbactivitymanager.h>
#include <hbframebackground.h>
#include <hbparameterlengthlimiter.h>
#include <HbSearchPanel>
#include <HbIconAnimationManager>
#include <HbVkbHostBridge>
#include <hbevent.h>
#include <hbstringutil.h>

#include <cntservicescontact.h>
#include <xqservicerequest.h>
#include <qtcontacts.h>
#include <qmobilityglobal.h>
#include <xqserviceutil.h>
#include <xqappmgr.h>
#include <xqserviceipcmarshal.h>
#include <xqsharablefile.h>
#include <fmfiledialog.h>
#include <xqaiwdecl.h>
#include <afstorageglobals.h>
#include <afactivitystorage.h>
#include <afactivation.h>
#include <tstasksettings.h>

#include <nmcommon.h>

// emailservices/nmutilities
#include <nmuieventsnotifier.h>
#include <nmuriparser.h>

// nmailuiengine
#include <nmuiengine.h>
#include <nmmailboxmetadata.h>
#include <nmmessageenvelope.h>
#include <nmmailboxlistmodel.h>
#include <nmmessagelistmodel.h>
#include <nmmessagelistmodelitem.h>
#include <nmmessage.h>
#include <nmaddress.h>
#include <nmoperation.h>
#include <nmmessagecreationoperation.h>
#include <nmaddattachmentsoperation.h>
#include <nmicons.h>
#include <emailmailboxinfo.h>
#include <email_services_api.h>

// other
#include <math.h>

// nmailui
#include "nmhtmllineedit.h"
#include "nmaction.h"
#include "nmactionobserver.h"
#include "nmactionrequest.h"
#include "nmactionresponse.h"
#include "nmuiextensionmanager.h"
#include "nmuiextensioninterface.h"
#include "nmapplication.h"
#include "nmbaseview.h"
#include "nmbaseviewscrollarea.h"
#include "nmmailboxlistview.h"
#include "nmmailboxlistviewitem.h"
#include "nmmessagelistview.h"
#include "nmmessagelistviewitem.h"
#include "nmmessagesearchlistview.h"
#include "nmviewerview.h"
#include "nmviewerwebview.h"
#include "nmviewerheader.h"
#include "nmviewerviewnetmanager.h"
#include "nmviewerviewnetreply.h"
#include "nmeditorview.h"
#include "nmeditorcontent.h"
#include "nmeditorheader.h"
#include "nmeditortextedit.h"
#include "nmuiviewids.h"
#include "nmrecipientfield.h"
#include "nmrecipientlineedit.h"
#include "nmstoreenvelopesoperation.h"
#include "nmsendserviceinterface.h"
#include "nmuriserviceinterface.h"
#include "nmmailboxselectiondialog.h"
#include "nmuidocumentloader.h"
#include "nmmailboxserviceinterface.h"
#include "nmviewerserviceinterface.h"
#include "nmutilities.h"
#include "nmattachmentlistwidget.h"
#include "nmattachmentlist.h"
#include "nmeditortextdocument.h"
#include "nmattachmentmanager.h"
#include "nmattachmentpicker.h"
#include "nmattachmentfetchobserver.h"
#include "nmuieffects.h"

// settingui
#include "nmsettingsviewlauncher.h"

#endif /* NMUIHEADERS_H_ */
