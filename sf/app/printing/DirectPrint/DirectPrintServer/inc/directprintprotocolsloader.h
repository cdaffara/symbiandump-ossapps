/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  Contains the CDirectPrintProtocolsLoader class definition.
*
*/


// Protection against nested includes
#ifndef DIRECTPRINTPROTOCOLSLOADER_H
#define DIRECTPRINTPROTOCOLSLOADER_H

// System includes
#include <e32base.h>

// User includes
#include "directprintprotocolinfo.h"

// Forward declarations
class MProtPrintingDevice;

class CDirectPrintProtocolsLoader : public CBase
	{
	public:
	
		static CDirectPrintProtocolsLoader* NewL();
		~CDirectPrintProtocolsLoader();

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
		 *	@brief Returns the Protocols supported by the DLL. It can be any of the KDirectPrint_PrinterProtocol* constants.
		 *	@return An OR'ed value with the supported protocols information.
		 */
		TUint SupportedProtocols() const;

		/**
		 *	Gets the protocol name in the position specified. 
		 *	@param aIndex Index of the desired protocol. If there are 3 protocols, the valid indexes are: 0, 1, 2.
		 *	@param aName Name of the protocol.
		 *	@param aUid Uid of the protocol.
		 *	@return error code.
		 */
		TInt GetProtocolName(TInt aIndex, TDes& aName, TInt& aUid);

	private:
	
		CDirectPrintProtocolsLoader();
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

		/// Pointer array used to store the reference to the protocol infomations.
		RPointerArray<CDirectPrintProtocolInfo> iProtocolInfos;
	};

#endif // DIRECTPRINTPROTOCOLSLOADER_H

//  End of File
