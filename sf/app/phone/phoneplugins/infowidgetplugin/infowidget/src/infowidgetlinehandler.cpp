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


#include "infowidgetlinehandler.h"
#include "infowidgetlogging.h"


/*! 
 */
InfoWidgetLineHandler::InfoWidgetLineHandler(QObject *parent):
    QObject(parent), 
    m_activeLine(1)
{
    DPRINT; 
}

/*!
 */
InfoWidgetLineHandler::~InfoWidgetLineHandler()
{
    DPRINT; 
}    

/*!
 */
int InfoWidgetLineHandler::activeLine() const
{
    DPRINT << ": active line: " << m_activeLine;
    return m_activeLine; 
}
    

// End of File. 

