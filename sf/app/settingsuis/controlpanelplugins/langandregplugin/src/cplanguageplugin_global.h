/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#ifndef CPLANGUAGEPLUGIN_GLOBAL_H
#define CPLANGUAGEPLUGIN_GLOBAL_H

#ifndef LANGUAGEPLUGIN_TEST
    #define LANGUAGEPLUGIN_TEST_FRIEND_CLASS(aClassName)
#else
    #define LANGUAGEPLUGIN_TEST_FRIEND_CLASS(aClassName) friend class aClassName;
#endif //LANGUAGEPLUGIN_TEST

#endif // CPLANGUAGEPLUGIN_GLOBAL_H
