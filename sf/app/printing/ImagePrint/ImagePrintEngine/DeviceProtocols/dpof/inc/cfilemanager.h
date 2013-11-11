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
* Description:  
*
*/


#ifndef CFILEMANAGER_H
#define CFILEMANAGER_H

#include <e32base.h>
#include <f32file.h>

class CFileManager : public CActive
	{
	public:

		static CFileManager* NewL( RFs& aFs );
		~CFileManager();	
		
	public:
	
		TInt Copy( const TDesC& aFile, const TDesC& aDir, TRequestStatus& aStatus );
		void CancelCopy();
		TInt RmDir( const TDesC& aDir, TRequestStatus& aStatus );
		void CancelRmDir();

	private:

		CFileManager( RFs& aFs );
		void ConstructL();
		
	protected: // from CActive
	
		void DoCancel();
		void RunL();
		TInt RunError( TInt aError );
		
	private:
	
		void CloseCopy();
		void InitCopyL( const TDesC& aFile, const TDesC& aDir );
		
	private:
		
		enum TOperation
			{
			EFileManCopy = 0,
			EBufferCopy,
			ERemoveDir
			};
				
	private: // data
	
		RFs& iFs;
		CFileMan* iFileMan;
		TOperation iOperation;
		TRequestStatus* iCallerStatus;		
		RFile iSource;
		RFile iDest;
		HBufC8* iBuffer;
		TInt iBytesLeft;
		TInt iBytesNum;		
	};


#endif // CFILEMANAGER_H

//  End of File
