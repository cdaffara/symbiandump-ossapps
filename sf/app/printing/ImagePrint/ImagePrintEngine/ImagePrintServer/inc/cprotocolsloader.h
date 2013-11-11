/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the CProtocolsLoader class definition.
*
*/


#ifndef CPROTOCOLSLOADER_H
#define CPROTOCOLSLOADER_H

#include <e32base.h>

class MProtPrintingDevice;

class CProtocolsLoader : public CBase
	{
	public:
	
		static CProtocolsLoader* NewL();
		~CProtocolsLoader();

		/**
		 *	Gets the number of protocol implementations that could be loaded.
		 *	@return Number of protocols available.
		 */
		TInt GetNumberOfProtocolsAvailable() const;
		
		/**
		 *	Gets the next protocol in the internal list of protocols.
		 *
		 *	Returns a pointer to the next protocol in the list. If the end of the list is reached, it starts the list over.
		 *	The order in which protocols will be returned is: DPOF, BPP, PictBridge, any other protocols (sorted alpha-
		 *	betically). This pointer should not be deleted. 
		 *	@return Pointer to the next protocol instance.
		 */
		MProtPrintingDevice* GetNextProtocol();
		
		/**
		 *	Gets the protocol located in the position specified. 
		 *	@param aIndex Index of the desired protocol. If there are 3 protocols, the valid indexes are: 0, 1, 2.
		 *	@return Pointer to the next protocol instance. This pointer should not be deleted. 
		 */
		MProtPrintingDevice* GetProtocolAt(TInt aIndex);
		
		/**
		 *	Resets the inner pointers to the beginning of the protocols' list.
		 */
		void Reset();
		
		/**
		 * Returns the index of the current protocol
		 */
		TInt GetProtocolIndex() const;
		
		/**
		 *	@brief Returns the Protocols supported by the DLL. It can be any of the KImagePrint_PrinterProtocol* constants.
		 *	@return An OR'ed value with the supported protocols information.
		 */
		TUint SupportedProtocols() const;

	private:
	
		CProtocolsLoader();
		void ConstructL();
		void LoadL();
		static void CleanupProt( TAny* aData );
	
	private: // data
	
		/// Used to store the number of protocols loaded and available for use.
		TInt iAvailableProtocols;
					
		/// Index of the next protocol available.
		TInt iProtIndex;

		/// Pointer array used to store the reference to the protocols.
		RPointerArray<MProtPrintingDevice> iProtocols;
				
		/// OR'ed value with the supported protocols information.
		TUint iSupportedProtocols;
	};

#endif // CPROTOCOLSLOADER_H

//  End of File
