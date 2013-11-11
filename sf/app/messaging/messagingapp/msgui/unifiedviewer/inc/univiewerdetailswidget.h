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
 * Description: This widget displays subject,timestamp & priority info
 *
 */

#ifndef UNIVIEWERDETAILSWIDGET_H
#define UNIVIEWERDETAILSWIDGET_H

#include <hbwidget.h>

class HbTextItem;
class HbIconItem;

/*
 *This widget displays subject,timestamp & priority info
 */
class UniViewerDetailsWidget: public HbWidget
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit UniViewerDetailsWidget(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~UniViewerDetailsWidget();

    /**
     * Sets the subject inside widget
     * @param subject
     */
    void setSubject(const QString &subject);

    /**
     * Sets the time stamp inside widget
     * @param aTimeStamp
     */
    void setTimeStamp(const QDateTime &aTimeStamp, const int &aSendingState);

    /**
     * Sets the priority inside widget
     * @param priority
     */
    void setPriorityIcon(int priority);

    /**
     * Clears the content on the widget
     */
    void clearContent();

private:

    /**
     * HbTextItem object
     * Owned
     */
    HbTextItem *mSubjectLabel;

    /**
     * HbIconItem object
     * Owned
     */
    HbIconItem* mPriorityIcon;

    /*
     * HbTextItem object
     * Owned
     */
    HbTextItem* mTime;
};

#endif // UNIVIEWERDETAILSWIDGET_H
// EOF
