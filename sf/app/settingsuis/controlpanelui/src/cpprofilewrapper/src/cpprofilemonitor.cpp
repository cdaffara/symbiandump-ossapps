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

#include "cpprofilemonitor.h"
#include "cpprofilemonitor_p.h"

/*!
     \class  CpProfileMonitor
     \brief This class will observe the profile change or modification, and emit related signals.
 */
/*!
     \fn  void profileActivated(int activeProfileId)
     This signal will emmit when a new profile activated
 */
/*!
     \fn      void activeProfileModified(int activeProfileId)
     This signal will emmit when active profile's settings are modified
 */
/*!
  Constructor
 */
CpProfileMonitor::CpProfileMonitor(QObject *parent)
    :QObject(parent),d_ptr(new CpProfileMonitorPrivate())
{
    d_ptr->initialize(this);
}

/*!
   Descontructor
 */
CpProfileMonitor::~CpProfileMonitor()
{    
    delete d_ptr;        
}
