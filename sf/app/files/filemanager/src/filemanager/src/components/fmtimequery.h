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
 * 
 * Description:
 *     The header file of the time query of file manager
 */

#ifndef FMTIMEQUERY_H
#define FMTIMEQUERY_H

#include "fmcommon.h"
#include "fmdialog.h"

#include <QTime>

class QGraphicsWidget;
class HbDateTimePicker;
class FmTimeQuery : public FmDialog
{
public:
    FmTimeQuery(QGraphicsItem *parent=0);
	~FmTimeQuery( void );

    /**
     * Set time to query
     * @param time time whant to display when query dialog shown
     */
    void setTime( const QTime &time );

    /**
     * Get the time that set by user
     * @return time
     */
    QTime getTime();

private:
    QGraphicsWidget *mContentWidget;
    HbDateTimePicker *mTimeWidget;

};

#endif //FMTIMEQUERY_H
