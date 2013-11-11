/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Contains the types necesary for MIdleObserver TPrinter class definition.
*
*/


#ifndef IDLEEVENT_H
#define IDLEEVENT_H

/** The severity associated of the notification.
 */
enum TSeverity
	{
		///The event doesn't have a specific severity. Used in cases where severity is irrelevant, unknown or can be ignored.
		ENotSpecified,
		///The event does not cause an error
		ENoError,
		///The event was caused by a warning. Generally this is recoverable.
		EWarning,
		///The event was caused by a fatal error. Generally this is not recoverable
		EFatal,
		///The event was caused by a severe error. Generally this is a async leave
		ESevere
	};

/** The type of event the observer is being notified of. The types of events being raised vary by protocol
 */
enum TEventType
	{
		///The protocol has made a connection to the printer, device or transport
		EConnect,
		///The protocol has been disconnected from the printer, device or transport
		EDisconnect,
		///The protocol can be disconnected
		EDisconnectReady,
		///The protocol cannot be disconnected
		EDisconnectNotReady,
		///The device or printer is ready to accept print jobs
		EPrintReady,
		///The device or printer is not ready to accept print jobs
		EPrintNotReady,
		///The event is notifying a change in the device status, or is reporting an error sent from the device
		EPrinterStatus,
		///The event is notifying that the capabilities that the printer supports have changed
		EPrinterCapabilityChanged,
		///The event is notifying that asynchronous leave has happened
		EAsynchronousLeave
	};

/** The event object. This object provides information about the event.
 */
class TEvent
	{
	public:
		/** The protocol that is sending the notification
		 *	@sa KImagePrint_PrinterProtocol_BPP @sa KImagePrint_PrinterProtocol_BIP @sa KImagePrint_PrinterProtocol_OPP 
		 *	@sa KImagePrint_PrinterProtocol_DPOF @sa KImagePrint_PrinterProtocol_PictBridge
		 */
		TInt iProtocol;
		///The associated severity of the event
		TSeverity iSeverity;
		///What kind of event is being sent
		TEventType iEventType;
	};


#endif	// IDLEEVENT_H

//  End of File
