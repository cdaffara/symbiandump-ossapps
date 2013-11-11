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

#ifndef HBSTUB_HELPER_H
#define HBSTUB_HELPER_H

class HbListViewItem;

/**
* Helper class to control logsdbconnector stub behavior
*
*/ 
class HbStubHelper 
{
    public:
        static void reset();
        static int widgetActionsCount();
        static bool menuShown();
        static bool dialogShown();
        static bool singleShotTimerActive();
        static bool quitCalled();
        static void setGestureState(int state);
        static void setWidgetOpen(bool isOpen);
        static bool isWidgetOpen();    
        static bool isWidgetRaised();
        static void setColorScheme(QColor col);
        static void setActivityReason(Hb::ActivationReason reason);
        static void setActivityId(QString activityId);
        static QList<HbListViewItem*>& listItems();
        static bool listScrollToCalled();
        static bool listEnsureVisibleCalled();
        static int menuShownPlacement();
        static void stringUtilDigitConversion(bool enabled);
        static bool listScrollBarPolicySet();
        static void resetListScrolling();
        static void createWindowSurface();
        static bool isTsTaskVisibilitySet();
        static bool tsTaskVisibility();
};

#endif
