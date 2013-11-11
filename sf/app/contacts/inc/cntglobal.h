/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CNTGLOBAL_H
#define CNTGLOBAL_H

#include <QtGlobal>

#ifdef PBK_UNIT_TEST
#define QTPBK_EXPORT
#else
#ifdef BUILD_QTPBK
#define QTPBK_EXPORT Q_DECL_EXPORT
#else
#define QTPBK_EXPORT Q_DECL_IMPORT
#endif
#endif

// Format: qtcontacts:<managerid>:<key>=<value>&<key>=<value>
static const QString SYMBIAN_BACKEND     = "qtcontacts:symbian:";
static const QString SIM_BACKEND         = "qtcontacts:symbiansim:";
static const QString SIM_BACKEND_ADN     = "qtcontacts:symbiansim:store=ADN";
static const QString SIM_BACKEND_SDN     = "qtcontacts:symbiansim:store=SDN";

/*
 * Contact action definitions used in the view parameters to inform views
 * of what action took place. Views also use this defintions to tell what
 * action they performed on a contact
 */
#define CNT_EDIT_ACTION         "edit"
#define CNT_CREATE_ACTION       "create"
#define CNT_ADD_ACTION          "add"
#define CNT_FOCUS_ACTION        "focus"
#define CNT_SAVE_ACTION         "save"
#define CNT_FIND_ACTION         "find"
#define CNT_ROOT_ACTION         "root"

#endif // CNTGLOBAL_H
