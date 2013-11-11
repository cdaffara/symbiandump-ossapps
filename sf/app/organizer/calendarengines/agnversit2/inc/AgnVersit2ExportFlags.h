#ifndef AGNVERSIT2EXPORTFLAGS_H
#define AGNVERSIT2EXPORTFLAGS_H/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Predefined values for exporting Agenda entries with various
*  iTip methods.
*
*/



// System includes.
#include <AgnExternalInterface.h> // TAgnEntryExport


enum TAgnExportMethod
	{
	EExportAll = 			0xFFFFFFFF,
	EVEventPublish =		KAgnExportDtStart		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer 	|
							KAgnExportSummary		|
							KAgnExportUID			|
							KAgnExportRecurrenceId	|
							KAgnExportSequence		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,
	
	EVEventRequest = 		KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportDtStart 		|
							KAgnExportOrganizer 	|
							KAgnExportSequence 		|
							KAgnExportSummary 		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,
					
	EVEventReply = 			KAgnExportAttendee 		|
							KAgnExportDtStamp		|	 
							KAgnExportOrganizer 	|
							KAgnExportUID 			|
							KAgnExportSequence		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportSummary		|
							KAgnExportXProp			|
							KAgnExportTzRules,
						
	EVEventAdd = 			KAgnExportDtStart 		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer 	|
							KAgnExportSequence 		|
							KAgnExportSummary 		|
							KAgnExportUID			|
							KAgnExportAttendee		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,	
	
	EVEventCancel = 		KAgnExportAttendee 		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer 	|
							KAgnExportSequence 		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportSummary		|
							KAgnExportXProp			|
							KAgnExportTzRules,
	
	EVEventRefresh = 		KAgnExportAttendee 		|
							KAgnExportDtStamp		|	
							KAgnExportOrganizer 	|
							KAgnExportUID			|
							KAgnExportRecurrenceId	|
							KAgnExportXProp,
						
	EVEventCounter = 		KAgnExportDtStart 		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer 	|
							KAgnExportSequence 		|
							KAgnExportSummary 		|
							KAgnExportUID			|
							KAgnExportAttendee		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,
						
	EVEventDeclineCounter = KAgnExportOrganizer 	|
							KAgnExportDtStamp		|	
							KAgnExportUID			|
							KAgnExportRecurrenceId	|
							KAgnExportSequence		|
							KAgnExportXProp,
							
	EVTodoPublish = 		KAgnExportDtStart		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer		|
							KAgnExportPriority		|
							KAgnExportSequence		|
							KAgnExportSummary		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,
	
	EVTodoRequest = 		KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportDtStart		|
							KAgnExportOrganizer		|
							KAgnExportPriority		|
							KAgnExportSequence		|
							KAgnExportSummary		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,
							
	EVTodoReply = 			KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRRule			|
							KAgnExportRecurrenceId	|
							KAgnExportSequence		|
							KAgnExportStatus		|
							KAgnExportSummary		|
							KAgnExportXProp			|
							KAgnExportTzRules,
							
	EVTodoAdd = 			KAgnExportOrganizer		|
							KAgnExportDtStamp		|
							KAgnExportPriority		|
							KAgnExportSequence		|
							KAgnExportSummary		|
							KAgnExportUID			|
							KAgnExportAttendee		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportRDate			|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,
							
	EVTodoCancel = 			KAgnExportAttendee		|
							KAgnExportUID			|
							KAgnExportOrganizer		|
							KAgnExportSequence		|
							KAgnExportDtStamp		|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportPriority		|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules,
							
	EVTodoRefresh = 		KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportUID			|
							KAgnExportRecurrenceId	|
							KAgnExportXProp,
							
	EVTodoCounter = 		KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer		|
							KAgnExportPriority		|
							KAgnExportSummary		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportSequence		|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules		|
							KAgnExportAlarm,
	
	EVTodoDeclineCounter = 	KAgnExportAttendee		|
							KAgnExportDtStamp		|
							KAgnExportOrganizer		|
							KAgnExportSequence		|
							KAgnExportUID			|
							KAgnExportCategory		|
							KAgnExportClass			|
							KAgnExportCreated		|
							KAgnExportDescription	|
							KAgnExportDtStart		|
							KAgnExportDtEnd			|
							KAgnExportExDate		|
							KAgnExportLastModified	|
							KAgnExportLocation		|
							KAgnExportCompleted		|
							KAgnExportPriority		|
							KAgnExportRDate			|
							KAgnExportRecurrenceId	|
							KAgnExportRRule			|
							KAgnExportStatus		|
							KAgnExportXProp			|
							KAgnExportTzRules
	};
	
#endif // AGNVERSIT2EXPORTFLAGS_H

// End of file.