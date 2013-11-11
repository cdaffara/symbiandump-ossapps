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
* Description:  
*
*/


#ifndef TMESSAGEWRP2_H
#define TMESSAGEWRP2_H

#include <e32base.h>

class TMessageWrp2
	{
	public:
	
		/**
		 *	Message wrapper constructor
		 *
		 *	@param aMessage message to be wrapped
		 *	@param aConnected session connection flag		 
		 */
		TMessageWrp2( const RMessage2& aMessage, const TBool& aConnected );
			
	public:
	
		/**
		 *	If session connected, write descriptor to the wrapped message
		 *
		 *	@param aParam message parameter id
		 *	@param aDes descriptor
		 *	@param aOffset offset from the start of the client's descriptor
		 */	 
		void WriteL(TInt aParam, const TDesC8 &aDes, TInt aOffset=0) const;
		
		/**
		 *	If session connected, complete the wrapped message
		 *
		 *	@param aReason completion code
		 */
		void Complete(TInt aReason) const;
		
		/**
		 *	If session connected, gets the 1st message argument as an integer value
		 *
		 *	@return 1st message argument as an integer value
		 */
		TInt Int0() const;
		
		/**
		 *	If session connected, gets the 2nd message argument as an integer value
		 *
		 *	@return 2nd message argument as an integer value
		 */					
		TInt Int1() const;
		
		/**
		 *	If session connected, gets the length of a descriptor argument 
		 *	in the client's process
		 *
		 *	@param aParam index value identifying the argument
		 *	@return the length of the descriptor or error code
		 */	
		TInt GetDesLength(TInt aParam) const;
		
		/**
		 *	If session connected, read data from the specified offset within 
		 *	the 8-bit descriptor argument, into the specified target descriptor
		 *
		 *	@param aParam message parameter id
		 *	@param aDes target descriptor
		 *	@param aOffset offset from the start of the client's descriptor
		 */	 
		void ReadL(TInt aParam, TDes8 &aDes, TInt aOffset=0) const;
		
		/**
		 *	If session connected, read data from the specified offset within 
		 *	the 16-bit descriptor argument, into the specified target descriptor
		 *
		 *	@param aParam message parameter id
		 *	@param aDes target descriptor
		 *	@param aOffset offset from the start of the client's descriptor
		 */	
		void ReadL(TInt aParam, TDes16 &aDes, TInt aOffset=0) const;
		
		/**
		 *	Panic the client thread
		 *
		 *	@param aCategory category descriptor
		 *	@param aReason reason id
		 */
		void Panic(const TDesC &aCategory, TInt aReason) const;
		
		/**
		 *	Gets the the number of the function requested by the client
		 *
		 *	@return the function number
		 */
		TInt Function() const;
		
		/**
		 *	Sets the disposable state. Disposable means that the message needs not
		 *	to be preserved
		 *
		 *	@param aDisposable disposable flag
		 */
		void SetDisposable( TBool aDisposable );
		
		/**
		 *	Gets the disposable state
		 *
		 *	@return the disposable flag
		 */
		TBool Disposable() const;

	private: // data
		RMessage2 iMessage;
		const TBool& iConnected;
		TBool iDisposable;
	};


#endif // TMESSAGEWRP2_H

//  End of File
