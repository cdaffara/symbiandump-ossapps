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
* Description:
*
*/

#ifndef DEBUG_TRACES_H_
#define  DEBUG_TRACES_H_

#ifdef _DEBUG
  #include <QtDebug>
  #define _DEBUG_TRACES_
  
  #define QDEBUG_WRITE(str) {qDebug() << str;}
  #define QDEBUG_WRITE_FORMAT(str, val) {qDebug() << str << val;}
  #define QCRITICAL_WRITE(str) {qCritical() << str;}
  #define QCRITICAL_WRITE_FORMAT(str, val) {qCritical() << str << val;}
#else
  #undef _DEBUG_TRACES_
  
  #define QDEBUG_WRITE(str)
  #define QDEBUG_WRITE_FORMAT(str, val)
  #define QCRITICAL_WRITE(str)
  #define QCRITICAL_WRITE_FORMAT(str, val)
#endif // _DEBUG
#endif // DEBUG_TRACES_H_