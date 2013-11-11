/*
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
* Description:  Utility classes.
*
*/


#ifndef NSMLDMSYNCUTIL_H
#define NSMLDMSYNCUTIL_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
* TUtil
* 
* This class contains general utility functions. 
*/
NONSHARABLE_CLASS (TUtil)
	{
    public:
    
        /**
        * Panic
        * @param aReason.
        * @return None
        */
        static void Panic( TInt aReason );
        
        /**
        * String copy tool
        * @param aTarget.
        * @param aSource.
        * @return None
        */        
		static void StrCopy( TDes& aTarget, const TDesC& aSource );
        /**
        * String copy tool
        * @param aTarget.
        * @param aSource.
        * @return None
        */      		
		static void StrCopy( TDes& aTarget, const TDesC8& aSource );
        
        /**
        * String copy tool
        * @param aTarget.
        * @param aSource.
        * @return None
        */      		
		static void StrCopy( TDes8& aTarget, const TDesC& aSource );
        
        /**
        * String append tool
        * @param aTarget.
        * @param aSource.
        * @return None
        */      		
		static void StrAppend( TDes& aTarget, const TDesC& aSource );
        
        /**
        * String insert tool
        * @param aTarget.
        * @param aSource.
        * @return None
        */      		
		static void StrInsert( TDes& aTarget, const TDesC& aSource );
        
        /**
        * Convert string to int
        * @param aText.
        * @param aNum.
        * @return None
        */		
		static TInt StrToInt( const TDesC& aText, TInt& aNum );
        
        /**
        * Check whether textline is empty
        * @param aText.
        * @return ETrue if empty
        */				
		static TBool IsEmpty( const TDesC& aText );
        
        /**
        * URI Parser
        * @param aURI
        * @param aAddress
        * @param aPort
        * @param aDocName
        * @return None
        */						
        static void ParseURIL( const TDesC& aURI, 
                               TDes& aAddress, 
                               TDes& aPort, 
                               TDes& aDocName );
      
        /**
        * Returns formatted sync time
        * @param aLastSync
        * @return Sync time
        */
        static HBufC* SyncTimeLC( TTime aLastSync );
        
        /**
        * Get data as text
        * @param 
        * @return 
        */        
        static void GetDateTextL( TDes& aText, TTime aDateTime );
        
        /**
        * Get time as text 
        * @param 
        * @return 
        */        
		static void GetTimeTextL( TDes& aText, TTime aDateTime );
        
        /**
        * Get data and time as text
        * @param 
        * @return 
        */        
		static void GetDateTimeTextL( TDes& aText, TTime aDateTime );
        
        /**
        * Check if SettingEnforcement is set
        * @param 
        * @return Boolean
        */        
		static TBool SettingEnforcementStateL();

        /**
        * Check if SettingEnforcement is set (non-leaving)
        * @param 
        * @return Boolean
        */        
		static TBool SettingEnforcementState();

    private:
    
        /**
        * Check if sync has happened today
        * @param atime
        * @return Boolean
        */    
        static TBool IsToday( TTime aTime );
        
        /**
        * Convert time to hometime
        * @param aUniversalTime
        * @return TTIme
        */            
        static TTime ConvertUniversalToHomeTime( const TTime& aUniversalTime );

	};

/**
*  TStr contains descriptor operations with range checking.
*/
NONSHARABLE_CLASS (TStr)
    {
    public:
        /**
        * C++ default constructor.
        */
        TStr(const TDesC& aStr);
       
    public:
        /**
        * String manipulating tools
        * @param aPos
        * @param aLen
        * @return TPtrC
        */        
        TPtrC Mid( TInt aPos, TInt aLen );
        
        /**
        * String manipulating tools
        * @param aPos
        * @return TPtrC
        */                
        TPtrC Mid( TInt aPos );

        /**
        * String manipulating tools
        * @param aPos
        * @return TPtrC
        */                
        TPtrC Left( TInt aPos );
        
        /**
        * String manipulating tools
        * @param aPos
        * @return TPtrC
        */               
        TPtrC Right( TInt aPos );
        
        /**
        * String manipulating tools
        * @param aStartPos
        * @param aEndPos
        * @return TPtrC
        */               
        TPtrC SubStr( TInt aStartPos, TInt aEndPos );

        /**
        * String manipulating tools
        * @param aStartPos
        * @param aEndPos
        * @return TPtrC
        */           
        TPtrC SubStrEx( TInt aStartPos, TInt aEndPos );

        /**
        * String manipulating tools
        * @return TInt
        */           
        TInt LastPos();

        /**
        * String manipulating tools
        * @return TInt
        */           
        TInt Len();

        /**
        * String manipulating tools
        * @param aPos
        * @param aChar
        * @return TBool
        */           
        TBool Compare( TInt aPos, TChar aChar );

        /**
        * String manipulating tools
        * @param aStartPos
        * @param aChar
        * @return TInt
        */
        TInt Find( TInt aPos, TChar aChar );
        
    private:
        // reference to descriptor
		const TDesC& iStr;
    };

/**
*  TURIParser is used for parsing URI.
*/
NONSHARABLE_CLASS (TURIParser)
    {
    public:
        /**
        * C++ default constructor.
        */
        TURIParser( const TDesC& aStr );
       
    public:
        /**
        * URI manipulating tools
        * @param aText
        * @return None
        */    
        void GetUriWithoutPort( TDes& aText );

        /**
        * URI manipulating tools
        * @param aText
        * @param aPort
        * @return None
        */            
        void GetUri( TDes& aText, const TInt aPort );

        /**
        * URI manipulating tools
        * @return TInt
        */            
        TInt Port();

        /**
        * URI manipulating tools
        * @return TInt
        */            
        TInt DefaultPort();

    private:
        /**
        * URI manipulating tools
        * @return TPtrC
        */          
        TPtrC ParseHttp();

        /**
        * URI manipulating tools
        * @return TPtrC
        */              
        TPtrC ParseAddress();

        /**
        * URI manipulating tools
        * @return TPtrC
        */              
        TPtrC ParseDocument();

        /**
        * URI manipulating tools
        * @param aPortNumber
        * @return TInt
        */              
        TInt ParsePort(TInt& aPortNumber);

        /**
        * URI manipulating tools
        * @return TPtrC
        */              
        TPtrC ParsePort();
        
     private:

        /**
        * URI manipulating tools
        * @param aStartPos
        * @param aEndPos
        * @return None
        */          
        void GetHttpPos(TInt& aStartPos, TInt& aEndPos);

        /**
        * URI manipulating tools
        * @return NOne
        */             
        void SkipHttp();
  
    private:
        // reference to original URI text
		const TDesC& iOriginalStr;
		
		// pointer to original URI text
		TPtrC iPtr;
		
		//reference to Boolean variable which tells about Slash in URI
		TBool iSlash;
    };

/**
* MNSmlDMActiveCallerObserver
*
* MNSmlDMActiveCallerObserver is for observing CNSmlDMActiveCaller.
*/
class MNSmlDMActiveCallerObserver
    {
    public:
	    virtual void HandleActiveCallL() = 0;
    };

/**
* CNSmlDMActiveCaller
*
* CNSmlDMActiveCaller is used for generating a call from active scheduler.
*/
NONSHARABLE_CLASS (CNSmlDMActiveCaller) : public CActive
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CNSmlDMActiveCaller* NewL(MNSmlDMActiveCallerObserver* aObserver);

	    /**
        * Destructor.
        */
        virtual ~CNSmlDMActiveCaller();

    private:
        /**
        * C++ default constructor.
        */
	    CNSmlDMActiveCaller(MNSmlDMActiveCallerObserver* aObserver);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

	public:
        /**
        * Calls observer using active object framework.
        * @param aCallId.
		* @param aMilliseconds Delay before observer is called.
        * @return None
        */
		void Start( TInt aCallId, TInt aMilliseconds = 0 );

    private:
        /**
        * From CActive. Called when asynchronous request completes.
        * @param None
        * @return None
        */
	    void RunL();
       
        /**
        * From CActive. Called when RunL Leaves.
        * @param None
        * @return error
        */
        TInt RunError( TInt aError );

        /**
        * From CActive. Cancels asynchronous request.
        * @param None
        * @return None
        */
	    void DoCancel();
        
        /**
        * Make active scheduler call RunL.
        * @param None
        * @return None
        */
		void CompleteSelf();

    private:
		// call that is passed to observer
		TInt iCallId;

		// timer
		RTimer iTimer;

		// observer that gets called
		MNSmlDMActiveCallerObserver* iObserver;
    };

#endif  // NSMLDMSYNCUTIL_H

// End of File
