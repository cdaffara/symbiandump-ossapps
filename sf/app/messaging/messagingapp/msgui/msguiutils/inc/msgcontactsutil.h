/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: This static utility class handles all contacts & phbk
 * services related common functions for UI modules.
 *
 */

#ifndef MSGCONTACTSUTIL_H_
#define MSGCONTACTSUTIL_H_

#ifdef MSGUIUTILS_UNIT_TEST
 #define MSGUI_UTILS_DLL_EXPORT
#else
#ifdef BUILD_MSGUI_UTILS_DLL
 #define MSGUI_UTILS_DLL_EXPORT Q_DECL_EXPORT
#else
 #define MSGUI_UTILS_DLL_EXPORT Q_DECL_IMPORT
#endif
#endif

/**
 * static utility class to handle contacts related common functionality
 * for UI modules
 */
class MSGUI_UTILS_DLL_EXPORT MsgContactsUtil
{

public:
    static bool launchVCardViewer(const QString& filepath);

private:
    static QString copyVCardToTemp(const QString& filepath);
    static void deleteVCardFromTemp(const QString& filepath);
    
#ifdef MSGUIUTILS_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgContactsUtil;
#endif
};

#endif /* MSGCONTACTSUTIL_H_ */
