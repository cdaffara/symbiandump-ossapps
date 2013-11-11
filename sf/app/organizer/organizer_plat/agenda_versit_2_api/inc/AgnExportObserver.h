#ifndef AGNEXPORTOBSERVER_H
#define AGNEXPORTOBSERVER_H/*
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
* Description:   Interface for reporting errors during the export of Agenda 
*  data to an external source.
*
*/



/**
Interface for reporting errors during the export of Agenda data to an
external source.
@publishedAll
*/
class MAgnExportObserver
	{
	public:		// Enumerations.
	
	// Possible responses to errors
	enum TExpResponse
		{
		/** Export as much as possible.*/
		EExpResponseContinue,
		/** Stop exporting and return.*/
		EExpResponseStop,
		/** Stop exporting and leave with KErrAbort.*/
		EExpResponseLeave
		};
		
	// Errors reported by MAgnExportObserver
	enum TExpError
		{
		/** No error to report.*/
		EExpErrorNoError,
		/** The data being exported has invalid values and cannot be handled.*/
		EExpErrorInvalidData,
		/** The data being exported is valid, but the data format this plugin
		uses cannot handle it.*/
		EExpErrorNotSupported
		};
			
	public:		// Interface.
		/**
		This function will be called by AgendaExternalInterface plugins when they
		encounter an error for which they feel the controlling application should
		determine the appropriate course of action.
		@param aType The type of the error
		@param aUid The UID of the entry in which the error occurred (if known)
		@param aContext A textual indication of the actual error
		@return a TExpResponse indicating the action to take
		@see CAgnExternalInterface::ExportL
		@publishedAll
		*/
		virtual TExpResponse AgnExportErrorL(TExpError aType, const TDesC8& aUid, const TDesC& aContext) = 0;
	};
	
#endif // AGNEXPORTOBSERVER_H

// End of file.