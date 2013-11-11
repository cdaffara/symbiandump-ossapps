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
 * Description:This class provides duration of media file by parsing it
 *
 */

#ifndef MSG_MEDIA_UTIL_H
#define MSG_MEDIA_UTIL_H

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
 * This class provides duration of media file by parsing it
 * 
 */
class MSGUI_UTILS_DLL_EXPORT MsgMediaUtil
    {
    
public:
    /**
     * Constructor
     */
    MsgMediaUtil();
    
    /**
     * Destructor
     */
    ~MsgMediaUtil();
    
    /**
     * Get the media file duration
     * @param mediaFile file needs to be parsed to get duration
     * @return QString returns duration in string format(ex: "05:14")
     */
    QString mediaDuration(const QString& mediaFile);
    
private: //internal methods
    
    /*
     * internal method traps all leaving functions
     * @param mediaFile file needs to be parsed to get duration 
     * @return media duration in miliseconds
     */
    TInt mediaDurationL(const TDesC& mediaFile);
    
#ifdef MSGUIUTILS_UNIT_TEST
    /**
     * Unit Testing
     */
    friend class TestMsgMediaUtil;
#endif
    };
    
#endif // MSG_MEDIA_UTIL_H

// EOF

