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

#ifndef NMHSWIDGETCONSTS_H
#define NMHSWIDGETCONSTS_H

/**************************************************
 * engine
 *************************************************/
//Maximum amount of envelopes that can be provided to client in getData function
//This is also the amount of envelopes that is kept in mData all the time
const int KMaxNumberOfEnvelopesProvided = 99;
//Maximum value for unread count
const int KMaxUnreadCount = 999; 

/**************************************************
 * nmhswidget
 *************************************************/
/**  docml file including UI definitions */
const QString KNmHsWidgetDocML = ":/docml/nmhswidget.docml";
/** container widget including all UI components */
const QString KNmHsWidgetMainContainer = "mainContainer";
/** container widget including title row and content container */
const QString KNmHsWidgetContainer = "nmhswidgetContainer";
/** container widget including content (mail rows / no mails label */
const QString KNmHsWidgetEmptySpaceContainer = "emptySpaceContainer";
/** container widget including content (mail rows / no mails label */
const QString KNmHsWidgetContentContainer = "mailRowContainer";
/** no mails label - shown when inbox is empty */
const QString KNmHsWidgetNoMailsLabel = "noMailsLabel";
/** mail list view - list component that contains the messages */
const QString KNmHsWidgetMailListView = "mailListView";
/** contents margin/spacing in all layouts / container widgets */
const int KNmHsWidgetContentsMargin = 0;
/** Widget's background image*/
const QString KNmHsWidgetBackgroundImage = "qtg_fr_hswidget_normal";
/** max number of mails shown in widget */
const int KMaxNumberOfMailsShown = KMaxNumberOfEnvelopesProvided;
/** Widget's localization file name*/
const QString KNmHsWidgetLocFileName = "mailwidget_";
/** Widget's localization file location*/
const QString KNmHsWidgetLocLocation = "/resource/qt/translations/";
/** Widget's state property values */
const QString KNmHsWidgetStateCollapsed = "collapsed";
const QString KNmHsWidgetStateExpanded = "expanded";
/** maximum home screen widget height in units */
const int KNmHsWidgetHSMaxWidgetHeightInUnits = 39;

/**************************************************
 * Title row widget
 *************************************************/
/**  docml file including title row UI definitions */
/**#define KNmHsWidgetTitleRowDocML ":/docml/nmhswidgettitle.docml" */
/**  docml file including title row UI definitions */
const QString KNmHsWidgetTitleRowContainer = "titleLayoutWidget";
/** mailbox icon */
const QString KNmHsWidgetTitleRowMailboxIcon = "mailboxIcon";
/** mailbox name label */
const QString KNmHsWidgetTitleRowMailboxNameLabel = "mailboxNameLabel";
/** unread count label */
const QString KNmHsWidgetTitleRowUnreadCountLabel = "unreadCountLabel";
/** expand collapse icon */
const QString KNmHsWidgetTitleRowCollapseExpandIconLabel = "collapseExpandIcon";

/**************************************************
 * Mail row widget
 *************************************************/
/**  docml file including mail row UI definitions */
const QString KNmHsWidgetMailRowDocML = ":/docml/nmhswidgetmail.docml";
/** container widget name for mail row UI items */
const QString KNmHsWidgetMailRowContainer  = "emailLayoutWidget";
/** sender label */
const QString KNmHsWidgetMailRowSenderLabel = "labelSender";
/** time label */
const QString KNmHsWidgetMailRowTimeLabel = "labelTime";
/** subject label */
const QString KNmHsWidgetMailRowSubjectLabel = "labelSubject";
/** new mail icon */
const QString KNmHsWidgetMailRowNewMailIcon = "iconNewMail";
/** left status icon */
const QString KNmHsWidgetMailRowLeftIcon = "iconOne";
/** middle status icon */
const QString KNmHsWidgetMailRowMiddleIcon = "iconTwo";
/** right status icon */
const QString KNmHsWidgetMailRowRightIcon = "iconThree";
/** separator icon */
const QString KNmHsWidgetMailSeparatorIcon = "iconSeparator";


#endif  // NMHSWIDGETCONSTS_H
