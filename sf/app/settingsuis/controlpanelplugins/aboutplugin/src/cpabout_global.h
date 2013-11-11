/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CPABOUT_GLOBAL_H
#define CPABOUT_GLOBAL_H

#ifndef ABOUT_TEST
    #define ABOUT_TEST_FRIEND_CLASS(aClassName)
#else
    #define ABOUT_TEST_FRIEND_CLASS(aClassName) friend class aClassName;
#endif //ABOUT_TEST

#endif // CPABOUT_GLOBAL_H
