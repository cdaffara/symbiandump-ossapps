/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for Calendar application's common includes.
 *
*/


#ifndef CALENCOMMON_H
#define CALENCOMMON_H

#define CALEN_AGENDAVIEW_XML_FILE ":/xml/calenagendaview.docml"
#define CALEN_AGENDAVIEW "agendaView"
#define CALEN_AGENDAVIEW_WIDGET "agendaViewWidget"
#define CALEN_AGENDAVIEW_LISTWIDGET "eventsList"
#define CALEN_AGENDAVIEW_HEADING "headingWidget"
#define CALEN_AGENDAVIEW_HEADING_REGIONALPLUGIN_WIDGET "headingRegionalPluginWidget"
#define CALEN_AGENDAVIEW_MENU_NEW_EVENT "newEventAction"
#define CALEN_AGENDAVIEW_MENU_GO_TO_TODAY "todayAction"
#define CALEN_AGENDAVIEW_MENU_GO_TO_DATE "goToDateAction"
#define CALEN_AGENDAVIEW_MENU_SWITCH_TO_DAYVIEW "switchToDayViewAction"
#define CALEN_AGENDAVIEW_MENU_SETTINGS "settingsAction"
#define CALEN_PORTRAIT "portrait"
#define CALEN_LANDSCAPE "landscape"
#define CALEN_MONTHVIEW_XML_FILE ":/xml/calenmonthview.docml"
#define CALEN_MONTHVIEW "monthView"
#define CALEN_MONTHVIEW_EXCEPT_PANE "monthViewExceptPreviewPane"
#define CALEN_MONTH_TITLE "monthTitle"
#define CALEN_DAYNAMES_WIDGET "dayNamesWidget"
#define CALEN_MONTHGRID "monthGrid"
#define CALEN_PREVPANELAYOUT "prevPaneLayout"
#define CALEN_PREVPREVIEWPARENT "prevPaneWidget"
#define CALEN_CURRPREVIEWPARENT "currPaneWidget"
#define CALEN_NEXTPREVIEWPARENT "nextPaneWidget"
#define CALEN_PREVREGIONALINFO "prevRegionalInfo"
#define CALEN_CURRREGIONALINFO "currRegionalInfo"
#define CALEN_NEXTREGIONALINFO "nextRegionalInfo"
#define CALEN_NEXTPANELAYOUT "nextPaneLayout"
#define CALEN_CURRPANELAYOUT "currPaneLayout"
#define CALEN_PREVPREVIEWPANE "prevPreviewPane"
#define CALEN_CURRPREVIEWPANE "currPreviewPane"
#define CALEN_NEXTPREVIEWPANE "nextPreviewPane"
#define CALEN_NOENTRIES_LABEL_PREV "NoEntriesLabelPrev"
#define CALEN_NOENTRIES_LABEL_CURR "NoEntriesLabelCurr"
#define CALEN_NOENTRIES_LABEL_NEXT "NoEntriesLabelNext"
#define CALEN_PREVIEWPANE_TITLE "previewPaneTitle"
#define CALEN_PREVIEWPANE_LISTWIDGET "listWidget"
#define CALEN_WEEKNUMBERS_WIDGET "weekNumbersWidget"
#define CALEN_MONTHGRID_WEEKNUMBERS_WIDGET "monthGridWeekNumWidget"
#define CALEN_MONTVIEW_DELETE_SUBMENU "deleteSubMenu"
#define CALEN_MONTVIEW_MENU_NEWEVENT "NewEventAction"
#define CALEN_MONTVIEW_MENU_GOTOTODAY "goToTodayAction"
#define CALEN_MONTVIEW_MENU_GOTODATE "goToDateAction"
#define CALEN_MONTVIEW_MENU_BEFOREDATE "beforeDateAction"
#define CALEN_MONTVIEW_MENU_ALLENTRIES "allEntriesAction"
#define CALEN_MONTVIEW_MENU_SETTINGS "settingsAction"
#define CALEN_MONTVIEW_FIRST_DAY_LABEL "firstDayLabel"
#define CALEN_MONTVIEW_SECOND_DAY_LABEL "secondDayLabel"
#define CALEN_MONTVIEW_THIRD_DAY_LABEL "thirdDayLabel"
#define CALEN_MONTVIEW_FOURTH_DAY_LABEL "fourthDayLabel"
#define CALEN_MONTVIEW_FIFTH_DAY_LABEL "fifthDayLabel"
#define CALEN_MONTVIEW_SIXTH_DAY_LABEL "sixthDayLabel"
#define CALEN_MONTVIEW_SEVENTH_DAY_LABEL "seventhDayLabel"
#define CALEN_MONTVIEW_FIRST_WEEK_LABEL "firstWeekLabel"
#define CALEN_MONTVIEW_SECOND_WEEK_LABEL "secondWeekLabel"
#define CALEN_MONTVIEW_THIRD_WEEK_LABEL "thirdWeekLabel"
#define CALEN_MONTVIEW_FOURTH_WEEK_LABEL "fourthWeekLabel"
#define CALEN_MONTVIEW_FIFTH_WEEK_LABEL "fifthWeekLabel"
#define CALEN_MONTVIEW_SIXTH_WEEK_LABEL "sixthWeekLabel"

#define CALEN_DAYVIEW_DOCML ":/xml/calendayview.docml"
#define CALEN_DAYVIEW "dayView"
#define CALEN_DAYVIEW_DAYINFO "dayViewDayInfo"
#define CALEN_DAYVIEW_CONTENTWIDGET "dayViewContentWidget"
#define CALEN_DAYVIEW_HOURSCROLLAREA "dayViewHourScrollArea"
#define CALEN_DAYVIEW_CONTENTSCROLLAREA "dayViewContentScrollArea"

/*!
	\class CalendarNamespace

	This will have all the common enums and declarations needed by the calendar
	application.
 */
class CalendarNamespace {

public:
	/*!
		\enum DataRole
	 */
	enum DataRole {
		CalendarMonthDayRole ,
		CalendarMonthFocusRole,
		CalendarMonthUnderlineRole,
		CalendarMonthEventRole,
		CalendarMonthTextColorRole		
	};
	
	/*!
	   \enum widgetType
	 */
	enum WidgetType {
	    CalenDayNameWidget = 0,
	    CalenWeekNumWidget
	};
};
#endif // CALENCOMMON_H

// End of file	--Don't remove this.
