/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of DM adapter test component
* 	This is part of omadmextensions/adapter test application.
*
*/






#ifndef __DMA_TEST_H__
#define __DMA_TEST_H__

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <apmstd.h>
#include <smldmadapter.h>

#include "NSmlDSSettingsAdapter.h"

class Cdmatest;


struct TMapping
	{
	TBuf8<256> iURI;
	TBuf8<64> iLuid;
	TMapping( const TDesC8 &aURI, const TDesC8 &aLuid )  : iURI( aURI ), iLuid( aLuid )
	{
		
		}
	};
	
typedef RArray<TMapping> RMappingArray;

typedef void (Cdmatest::* ResultsFunction)( TInt , CBufBase& , const TDesC8&  ) ; 


// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class Cdmatest : public CScriptBase, public MSmlDmCallback
    {
    
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~Cdmatest();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem ) = 0;
    

	public:
		/**
		The function is used to return the data in case of FetchLeafObjectL(),
		FetchLeafObjectSizeL() and ChildURIListL() functions. It should not be
		called where the DM command has failed, i.e. the error code returned in
		SetStatusL is something other than EOk.
		@param aResultsRef	Reference to correct command
		@param aObject		The data which should be returned
		@param aType			MIME type of the object
		@publishedPartner
		@prototype
		*/
		void SetResultsL( TInt aResultsRef, CBufBase& aObject,
								  const TDesC8& aType );
		
		/**
		The function is used to return the data in case of FetchLeafObjectL() and
		ChildURIListL() functions, where the size of the data being returned is
		large enough for the Adapter to stream it. This function should not be
		called when command was failed, i.e. the error code returned in SetStatusL
		is something other than EOk.
		@param aResultsRef	Reference to correct command
		@param aStream		Large data which should be returned, DM engine
								closes stream when it has read all the data
		@param aType			MIME type of the object
		@publishedPartner
		@prototype
		*/
		void SetResultsL( TInt /*aResultsRef*/, RReadStream*& /*aStream*/,
								  const TDesC8& /*aType*/ ) 
		{
			
		}

		/**
		The function returns information about the Add,Update,Delete and Fetch
		commands success to DM engine. The reference to correct command must be
		used when calling the SetStatusL function, the reference is got from the
		argument of the command functions. The SetStatusL function must be called
		separately for every single command.
		@param aStatusRef	Reference to correct command
		@param aErrorCode	Information about the command success
		@publishedPartner
		@prototype
		*/
		void SetStatusL( TInt aStatusRef,
								 MSmlDmAdapter::TError aErrorCode ) ;

		/**
		The function passes map information to DM Module. This function is called
		for a new management object, both for node objects and for leaf objects.
		In addition if ChildURIListL() function has returned new objects a mapping
		information of the new objects must be passed. A mapping is treated as
		inheritable. If the mapping is not set with this function, the mapping
		LUID of the parent object is passed in following commands to the object.
		@param aURI	URI of the object. 
		@param aLUID	LUID of the object. LUID must contain the all information,
						which is needed for retrieve the invidual object from the
						database. Typically it is ID for the database table. In
						more complicated structures it can be combination of IDs,
						which represent path to the object.
		@publishedPartner
		@prototype
		*/
		void SetMappingL( const TDesC8& aURI, const TDesC8& aLUID );

		/**
		The function is used to make a fetch to other adapters. The most common
		use is to make a fetch to the AP adapter, because when managing the access
		points, the data comes as URI. For example, there are ToNAPId field in
		some adapters, and data to it can be something like AP/IAPidx, and then
		the link to AP adapter is needed.
		Using FetchLinkL causes the DM Framework to make a Get request to the
		appropriate DM adapter.  The receiving adapter MUST complete the Get
		request synchronously.
		@param aURI		URI of the object. 
		@param aData		Reference to data, i.e. data is returned here
		@param aStatus	The status of fetch command is returned here
		@publishedPartner
		@prototype
		*/
		void FetchLinkL( const TDesC8& /*aURI*/, CBufBase& /*aData*/,
								 MSmlDmAdapter::TError& /*aStatus*/ ) 
			{
				
			}

		/**
		The function returns the LUID which is mapped to aURI. If LUID is not
		found, the function allocates a null length string, i.e. the function
		allocates memory in every case.
		@param aURI	URI of the object. 
		@publishedPartner
		@prototype
		*/
		HBufC8* GetLuidAllocL( const TDesC8& aURI ) ;

#ifdef __TARM_SYMBIAN_CONVERGENCY
    
    void GetMappingInfoListL( const TDesC8& aURI,
								CArrayFix<TSmlDmMappingInfo>& aSegmentList );

#else
// nothing
#endif
		        
    protected:  // New functions

        
		void FetchNodeResultsL( TInt aResultsRef, CBufBase& aObject,
							  const TDesC8& aType );
		void SaveDataL( TInt aResultsRef, CBufBase& aObject,
							  const TDesC8& aType ) ;
							  
		TPtrC8 LastURISeg( const TDesC8& aURI );
		TPtrC8 RemoveLastURISeg( const TDesC8& aURI );
		TPtrC RemoveLastURISeg( const TDesC& aURI );
		void SetURIL( const TDesC& aURI );
		void SetURIL( const TDesC8& aURI );
		void SetURIL( HBufC8* aURI );		
		HBufC8 *LoadFileLC( const TDesC &aFileName, TDataType &aType );
		HBufC8 *LoadFileLC( const TDesC8 &aFileName, TDataType &aType );
		HBufC8 *GetNextStringLC ( CStifItemParser& aItem, const TDesC &aName );
		HBufC8* GetLuidAllocLC( const TDesC8& aURI ) ;
		TPtrC8 RemoveLastSeg(const TDesC8& aURI);
		
		TInt FetchNodeL( CStifItemParser& aItem ) ;
		TInt FetchLeafL( CStifItemParser& aItem ) ;
		TInt AddNodeL( CStifItemParser& aItem );
		TInt DeleteObjectL( CStifItemParser& aItem );	
		TInt UpdateLeafL( CStifItemParser& aItem )	;
		TInt UpdateLeafDataL( CStifItemParser& aItem ) ;
		TInt UpdateLeafDataURLL( CStifItemParser& aItem ) ;
		
		TInt ExecuteLeafL ( CStifItemParser& aItem ) ;
		TInt ExecuteLeafDataL ( CStifItemParser& aItem ) ;
		TInt StartAtomicL( CStifItemParser& aItem )	;
		TInt CommitAtomicL( CStifItemParser& aItem )	;
		TInt RollbackAtomicL( CStifItemParser& aItem )	;
		TInt CompleteCommandsL( CStifItemParser& aItem );
		TInt DDFStructureL( CStifItemParser& aItem )	;
//        TInt DeliverL( CStifItemParser& aItem ) ;
//        TInt DetailsL( CStifItemParser& aItem ) ;
//		TInt InstallL( CStifItemParser& aItem ) ;
//		TInt BareInstallL( CStifItemParser& aItem ) ;		

    protected:

        /**
        * C++ default constructor.
        */
        Cdmatest( CTestModuleIf& aTestModuleIf, TUid aUid );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // Cdmatest( const Cdmatest& );
        // Prohibit assigment operator if not deriving from CBase.
        // Cdmatest& operator=( const Cdmatest& );

		/**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        virtual void Delete();
        void LoadMappingsL();
        void SaveMappingsL();
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
		MSmlDmAdapter::TError iStatus ;
        ResultsFunction iResultsFunction;
        //CSmlDmAdapter *iAdapter ;
        CNSmlDSSettingsAdapter *iAdapter ;
        HBufC8 *iURI; 
        CArrayFix<TSmlDmMappingInfo> *iEmptyMappingInfoArray;
		TFileName iSaveFileName;
		RMappingArray iMappingTable;
		TUid iUid;
		TInt iCounter;
		//class CNSmlDmMgmtTree* iMgmtTree;
		class CNSmlDmMgmtTree* iMgmtTree;
		CNSmlDSSettingsAdapter *Adapter() ;
    };

#endif      // __DMA_TEST_H__
            
// End of File
