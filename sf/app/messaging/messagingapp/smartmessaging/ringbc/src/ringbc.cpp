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
 *     A class which takes care of reading the possible new
 *     service centres from SIM and adds them to pda-side Sms Settings.
 *
 */

// INCLUDE FILES

#include <hbmessagebox.h>

#include "ringbc.h"
#include "ringbc_p.h"
#include "ringbctoneconverter.h"
#include "debugtraces.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// RingBc::RingBc
// @see ringbc.h
// ----------------------------------------------------------------------------
RingBc::RingBc()
    {
    QDEBUG_WRITE("RingBc::RingBc : Enter")
    d_ptr = q_check_ptr(new RingBcPrivate());
    QDEBUG_WRITE("RingBc::RingBc : Exit")
    }

// ----------------------------------------------------------------------------
// RingBc::RingBc
// @see ringbc.h
// ----------------------------------------------------------------------------
RingBc::~RingBc()
    {
    QDEBUG_WRITE("RingBc::~RingBc : Enter")
    delete d_ptr;	
    QDEBUG_WRITE("~RingBc::RingBc : Exit")
    }

// ----------------------------------------------------------------------------
// RingBc::saveTone
// @see ringbc.h
// ----------------------------------------------------------------------------
void RingBc::saveTone(const QString &path)
    {
    QDEBUG_WRITE("RingBc::saveTone : Enter")
    d_ptr->saveTone(path);
    QDEBUG_WRITE("RingBc::saveTone : Exit")
    }

// ----------------------------------------------------------------------------
// RingBc::toneTitle
// @see ringbc.h
// ----------------------------------------------------------------------------
QString RingBc::toneTitle(const QString &path)
    {
    QDEBUG_WRITE("RingBc::toneTitle : Enter")
    return d_ptr->toneTitle(path);
    }

//  End of File  
