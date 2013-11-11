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
* Description:  CalenDayUtils utility class header.
*
*/

#ifndef CALENDAYUTILS_H_
#define CALENDAYUTILS_H_

// Forward declarations
class HbMainWindow;
class QDateTime;
class AgendaEntry;

class CalenDayUtils
{
public:
    static CalenDayUtils *instance();
    ~CalenDayUtils();
    
    qreal screenWidth() const;
    qreal hourElementWidth() const;
    qreal hourElementHeight() const;
    qreal minEventHeight() const;
    qreal contentWidth() const;
    
    Qt::Orientation orientation() const;
    
    HbMainWindow* mainWindow();
    
    void getEventValidStartEndTime(
        QDateTime& start,
        QDateTime& end,
        const AgendaEntry& entry,
        QDateTime& currentDate);
    
protected:
    CalenDayUtils();
    
private:
    qreal calculateHourElementWidth() const;
    qreal calculateHourElementHeight() const;
    qreal calculateMinEventHeight() const;
    
private:    // data
    
    static CalenDayUtils* mInstance;
    
    HbMainWindow* mMainWindow;  //!< Pointer to main window
    
    qreal mHourElementWidth;    //!< The width of hour element
    qreal mHourElementHeight;   //!< The height of hour element
    qreal mMinEventHeight;   //!< The minimum height of event bubble
};

#endif /* CALENDAYUTILS_H_ */
