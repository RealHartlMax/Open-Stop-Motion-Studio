//================================================================================================
// Copyright Nikon Corporation - All rights reserved
//
// View this file in a non-proportional font, tabs = 3
//================================================================================================

#if defined( _WIN32 )
	#include <io.h>
	#include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/stat.h>

#include "Maid3.h"
#include "Maid3d1.h"
#include "CtrlSample.h"

extern ULONG	g_ulCameraType;	// CameraType
#define VIDEO_SIZE_BLOCK  0x500000		// movie data read block size : 5MB

#define ObjectBitmapHandle_Format_MOV	11	//MOV
#define ObjectBitmapHandle_Format_MP4	12	//MP4

BOOL g_bCancel = FALSE;

#if defined( _WIN32 )
	BOOL WINAPI	cancelhandler(DWORD dwCtrlType);
#elif defined(__APPLE__)
	void	cancelhandler(int sig);
#endif

//------------------------------------------------------------------------------------------------
//
SLONG CallMAIDEntryPoint( 
		LPNkMAIDObject	pObject,				// module, source, item, or data object
		ULONG				ulCommand,			// Command, one of eNkMAIDCommand
		ULONG				ulParam,				// parameter for the command
		ULONG				ulDataType,			// Data type, one of eNkMAIDDataType
		NKPARAM			data,					// Pointer or long integer
		LPNKFUNC			pfnComplete,		// Completion function, may be NULL
		NKREF				refComplete )		// Value passed to pfnComplete
{
	return (*(LPMAIDEntryPointProc)g_pMAIDEntryPoint)( 
						pObject, ulCommand, ulParam, ulDataType, data, pfnComplete, refComplete );
}
//------------------------------------------------------------------------------------------------
//
BOOL Search_Module( void* Path )
{
#if defined( _WIN32 )
	char	TempPath[MAX_PATH];
	struct	_finddata_t c_file;
	intptr_t	hFile;

	// Search a module file in the current directory.
	GetCurrentDirectory( MAX_PATH - 11, TempPath );
	strcat( TempPath, "\\Type0029.md3" );
	if ( (hFile = _findfirst( TempPath, &c_file )) == -1L ) {
		return FALSE;
	}
	strcpy( (char*)Path, TempPath );
	return TRUE;
#elif defined(__APPLE__)
    CFStringRef modulePath = NULL;
    
    CFStringRef moduleName = CFSTR( "Type0029 Module.bundle" );
    CFBundleRef bundle = CFBundleGetMainBundle();
    if(bundle != NULL)
    {
        CFURLRef executableURL = CFBundleCopyExecutableURL( bundle );
        if(executableURL != NULL)
        {
            CFURLRef parentFolderURL = CFURLCreateCopyDeletingLastPathComponent( kCFAllocatorDefault, executableURL );
            if(parentFolderURL != NULL)
            {
                CFURLRef moduleURL = CFURLCreateCopyAppendingPathComponent( kCFAllocatorDefault, parentFolderURL, moduleName, FALSE );
                if(moduleURL != NULL)
                {
                    if(CFURLResourceIsReachable( moduleURL, NULL ))
                    {
                        modulePath = CFURLCopyFileSystemPath( moduleURL, kCFURLPOSIXPathStyle );
                    }
                    CFRelease( moduleURL );
                }
                CFRelease( parentFolderURL );
            }
            CFRelease( executableURL );
        }
    }
    
    Boolean result = FALSE;
    if(modulePath != NULL)
    {
        result = CFStringGetCString( modulePath, (char *)Path, PATH_MAX, kCFStringEncodingUTF8 );
        CFRelease( modulePath );
    }
    return result;
#endif
}
//------------------------------------------------------------------------------------------------
//
BOOL Load_Module( void* Path )
{
#if defined( _WIN32 )
	g_hInstModule = LoadLibrary( (LPCSTR)Path );

	if (g_hInstModule) {
		g_pMAIDEntryPoint = (LPMAIDEntryPointProc)GetProcAddress( g_hInstModule, "MAIDEntryPoint" );
		if ( g_pMAIDEntryPoint == NULL )
			puts( "MAIDEntryPoint cannot be found.\n" ); 
	} else {
		g_pMAIDEntryPoint = NULL;
		printf( "\"%s\" cannot be opened.\n", (LPCSTR)Path );
	}
	return (g_hInstModule != NULL) && (g_pMAIDEntryPoint != NULL);
#elif defined(__APPLE__)
    CFStringRef modulePath = CFStringCreateWithCString( kCFAllocatorDefault, (const char *)Path, kCFStringEncodingUTF8 );
    if(modulePath != NULL)
    {
        CFURLRef moduleURL = CFURLCreateWithFileSystemPath( kCFAllocatorDefault, modulePath, kCFURLPOSIXPathStyle, TRUE );
        if(moduleURL != NULL)
        {
            if(gBundle != NULL)
            {
                CFRelease( gBundle );
                gBundle = NULL;
            }
            gBundle = CFBundleCreate( kCFAllocatorDefault, moduleURL );
            CFRelease( moduleURL );
        }
        CFRelease( modulePath );
    }
    
    if(gBundle == NULL)
    {
        return FALSE;
    }
    
	// Load and link dynamic CFBundle object
	if(!CFBundleLoadExecutable(gBundle))
    {
		CFRelease( gBundle );
		gBundle = NULL;
		return FALSE;
	}
    
	// Get entry point from BundleRef
	// Set the pointer for Maid entry point LPMAIDEntryPointProc type variabl
	g_pMAIDEntryPoint = (LPMAIDEntryPointProc)CFBundleGetFunctionPointerForName( gBundle, CFSTR("MAIDEntryPoint") );
	return (g_pMAIDEntryPoint != NULL);
#endif
}
//------------------------------------------------------------------------------------------------
//
BOOL Command_Open(LPNkMAIDObject pParentObj, LPNkMAIDObject pChildObj, ULONG ulChildID )
{
	SLONG lResult = CallMAIDEntryPoint( pParentObj, kNkMAIDCommand_Open, ulChildID, 
									kNkMAIDDataType_ObjectPtr, (NKPARAM)pChildObj, NULL, NULL );
	return lResult == kNkMAIDResult_NoError;
}
//------------------------------------------------------------------------------------------------
//
BOOL Command_Close( LPNkMAIDObject pObject )
{
	SLONG nResult = CallMAIDEntryPoint( pObject, kNkMAIDCommand_Close, 0, 0, 0, NULL, NULL );

	return nResult == kNkMAIDResult_NoError;
}
//------------------------------------------------------------------------------------------------
//
BOOL Close_Module( LPRefObj pRefMod )
{
	BOOL bRet;
	LPRefObj pRefSrc, pRefItm, pRefDat;
	ULONG i, j, k;

	if(pRefMod->pObject != NULL)
	{
		for(i = 0; i < pRefMod->ulChildCount; i ++)
		{
			pRefSrc = GetRefChildPtr_Index( pRefMod, i );
			for(j = 0; j < pRefSrc->ulChildCount; j ++)
			{
				pRefItm = GetRefChildPtr_Index( pRefSrc, j );
				for(k = 0; k < pRefItm->ulChildCount; k ++)
				{
					pRefDat = GetRefChildPtr_Index( pRefItm, k );
					bRet = ResetProc( pRefDat );
					if ( bRet == FALSE )	return FALSE;
					bRet = Command_Close( pRefDat->pObject );
					if ( bRet == FALSE )	return FALSE;
					free(pRefDat->pObject);
					free(pRefDat->pCapArray);
					free(pRefDat);//
					pRefDat = NULL;//
				}
				bRet = ResetProc( pRefItm );
				if ( bRet == FALSE )	return FALSE;
				bRet = Command_Close( pRefItm->pObject );
				if ( bRet == FALSE )	return FALSE;
				free(pRefItm->pObject);
				free(pRefItm->pRefChildArray);
				free(pRefItm->pCapArray);
				free(pRefItm);//
				pRefItm = NULL;//
			}
			bRet = ResetProc( pRefSrc );
			if ( bRet == FALSE )	return FALSE;
			bRet = Command_Close( pRefSrc->pObject );
			if ( bRet == FALSE )	return FALSE;
			free(pRefSrc->pObject);
			free(pRefSrc->pRefChildArray);
			free(pRefSrc->pCapArray);
			free(pRefSrc);//
			pRefSrc = NULL;//
		}
		bRet = ResetProc( pRefMod );
		if ( bRet == FALSE )	return FALSE;
		bRet = Command_Close( pRefMod->pObject );
		if ( bRet == FALSE )	return FALSE;
		free(pRefMod->pObject);
		pRefMod->pObject = NULL;

		free(pRefMod->pRefChildArray);
		pRefMod->pRefChildArray = NULL;
		pRefMod->ulChildCount = 0;
	
		free(pRefMod->pCapArray);
		pRefMod->pCapArray = NULL;
		pRefMod->ulCapCount = 0;
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------
//
void InitRefObj( LPRefObj pRef )
{
	pRef->pObject = NULL;
	pRef->lMyID = 0x8000;
	pRef->pRefParent = NULL;
	pRef->ulChildCount = 0;
	pRef->pRefChildArray = NULL;
	pRef->ulCapCount = 0;
	pRef->pCapArray = NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------
// issue async command while wait for the CompletionProc called.
BOOL IdleLoop( LPNkMAIDObject pObject, ULONG* pulCount, ULONG ulEndCount )
{
	BOOL bRet = TRUE;
	while( *pulCount < ulEndCount && bRet == TRUE ) {
		bRet = Command_Async( pObject );
	#if defined( _WIN32 )
		Sleep(10);
	#elif defined(__APPLE__)
		struct timespec t;
        t.tv_sec = 0;
        t.tv_nsec = 10 * 1000;// 10 msec == 10 * 1000 nsec
        nanosleep(&t, NULL);
	#endif
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Wait for Apple event. On MacOSX, the event from camera is an Apple event. 
void WaitEvent()
{
	#if defined( _WIN32 )
		// Do nothing
	#elif defined(__APPLE__)
		// Do nothing
	#endif
}
//------------------------------------------------------------------------------------------------------------------------------------
// enumerate capabilities belong to the object that 'pObject' points to.
BOOL EnumCapabilities( LPNkMAIDObject pObject, ULONG* pulCapCount, LPNkMAIDCapInfo* ppCapArray, LPNKFUNC pfnComplete, NKREF refComplete )
{
	SLONG nResult;

	do {
 		// call the module to get the number of the capabilities.
		ULONG	ulCount = 0L;
		LPRefCompletionProc pRefCompletion;
		// This memory block is freed in the CompletionProc.
		pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );
		pRefCompletion->pulCount = &ulCount;
		pRefCompletion->pRef = NULL;
		nResult = CallMAIDEntryPoint(	pObject,
												kNkMAIDCommand_GetCapCount,
												0,
												kNkMAIDDataType_UnsignedPtr,
												(NKPARAM)pulCapCount,
												(LPNKFUNC)CompletionProc,
												(NKREF)pRefCompletion );
		IdleLoop( pObject, &ulCount, 1 );
 
 		if ( nResult == kNkMAIDResult_NoError )
 		{
 			// allocate memory for the capability array
 			*ppCapArray = (LPNkMAIDCapInfo)malloc( *pulCapCount * sizeof( NkMAIDCapInfo ) );
  
 			if ( *ppCapArray != NULL )
 			{
 				// call the module to get the capability array
   				ulCount = 0L;
				// This memory block is freed in the CompletionProc.
				pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );
				pRefCompletion->pulCount = &ulCount;
				pRefCompletion->pRef = NULL;
				nResult = CallMAIDEntryPoint(	pObject,
														kNkMAIDCommand_GetCapInfo,
														*pulCapCount,
														kNkMAIDDataType_CapInfoPtr,
														(NKPARAM)*ppCapArray,
														(LPNKFUNC)CompletionProc,
														(NKREF)pRefCompletion );
				IdleLoop( pObject, &ulCount, 1 );

 				if (nResult == kNkMAIDResult_BufferSize)
 				{
					free( *ppCapArray );
					*ppCapArray = NULL;
				}
			}
		}
	}
	// repeat the process if the number of capabilites changed between the two calls to the module
	while (nResult == kNkMAIDResult_BufferSize);

	// return TRUE if the capabilities were successfully enumerated
	return (nResult == kNkMAIDResult_NoError || nResult == kNkMAIDResult_Pending);
}
//------------------------------------------------------------------------------------------------------------------------------------
// enumerate child object
BOOL EnumChildrten(LPNkMAIDObject pobject)
{
	SLONG nResult;
	ULONG	ulCount = 0L;
	LPRefCompletionProc pRefCompletion;
	pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = NULL;
	nResult = CallMAIDEntryPoint(	pobject,
											kNkMAIDCommand_EnumChildren, 
											0,
											kNkMAIDDataType_Null,
											(NKPARAM)NULL,
											(LPNKFUNC)CompletionProc,
											(NKREF)pRefCompletion );
	IdleLoop( pobject, &ulCount, 1 );

	return ( nResult == kNkMAIDResult_NoError );
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL Command_CapGetArray(LPNkMAIDObject pobject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete )
{
	SLONG nResult;
	ULONG	ulCount = 0L;
	LPRefCompletionProc pRefCompletion;
	pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = NULL;
	nResult = CallMAIDEntryPoint(	pobject,
											kNkMAIDCommand_CapGetArray,
											ulParam,
											ulDataType,
											pData,
											(LPNKFUNC)CompletionProc,
											(NKREF)pRefCompletion );
	IdleLoop( pobject, &ulCount, 1 );

	return (nResult == kNkMAIDResult_NoError);
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL Command_CapGetDefault(LPNkMAIDObject pobject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete )
{
	SLONG nResult;
	ULONG	ulCount = 0L;
	LPRefCompletionProc pRefCompletion;
	pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = NULL;
	nResult = CallMAIDEntryPoint(	pobject,
											kNkMAIDCommand_CapGetDefault,
											ulParam,
											ulDataType,
											pData,
											(LPNKFUNC)CompletionProc,
											(NKREF)pRefCompletion );
	IdleLoop( pobject, &ulCount, 1 );

	return (nResult == kNkMAIDResult_NoError);
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL Command_CapGet(LPNkMAIDObject pobject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete )
{
	SLONG nResult;
	ULONG	ulCount = 0L;
	LPRefCompletionProc pRefCompletion;
	pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = NULL;
	nResult = CallMAIDEntryPoint(	pobject,
											kNkMAIDCommand_CapGet, 
											ulParam,
											ulDataType,
											pData,
											(LPNKFUNC)CompletionProc,
											(NKREF)pRefCompletion );
	IdleLoop( pobject, &ulCount, 1 );

	return ( nResult == kNkMAIDResult_NoError );
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL Command_CapSet(LPNkMAIDObject pobject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete )
{
	BOOL bSuccess = FALSE;
	SLONG nResult;
	ULONG	ulCount = 0L;
	LPRefCompletionProc pRefCompletion;
	pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = NULL;
	nResult = CallMAIDEntryPoint(	pobject,
											kNkMAIDCommand_CapSet, 
											ulParam,
											ulDataType,
											pData,
											(LPNKFUNC)CompletionProc,
											(NKREF)pRefCompletion );
	IdleLoop( pobject, &ulCount, 1 );

	// MovRecInCardStatusの場合、Result Codesがゼロ(No Error)以外にも成功コードが存在する
	if (ulParam == kNkMAIDCapability_MovRecInCardStatus)
	{
		switch (nResult)
		{
		case kNkMAIDResult_NoError:
			bSuccess = TRUE;
			break;
		case kNkMAIDResult_RecInCard:
			printf("Card Recording\n");
			bSuccess = TRUE;
			break;
		case kNkMAIDResult_RecInExternalDevice:
			printf("ExternalDevice Recording\n");
			bSuccess = TRUE;
			break;
		case kNkMAIDResult_RecInCardAndExternalDevice:
			printf("Card and ExternalDevice Recording\n");
			bSuccess = TRUE;
			break;
		default:
			bSuccess = FALSE;
			break;
		}
	}
	else
	{
		bSuccess = (nResult == kNkMAIDResult_NoError) ? TRUE : FALSE;
	}

	return (bSuccess);
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL Command_CapGetSB(LPNkMAIDObject pobject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete, SLONG* pnResult)
{
	SLONG nResult;
	ULONG	ulCount = 0L;
	LPRefCompletionProc pRefCompletion;
	pRefCompletion = (LPRefCompletionProc)malloc(sizeof(RefCompletionProc));
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = NULL;
	nResult = CallMAIDEntryPoint(pobject,
		kNkMAIDCommand_CapGet,
		ulParam,
		ulDataType,
		pData,
		(LPNKFUNC)CompletionProc,
		(NKREF)pRefCompletion);
	IdleLoop(pobject, &ulCount, 1);

	*pnResult = nResult;
	return (nResult == kNkMAIDResult_NoError);
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL Command_CapSetSB(LPNkMAIDObject pobject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete, SLONG* pnResult)
{
	SLONG nResult;
	ULONG	ulCount = 0L;
	LPRefCompletionProc pRefCompletion;
	pRefCompletion = (LPRefCompletionProc)malloc(sizeof(RefCompletionProc));
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = NULL;
	nResult = CallMAIDEntryPoint(pobject,
		kNkMAIDCommand_CapSet,
		ulParam,
		ulDataType,
		pData,
		(LPNKFUNC)CompletionProc,
		(NKREF)pRefCompletion);
	IdleLoop(pobject, &ulCount, 1);

	*pnResult = nResult;
	return (nResult == kNkMAIDResult_NoError);
}

//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL Command_CapStart(LPNkMAIDObject pobject, ULONG ulParam, LPNKFUNC pfnComplete, NKREF refComplete, SLONG* pnResult)
{
	SLONG nResult = CallMAIDEntryPoint( pobject,
													kNkMAIDCommand_CapStart, 
													ulParam,
													kNkMAIDDataType_Null,
													(NKPARAM)NULL,
													pfnComplete,
													refComplete );
	if ( pnResult != NULL ) *pnResult = nResult;

	return ( nResult == kNkMAIDResult_NoError || nResult == kNkMAIDResult_Pending || nResult == kNkMAIDResult_BulbReleaseBusy || 
		     nResult == kNkMAIDResult_SilentReleaseBusy || nResult == kNkMAIDResult_MovieFrameReleaseBusy ||
			 nResult == kNkMAIDResult_Waiting_2ndRelease );
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL Command_CapStartGeneric( LPNkMAIDObject pObject, ULONG ulParam, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete, SLONG* pnResult )
{
	SLONG nResult = CallMAIDEntryPoint( pObject,
													kNkMAIDCommand_CapStart, 
													ulParam,
													kNkMAIDDataType_GenericPtr,
													pData,
													pfnComplete,
													refComplete );
	if ( pnResult != NULL ) *pnResult = nResult;

	return ( nResult == kNkMAIDResult_NoError || nResult == kNkMAIDResult_Pending );
}

//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL Command_Abort(LPNkMAIDObject pobject, LPNKFUNC pfnComplete, NKREF refComplete)
{
	SLONG lResult = CallMAIDEntryPoint( pobject,
													kNkMAIDCommand_Abort, 
													(ULONG)NULL,
													kNkMAIDDataType_Null,
													(NKPARAM)NULL,
													pfnComplete,
													refComplete );
	return lResult == kNkMAIDResult_NoError;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL Command_Async(LPNkMAIDObject pobject)
{
	SLONG lResult = CallMAIDEntryPoint( pobject,
										kNkMAIDCommand_Async,
										0,
										kNkMAIDDataType_Null,
										(NKPARAM)NULL,
										(LPNKFUNC)NULL,
										(NKREF)NULL );
	return( lResult == kNkMAIDResult_NoError || lResult == kNkMAIDResult_Pending );
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL SelectSource( LPRefObj pRefObj, ULONG *pulSrcID )
{
	BOOL	bRet;
	NkMAIDEnum	stEnum;
	char	buf[256];
	UWORD	wSel;
	ULONG	i;
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_Children );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Enum ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_Children, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, kNkMAIDCapability_Children, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if( bRet == FALSE ) return FALSE;

	// check the data of the capability.
	if ( stEnum.wPhysicalBytes != 4 ) return FALSE;

	if ( stEnum.ulElements == 0 ) {
		printf( "There is no Source object.\n0. Exit\n>" );
		scanf( "%s", buf );
		return TRUE;
	}

	// allocate memory for array data
	stEnum.pData = malloc( stEnum.ulElements * stEnum.wPhysicalBytes );
	if ( stEnum.pData == NULL ) return FALSE;
	// get array data
	bRet = Command_CapGetArray( pRefObj->pObject, kNkMAIDCapability_Children, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if( bRet == FALSE ) {
		free( stEnum.pData );
		return FALSE;
	}

	// show the list of selectable Sources
	for ( i = 0; i < stEnum.ulElements; i++ )
		printf( "%d. ID = %d\n", i + 1, ((ULONG*)stEnum.pData)[i] );

	if ( stEnum.ulElements == 1 )
		printf( "0. Exit\nSelect (1, 0)\n>" );
	else
		printf( "0. Exit\nSelect (1-%d, 0)\n>", stEnum.ulElements );

	scanf( "%s", buf );
	wSel = atoi( buf );

	if ( wSel > 0 && wSel <= stEnum.ulElements ) {
		*pulSrcID = ((ULONG*)stEnum.pData)[wSel - 1];
		free( stEnum.pData );
	} else {
		free( stEnum.pData );
		if ( wSel != 0 ) return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL SelectItem( LPRefObj pRefObj, ULONG *pulItemID )
{
	BOOL	bRet;
	NkMAIDEnum	stEnum;
	char	buf[256];
	UWORD	wSel;
	ULONG	i;
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_Children );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Enum ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_Children, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, kNkMAIDCapability_Children, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if( bRet == FALSE ) return FALSE;

	// check the data of the capability.
	if ( stEnum.ulElements == 0 ) {
		printf( "There is no item.\n" );
		return TRUE;
	}

	// check the data of the capability.
	if ( stEnum.wPhysicalBytes != 4 ) return FALSE;

	// allocate memory for array data
	stEnum.pData = malloc( stEnum.ulElements * stEnum.wPhysicalBytes );
	if ( stEnum.pData == NULL ) return FALSE;
	// get array data
	bRet = Command_CapGetArray( pRefObj->pObject, kNkMAIDCapability_Children, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if( bRet == FALSE ) {
		free( stEnum.pData );
		return FALSE;
	}

	// show the list of selectable Items
	for ( i = 0; i < stEnum.ulElements; i++ )
		printf( "%d. Internal ID = %08X\n", i + 1, ((ULONG*)stEnum.pData)[i] );

	if ( stEnum.ulElements == 0 )
		printf( "There is no Item object.\n0. Exit\n>" );
	else if ( stEnum.ulElements == 1 )
		printf( "0. Exit\nSelect (1, 0)\n>" );
	else
		printf( "0. Exit\nSelect (1-%d, 0)\n>", stEnum.ulElements );

	scanf( "%s", buf );
	wSel = atoi( buf );

	if ( wSel > 0 && wSel <= stEnum.ulElements ) {
		*pulItemID = ((ULONG*)stEnum.pData)[wSel - 1];
		free( stEnum.pData );
	} else {
		free( stEnum.pData );
		if ( wSel != 0 ) return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL SelectData( LPRefObj pRefObj, ULONG *pulDataType )
{
	BOOL	bRet;
	char	buf[256];
	UWORD	wSel;
	ULONG	ulDataTypes, i = 0, DataTypes[8];
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_DataTypes );
	if ( pCapInfo == NULL ) return FALSE;

	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_DataTypes, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, kNkMAIDCapability_DataTypes, kNkMAIDDataType_UnsignedPtr, (NKPARAM)&ulDataTypes, NULL, NULL );
	if( bRet == FALSE ) return FALSE;

	// show the list of selectable Data type object.
	if ( ulDataTypes & kNkMAIDDataObjType_Image ) {
	
		DataTypes[i++] = kNkMAIDDataObjType_Image;
		printf( "%d. Image\n", i );
	}
	if ( ulDataTypes & kNkMAIDDataObjType_Video ) {
	
		DataTypes[i++] = kNkMAIDDataObjType_Video;
		printf( "%d. Movie\n", i );
	}
	if ( ulDataTypes & kNkMAIDDataObjType_Thumbnail ) {
		DataTypes[i++] = kNkMAIDDataObjType_Thumbnail;
		printf( "%d. Thumbnail\n", i );
	}

	if ( i == 0 )
		printf( "There is no Data object.\n0. Exit\n>" );
	else if ( i == 1 )
		printf( "0. Exit\nSelect (1, 0)\n>" );
	else
		printf( "0. Exit\nSelect (1-%d, 0)\n>", i );

	scanf( "%s", buf );
	wSel = atoi( buf );

	if ( wSel > 0 && wSel <= i )
		*pulDataType = DataTypes[wSel - 1];

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL CheckDataType( LPRefObj pRefObj, ULONG *pulDataType )
{
	BOOL	bRet;
	ULONG	ulDataTypes = 0;
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_DataTypes );
	if ( pCapInfo == NULL ) return FALSE;

	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_DataTypes, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, kNkMAIDCapability_DataTypes, kNkMAIDDataType_UnsignedPtr, (NKPARAM)&ulDataTypes, NULL, NULL );
	if( bRet == FALSE ) return FALSE;

	// show the list of selectable Data type object.
	if ( ulDataTypes & kNkMAIDDataObjType_Video )
	{
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
char* GetEnumString( ULONG ulCapID, ULONG ulValue, char *psString )
{
	switch ( ulCapID ) {
		case kNkMAIDCapability_FlashMode:
			switch( ulValue ){
				case kNkMAIDFlashMode_FillFlash:
					strcpy( psString, "Normal" );
					break;
				case kNkMAIDFlashMode_RearCurtainSync:
					strcpy( psString, "Rear-sync" );
					break;
				case kNkMAIDFlashMode_SlowSyncFillFlash:
					strcpy( psString, "Slow-sync" );
					break;
				case kNkMAIDFlashMode_RedEyeReductionLighting:
					strcpy( psString, "Red Eye Reduction" );
					break;
				case kNkMAIDFlashMode_SlowSyncRedEye:
					strcpy( psString, "Slow-sync Red Eye Reduction" );
					break;
				case kNkMAIDFlashMode_Off:
					strcpy( psString, "flash off" );
					break;
				default:
					sprintf( psString, "FlashMode %u", ulValue );
			}
			break;
		case kNkMAIDCapability_ExposureMode:
			switch( ulValue ){
				case kNkMAIDExposureMode_Program:
					strcpy( psString, "Program" );
					break;
				case kNkMAIDExposureMode_AperturePriority:
					strcpy( psString, "Aperture" );
					break;
				case kNkMAIDExposureMode_SpeedPriority:
					strcpy( psString, "Speed" );
					break;
				case kNkMAIDExposureMode_Manual:
					strcpy(psString, "Manual");
					break;
				case kNkMAIDExposureMode_Auto:
					strcpy(psString, "Auto");
					break;
				case kNkMAIDExposureMode_UserMode1:
					strcpy(psString, "UserMode1");
					break;
				case kNkMAIDExposureMode_UserMode2:
					strcpy(psString, "UserMode2");
					break;
				case kNkMAIDExposureMode_UserMode3:
					strcpy(psString, "UserMode3");
					break;
				default:
					sprintf( psString, "ExposureMode %u", ulValue );
			}
			break;
		case kNkMAIDCapability_PictureControl:
			switch( ulValue ){
				case kNkMAIDPictureControl_Undefined:
					strcpy( psString, "Undefined" );
					break;
				case kNkMAIDPictureControl_Standard:
					strcpy( psString, "Standard" );
					break;
				case kNkMAIDPictureControl_Neutral:
					strcpy( psString, "Neutral" );
					break;
				case kNkMAIDPictureControl_Vivid:
					strcpy( psString, "Vivid" );
					break;
				case kNkMAIDPictureControl_Monochrome:
					strcpy( psString, "Monochrome" );
					break;
				case kNkMAIDPictureControl_Portrait:
					strcpy( psString, "Portrait" );
					break;
				case kNkMAIDPictureControl_Landscape:
					strcpy( psString, "Landscape" );
					break;
				case kNkMAIDPictureControl_Flat:
					strcpy( psString, "Flat" );
					break;
				case kNkMAIDPictureControl_Auto:
					strcpy(psString, "Auto");
					break;
				case kNkMAIDPictureControl_Dream:
					strcpy(psString, "Dream");
					break;
				case kNkMAIDPictureControl_Morning:
					strcpy(psString, "Morning");
					break;
				case kNkMAIDPictureControl_Pop:
					strcpy(psString, "Pop");
					break;
				case kNkMAIDPictureControl_Sunday:
					strcpy(psString, "Sunday");
					break;
				case kNkMAIDPictureControl_Somber:
					strcpy(psString, "Somber");
					break;
				case kNkMAIDPictureControl_Dramatic:
					strcpy(psString, "Dramatic");
					break;
				case kNkMAIDPictureControl_Silence:
					strcpy(psString, "Silence");
					break;
				case kNkMAIDPictureControl_Breached:
					strcpy(psString, "Breached");
					break;
				case kNkMAIDPictureControl_Melancholic:
					strcpy(psString, "Melancholic");
					break;
				case kNkMAIDPictureControl_Pure:
					strcpy(psString, "Pure");
					break;
				case kNkMAIDPictureControl_Denim:
					strcpy(psString, "Denim");
					break;
				case kNkMAIDPictureControl_Toy:
					strcpy(psString, "Toy");
					break;
				case kNkMAIDPictureControl_Sepia:
					strcpy(psString, "Sepia");
					break;
				case kNkMAIDPictureControl_Blue:
					strcpy(psString, "Blue");
					break;
				case kNkMAIDPictureControl_Red:
					strcpy(psString, "Red");
					break;
				case kNkMAIDPictureControl_Pink:
					strcpy(psString, "Pink");
					break;
				case kNkMAIDPictureControl_Charcoal:
					strcpy(psString, "Charcoal");
					break;
				case kNkMAIDPictureControl_Graphite:
					strcpy(psString, "Graphite");
					break;
				case kNkMAIDPictureControl_Binary:
					strcpy(psString, "Binary");
					break;
				case kNkMAIDPictureControl_Carbon:
					strcpy(psString, "Carbon");
					break;
				case kNkMAIDPictureControl_Custom1:
				case kNkMAIDPictureControl_Custom2:
				case kNkMAIDPictureControl_Custom3:
				case kNkMAIDPictureControl_Custom4:
				case kNkMAIDPictureControl_Custom5:
				case kNkMAIDPictureControl_Custom6:
				case kNkMAIDPictureControl_Custom7:
				case kNkMAIDPictureControl_Custom8:
				case kNkMAIDPictureControl_Custom9:
					sprintf( psString, "Custom Picture Contol %d", ulValue-200 );
					break;
				default:
					sprintf( psString, "Picture Control %u", ulValue );
			}
			break;
		case kNkMAIDCapability_MoviePictureControl:
			switch (ulValue){
			case kNkMAIDMoviePictureControl_Undefined:
				strcpy(psString, "Undefined");
				break;
			case kNkMAIDMoviePictureControl_Standard:
				strcpy(psString, "Standard");
				break;
			case kNkMAIDMoviePictureControl_Neutral:
				strcpy(psString, "Neutral");
				break;
			case kNkMAIDMoviePictureControl_Vivid:
				strcpy(psString, "Vivid");
				break;
			case kNkMAIDMoviePictureControl_Monochrome:
				strcpy(psString, "Monochrome");
				break;
			case kNkMAIDMoviePictureControl_Portrait:
				strcpy(psString, "Portrait");
				break;
			case kNkMAIDMoviePictureControl_Landscape:
				strcpy(psString, "Landscape");
				break;
			case kNkMAIDMoviePictureControl_Flat:
				strcpy(psString, "Flat");
				break;
			case kNkMAIDMoviePictureControl_Auto:
				strcpy(psString, "Auto");
				break;
			case kNkMAIDMoviePictureControl_SamePhoto:
				strcpy(psString, "SamePhoto");
				break;
			case kNkMAIDMoviePictureControl_Dream:
				strcpy(psString, "Dream");
				break;
			case kNkMAIDMoviePictureControl_Morning:
				strcpy(psString, "Morning");
				break;
			case kNkMAIDMoviePictureControl_Pop:
				strcpy(psString, "Pop");
				break;
			case kNkMAIDMoviePictureControl_Sunday:
				strcpy(psString, "Sunday");
				break;
			case kNkMAIDMoviePictureControl_Somber:
				strcpy(psString, "Somber");
				break;
			case kNkMAIDMoviePictureControl_Dramatic:
				strcpy(psString, "Dramatic");
				break;
			case kNkMAIDMoviePictureControl_Silence:
				strcpy(psString, "Silence");
				break;
			case kNkMAIDMoviePictureControl_Breached:
				strcpy(psString, "Breached");
				break;
			case kNkMAIDMoviePictureControl_Melancholic:
				strcpy(psString, "Melancholic");
				break;
			case kNkMAIDMoviePictureControl_Pure:
				strcpy(psString, "Pure");
				break;
			case kNkMAIDMoviePictureControl_Denim:
				strcpy(psString, "Denim");
				break;
			case kNkMAIDMoviePictureControl_Toy:
				strcpy(psString, "Toy");
				break;
			case kNkMAIDMoviePictureControl_Sepia:
				strcpy(psString, "Sepia");
				break;
			case kNkMAIDMoviePictureControl_Blue:
				strcpy(psString, "Blue");
				break;
			case kNkMAIDMoviePictureControl_Red:
				strcpy(psString, "Red");
				break;
			case kNkMAIDMoviePictureControl_Pink:
				strcpy(psString, "Pink");
				break;
			case kNkMAIDMoviePictureControl_Charcoal:
				strcpy(psString, "Charcoal");
				break;
			case kNkMAIDMoviePictureControl_Graphite:
				strcpy(psString, "Graphite");
				break;
			case kNkMAIDMoviePictureControl_Binary:
				strcpy(psString, "Binary");
				break;
			case kNkMAIDMoviePictureControl_Carbon:
				strcpy(psString, "Carbon");
				break;
			case kNkMAIDMoviePictureControl_Custom1:
			case kNkMAIDMoviePictureControl_Custom2:
			case kNkMAIDMoviePictureControl_Custom3:
			case kNkMAIDMoviePictureControl_Custom4:
			case kNkMAIDMoviePictureControl_Custom5:
			case kNkMAIDMoviePictureControl_Custom6:
			case kNkMAIDMoviePictureControl_Custom7:
			case kNkMAIDMoviePictureControl_Custom8:
			case kNkMAIDMoviePictureControl_Custom9:
				sprintf(psString, "Custom Picture Contol %d", ulValue - 200);
				break;
			default:
				sprintf(psString, "Picture Control %u", ulValue);
			}
			break;
		case kNkMAIDCapability_LiveViewImageSize:
			switch( ulValue ){
				case kNkMAIDLiveViewImageSize_QVGA:
					strcpy( psString, "QVGA" );
					break;
				case kNkMAIDLiveViewImageSize_VGA:
					strcpy( psString, "VGA" );
					break;
				case kNkMAIDLiveViewImageSize_XGA:
					strcpy(psString, "XGA");
					break;
				default:
					sprintf( psString, "LiveViewImageSize %u", ulValue );
			}
			break;
		case kNkMAIDCapability_SBWirelessMode:
			switch (ulValue){
			case kNkMAIDSBWirelessMode_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDSBWirelessMode_Radio:
				strcpy(psString, "Radio");
				break;
			case kNkMAIDSBWirelessMode_Optical:
				strcpy(psString, "Optical");
				break;
			case kNkMAIDSBWirelessMode_OpticalandRadio:
				strcpy(psString, "OpticalandRadio");
				break;
			default:
				sprintf(psString, "SBWirelessMode %u", ulValue);
			}
			break;
		case kNkMAIDCapability_SBWirelessMultipleFlashMode:
			switch (ulValue){
				case kNkMAIDSBWirelessMultipleFlashMode_Group:
				strcpy(psString, "Group");
				break;
			case kNkMAIDSBWirelessMultipleFlashMode_QuickWireless:
				strcpy(psString, "QuickWireless");
				break;
			case kNkMAIDSBWirelessMultipleFlashMode_Repeat:
				strcpy(psString, "Repeat");
				break;
			default:
				sprintf(psString, "SBWirelessMultipleFlashMode %u", ulValue);
			}
			break;
		case kNkMAIDCapability_WirelessCLSEntryMode:
			switch (ulValue){
			case kNkMAIDWirelessCLSEntryMode_Peering:
				strcpy(psString, "Peering");
				break;
			case kNkMAIDWirelessCLSEntryMode_PINCode:
				strcpy(psString, "PINCode");
				break;
			default:
				sprintf(psString, "WirelessCLSEntryMode %u", ulValue);
			}
			break;
		case kNkMAIDCapability_OpticalMultipleFlashChannel:
			switch (ulValue){
			case kNkMAIDOpticalMultipleFlashChannel_1ch:
				strcpy(psString, "1ch");
				break;
			case kNkMAIDOpticalMultipleFlashChannel_2ch:
				strcpy(psString, "2ch");
				break;
			case kNkMAIDOpticalMultipleFlashChannel_3ch:
				strcpy(psString, "3ch");
				break;
			case kNkMAIDOpticalMultipleFlashChannel_4ch:
				strcpy(psString, "4ch");
				break;
			default:
				sprintf(psString, "OpticalMultipleFlashChannel %u", ulValue);
			}
			break;
		case kNkMAIDCapability_FlashRangeDisplay:
			switch (ulValue){
			case kNkMAIDFlashRangeDisplay_m:
				strcpy(psString, "meter");
				break;
			case kNkMAIDFlashRangeDisplay_ft:
				strcpy(psString, "feet");
				break;
			default:
				sprintf(psString, "FlashRangeDisplay %u", ulValue);
			}
			break;
		case kNkMAIDCapability_SBSettingMemberLock:
			switch (ulValue){
			case kNkMAIDSBSettingMemberLock_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDSBSettingMemberLock_On:
				strcpy(psString, "On");
				break;
			default:
				sprintf(psString, "SBSettingMemberLock %u", ulValue);
			}
			break;
		case kNkMAIDCapability_SBIntegrationFlashReady:
			switch (ulValue){
			case kNkMAIDSBIntegrationFlashReady_NotReady:
				strcpy(psString, "NotReady");
				break;
			case kNkMAIDSBIntegrationFlashReady_Ready:
				strcpy(psString, "Ready");
				break;
			default:
				sprintf(psString, "SBIntegrationFlashReady %u", ulValue);
			}
			break;
		case kNkMAIDCapability_ApplyLiveViewSetting:
			switch (ulValue){
			case kNkMAIDApplyLiveViewSetting_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDApplyLiveViewSetting_On:
				strcpy(psString, "On");
				break;
			default:
				sprintf(psString, "ApplyLiveViewSetting %u", ulValue);
			}
			break;
		case kNkMAIDCapability_DetectionPeaking:
			switch (ulValue){
			case kNkMAIDDetectionPeaking_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDDetectionPeaking_Low:
				strcpy(psString, "Low");
				break;
			case kNkMAIDDetectionPeaking_Normal:
				strcpy(psString, "Normal");
				break;
			case kNkMAIDDetectionPeaking_High:
				strcpy(psString, "High");
				break;
			default:
				sprintf(psString, "DetectionPeaking %u", ulValue);
			}
			break;
		case kNkMAIDCapability_HighlightBrightness:
			switch (ulValue){
			case kNkMAIDHighlightBrightness_180:
				strcpy(psString, "180");
				break;
			case kNkMAIDHighlightBrightness_191:
				strcpy(psString, "191");
				break;
			case kNkMAIDHighlightBrightness_202:
				strcpy(psString, "202");
				break;
			case kNkMAIDHighlightBrightness_213:
				strcpy(psString, "213");
				break;
			case kNkMAIDHighlightBrightness_224:
				strcpy(psString, "224");
				break;
			case kNkMAIDHighlightBrightness_235:
				strcpy(psString, "235");
				break;
			case kNkMAIDHighlightBrightness_248:
				strcpy(psString, "248");
				break;
			case kNkMAIDHighlightBrightness_255:
				strcpy(psString, "255");
				break;
			default:
				sprintf(psString, "HighlightBrightness %u", ulValue);
			}
			break;
		case kNkMAIDCapability_LowLightAF:
			switch (ulValue){
			case kNkMAIDLowLightAF_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDLowLightAF_On:
				strcpy(psString, "On");
				break;
			default:
				sprintf(psString, "LowLightAF %u", ulValue);
			}
			break;
		case kNkMAIDCapability_DiffractionCompensation:
			switch (ulValue){
			case kNkMAIDDiffractionCompensation_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDDiffractionCompensation_On:
				strcpy(psString, "On");
				break;
			default:
				sprintf(psString, "DiffractionCompensation %u", ulValue);
			}
			break;
		case kNkMAIDCapability_ShootingMode:
			switch (ulValue){
			case eNkMAIDShootingMode_S:
				strcpy(psString, "Single frame");
				break;
			case eNkMAIDShootingMode_C:
				strcpy(psString, "Continuous L");
				break;
			case eNkMAIDShootingMode_CH:
				strcpy(psString, "Continuous H");
				break;
			case eNkMAIDShootingMode_SelfTimer:
				strcpy(psString, "Self-timer");
				break;
			case eNkMAIDShootingMode_CHPriorityFrame:
				strcpy(psString, "Continuous H(extended)");
				break;
			default:
				sprintf(psString, "ShootingMode %u", ulValue);
			}
			break;
		case kNkMAIDCapability_VibrationReduction:
			switch (ulValue){
			case kNkMAIDVibrationReduction_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDVibrationReduction_Normal:
				strcpy(psString, "Normal(ON)");
				break;
			case kNkMAIDVibrationReduction_Sport:
				strcpy(psString, "Sport");
				break;
			default:
				sprintf(psString, "VibrationReduction %u", ulValue);
			}
			break;
		case kNkMAIDCapability_HDMIOutputDataDepth:
			switch (ulValue){
			case kNkMAIDHDMIOutputDataDepth_8bit:
				strcpy(psString, "8bit");
				break;
			case kNkMAIDHDMIOutputDataDepth_10bit:
				strcpy(psString, "10bit");
				break;
			default:
				sprintf(psString, "HDMIOutputDataDepth %u", ulValue);
			}
			break;
		case kNkMAIDCapability_HDRSaveIndividualImages:
			switch (ulValue){
			case kNkMAIDHDRSaveIndividualImages_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDHDRSaveIndividualImages_On:
				strcpy(psString, "On");
				break;
			default:
				sprintf(psString, "HDRSaveIndividualImages %u", ulValue);
			}
			break;
		case kNkMAIDCapability_ExternalRecordingControl:
			switch (ulValue){
			case kNkMAIDExternalRecordingControl_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDExternalRecordingControl_On:
				strcpy(psString, "On");
				break;
			default:
				sprintf(psString, "ExternalRecordingControl %u", ulValue);
			}
			break;
		case kNkMAIDCapability_MovieAfSpeedWhenToApply:
			switch (ulValue){
			case kNkMAIDMovieAfSpeedWhenToApply_Always:
				strcpy(psString, "Always");
				break;
			case kNkMAIDMovieAfSpeedWhenToApply_RecOnly:
				strcpy(psString, "RecOnly");
				break;
			default:
				sprintf(psString, "ExternalRecordingControl %u", ulValue);
			}
			break;
		case kNkMAIDCapability_MovieAttenuator:
			switch (ulValue){
			case kNkMAIDMovieAttenuator_disable:
				strcpy(psString, "disable");
				break;
			case kNkMAIDMovieAttenuator_enable:
				strcpy(psString, "enable");
				break;
			default:
				sprintf(psString, "MovieAttenuator %u", ulValue);
			}
			break;
		case kNkMAIDCapability_MovieAutoDistortion:
			switch (ulValue){
			case kNkMAIDMovieAutoDistortion_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDMovieAutoDistortion_On:
				strcpy(psString, "On");
				break;
			default:
				sprintf(psString, "MovieAutoDistortion %u", ulValue);
			}
			break;
		case kNkMAIDCapability_MovieDiffractionCompensation:
			switch (ulValue){
			case kNkMAIDMovieDiffractionCompensation_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDMovieDiffractionCompensation_On:
				strcpy(psString, "On");
				break;
			default:
				sprintf(psString, "MovieDiffractionCompensation %u", ulValue);
			}
			break;
		case kNkMAIDCapability_MovieFileType:
			switch (ulValue){
			case kNkMAIDMovieFileType_MOV:
				strcpy(psString, "MOV");
				break;
			case kNkMAIDMovieFileType_MP4:
				strcpy(psString, "MP4");
				break;
			default:
				sprintf(psString, "MovieFileType %u", ulValue);
			}
			break;
		case kNkMAIDCapability_MovieFocusMode:
			switch (ulValue){
			case kNkMAIDMovieFocusMode_AFs:
				strcpy(psString, "AF-S");
				break;
			case kNkMAIDMovieFocusMode_AFc:
				strcpy(psString, "AF-C");
				break;
			case kNkMAIDMovieFocusMode_AFf:
				strcpy(psString, "AF-F");
				break;
			case kNkMAIDMovieFocusMode_MF_SEL:
				strcpy(psString, "MF");
				break;
			default:
				sprintf(psString, "MovieFocusMode %u", ulValue);
			}
			break;
		case kNkMAIDCapability_MovieLogOutput:
			switch (ulValue){
			case kNkMAIDMovieLogOutput_NotOutputting:
				strcpy(psString, "NotOutputting");
				break;
			case kNkMAIDMovieLogOutput_Outputting:
				strcpy(psString, "Outputting");
				break;
			default:
				sprintf(psString, "MovieLogOutput %u", ulValue);
			}
			break;
		case kNkMAIDCapability_MovieLogSetting:
			switch (ulValue){
			case kNkMAIDMovieLogSetting_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDMovieLogSetting_NLog:
				strcpy(psString, "N-Log");
				break;
			case kNkMAIDMovieLogSetting_HDR:
				strcpy(psString, "HDR");
				break;
			default:
				sprintf(psString, "MovieLogSetting %u", ulValue);
			}
			break;
		case kNkMAIDCapability_MovieVibrationReduction:
			switch (ulValue){
			case kNkMAIDMovieVibrationReduction_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDMovieVibrationReduction_Normal:
				strcpy(psString, "Normal(ON)");
				break;
			case kNkMAIDMovieVibrationReduction_Sport:
				strcpy(psString, "Sport");
				break;
			case kNkMAIDMovieVibrationReduction_SamePhoto:
				strcpy(psString, "SamePhoto");
				break;
			default:
				sprintf(psString, "MovieVibrationReduction %u", ulValue);
			}
			break;
		case kNkMAIDCapability_MovieVignetteControl:
			switch (ulValue){
			case kNkMAIDMovieVignetteControl_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDMovieVignetteControl_Low:
				strcpy(psString, "Low");
				break;
			case kNkMAIDMovieVignetteControl_Normal:
				strcpy(psString, "Normal");
				break;
			case kNkMAIDMovieVignetteControl_High:
				strcpy(psString, "High");
				break;
			case kNkMAIDMovieVignetteControl_SamePhoto:
				strcpy(psString, "SamePhoto");
				break;
			default:
				sprintf(psString, "MovieVignetteControl %u", ulValue);
			}
			break;
		case kNkMAIDCapability_RecordTimeCodes:
			switch (ulValue){
			case kNkMAIDRecordTimeCodes_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDRecordTimeCodes_On:
				strcpy(psString, "On");
				break;
			case kNkMAIDRecordTimeCodes_OnWithHDMIOutput:
				strcpy(psString, "OnWithHDMIOutput");
				break;
			default:
				sprintf(psString, "RecordTimeCodes %u", ulValue);
			}
			break;
		case kNkMAIDCapability_CountUpMethod:
			switch (ulValue){
			case kNkMAIDCountUpMethod_RecordRun:
				strcpy(psString, "RecordRun");
				break;
			case kNkMAIDCountUpMethod_FreeRun:
				strcpy(psString, "FreeRun");
				break;
			default:
				sprintf(psString, "CountUpMethod %u", ulValue);
			}
			break;
		case kNkMAIDCapability_DropFrame:
			switch (ulValue){
			case kNkMAIDDropFrame_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDDropFrame_On:
				strcpy(psString, "On");
				break;
			default:
				sprintf(psString, "DropFrame %u", ulValue);
			}
			break;
		case kNkMAIDCapability_LiveViewZoomArea:
			switch (ulValue){
			case kNkMAIDLiveViewZoomArea_0:
				strcpy(psString, "0");
				break;
			case kNkMAIDLiveViewZoomArea_512:
				strcpy(psString, "512 x 342");
				break;
			case kNkMAIDLiveViewZoomArea_640:
				strcpy(psString, "640 x 426");
				break;
			case kNkMAIDLiveViewZoomArea_1024:
				strcpy(psString, "1024 x 682");
				break;
			case kNkMAIDLiveViewZoomArea_1280:
				strcpy(psString, "1280 x 854");
				break;
			case kNkMAIDLiveViewZoomArea_2048:
				strcpy(psString, "2048 x 1364");
				break;
			case kNkMAIDLiveViewZoomArea_2560:
				strcpy(psString, "2560 x 1706");
				break;
			default:
				sprintf(psString, "LiveViewZoomArea %u", ulValue);
			}
			break;
		case kNkMAIDCapability_LiveViewImageStatus:
			switch (ulValue){
			case kNkMAIDLiveViewImageStatus_CannotAcquire:
				strcpy(psString, "CannotAcquire");
				break;
			case kNkMAIDLiveViewImageStatus_CanAcquire:
				strcpy(psString, "CanAcquire");
				break;
			default:
				sprintf(psString, "LiveViewImageStatus %u", ulValue);
			}
			break;
		case kNkMAIDCapability_ElectronicFrontCurtainShutterEx:
			switch (ulValue) {
			case kNkMAIDElectronicFrontCurtainShutter_Off:
				strcpy(psString, "Mechanical shutter");
				break;
			case kNkMAIDElectronicFrontCurtainShutter_On:
				strcpy(psString, "Electronic front-curtain shutter");
				break;
			case kNkMAIDElectronicFrontCurtainShutter_Auto:
				strcpy(psString, "Auto");
				break;
			}
			break;
		case kNkMAIDCapability_ChangeMonitorOffStatus:
			switch (ulValue) {
			case kNkMAIDChangeMonitorOffStatus_CancelForcedTurnOff:
				strcpy(psString, "Cancel forced turn Off");
				break;
			case kNkMAIDChangeMonitorOffStatus_ForcedTurnOff:
				strcpy(psString, "Forced turn Off");
				break;
			default:
				sprintf(psString, "ChangeMonitorOffStatus %u", ulValue);
			}
			break;
		case kNkMAIDCapability_ExtendedShutterSpeedsManual:
			switch (ulValue) {
			case kNkMAIDExtendedShutterSpeedsManual_Off:
				strcpy(psString, "Off");
				break;
			case kNkMAIDExtendedShutterSpeedsManual_On:
				strcpy(psString, "On");
				break;
			default:
				sprintf(psString, "ExtendedShutterSpeedsManual %u", ulValue);
			}
			break;
		case kNkMAIDCapability_StillCaptureModeSaveFrame:
			switch (ulValue) {
			case eNkMAIDStillCaptureModeSaveFrame_S:
				strcpy(psString, "Single frame");
				break;
			case eNkMAIDStillCaptureModeSaveFrame_CH:
				strcpy(psString, "Continuous");
				break;
			default:
				sprintf(psString, "StillCaptureModeSaveFrame %u", ulValue);
			}
			break;

		default:
			strcpy( psString, "Undefined String" ); 
	}
	return psString;
}
//------------------------------------------------------------------------------------------------------------------------------------
char*	GetUnsignedString( ULONG ulCapID, ULONG ulValue, char *psString )
{
	char buff[256];

	switch ( ulCapID )
	{
	case kNkMAIDCapability_MeteringMode:
		sprintf( buff, "%d : Matrix\n", kNkMAIDMeteringMode_Matrix );
		strcpy( psString, buff );
		sprintf( buff, "%d : CenterWeighted\n", kNkMAIDMeteringMode_CenterWeighted );
		strcat( psString, buff );
		sprintf( buff, "%d : Spot\n", kNkMAIDMeteringMode_Spot );
		strcat( psString, buff );
		sprintf(buff, "%d : HighLight\n", kNkMAIDMeteringMode_HighLight);
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_FocusMode:
		sprintf( buff, "%d : MF\n", kNkMAIDFocusMode_MF );
		strcpy( psString, buff );
		sprintf( buff, "%d : AF-S\n", kNkMAIDFocusMode_AFs );
		strcat( psString, buff );
		sprintf( buff, "%d : AF-C\n", kNkMAIDFocusMode_AFc );
		strcat( psString, buff );
		sprintf( buff, "%d : AF-F\n", kNkMAIDFocusMode_AFf );
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_RawJpegImageStatus:
		sprintf( buff, "%d : Single\n", eNkMAIDRawJpegImageStatus_Single );
		strcpy( psString, buff );
		sprintf( buff, "%d : Raw + Jpeg\n", eNkMAIDRawJpegImageStatus_RawJpeg );
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_DataTypes:
		strcpy( psString, "\0" );
		if ( ulValue & kNkMAIDDataObjType_Image )
		{
			strcat( psString, "Image, " );
		}
		if ( ulValue & kNkMAIDDataObjType_Sound )
		{
			strcat( psString, "Sound, " );
		}
		if ( ulValue & kNkMAIDDataObjType_Video )
		{
			strcat( psString, "Video, " );
		}
		if ( ulValue & kNkMAIDDataObjType_Thumbnail )
		{
			strcat( psString, "Thumbnail, " );
		}
		if ( ulValue & kNkMAIDDataObjType_File )
		{
			strcat( psString, "File " );
		}
		strcat( psString, "\n" );
		break;
	case kNkMAIDCapability_ModuleType:
		sprintf( buff, "%d : Scanner\n", kNkMAIDModuleType_Scanner );
		strcpy( psString, buff );
		sprintf( buff, "%d : Camera\n", kNkMAIDModuleType_Camera );
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_WBFluorescentType:
		sprintf( buff, "%d : Sodium-vapor lamps\n", kNkWBFluorescentType_SodiumVapor );
		strcpy( psString, buff );
		sprintf( buff, "%d : Warm-white fluorescent\n", kNkWBFluorescentType_WarmWhite );
		strcat( psString, buff );
		sprintf( buff, "%d : White fluorescent\n", kNkWBFluorescentType_White );
		strcat( psString, buff );
		sprintf( buff, "%d : Cool-white fluorescent\n", kNkWBFluorescentType_CoolWhite );
		strcat( psString, buff );
		sprintf( buff, "%d : Day white fluorescent\n", kNkWBFluorescentType_DayWhite );
		strcat( psString, buff );
		sprintf( buff, "%d : Daylight fluorescent\n", kNkWBFluorescentType_Daylight );
		strcat( psString, buff );
		sprintf( buff, "%d : High temp. mercury-vapor\n", kNkWBFluorescentType_HiTempMercuryVapor );
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_LiveViewProhibit:
		if (ulValue & kNkMAIDLiveViewProhibit_Retractable)
		{
			strcat(psString, "Lens factor,");
		}
		if (ulValue & kNkMAIDLiveViewProhibit_TempRise)
		{
			strcat( psString, "High Temperature," );
		}
		if (ulValue & kNkMAIDLiveViewProhibit_Capture)
		{
			strcat(psString, "Executing Capture,");
		}
		if (ulValue & kNkMAIDLiveViewProhibit_Battery)
		{
			strcat( psString, "Battery, " );
		}
		if (ulValue & kNkMAIDLiveViewProhibit_Sequence)
		{
			strcat(psString, "Sequence, ");
		}
		strcat(psString, "\n");
		break;
	case kNkMAIDCapability_LiveViewStatus:
		sprintf( buff, "%d : OFF\n", kNkMAIDLiveViewStatus_OFF );
		strcpy( psString, buff );
		sprintf(buff, "%d : ON_RemoteLV\n", kNkMAIDLiveViewStatus_ON_RemoteLV);
		strcat(psString, buff);
		sprintf(buff, "%d : ON_CameraLV\n", kNkMAIDLiveViewStatus_ON_CameraLV);
		strcat(psString, buff);
		break;
	case kNkMAIDCapability_MovRecInCardStatus:
		sprintf( buff, "%d : OFF\n", kNkMAIDMovRecInCardStatus_Off );
		strcpy( psString, buff );
		sprintf( buff, "%d : ON\n", kNkMAIDMovRecInCardStatus_On );
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_MovRecInCardProhibit:
		strcpy(psString, "\0");
		if (ulValue & kNkMAIDMovRecInCardProhibit_ErrorByLens)
		{
			strcat(psString, "Error by lens, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_SequenceError)
		{
			strcat(psString, "Sequence error, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_HighLoad)
		{
			strcat(psString, "HighLoad, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_MovieLogOutput)
		{
			strcat(psString, "Movie log output on and external recording control off");
			strcat(psString, " or ");
			strcat(psString, "Live view status is on(remote live view), ");

		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_LVPhoto)
		{
			strcat(psString, "LiveViewPhoto, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_LVImageZoom)
		{
			strcat(psString, "LiveViewZoom, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_CardProtect)
		{
			strcat(psString, "CardProtect, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_RecMov)
		{
			strcat(psString, "Recording movie, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_MovInBuf)
		{
			strcat(psString, "Movie in buffer, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_CardFull)
		{
			strcat(psString, "Card full, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_NoFormat)
		{
			strcat(psString, "Card unformatted, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_CardErr)
		{
			strcat(psString, "Card error, ");
		}
		if (ulValue & kNkMAIDMovRecInCardProhibit_NoCard)
		{
			strcat(psString, "No card, ");
		}
		strcat(psString, "\n");
		break;
	case kNkMAIDCapability_SaveMedia:
		sprintf( buff, "%d : Card\n", kNkMAIDSaveMedia_Card );
		strcpy( psString, buff );
		sprintf( buff, "%d : SDRAM\n", kNkMAIDSaveMedia_SDRAM );
		strcat( psString, buff );
		sprintf( buff, "%d : Card + SDRAM\n", kNkMAIDSaveMedia_Card_SDRAM );
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_MovieMeteringMode:
		sprintf(buff, "%d : Matrix\n", kNkMAIDMovieMeteringMode_Matrix);
		strcpy(psString, buff);
		sprintf(buff, "%d : CenterWeighted\n", kNkMAIDMovieMeteringMode_CenterWeighted);
		strcat(psString, buff);
		sprintf(buff, "%d : HighLight\n", kNkMAIDMovieMeteringMode_HighLight);
		strcat(psString, buff);
		break;
	case kNkMAIDCapability_AFLockOnAcross:
		sprintf(buff, "%d : 1(Quick)\n", kNkMAIDAFLockOnAcross_1);
		strcpy(psString, buff);
		sprintf(buff, "%d : 2\n", kNkMAIDAFLockOnAcross_2);
		strcat(psString, buff);
		sprintf(buff, "%d : 3\n", kNkMAIDAFLockOnAcross_3);
		strcat(psString, buff);
		sprintf(buff, "%d : 4\n", kNkMAIDAFLockOnAcross_4);
		strcat(psString, buff);
		sprintf(buff, "%d : 5(Slow)\n", kNkMAIDAFLockOnAcross_5);
		strcat(psString, buff);
		break;
	case kNkMAIDCapability_ExposureCompFlashUsed:
		sprintf(buff, "%d : Entireframe\n", kNkMAIDExposureCompFlashUsed_Entireframe);
		strcpy(psString, buff);
		sprintf(buff, "%d : Backgroundonly\n", kNkMAIDExposureCompFlashUsed_Backgroundonly);
		strcat(psString, buff);
		break;
	case kNkMAIDCapability_AllTestFiringDisable:
		sprintf( buff, "%d : Flash permitt\n", kNkMAIDAllTestFiringDisable_Enable );
		strcpy( psString, buff );
		sprintf( buff, "%d : Flash off\n", kNkMAIDAllTestFiringDisable_Disable );
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_RadioMultipleFlashChannel:
		sprintf( buff, "%d : Unknown\n", kNkMAIDRadioMultipleFlashChannel_Unknown );
		strcpy( psString, buff );
		sprintf( buff, "%d : 5CH\n", kNkMAIDRadioMultipleFlashChannel_5ch );
		strcat( psString, buff );
		sprintf( buff, "%d : 10CH\n", kNkMAIDRadioMultipleFlashChannel_10ch );
		strcat( psString, buff );
		sprintf( buff, "%d : 15CH\n", kNkMAIDRadioMultipleFlashChannel_15ch );
		strcat( psString, buff );
		break;
		
	case kNkMAIDCapability_FlickerReductionSetting:
		sprintf( buff, "%d : Off\n", kNkMAIDFlickerReductionSetting_Off );
		strcpy( psString, buff );
		sprintf( buff, "%d : On\n", kNkMAIDFlickerReductionSetting_On );
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_MovieActive_D_Lighting:
		sprintf( buff, "%d : Off\n", kNkMAIDMovieActive_D_Lighting_Off );
		strcpy( psString, buff );
		sprintf( buff, "%d : Low\n", kNkMAIDMovieActive_D_Lighting_Low );
		strcat( psString, buff );
		sprintf( buff, "%d : Normal\n", kNkMAIDMovieActive_D_Lighting_Normal );
		strcat( psString, buff );
		sprintf( buff, "%d : High\n", kNkMAIDMovieActive_D_Lighting_High );
		strcat( psString, buff );
		sprintf( buff, "%d : Extra high\n", kNkMAIDMovieActive_D_Lighting_ExtraHigh );
		strcat( psString, buff );
		sprintf( buff, "%d : Same as photo\n", kNkMAIDMovieActive_D_Lighting_SamePhoto );
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_ElectronicVR:
		sprintf( buff, "%d : Off\n", kMAIDElectronicVR_OFF );
		strcpy( psString, buff );
		sprintf( buff, "%d : On\n", kMAIDElectronicVR_ON );
		strcat( psString, buff );
		break;
	case kNkMAIDCapability_WBAutoType:
		sprintf(buff, "%d : Keep warm loghting colors\n", kNkWBAutoType_WarmWhite);
		strcpy(psString, buff);
		sprintf(buff, "%d : Keep white\n", kNkWBAutoType_KeepWhite);
		strcat(psString, buff);
		sprintf(buff, "%d : Keep overall atmosphere\n", kNkWBAutoType_KeepAtmosphere);
		strcat(psString, buff);
		break;
	case kNkMAIDCapability_MovieScreenSize:
		if (g_ulCameraType == kNkMAIDCameraType_Z_6II)
		{
			sprintf(buff, "%d : 3840x2160 30p\n", kNkMAIDMovieScreenSize8_QFHD_30p);
			strcpy(psString, buff);
			sprintf(buff, "%d : 3840x2160 25p\n", kNkMAIDMovieScreenSize8_QFHD_25p);
			strcat(psString, buff);
			sprintf(buff, "%d : 3840x2160 24p\n", kNkMAIDMovieScreenSize8_QFHD_24p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 120p\n", kNkMAIDMovieScreenSize8_FullHD_120p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 100p\n", kNkMAIDMovieScreenSize8_FullHD_100p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 60p\n", kNkMAIDMovieScreenSize8_FullHD_60p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 50p\n", kNkMAIDMovieScreenSize8_FullHD_50p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 30p\n", kNkMAIDMovieScreenSize8_FullHD_30p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 25p\n", kNkMAIDMovieScreenSize8_FullHD_25p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 24p\n", kNkMAIDMovieScreenSize8_FullHD_24p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 30p x4 Slow\n", kNkMAIDMovieScreenSize8_FullHD_30p_x4_Slow);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 25p x4 Slow\n", kNkMAIDMovieScreenSize8_FullHD_25p_x4_Slow);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 24p x5 Slow\n", kNkMAIDMovieScreenSize8_FullHD_24p_x5_Slow);
			strcat(psString, buff);
		}
		else if (g_ulCameraType == kNkMAIDCameraType_Z_7II || kNkMAIDCameraType_Z_6II_FU1)
		{
			sprintf(buff, "%d : 3840x2160 60p\n", kNkMAIDMovieScreenSize8_QFHD_60p);
			strcpy(psString, buff);
			sprintf(buff, "%d : 3840x2160 50p\n", kNkMAIDMovieScreenSize8_QFHD_50p);
			strcat(psString, buff);
			sprintf(buff, "%d : 3840x2160 30p\n", kNkMAIDMovieScreenSize8_QFHD_30p);
			strcat(psString, buff);
			sprintf(buff, "%d : 3840x2160 25p\n", kNkMAIDMovieScreenSize8_QFHD_25p);
			strcat(psString, buff);
			sprintf(buff, "%d : 3840x2160 24p\n", kNkMAIDMovieScreenSize8_QFHD_24p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 120p\n", kNkMAIDMovieScreenSize8_FullHD_120p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 100p\n", kNkMAIDMovieScreenSize8_FullHD_100p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 60p\n", kNkMAIDMovieScreenSize8_FullHD_60p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 50p\n", kNkMAIDMovieScreenSize8_FullHD_50p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 30p\n", kNkMAIDMovieScreenSize8_FullHD_30p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 25p\n", kNkMAIDMovieScreenSize8_FullHD_25p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 24p\n", kNkMAIDMovieScreenSize8_FullHD_24p);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 30p x4 Slow\n", kNkMAIDMovieScreenSize8_FullHD_30p_x4_Slow);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 25p x4 Slow\n", kNkMAIDMovieScreenSize8_FullHD_25p_x4_Slow);
			strcat(psString, buff);
			sprintf(buff, "%d : 1920x1080 24p x5 Slow\n", kNkMAIDMovieScreenSize8_FullHD_24p_x5_Slow);
			strcat(psString, buff);

		}
		break;
	case kNkMAIDCapability_LensType:
		if (ulValue & kNkMAIDLensType_D)
		{
			strcat(psString, "D Type, ");
		}
		if (ulValue & kNkMAIDLensType_G)
		{
			strcat(psString, "G Type, ");
		}
		if (ulValue & kNkMAIDLensType_VR)
		{
			strcat(psString, "VR, ");
		}
		if (ulValue & kNkMAIDLensType_DX)
		{
			strcat(psString, "DX, ");
		}
		if (ulValue & kNkMAIDLensType_AFS)
		{
			strcat(psString, "AF-S Lens, ");
		}
		if (ulValue & kNkMAIDLensType_AD)
		{
			strcat(psString, "Auto Distortion, ");
		}
		if (ulValue & kNkMAIDLensType_RET)
		{
			strcat(psString, "Retractable lens, ");
		}
		if (ulValue & kNkMAIDLensType_E)
		{
			strcat(psString, "E type, ");
		}
		if (ulValue & kNkMAIDLensType_STM)
		{
			strcat(psString, "STM, ");
		}
		if (ulValue & kNkMAIDLensType_CD)
		{
			strcat(psString, "Constantly Distortion, ");
		}
		strcat(psString, "\n");
		break;
	case kNkMAIDCapability_CameraType:
		if (ulValue == kNkMAIDCameraType_Z_6II)
		{
			strcat(psString, "Z 6II");
		}
        else if (ulValue== kNkMAIDCameraType_Z_7II)
        {
			strcat(psString, "Z 7II");
        }
		else if (ulValue == kNkMAIDCameraType_Z_6II_FU1)
		{
			strcat(psString, "Z 6II_FU1");
		}
		strcat(psString, "\n");
		break;

	case kNkMAIDCapability_UserMode1:
	case kNkMAIDCapability_UserMode2:
	case kNkMAIDCapability_UserMode3:
		sprintf(buff, "%d : Program auto\n", kNkMAIDUserMode_Program);
		strcpy(psString, buff);
		sprintf(buff, "%d : Shutter speed priority\n", kNkMAIDUserMode_SpeedPriority);
		strcat(psString, buff);
		sprintf(buff, "%d : Aperture priority\n", kNkMAIDUserMode_AperturePriority);
		strcat(psString, buff);
		sprintf(buff, "%d : Manual\n", kNkMAIDUserMode_Manual);
		strcat(psString, buff);
		sprintf(buff, "%d : Auto\n", kNkMAIDUserMode_Auto);
		strcat(psString, buff);
		break;


	default:
		psString[0] = '\0';
		break; 
	}
	return psString;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Distribute the function according to array type.
BOOL SetEnumCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet;
	NkMAIDEnum	stEnum;
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Enum ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if( bRet == FALSE ) return FALSE;

	switch ( stEnum.ulType ) {
		case kNkMAIDArrayType_Unsigned:
			return SetEnumUnsignedCapability( pRefObj, ulCapID, &stEnum );
			break;
		case kNkMAIDArrayType_PackedString:
			return SetEnumPackedStringCapability( pRefObj, ulCapID, &stEnum );
			break;
		case kNkMAIDArrayType_String:
			return SetEnumStringCapability( pRefObj, ulCapID, &stEnum );
			break;
		default:
			return FALSE;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a Enum(Unsigned Integer) type capability and set a value for it.
BOOL SetEnumUnsignedCapability( LPRefObj pRefObj, ULONG ulCapID, LPNkMAIDEnum pstEnum )
{
	BOOL	bRet;
	char	psString[64], buf[256];
	UWORD	wSel;
	ULONG	i;
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check the data of the capability.
	if ( pstEnum->wPhysicalBytes != 4 ) return FALSE;

	// check if this capability has elements.
	if( pstEnum->ulElements == 0 )
	{
		// This capablity has no element and is not available.
		printf( "There is no element in this capability. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
		return TRUE;
	}

	// allocate memory for array data
	pstEnum->pData = malloc( pstEnum->ulElements * pstEnum->wPhysicalBytes );
	if ( pstEnum->pData == NULL ) return FALSE;
	// get array data
	bRet = Command_CapGetArray( pRefObj->pObject, ulCapID, kNkMAIDDataType_EnumPtr, (NKPARAM)pstEnum, NULL, NULL );
	if( bRet == FALSE ) {
		free( pstEnum->pData );
		return FALSE;
	}

	// show selectable items for this capability and current setting
	printf( "[%s]\n", pCapInfo->szDescription );
	
	for ( i = 0; i < pstEnum->ulElements; i++ )
		printf( "%2d. %s\n", i + 1, GetEnumString( ulCapID, ((ULONG*)pstEnum->pData)[i], psString ) );
	printf( "Current Setting: %d\n", pstEnum->ulValue + 1 );

	// check if this capability supports CapSet operation.
	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input new value\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );
		if ( wSel > 0 && wSel <= pstEnum->ulElements ) {
			pstEnum->ulValue = wSel - 1;
			// send the selected number
			bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_EnumPtr, (NKPARAM)pstEnum, NULL, NULL );
			// This statement can be changed as follows.
			//bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_Unsigned, (NKPARAM)pstEnum->ulValue, NULL, NULL );
			if( bRet == FALSE ) {
				free( pstEnum->pData );
				return FALSE;
			}
		}
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	free( pstEnum->pData );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a Enum(Packed String) type capability and set a value for it.
BOOL SetEnumPackedStringCapability( LPRefObj pRefObj, ULONG ulCapID, LPNkMAIDEnum pstEnum )
{
	BOOL	bRet;
	char	*psStr, buf[256];
	UWORD	wSel;
	size_t  i;
	ULONG	ulCount = 0;
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check the data of the capability.
	if ( pstEnum->wPhysicalBytes != 1 ) return FALSE;

	// check if this capability has elements.
	if( pstEnum->ulElements == 0 )
	{
		// This capablity has no element and is not available.
		printf( "There is no element in this capability. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
		return TRUE;
	}

	// allocate memory for array data
	pstEnum->pData = malloc( pstEnum->ulElements * pstEnum->wPhysicalBytes );
	if ( pstEnum->pData == NULL ) return FALSE;
	// get array data
	bRet = Command_CapGetArray( pRefObj->pObject, ulCapID, kNkMAIDDataType_EnumPtr, (NKPARAM)pstEnum, NULL, NULL );
	if( bRet == FALSE ) {
		free( pstEnum->pData );
		return FALSE;
	}

	// show selectable items for this capability and current setting
	printf( "[%s]\n", pCapInfo->szDescription );
	for ( i = 0; i < pstEnum->ulElements; ) {
		psStr = (char*)((char*)pstEnum->pData + i);
		printf( "%2d. %s\n", ++ulCount, psStr );
		i += strlen( psStr ) + 1;
	}
	printf( "Current Setting: %d\n", pstEnum->ulValue + 1 );

	// check if this capability supports CapSet operation.
	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input new value\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );
		if ( wSel > 0 && wSel <= pstEnum->ulElements ) {
			pstEnum->ulValue = wSel - 1;
			// send the selected number
			bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_EnumPtr, (NKPARAM)pstEnum, NULL, NULL );
			// This statement can be changed as follows.
			//bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_Unsigned, (NKPARAM)pstEnum->ulValue, NULL, NULL );
			if( bRet == FALSE ) {
				free( pstEnum->pData );
				return FALSE;
			}
		}
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	free( pstEnum->pData );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a Enum(String Integer) type capability and set a value for it.
BOOL SetEnumStringCapability( LPRefObj pRefObj, ULONG ulCapID, LPNkMAIDEnum pstEnum )
{
	BOOL	bRet;
	char	buf[256];
	UWORD	wSel;
	ULONG	i;
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check the data of the capability.
	if ( pstEnum->wPhysicalBytes != 256 ) return FALSE;

	// check if this capability has elements.
	if( pstEnum->ulElements == 0 )
	{
		// This capablity has no element and is not available.
		printf( "There is no element in this capability. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
		return TRUE;
	}

	// allocate memory for array data
	pstEnum->pData = malloc( pstEnum->ulElements * pstEnum->wPhysicalBytes );
	if ( pstEnum->pData == NULL ) return FALSE;
	// get array data
	bRet = Command_CapGetArray( pRefObj->pObject, ulCapID, kNkMAIDDataType_EnumPtr, (NKPARAM)pstEnum, NULL, NULL );
	if( bRet == FALSE ) {
		free( pstEnum->pData );
		return FALSE;
	}

	// show selectable items for this capability and current setting
	printf( "[%s]\n", pCapInfo->szDescription );
	for ( i = 0; i < pstEnum->ulElements; i++ )
		printf( "%2d. %s\n", i + 1, ((NkMAIDString*)pstEnum->pData)[i].str );
	printf( "Current Setting: %2d\n", pstEnum->ulValue + 1 );

	// check if this capability supports CapSet operation.
	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input new value\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );
		if ( wSel > 0 && wSel <= pstEnum->ulElements ) {
			pstEnum->ulValue = wSel - 1;
			// send the selected number
			bRet =Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_EnumPtr, (NKPARAM)pstEnum, NULL, NULL );
			// This statement can be changed as follows.
			//bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_Unsigned, (NKPARAM)pstEnum->ulValue, NULL, NULL );
			if( bRet == FALSE ) {
				free( pstEnum->pData );
				return FALSE;
			}
		}
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	free( pstEnum->pData );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a Integer type capability and set a value for it.
BOOL SetIntegerCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet;
	SLONG	lValue;
	char	buf[256];
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Integer ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_IntegerPtr, (NKPARAM)&lValue, NULL, NULL );
	if( bRet == FALSE ) return FALSE;

	// show current value of this capability
	printf( "[%s]\n", pCapInfo->szDescription );
	printf( "Current Value: %d\n", lValue );

	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input new value\n>" );
		scanf( "%s", buf );
		lValue = (SLONG)atol( buf );
		bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_Integer, (NKPARAM)lValue, NULL, NULL );
		if( bRet == FALSE ) return FALSE;
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a Unsigned Integer type capability and set a value for it.
BOOL SetUnsignedCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet;
	ULONG	ulValue;
	char	buf[512];
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Unsigned ) return FALSE;
	// check if this capability suports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;

	ulValue = 0;
	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_UnsignedPtr, (NKPARAM)&ulValue, NULL, NULL );
	if( bRet == FALSE ) return FALSE;
	// show current value of this capability
	memset( buf, 0x00, sizeof(buf) );
	printf( "[%s]\n", pCapInfo->szDescription );
	printf( "%s", GetUnsignedString( ulCapID, ulValue, buf ) );
	printf("Current Value: %d\n", ulValue);

	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input new value\n>" );
		scanf( "%s", buf );
		ulValue = (ULONG)atol( buf );
		bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_Unsigned, (NKPARAM)ulValue, NULL, NULL );
		if( bRet == FALSE ) return FALSE;
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Get the current setting of a Unsigned Integer type capability.
BOOL GetUnsignedCapability( LPRefObj pRefObj, ULONG ulCapID, ULONG* pulValue )
{
    LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
    if( pCapInfo == NULL ) return FALSE;

    // check data type of the capability
    if( pCapInfo->ulType != kNkMAIDCapType_Unsigned ) return FALSE;
    // check if this capability suports CapGet operation.
    if( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;

    return Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_UnsignedPtr, ( NKPARAM )pulValue, NULL, NULL );
}
//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a Float type capability and set a value for it.
BOOL SetFloatCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet;
	double	lfValue;
	char	buf[256];
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Float ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_FloatPtr, (NKPARAM)&lfValue, NULL, NULL );
	if( bRet == FALSE ) return FALSE;
	// show current value of this capability
	printf( "[%s]\n", pCapInfo->szDescription );
	printf( "Current Value: %f\n", lfValue );

	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input new value\n>" );
		scanf( "%s", buf );
		lfValue = atof( buf );
		bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_FloatPtr, (NKPARAM)&lfValue, NULL, NULL );
		if( bRet == FALSE ) return FALSE;
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a String type capability and set a value for it.
BOOL SetStringCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet;
	NkMAIDString	stString;
	char szBuf[256] = { 0 };
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_String ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_StringPtr, (NKPARAM)&stString, NULL, NULL );
	if( bRet == FALSE ) return FALSE;
	// show current value of this capability
	printf( "[%s]\n", pCapInfo->szDescription );
	printf( "Current String: %s\n", stString.str );

	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input new string\n>" );
#if defined( _WIN32 )
		rewind(stdin);		// clear stdin
#elif defined(__APPLE__)
		fgets( szBuf, sizeof(szBuf), stdin );
#endif
		fgets( szBuf, sizeof(szBuf), stdin );
		sscanf( szBuf, "%s", (char*)stString.str );
		bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_StringPtr, (NKPARAM)&stString, NULL, NULL );
		if( bRet == FALSE ) return FALSE;
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", stString.str );
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a Size type capability and set a value for it.
BOOL SetSizeCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet;
	NkMAIDSize	stSize;
	char	buf[256];
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Size ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_SizePtr, (NKPARAM)&stSize, NULL, NULL );
	if( bRet == FALSE ) return FALSE;
	// show current value of this capability
	printf( "[%s]\n", pCapInfo->szDescription );
	printf( "Current Size: Width = %d    Height = %d\n", stSize.w, stSize.h );

	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input Width\n>" );
		scanf( "%s", buf );
		stSize.w = (ULONG)atol( buf );
		printf( "Input Height\n>" );
		scanf( "%s", buf );
		stSize.h = (ULONG)atol( buf );
		bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_SizePtr, (NKPARAM)&stSize, NULL, NULL );
		if( bRet == FALSE ) return FALSE;
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a DateTime type capability and set a value for it.
BOOL SetDateTimeCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet;
	NkMAIDDateTime	stDateTime;
	char	buf[256];
	UWORD	wValue;
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_DateTime ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_DateTimePtr, (NKPARAM)&stDateTime, NULL, NULL );
	if( bRet == FALSE ) return FALSE;
	// show current value of this capability
	printf( "[%s]\n", pCapInfo->szDescription );
	printf( "Current DateTime: %d/%02d/%4d %d:%02d:%02d\n",
		stDateTime.nMonth + 1, stDateTime.nDay, stDateTime.nYear, stDateTime.nHour, stDateTime.nMinute, stDateTime.nSecond );

	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input Month(1-12) or Cancel:'c'\n>" );
		scanf( "%s", buf );
		if ( *buf == 'c' || *buf == 'C') return TRUE;
		wValue = atoi( buf );
		if ( wValue >= 1 && wValue <= 12 )
			stDateTime.nMonth = wValue - 1;

		printf( "Input Day(1-31) or Cancel:'c'\n>" );
		scanf( "%s", buf );
		if ( *buf == 'c' || *buf == 'C') return TRUE;
		wValue = atoi( buf );
		if ( wValue >= 1 && wValue <= 31 )
			stDateTime.nDay = wValue;

		printf( "Input Year(4 digits) or Cancel:'c'\n>" );
		scanf( "%s", buf );
		if ( *buf == 'c' || *buf == 'C') return TRUE;
		wValue = atoi( buf );
		if ( wValue > 0 )
			stDateTime.nYear = wValue;

		printf( "Input Hour(0-23) or Cancel:'c'\n>" );
		scanf( "%s", buf );
		if ( *buf == 'c' || *buf == 'C') return TRUE;
		wValue = atoi( buf );
		if ( wValue >= 0 && wValue <= 23 )
			stDateTime.nHour = wValue;

		printf( "Input Minute(0-59) or Cancel:'c'\n>" );
		scanf( "%s", buf );
		if ( *buf == 'c' || *buf == 'C') return TRUE;
		wValue = atoi( buf );
		if ( wValue >= 0 && wValue <= 59 )
			stDateTime.nMinute = wValue;

		printf( "Input Second(0-59) or Cancel:'c'\n>" );
		scanf( "%s", buf );
		if ( *buf == 'c' || *buf == 'C') return TRUE;
		wValue = atoi( buf );
		if ( wValue >= 0 && wValue <= 59 )
			stDateTime.nSecond = wValue;

		bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_DateTimePtr, (NKPARAM)&stDateTime, NULL, NULL );
		if( bRet == FALSE ) return FALSE;
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a Boolean type capability and set a value for it.
BOOL SetBoolCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet;
	BYTE	bFlag;
	char	buf[256];
	UWORD	wSel;
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Boolean ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_BooleanPtr, (NKPARAM)&bFlag, NULL, NULL );
	if( bRet == FALSE ) return FALSE;
	// show current setting of this capability
	printf( "[%s]\n", pCapInfo->szDescription );
	printf( "1. On      2. Off\n" );
	printf( "Current Setting: %d\n", bFlag ? 1 : 2 );

	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input '1' or '2'\n>" );
		scanf( "%s", buf );
		wSel = atoi( buf );
		if ( (wSel == 1) || (wSel == 2) ) {
			bFlag = (wSel == 1) ? TRUE : FALSE;
			bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_Boolean, (NKPARAM)bFlag, NULL, NULL );
			if( bRet == FALSE ) return FALSE;
		}
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Show the current setting of a Range type capability and set a value for it.
BOOL SetRangeCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet;
	NkMAIDRange	stRange;
	double	lfValue;
	char	buf[256];
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Range ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;

	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_RangePtr, (NKPARAM)&stRange, NULL, NULL );
	if( bRet == FALSE ) return FALSE;
	// show current value of this capability
	printf( "[%s]\n", pCapInfo->szDescription );
	
	// check if this capability supports CapSet operation.
	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		if ( stRange.ulSteps == 0 ) {
			// the value of this capability is set to 'lfValue' directly
			printf( "Current Value: %f  (Max: %f  Min: %f)\n", stRange.lfValue, stRange.lfUpper, stRange.lfLower );
			printf( "Input new value.\n>" );
			scanf( "%s", buf );
			stRange.lfValue = atof( buf );
		} else {
			// the value of this capability is calculated from 'ulValueIndex'
			lfValue = stRange.lfLower + stRange.ulValueIndex * (stRange.lfUpper - stRange.lfLower) / (stRange.ulSteps - 1);
			printf( "Current Value: %f  (Max: %f  Min: %f)\n", lfValue, stRange.lfUpper, stRange.lfLower );
			printf( "Input new value.\n>" );
			scanf( "%s", buf );
			lfValue = atof( buf );
			stRange.ulValueIndex = (ULONG)((lfValue - stRange.lfLower) * (stRange.ulSteps - 1) / (stRange.lfUpper - stRange.lfLower));
		}
		bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_RangePtr, (NKPARAM)&stRange, NULL, NULL );
		if( bRet == FALSE ) return FALSE;
	} else {
		// This capablity is read-only.
		if ( stRange.ulSteps == 0 ) {
			// the value of this capability is set to 'lfValue' directly
			lfValue = stRange.lfValue;
		} else {
			// the value of this capability is calculated from 'ulValueIndex'
			lfValue = stRange.lfLower + stRange.ulValueIndex * (stRange.lfUpper - stRange.lfLower) / (stRange.ulSteps - 1);
		}
		printf( "Current Value: %f  (Max: %f  Min: %f)\n", lfValue, stRange.lfUpper, stRange.lfLower );
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------------------
// Distribute the function according to Point type.
BOOL SetPointCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet;
	NkMAIDPoint	stPoint;
	char	buf[256];
	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Point ) return FALSE;

	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// This capablity can be set.
		printf( "Input x\n>" );
		scanf( "%s", buf );
		stPoint.x = atoi( buf );
		printf( "Input y\n>" );
		scanf( "%s", buf );
		stPoint.y = atoi( buf );
		bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_PointPtr, (NKPARAM)&stPoint, NULL, NULL );
		if( bRet == FALSE ) return FALSE;
	} else {
		// This capablity is read-only.
		printf( "This value cannot be changed. Enter '0' to exit.\n>" );
		scanf( "%s", buf );
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Set/Get PictureControlDataCapability
BOOL PictureControlDataCapability(LPRefObj pRefSrc, ULONG ulCapID)
{
	char buf[256], filename[256];
	NkMAIDPicCtrlData stPicCtrlData;
	ULONG	ulSel, ulPicConNo, ulTemp = 0;
	BOOL	bRet = TRUE;

	memset(filename, 0x00, sizeof(filename));

	switch (ulCapID)
	{
	case kNkMAIDCapability_PictureControlDataEx2:
		strcpy(filename, "PicCtrlData.dat");
		break;
	case kNkMAIDCapability_MoviePictureControlDataEx2:
		strcpy(filename, "MovPicCtrlData.dat");
		break;
	}
	// sub command loop
	do {
		memset(&stPicCtrlData, 0, sizeof(NkMAIDPicCtrlData));

		printf("\nSelect (1-2, 0)\n");
		printf(" 1. Set Picture ControlEx2 data the file named \"%s\"\n", filename);
		printf(" 2. Get Picture ControlEx2 data\n");
		printf(" 0. Exit\n>");
		scanf("%s", buf);
		ulSel = (ULONG)atol(buf);
		switch (ulSel)
		{
		case 1://Set Picture Control data 
			printf("\nSelect Picture Control(1-37, 0)\n");
			printf(" 1. Standard                    2. Neutral\n");
			printf(" 3. Vivid                       4. Monochrome\n");
			printf(" 5. Portrait                    6. Landscape\n");
			printf(" 7. Flat                        8. Auto\n");
			printf(" 9. Dream                       10. Morning\n");
			printf(" 11. Pop                        12. Sunday\n");
			printf(" 13. Somber                     14. Dramatic\n");
			printf(" 15. Silence                    16. Breached\n");
			printf(" 17. Melancholic                18. Pure\n");
			printf(" 19. Denim                      20. Toy\n");
			printf(" 21. Sepia                      22. Blue\n");
			printf(" 23. Red                        24. Pink\n");
			printf(" 25. Charcoal                   26. Graphite\n");
			printf(" 27. Binary                     28. Carbon\n");
			printf(" 29. Custom Picture Contol 1    30. Custom Picture Contol 2 \n");
			printf(" 31. Custom Picture Contol 3    32. Custom Picture Contol 4 \n");
			printf(" 33. Custom Picture Contol 5    34. Custom Picture Contol 6 \n");
			printf(" 35. Custom Picture Contol 7    36. Custom Picture Contol 8 \n");
			printf(" 37. Custom Picture Contol 9\n");
			printf(" 0. Exit\n>");
			scanf("%s", buf);
			ulPicConNo = atoi(buf);
			if (ulPicConNo == 0) break; //Exit
			if (ulPicConNo < 1 || ulPicConNo > 37)
			{
				printf("Invalid Picture Control\n");
				return FALSE;
			}

			if (9 <= ulPicConNo && ulPicConNo <= 28)
			{
				ulPicConNo += 92; // eNkMAIDPictureControl 101:Dream - 120:Carbon 
			}
			else if (ulPicConNo >= 29)
			{
				ulPicConNo += 172; // Custom 201 - 209
			}

			// set target Picture Control
			stPicCtrlData.ulPicCtrlItem = ulPicConNo;

			// initial registration is not supported about 1-28
			if ((stPicCtrlData.ulPicCtrlItem >= 1 && stPicCtrlData.ulPicCtrlItem <= 28))
			{
				printf("\nSelect ModifiedFlag (1, 0)\n");
				printf(" 1. edit\n");
				printf(" 0. Exit\n>");
				scanf("%s", buf);
				ulTemp = atoi(buf);
				if (ulTemp == 0) break; // Exit
				if (ulTemp < 1 || ulTemp > 1)
				{
					printf("Invalid ModifiedFlag\n");
					break;
				}
				// set Modification flas
				stPicCtrlData.bModifiedFlag = TRUE;
			}
			else
			{
				printf("\nSelect ModifiedFlag (1-2, 0)\n");
				printf(" 1. initial registration          2. edit\n");
				printf(" 0. Exit\n>");
				scanf("%s", buf);
				ulTemp = atoi(buf);
				if (ulTemp == 0) break; // Exit
				if (ulTemp < 1 || ulTemp > 2)
				{
					printf("Invalid ModifiedFlag\n");
					break;
				}
				// set Modification flas
				stPicCtrlData.bModifiedFlag = (ulTemp == 1) ? FALSE : TRUE;
			}

			bRet = SetPictureControlDataCapability(pRefSrc, &stPicCtrlData, filename, ulCapID);
			break;

		case 2://Get Picture Control data
			printf("\nSelect Picture Control(1-37, 0)\n");
			printf(" 1. Standard                    2. Neutral\n");
			printf(" 3. Vivid                       4. Monochrome\n");
			printf(" 5. Portrait                    6. Landscape\n");
			printf(" 7. Flat                        8. Auto\n");
			printf(" 9. Dream                       10. Morning\n");
			printf(" 11. Pop                        12. Sunday\n");
			printf(" 13. Somber                     14. Dramatic\n");
			printf(" 15. Silence                    16. Breached\n");
			printf(" 17. Melancholic                18. Pure\n");
			printf(" 19. Denim                      20. Toy\n");
			printf(" 21. Sepia                      22. Blue\n");
			printf(" 23. Red                        24. Pink\n");
			printf(" 25. Charcoal                   26. Graphite\n");
			printf(" 27. Binary                     28. Carbon\n");
			printf(" 29. Custom Picture Contol 1    30. Custom Picture Contol 2 \n");
			printf(" 31. Custom Picture Contol 3    32. Custom Picture Contol 4 \n");
			printf(" 33. Custom Picture Contol 5    34. Custom Picture Contol 6 \n");
			printf(" 35. Custom Picture Contol 7    36. Custom Picture Contol 8 \n");
			printf(" 37. Custom Picture Contol 9\n");
			printf(" 0. Exit\n>");
			scanf("%s", buf);
			ulPicConNo = atoi(buf);
			if (ulPicConNo == 0) break; //Exit
			if (ulPicConNo < 1 || ulPicConNo > 37)
			{
				printf("Invalid Picture Control\n");
				return FALSE;
			}

			if (9 <= ulPicConNo && ulPicConNo <= 28)
			{
				ulPicConNo += 92; // eNkMAIDPictureControl 101:Dream - 120:Carbon 
			}
			else if (ulPicConNo >= 29)
			{
				ulPicConNo += 172; // Custom 201 - 209
			}

			// set target Picture Control
			stPicCtrlData.ulPicCtrlItem = ulPicConNo;

			bRet = GetPictureControlDataCapability(pRefSrc, &stPicCtrlData, ulCapID);
			break;
		default:
			ulSel = 0;
		}

		if (bRet == FALSE)
		{
			printf("An Error occured. \n");
		}
	} while (ulSel > 0);

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Set PictureControlDataCapability
BOOL SetPictureControlDataCapability(LPRefObj pRefObj, NkMAIDPicCtrlData* pPicCtrlData, char* filename, ULONG ulCapID)
{
	BOOL	bRet = TRUE;
	FILE	*stream;
	ULONG	count = 0;
	ULONG   ulTotal = 0;
	char	*ptr = NULL;

	if (ulCapID == kNkMAIDCapability_PictureControlDataEx2 ||
		ulCapID == kNkMAIDCapability_MoviePictureControlDataEx2)
	{
		LPNkMAIDCapInfo	pCapInfo = GetCapInfo(pRefObj, ulCapID);
		if (pCapInfo == NULL) return FALSE;

		// check data type of the capability
		if (pCapInfo->ulType != kNkMAIDCapType_Generic) return FALSE;
		// check if this capability supports CapSet operation.
		if (!CheckCapabilityOperation(pRefObj, ulCapID, kNkMAIDCapOperation_Set)) return FALSE;
	}

	// Read preset data from file.
	if ((stream = fopen(filename, "rb")) == NULL)
	{
		printf("\nfile open error.\n");
		return FALSE;
	}

	if (ulCapID == kNkMAIDCapability_PictureControlDataEx2 ||
		ulCapID == kNkMAIDCapability_MoviePictureControlDataEx2)
	{
		// Max Picture Control data size is 614
		// Allocate memory for Picture Control data.
		pPicCtrlData->pData = (char*)malloc(614);
		ptr = (char*)pPicCtrlData->pData;
		while (!feof(stream))
		{
			// Read file until eof.
			count = (ULONG)fread(ptr, sizeof(char), 100, stream);
			if (ferror(stream))
			{
				printf("\nfile read error.\n");
				fclose(stream);
				free(pPicCtrlData->pData);
				return FALSE;
			}
			/* Total count up actual bytes read */
			ulTotal += count;
			ptr += count;
			if (614 < ulTotal)
			{

				printf("\nThe size of File is over 614 byte.\n");
				fclose(stream);
				free(pPicCtrlData->pData);
				return FALSE;
			}
		}
	}
	pPicCtrlData->ulSize = ulTotal;

	if (ulCapID == kNkMAIDCapability_PictureControlDataEx2 ||
		ulCapID == kNkMAIDCapability_MoviePictureControlDataEx2)
	{
		// Set Picture Control data.
		bRet = Command_CapSet(pRefObj->pObject, ulCapID, kNkMAIDDataType_GenericPtr, (NKPARAM)pPicCtrlData, NULL, NULL);
		if (bRet == FALSE)
		{
			printf("\nFailed in setting Picture Contol Data.\n");
		}
	}
	else
	{
		// Set Picture Control data.
		bRet = Command_CapSet(pRefObj->pObject, ulCapID, kNkMAIDDataType_GenericPtr, (NKPARAM)pPicCtrlData, NULL, NULL);
		if (bRet == FALSE)
		{
			printf("\nFailed in setting Picture Contol DataEx.\n");
		}
	}

	fclose(stream);
	free(pPicCtrlData->pData);

	return bRet;
}

//------------------------------------------------------------------------------------------------------------------------------------
// Get PictureControlDataCapability
BOOL GetPictureControlDataCapability(LPRefObj pRefObj, NkMAIDPicCtrlData* pPicCtrlData, ULONG ulCapID)
{
	BOOL	bRet = TRUE;
	FILE	*stream = NULL;
	unsigned char* pucData = NULL;	// Picture Control Data pointer

	if (ulCapID == kNkMAIDCapability_PictureControlDataEx2 ||
		ulCapID == kNkMAIDCapability_MoviePictureControlDataEx2)
	{
		LPNkMAIDCapInfo	pCapInfo = GetCapInfo(pRefObj, ulCapID);
		if (pCapInfo == NULL) return FALSE;

		// check data type of the capability
		if (pCapInfo->ulType != kNkMAIDCapType_Generic) return FALSE;
		// check if this capability supports CapGet operation.
		if (!CheckCapabilityOperation(pRefObj, ulCapID, kNkMAIDCapOperation_Get)) return FALSE;
	}

	// Max Picture Control data size is 614
	// Allocate memory for Picture Control data.
	pPicCtrlData->ulSize = 614;
	pPicCtrlData->pData = (char*)malloc(614);

	if (ulCapID == kNkMAIDCapability_PictureControlDataEx2 ||
		ulCapID == kNkMAIDCapability_MoviePictureControlDataEx2)
	{
		// Get Picture Control data.
		bRet = Command_CapGet(pRefObj->pObject, ulCapID, kNkMAIDDataType_GenericPtr, (NKPARAM)pPicCtrlData, NULL, NULL);
		if (bRet == FALSE)
		{
			printf("\nFailed in getting Picture Control Data.\n");
			free(pPicCtrlData->pData);
			return FALSE;
		}
	}

	// Save to file
	// open file
	switch (ulCapID)
	{
	case kNkMAIDCapability_PictureControlDataEx2:
		stream = fopen("PicCtrlData.dat", "wb");
		break;
	case kNkMAIDCapability_MoviePictureControlDataEx2:
		stream = fopen("MovPicCtrlData.dat", "wb");
		break;
	}

	if (stream == NULL)
	{
		printf("\nfile open error.\n");
		free(pPicCtrlData->pData);
		return FALSE;
	}

	// Get data pointer
	pucData = (unsigned char*)pPicCtrlData->pData;

	// write file
	fwrite(pucData, 1, pPicCtrlData->ulSize, stream);
	switch (ulCapID)
	{
	case kNkMAIDCapability_PictureControlDataEx2:
		printf("\nPicCtrlData.dat was saved.\n");
		break;
	case kNkMAIDCapability_MoviePictureControlDataEx2:
		printf("\nMovPicCtrlData.dat was saved.\n");
		break;
	}

	// close file
	fclose(stream);
	free(pPicCtrlData->pData);

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------------------
// Set TimeCodeOrigin.
BOOL TimeCodeOriginCapability(LPRefObj pRefSrc)
{
	BOOL	bRet;
	NkMAIDTimeCodeOrigin stTimeCode;
	int		i = 0;
	char	buf[256];
	ULONG	ulValue;

	memset(&stTimeCode, 0, sizeof(NkMAIDTimeCodeOrigin));

	//Check Operation
	LPNkMAIDCapInfo pCapInfo = GetCapInfo(pRefSrc, kNkMAIDCapability_TimeCodeOrigin);
	if (pCapInfo == NULL) return FALSE;

	// check data type of the capability
	if (pCapInfo->ulType != kNkMAIDCapType_Generic) return FALSE;
	// check if this capability supports CapGet operation.
	if (!CheckCapabilityOperation(pRefSrc, kNkMAIDCapability_TimeCodeOrigin, kNkMAIDCapOperation_Get)) return FALSE;


	bRet = Command_CapGet(pRefSrc->pObject, kNkMAIDCapability_TimeCodeOrigin, kNkMAIDDataType_GenericPtr, (NKPARAM)&stTimeCode, NULL, NULL);
	if (bRet == FALSE) return FALSE;
	// show current value of this capability

	printf("[%s]\n", pCapInfo->szDescription);
	printf("%d : Reset\n", i );
	printf("%d : Manual\n", i + 1);
	printf("%d : Current time\n", i + 2 );
	printf("Current Setting: %d\n", stTimeCode.ucTimeCodeInfo);
	if (stTimeCode.ucTimeCodeInfo == 1){
	//ManualSetting
		printf("Setting Value : %02d:%02d:%02d,%02d\n", stTimeCode.ucTimeCode[0], stTimeCode.ucTimeCode[1], stTimeCode.ucTimeCode[2], stTimeCode.ulFrame);
	}

	if (CheckCapabilityOperation(pRefSrc, kNkMAIDCapability_TimeCodeOrigin, kNkMAIDCapOperation_Set)) {
		// This capablity can be set.
		printf("Input new TimeCodeInfo\n>");
		scanf("%s", buf);
		ulValue = (ULONG)(atoi(buf));
		stTimeCode.ucTimeCodeInfo = (UCHAR)ulValue;
		if (stTimeCode.ucTimeCodeInfo == 1){
			do{
				printf("Input Hour(0-12) or Cancel:'c'\n>");
				scanf("%s", buf);
				if (*buf == 'c' || *buf == 'C') return TRUE;
				ulValue = (ULONG)(atoi(buf));
				if (ulValue <= 12){
					stTimeCode.ucTimeCode[0] = (UCHAR)ulValue;
				}else{
					printf("Value is out of range.Please retry.\n");
				}
			} while (12<ulValue);
			do{
				printf("Input Minite(0-59) or Cancel:'c'\n>");
				scanf("%s", buf);
				if (*buf == 'c' || *buf == 'C') return TRUE;
				ulValue = (ULONG)(atoi(buf));
				if (ulValue <= 59){
					stTimeCode.ucTimeCode[1] = (UCHAR)ulValue;
				}else{
					printf("Value is out of range.Please retry.\n");
				}
			} while (59<ulValue);
			do{
				printf("Input Second(0-59) or Cancel:'c'\n>");
				scanf("%s", buf);
				if (*buf == 'c' || *buf == 'C') return TRUE;
				ulValue = (ULONG)(atoi(buf));
				if (ulValue <= 59){
					stTimeCode.ucTimeCode[2] = (UCHAR)ulValue;
				}else{
					printf(" Value is out of range.Please retry.\n");
				}
			} while (59<ulValue);
			do{
				printf("Input Frame(0-29) or Cancel:'c'\n>");
				scanf("%s", buf);
				if (*buf == 'c' || *buf == 'C') return TRUE;
				ulValue = (ULONG)(atoi(buf));
				if (ulValue <= 29){
					stTimeCode.ulFrame = (UCHAR)ulValue;
				}else{
					printf(" Value is out of range.Please retry.\n");
				}
			} while (29<ulValue);
		}
		bRet = Command_CapSet(pRefSrc->pObject, kNkMAIDCapability_TimeCodeOrigin, kNkMAIDDataType_GenericPtr, (NKPARAM)&stTimeCode, NULL, NULL);
		if (bRet == FALSE) return FALSE;
	}else {
			// This capablity is read-only.
			printf("This value cannot be changed. Enter '0' to exit.\n>");
			scanf("%s", buf);
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Get ManualSettingLensData.
BOOL GetManualSettingLensDataCapability(LPRefObj pRefSrc)
{
	BOOL	bRet;
	NkMAIDGetManualSettingLensData stManualSettingLensData;
	GetLensDataInfo	stGetLensDataInfo;
	char	buf[256];
	ULONG	ulValue;

	memset(&stManualSettingLensData, 0, sizeof(NkMAIDGetManualSettingLensData));

	//Check Operation
	LPNkMAIDCapInfo pCapInfo = GetCapInfo(pRefSrc, kNkMAIDCapability_GetManualSettingLensData);
	if (pCapInfo == NULL) return FALSE;

	// check data type of the capability
	if (pCapInfo->ulType != kNkMAIDCapType_Generic) return FALSE;
	// check if this capability supports CapGet operation.
	if (!CheckCapabilityOperation(pRefSrc, kNkMAIDCapability_GetManualSettingLensData, kNkMAIDCapOperation_Get)) return FALSE;

	printf("Input Lens number(1-20).If you watch all settings, Input '100'\n");
	scanf("%s", buf);
	if (strcmp(buf,"100")==0){
		ulValue = 0xFFFFFFFF;
	}
	else
	{
		ulValue = atoi(buf)-1;
	}
	stManualSettingLensData.ulLensID = ulValue;
	bRet = Command_CapGet(pRefSrc->pObject, kNkMAIDCapability_GetManualSettingLensData, kNkMAIDDataType_GenericPtr, (NKPARAM)&stManualSettingLensData, NULL, NULL);
	if (bRet == FALSE) return FALSE;
	// show current value of this capability

	stManualSettingLensData.pData = (void*)malloc(stManualSettingLensData.ulSize);
	if (stManualSettingLensData.pData == NULL)
	{
		// abnomal.
		printf("\nMemory allocation error.\n");

		return FALSE; 
	}
	bRet = Command_CapGet(pRefSrc->pObject, kNkMAIDCapability_GetManualSettingLensData, kNkMAIDDataType_GenericPtr, (NKPARAM)&stManualSettingLensData, NULL, NULL);
	if (bRet == FALSE) return FALSE;

	stGetLensDataInfo = *((GetLensDataInfo*)stManualSettingLensData.pData);
	ConvertLensDataString(ulValue, &stGetLensDataInfo);
				
	// This capablity is read-only.
	printf("This value cannot be changed. Enter '0' to exit.\n>");
	scanf("%s", buf);
	
	return TRUE;

}
//------------------------------------------------------------------------------------------------------------------------------------
// Convert LensDataString.
BOOL ConvertLensDataString(ULONG ulLensID, LPGetLensDataInfo pGetLensDataInfo)
{
	UCHAR	i = 0;

	for (i = 0; i < (pGetLensDataInfo->ulCount); i++){

		if (ulLensID == 0xFFFFFFFF){
			printf("No.%2d ", i + 1);
		}
		else{
			printf("No.%2d ", ulLensID + 1);
		}
		switch (pGetLensDataInfo->pSettingLensData[i].ucFmmManualSetting)
		{
		case kNkMAIDFmmManualSetting_NoSettting:
			printf("Focal length : NoSetting ");
			break;
		case kNkMAIDFmmManualSetting_6mm:
			printf("Focal length : 6  mm ");
			break;
		case kNkMAIDFmmManualSetting_8mm:
			printf("Focal length : 8  mm ");
			break;
		case kNkMAIDFmmManualSetting_13mm:
			printf("Focal length : 13 mm ");
			break;
		case kNkMAIDFmmManualSetting_15mm:
			printf("Focal length : 15 mm ");
			break;
		case kNkMAIDFmmManualSetting_16mm:
			printf("Focal length : 16 mm ");
			break;
		case kNkMAIDFmmManualSetting_18mm:
			printf("Focal length : 18 mm ");
			break;
		case kNkMAIDFmmManualSetting_20mm:
			printf("Focal length : 20 mm ");
			break;
		case kNkMAIDFmmManualSetting_24mm:
			printf("Focal length : 24 mm ");
			break;
		case kNkMAIDFmmManualSetting_25mm:
			printf("Focal length : 25 mm ");
			break;
		case kNkMAIDFmmManualSetting_28mm:
			printf("Focal length : 28 mm ");
			break;
		case kNkMAIDFmmManualSetting_35mm:
			printf("Focal length : 35 mm ");
			break;
		case kNkMAIDFmmManualSetting_43mm:
			printf("Focal length : 43 mm ");
			break;
		case kNkMAIDFmmManualSetting_45mm:
			printf("Focal length : 45 mm ");
			break;
		case kNkMAIDFmmManualSetting_50mm:
			printf("Focal length : 50 mm ");
			break;
		case kNkMAIDFmmManualSetting_55mm:
			printf("Focal length : 55 mm ");
			break;
		case kNkMAIDFmmManualSetting_58mm:
			printf("Focal length : 58 mm ");
			break;
		case kNkMAIDFmmManualSetting_70mm:
			printf("Focal length : 70 mm ");
			break;
		case kNkMAIDFmmManualSetting_80mm:
			printf("Focal length : 80 mm ");
			break;
		case kNkMAIDFmmManualSetting_85mm:
			printf("Focal length : 85 mm ");
			break;
		case kNkMAIDFmmManualSetting_86mm:
			printf("Focal length : 86 mm ");
			break;
		case kNkMAIDFmmManualSetting_100mm:
			printf("Focal length : 100mm ");
			break;
		case kNkMAIDFmmManualSetting_105mm:
			printf("Focal length : 105mm ");
			break;
		case kNkMAIDFmmManualSetting_135mm:
			printf("Focal length : 135mm ");
			break;
		case kNkMAIDFmmManualSetting_180mm:
			printf("Focal length : 180mm ");
			break;
		case kNkMAIDFmmManualSetting_200mm:
			printf("Focal length : 200mm ");
			break;
		case kNkMAIDFmmManualSetting_300mm:
			printf("Focal length : 300mm ");
			break;
		case kNkMAIDFmmManualSetting_360mm:
			printf("Focal length : 360mm ");
			break;
		case kNkMAIDFmmManualSetting_400mm:
			printf("Focal length : 400mm ");
			break;
		case kNkMAIDFmmManualSetting_500mm:
			printf("Focal length : 500mm ");
			break;
		case kNkMAIDFmmManualSetting_600mm:
			printf("Focal length : 600mm ");
			break;
		case kNkMAIDFmmManualSetting_800mm:
			printf("Focal length : 800mm ");
			break;
		case kNkMAIDFmmManualSetting_1000mm:
			printf("Focal length : 1000mm ");
			break;
		case kNkMAIDFmmManualSetting_1200mm:
			printf("Focal length : 1200mm ");
			break;
		case kNkMAIDFmmManualSetting_1400mm:
			printf("Focal length : 1400mm ");
			break;
		case kNkMAIDFmmManualSetting_1600mm:
			printf("Focal length : 1600mm ");
			break;
		case kNkMAIDFmmManualSetting_2000mm:
			printf("Focal length : 2000mm ");
			break;
		case kNkMAIDFmmManualSetting_2400mm:
			printf("Focal length : 2400mm ");
			break;
		case kNkMAIDFmmManualSetting_2800mm:
			printf("Focal length : 2800mm ");
			break;
		case kNkMAIDFmmManualSetting_3200mm:
			printf("Focal length : 3200mm ");
			break;
		case kNkMAIDFmmManualSetting_4000mm:
			printf("Focal length : 4000mm ");
			break;
		default:
			printf("Focal length : Out Of Range ");
			break;
		}

		switch (pGetLensDataInfo->pSettingLensData[i].ucF0ManualSetting)
		{
		case kNkMAIDF0ManualSetting_NoSettting:
			printf("F Number : NoSetting\n");
			break;
		case kNkMAIDF0ManualSetting_F1_2:
			printf("F Number : F1.2\n");
			break;
		case kNkMAIDF0ManualSetting_F1_4:
			printf("F Number : F1.4\n");
			break;
		case kNkMAIDF0ManualSetting_F1_8:
			printf("F Number : F1.8\n");
			break;
		case kNkMAIDF0ManualSetting_F2_0:
			printf("F Number : F2.0\n");
			break;
		case kNkMAIDF0ManualSetting_F2_5:
			printf("F Number : F2.5\n");
			break;
		case kNkMAIDF0ManualSetting_F2_8:
			printf("F Number : F2.8\n");
			break;
		case kNkMAIDF0ManualSetting_F3_3:
			printf("F Number : F3.3\n");
			break;
		case kNkMAIDF0ManualSetting_F3_5:
			printf("F Number : F3.5\n");
			break;
		case kNkMAIDF0ManualSetting_F4_0:
			printf("F Number : F4.0\n");
			break;
		case kNkMAIDF0ManualSetting_F4_5:
			printf("F Number : F4.5\n");
			break;
		case kNkMAIDF0ManualSetting_F5_0:
			printf("F Number : F5.0\n");
			break;
		case kNkMAIDF0ManualSetting_F5_6:
			printf("F Number : F5.6\n");
			break;
		case kNkMAIDF0ManualSetting_F6_3:
			printf("F Number : F6.3\n");
			break;
		case kNkMAIDF0ManualSetting_F7_1:
			printf("F Number : F7.1\n");
			break;
		case kNkMAIDF0ManualSetting_F8_0:
			printf("F Number : F8.0\n");
			break;
		case kNkMAIDF0ManualSetting_F9_5:
			printf("F Number : F9.5\n");
			break;
		case kNkMAIDF0ManualSetting_F11:
			printf("F Number : F11\n");
			break;
		case kNkMAIDF0ManualSetting_F13:
			printf("F Number : F13\n");
			break;
		case kNkMAIDF0ManualSetting_F15:
			printf("F Number : F15\n");
			break;
		case kNkMAIDF0ManualSetting_F16:
			printf("F Number : F16\n");
			break;
		case kNkMAIDF0ManualSetting_F19:
			printf("F Number : F19\n");
			break;
		case kNkMAIDF0ManualSetting_F22:
			printf("F Number : F22\n");
			break;
		default:
			printf("F Number : Out Of Range\n");
			break;
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Set TrackingAFArea.
BOOL SetTrackingAFAreaCapability(LPRefObj pRefSrc)
{
	BOOL	bRet;
	NkMAIDTrackingAFArea stTrackingAFArea;
	char	buf[256];
	UCHAR	ucValue;

	memset(&stTrackingAFArea, 0, sizeof(NkMAIDTrackingAFArea));

	//Check Operation
	LPNkMAIDCapInfo pCapInfo = GetCapInfo(pRefSrc, kNkMAIDCapability_TrackingAFArea);
	if (pCapInfo == NULL) return FALSE;

	// check data type of the capability
	if (pCapInfo->ulType != kNkMAIDCapType_Generic) return FALSE;

	if (!CheckCapabilityOperation(pRefSrc, kNkMAIDCapability_TrackingAFArea, kNkMAIDCapOperation_Set)) {
		// This capablity is read-only.
		printf("This value cannot be changed. Enter '0' to exit.\n>");
		scanf("%s", buf);
		return FALSE;
	}

	printf("1. Stop Tracking \n");
	printf("2. Start Tracking \n");
	printf("Input Number \n>");
	scanf("%s", buf);
	ucValue = (UCHAR)(atoi(buf));
	if (ucValue == 1)
	{
		stTrackingAFArea.ulTrackingStatus = 0;
	}
	else
	if (ucValue == 2)
	{
		stTrackingAFArea.ulTrackingStatus = 1;
		printf("Input XY coordinates \n");
		printf("X = \n");
		scanf("%s", buf);
		stTrackingAFArea.stAfPoint.x = atoi(buf);
		printf("Y = \n");
		scanf("%s", buf);
		stTrackingAFArea.stAfPoint.y = atoi(buf);
	}
	else
	{
		printf("Invalid Number\n");
		return FALSE;
	}


	// This capablity can be set.
	bRet = Command_CapSet(pRefSrc->pObject, kNkMAIDCapability_TrackingAFArea, kNkMAIDDataType_GenericPtr, (NKPARAM)&stTrackingAFArea, NULL, NULL);
	if (bRet == FALSE) return FALSE;

	return TRUE;

}


//------------------------------------------------------------------------------------------------------------------------------------
//Delete Dram Image
BOOL DeleteDramCapability( LPRefObj pRefItem, ULONG ulItmID )
{
	LPRefObj	pRefSrc = (LPRefObj)pRefItem->pRefParent;
	LPRefObj	pRefDat = NULL;
	BOOL	bRet = TRUE;
	NkMAIDCallback	stProc;
	LPRefDataProc	pRefDeliver;
	LPRefCompletionProc	pRefCompletion;
	ULONG	ulCount = 0L;
	SLONG nResult;


	// 1. Open ImageObject
	pRefDat = GetRefChildPtr_ID( pRefItem, kNkMAIDDataObjType_Image );
	if ( pRefDat == NULL )
	{
		// Create Image object and RefSrc structure.
		if ( AddChild( pRefItem, kNkMAIDDataObjType_Image ) == FALSE )
		{
			printf("Image object can't be opened.\n");
			return FALSE;
		}
		pRefDat = GetRefChildPtr_ID( pRefItem, kNkMAIDDataObjType_Image );
	}

	// 2. Set DataProc function
	// 2-1. set reference from DataProc
	pRefDeliver = (LPRefDataProc)malloc( sizeof(RefDataProc) );// this block will be freed in CompletionProc.
	pRefDeliver->pBuffer = NULL;
	pRefDeliver->ulOffset = 0L;
	pRefDeliver->ulTotalLines = 0L;
	pRefDeliver->lID = pRefItem->lMyID;
	// 2-2. set reference from CompletionProc
	pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );// this block will be freed in CompletionProc.
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = pRefDeliver;
	// 2-3. set reference from DataProc
	stProc.pProc = (LPNKFUNC)DataProc;
	stProc.refProc = (NKREF)pRefDeliver;
	// 2-4. set DataProc as data delivery callback function
	if( CheckCapabilityOperation( pRefDat, kNkMAIDCapability_DataProc, kNkMAIDCapOperation_Set ) )
	{
		bRet = Command_CapSet( pRefDat->pObject, kNkMAIDCapability_DataProc, kNkMAIDDataType_CallbackPtr, (NKPARAM)&stProc, NULL, NULL );
		if ( bRet == FALSE ) return FALSE;
	} 
	else
	{
		return FALSE;
	}
		
	// 3. Acquire image
	bRet = Command_CapStart( pRefDat->pObject, kNkMAIDCapability_Acquire, (LPNKFUNC)CompletionProc, (NKREF)pRefCompletion, &nResult );
	if ( bRet == FALSE ) return FALSE;

	if (nResult == kNkMAIDResult_NoError)
	{
		// image had read before issuing delete command.
		printf("\nInternal ID [0x%08X] had read before issuing delete command.\n", ulItmID );
	}
	else
	if (nResult == kNkMAIDResult_Pending)
	{
		// 4. Async command
		bRet = Command_Async( pRefDat->pObject );
		if ( bRet == FALSE ) return FALSE;

		// 5. Abort
		bRet = Command_Abort( pRefDat->pObject, NULL, NULL);
		if ( bRet == FALSE ) return FALSE;
		
		// 6. Set Item ID
		bRet = Command_CapSet( pRefSrc->pObject, kNkMAIDCapability_CurrentItemID, kNkMAIDDataType_Unsigned, (NKPARAM)ulItmID, NULL, NULL );
		if ( bRet == FALSE ) return FALSE;

		// 7. Delete DRAM (Delete timing No.2)
		bRet = Command_CapStart( pRefSrc->pObject, kNkMAIDCapability_DeleteDramImage, NULL, NULL, NULL );
		if ( bRet == FALSE ) return FALSE;

		// 8. Reset DataProc
		bRet = Command_CapSet( pRefDat->pObject, kNkMAIDCapability_DataProc, kNkMAIDDataType_Null, (NKPARAM)NULL, NULL, NULL );
		if ( bRet == FALSE ) return FALSE;

		printf("\nInternal ID [0x%08X] was deleted.\n", ulItmID );
	}
	
	// Upper function to close ItemObject. 
	g_bFileRemoved = TRUE;
	// progress proc flag reset 
	g_bFirstCall = TRUE;

	// 9. Close ImageObject
	bRet = RemoveChild( pRefItem, kNkMAIDDataObjType_Image );


	return bRet;
}

//------------------------------------------------------------------------------------------------------------------------------------
// Get Live view image
BOOL GetLiveViewImageCapability( LPRefObj pRefSrc )
{
	char	HeaderFileName[256], ImageFileName[256];
	FILE*	hFileHeader = NULL;		// LiveView Image file name
	FILE*	hFileImage = NULL;		// LiveView header file name
	ULONG	ulHeaderSize = 0;		//The header size of LiveView
	NkMAIDArray	stArray;
	int i = 0;
	unsigned char* pucData = NULL;	// LiveView data pointer
	BOOL	bRet = TRUE;


	// Set header size of LiveView
	ulHeaderSize = 512;

	memset( &stArray, 0, sizeof(NkMAIDArray) );		
	
	bRet = GetArrayCapability( pRefSrc, kNkMAIDCapability_GetLiveViewImage, &stArray );
	if ( bRet == FALSE ) return FALSE;
		
	// create file name
	while( TRUE )
	{
		sprintf( HeaderFileName, "LiveView%03d_H.%s", ++i, "dat" );
		sprintf( ImageFileName, "LiveView%03d.%s", i, "jpg" );
		if ( (hFileHeader = fopen(HeaderFileName, "r") ) != NULL ||
				(hFileImage  = fopen(ImageFileName, "r") )  != NULL )
		{
			// this file name is already used.
			if (hFileHeader)
			{
				fclose( hFileHeader );
				hFileHeader = NULL;
			}
			if (hFileImage)
			{
				fclose( hFileImage );
				hFileImage = NULL;		
			}
		}	 
		else
		{
			break;
		}
	}
		
	// open file
	hFileHeader = fopen( HeaderFileName, "wb" );
	if ( hFileHeader == NULL )
	{
		printf("file open error.\n");
		return FALSE;
	}				
	hFileImage = fopen( ImageFileName, "wb" );
	if ( hFileImage == NULL )
	{
		fclose( hFileHeader );
		printf("file open error.\n");
		return FALSE;
	}
	
	// Get data pointer
	pucData = (unsigned char*)stArray.pData;

	// write file
	fwrite( pucData, 1, ulHeaderSize, hFileHeader );
	fwrite( pucData+ulHeaderSize, 1, (stArray.ulElements-ulHeaderSize), hFileImage );
	printf("\n%s was saved.\n", HeaderFileName);
	printf("%s was saved.\n", ImageFileName);
		
	// close file
	fclose( hFileHeader );
	fclose( hFileImage );
	free( stArray.pData );

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------------------
// Delete Custom Picture Control
BOOL DeleteCustomPictureControlCapability(LPRefObj pRefSrc, ULONG ulCapID)
{
	ULONG	ulSel = 0;
	BOOL	bRet;
	ULONG	ulValue;
	char	buf[256];

	printf( "\nSelect Custom Picture Control to delete.(1-9, 0)\n");

	printf( "1. Custom Picture Contol 1\n");
	printf( "2. Custom Picture Contol 2\n");
	printf( "3. Custom Picture Contol 3\n");
	printf( "4. Custom Picture Contol 4\n");
	printf( "5. Custom Picture Contol 5\n");
	printf( "6. Custom Picture Contol 6\n");
	printf( "7. Custom Picture Contol 7\n");
	printf( "8. Custom Picture Contol 8\n");
	printf( "9. Custom Picture Contol 9\n");
	printf( "0. Exit\n>" );
	scanf( "%s", buf );

	ulSel = atoi( buf );
	if ( ulSel == 0 ) return TRUE; // Exit
	if ( ulSel < 1 || ulSel > 9 ) 
	{
		printf("Invalid Custom Picture Control\n");
		return FALSE;
	}
	ulSel += 200;		// Custom 201 - 209
	ulValue = ulSel;	// Set Custom Picture Control to delete
	if (CheckCapabilityOperation(pRefSrc, ulCapID, kNkMAIDCapOperation_Set)){
		bRet = Command_CapSet(pRefSrc->pObject, ulCapID, kNkMAIDDataType_Unsigned, (NKPARAM)ulValue, NULL, NULL);
		return bRet;
	}
	else{
		return FALSE;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------
// Get SBHandlesCapability
BOOL GetSBHandlesCapability( LPRefObj pRefObj )
{
	BOOL					bRet = TRUE;
	char					buf[256] = {0};
	ULONG					ulSel;
	NkMAIDGetSBHandles		stSbHandles;
	LPNkMAIDCapInfo			pCapInfo = NULL;
	FILE					*fileStream;

	// To clear the structure.
	memset( &stSbHandles, 0, sizeof( NkMAIDGetSBHandles ));

	// To display the menu.
	printf( "\nSelect SBGroupID(1-8, 0)\n" );
	printf( " 1. SBGroupID ALL\n" );
	printf( " 2. SBGroupID Master\n" );
	printf( " 3. SBGroupID A\n" );
	printf( " 4. SBGroupID B\n" );
	printf( " 5. SBGroupID C\n" );
	printf( " 6. SBGroupID D\n" );
	printf( " 7. SBGroupID E\n" );
	printf( " 8. SBGroupID F\n" );
	printf( " 0. Exit\n>" );
	scanf( "%s", buf );
	ulSel = ( ULONG )atoi( buf );

	// Check the input value.
	if(( ulSel == 0 ) || ( ulSel > 8))
	{
		return TRUE;
	}

	pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_GetSBHandles );
	if( pCapInfo == NULL )
	{
		return FALSE;
	}

	// check data type of the capability
	if( pCapInfo->ulType != kNkMAIDCapType_Generic )
	{
		return FALSE;
	}

	// check if this capability supports CapGet operation.
	if( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_GetSBHandles, kNkMAIDCapOperation_Get ) )
	{
		return FALSE;
	}

	// 1.To get the SB handle the total number.
	{
		// ulSBGroupID：To specify the SBGroupID to be acquisition target.
		switch( ulSel )
		{
			// SBGroupID ALL
			case 1:
				stSbHandles.ulSBGroupID = kNkMAIDSBGroupID_ALL;
				break;
			// SBGroupID Master
			case 2:
				stSbHandles.ulSBGroupID = kNkMAIDSBGroupID_Master;
				break;
			// SBGroupID A
			case 3:
				stSbHandles.ulSBGroupID = kNkMAIDSBGroupID_A;
				break;
			// SBGroupID B
			case 4:
				stSbHandles.ulSBGroupID = kNkMAIDSBGroupID_B;
				break;
			// SBGroupID C
			case 5:
				stSbHandles.ulSBGroupID = kNkMAIDSBGroupID_C;
				break;
			// SBGroupID D
			case 6:
				stSbHandles.ulSBGroupID = kNkMAIDSBGroupID_D;
				break;
			// SBGroupID E
			case 7:
				stSbHandles.ulSBGroupID = kNkMAIDSBGroupID_E;
				break;
			// SBGroupID F
			case 8:
				stSbHandles.ulSBGroupID = kNkMAIDSBGroupID_F;
				break;
			default:
				stSbHandles.ulSBGroupID = 0;
				break;
		}

		// ulNumber：0 is specified.
		stSbHandles.ulNumber = 0;

		// To get the SB handle the total number.
		bRet = Command_CapGet( pRefObj->pObject, kNkMAIDCapability_GetSBHandles, kNkMAIDDataType_GenericPtr, ( NKPARAM )&stSbHandles, NULL, NULL );
		if( bRet == FALSE )
		{
			// abnomal.
			printf( "\nFailed in getting SBHandles.\n" );

			return FALSE;
		}
	}

	// 2.To get the SB handle list .
	{
		// ulSBGroupID：To specify the SBGroupID to be acquisition target. ( Already set )
		// ulNumber：The total number of SBHandle to be an acquisition target. ( Already set )
		// ulSize：Buffer Isaizu for the acquisition to SBHandle list set to pData. ( Already set )
		// pData：Data pointer for the acquisition to SBHandle list.
		// To ensure the memory area.
		stSbHandles.pData = ( char * )malloc( stSbHandles.ulSize );
		if( stSbHandles.pData == NULL )
		{
			// abnomal.
			printf( "\nMemory allocation error.\n" );

			return FALSE;
		}

		// To get the SB handle list .
		bRet = Command_CapGet( pRefObj->pObject, kNkMAIDCapability_GetSBHandles, kNkMAIDDataType_GenericPtr, ( NKPARAM )&stSbHandles, NULL, NULL );
		if( bRet == FALSE )
		{
			// abnomal.
			printf( "\nFailed in getting SBHandles.\n" );
			free( stSbHandles.pData );

			return FALSE;
		}
	}

	// Save to file
	// open file
	fileStream = fopen( "SBHandles.dat", "wb" );
	if( fileStream == NULL )
	{
		// abnomal.
		printf( "\nfile open error.\n" );
		free( stSbHandles.pData );

		return FALSE;
	}

	// Get data pointer
	unsigned char* pucData = NULL;
	pucData = ( unsigned char* )stSbHandles.pData;

	// write file
	fwrite( pucData, 1, stSbHandles.ulSize, fileStream );
	printf( "\nSBHandles.dat was saved.\n" );

	// close file
	fclose( fileStream );
	free( stSbHandles.pData );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Get SBAttrDesc
BOOL GetSBAttrDescCapability( LPRefObj pRefObj )
{
	BOOL					bRet = TRUE;
	char					buf[256] = { 0 };
	ULONG					ulSBHandleMenu = 0;
	ULONG					ulSBHandle = 0;
	ULONG					ulSBAttrID = 0;
	FILE					*fileStream;
	NkMAIDGetSBAttrDesc		stSbAttrDesc;
	LPNkMAIDCapInfo			pCapInfo = NULL;
	SLONG nResult = kNkMAIDResult_NoError;

	// To clear the structure.
	memset( &stSbAttrDesc, 0, sizeof( NkMAIDGetSBAttrDesc ));

	// To display the menu.
	printf( "\nSelect SBHandle Menu(0-1, 2)\n" );
	printf( " 0. All\n" );
	printf( " 1. Any value\n" );
	printf( " 2. Exit\n>" );
	scanf( "%s", buf );
	ulSBHandleMenu = ( ULONG )atoi( buf );

	// Check the input value.
	if( ulSBHandleMenu >= 2 )
	{
		return TRUE;
	}
	else if( ulSBHandleMenu == 1 )
	{
		// To display the menu.
		printf( "\nEnter SBHandle Value(Hex)\n" );
		printf( "0x" );
		scanf( "%s", buf );

		char *pcErrbuf;
		ulSBHandle = (ULONG)strtol( buf, &pcErrbuf, 16 );
		if( *pcErrbuf != '\0' )
		{
			return TRUE;
		}
	}
	else
	{
		ulSBHandle = kNkMAIDSBHandle_ALL;
	}

	// To display the menu.
	printf( "\nSelect SB Attribute(1-5, 0)\n" );
	printf( " 1. SB Attribute All\n" );
	printf( " 2. SB Attribute Name\n" );
	printf( " 3. SB Attribute GroupID\n" );
	printf( " 4. SB Attribute Status\n" );
	printf( " 5. SB Attribute TestFlashDisable\n" );
	printf( " 0. Exit\n>" );
	scanf( "%s", buf );
	ulSBAttrID = ( ULONG )atoi( buf );

	// Check the input value.
	if(( ulSBAttrID == 0 ) || ( ulSBAttrID > 5 ))
	{
		return TRUE;
	}

	pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_GetSBAttrDesc );
	if( pCapInfo == NULL )
	{
		return FALSE;
	}

	// check data type of the capability
	if( pCapInfo->ulType != kNkMAIDCapType_Generic )
	{
		return FALSE;
	}

	// check if this capability supports CapGet operation.
	if( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_GetSBAttrDesc, kNkMAIDCapOperation_Get ) )
	{
		return FALSE;
	}

	// 1.To get the size of the order of the SB attribute descriptor acquisition.
	{
		// ulSBHandle：To specify the SBHandle to be acquisition target.
		stSbAttrDesc.ulSBHandle = ulSBHandle;

		// ulSBAttrID：To specify the SBAttrID to be acquisition target.
		switch( ulSBAttrID )
		{
			case 1:
				// SBAttribute_ALL
				stSbAttrDesc.ulSBAttrID = kNkMAIDSBAttribute_ALL;
				break;
			case 2:
				// SBAttribute_Name
				stSbAttrDesc.ulSBAttrID = kNkMAIDSBAttribute_Name;
				break;
			case 3:
				// SBAttribute_GroupID
				stSbAttrDesc.ulSBAttrID = kNkMAIDSBAttribute_GroupID;
				break;
			case 4:
				// SBAttribute_Status
				stSbAttrDesc.ulSBAttrID = kNkMAIDSBAttribute_Status;
				break;
			case 5:
				// SBAttribute_TestFlashDisable
				stSbAttrDesc.ulSBAttrID = kNkMAIDSBAttribute_TestFlashDisable;
				break;
			default:
				stSbAttrDesc.ulSBAttrID = 0;
				break;
		}

		// ulSize：0 is specified.
		stSbAttrDesc.ulSize = 0;

		// To get the size of the order of the SB attribute descriptor acquisition.
		bRet = Command_CapGetSB(pRefObj->pObject, kNkMAIDCapability_GetSBAttrDesc, kNkMAIDDataType_GenericPtr, (NKPARAM)&stSbAttrDesc, NULL, NULL, &nResult);
		if( bRet == FALSE )
		{
			if (nResult == kNkMAIDResult_UnexpectedError)
			{
				printf("\nNot supported SBAttrDesc.\n");
				return TRUE;
			}
			else
			{
			// abnomal.
			printf( "\nFailed in getting SBAttrDesc.\n" );

			return FALSE;
		}
	}
	}

	// 2.To get the SB attribute descriptor.
	{
		// ulSBHandle：To specify the SBHandle to be acquisition target. ( Already set )
		// ulSBAttrID：To specify the SBAttrID to be acquisition target. ( Already set )
		// ulSize：Data size for SBHandleAttributeDescList acquired set to pData. ( Already set )
		// pData：Data pointer for SBHandleAttributeDescList acquisition.
		stSbAttrDesc.pData = ( char * )malloc( stSbAttrDesc.ulSize );
		if( stSbAttrDesc.pData == NULL )
		{
			// abnomal.
			printf( "\nMemory allocation error.\n" );

			return FALSE;
		}

		// To get the SB attribute descriptor.
		nResult = kNkMAIDResult_NoError;
		bRet = Command_CapGetSB(pRefObj->pObject, kNkMAIDCapability_GetSBAttrDesc, kNkMAIDDataType_GenericPtr, (NKPARAM)&stSbAttrDesc, NULL, NULL, &nResult);
		if( bRet == FALSE )
		{
			free( stSbAttrDesc.pData );
			if (nResult == kNkMAIDResult_UnexpectedError)
			{
				printf("\nNot supported SBAttrDesc.\n");
				return TRUE;
			}
			else
			{
			// abnomal.
			printf( "\nFailed in getting SBAttrDesc.\n" );
			return FALSE;
		}
	}
	}

	// Save to file
	// open file
	fileStream = fopen( "SBAttrDesc.dat", "wb" );
	if( fileStream == NULL )
	{
		// abnomal.
		printf( "\nFile open error.\n" );
		free( stSbAttrDesc.pData );

		return FALSE;
	}

	// Get data pointer
	unsigned char* pucData = NULL;
	pucData = ( unsigned char* )stSbAttrDesc.pData;

	// write file
	fwrite( pucData, 1, stSbAttrDesc.ulSize, fileStream );
	printf( "\nSBAttrDesc.dat was saved.\n" );

	// close file
	fclose( fileStream );
	free( stSbAttrDesc.pData );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Set/Get SBAttrValueCapability
BOOL SBAttrValueCapability( LPRefObj pRefObj )
{
	BOOL					bRet = TRUE;
	char					buf[256] = { 0 };
	ULONG					ulSel;
	NkMAIDSBAttrValue		stSbAttrValue;

	do
	{
		// To clear the structure.
		memset( &stSbAttrValue, 0, sizeof( NkMAIDSBAttrValue ));

		// To display the menu.
		printf( "\nSelect (1-2, 0)\n" );
		printf( " 1. Set SB Attribute\n" );
		printf( " 2. Get SB Attribute\n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		ulSel = ( ULONG )atol( buf );
		switch( ulSel )
		{
			case 0:
				// Exit.
				break;
			case 1:
				// Set SBAttrValue.
				bRet = SetSBAttrValueCapability( pRefObj, &stSbAttrValue );
				break;
			case 2:
				// GetSBAttrValue.
				bRet = GetSBAttrValueCapability( pRefObj, &stSbAttrValue );
				break;
			default:
				break;
		}

		if( bRet == FALSE )
		{
			// abnomal.
			printf( "An Error occured. \n" );
		}
	} while( ulSel != 0 );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Set SBAttrValueCapability
BOOL SetSBAttrValueCapability( LPRefObj pRefObj, NkMAIDSBAttrValue *pstSbAttrValue )
{
	BOOL				bRet = FALSE;
	char				cFileName[256] = { 0 };
	char				*pData = NULL;
	ULONG				ulCount = 0;
	ULONG				ulTotal = 0;
	LPNkMAIDCapInfo		pCapInfo = NULL;
	FILE				*fileStream;

	pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_SBAttrValue );
	if( pCapInfo == NULL )
	{
		return FALSE;
	}

	// check data type of the capability
	if( pCapInfo->ulType != kNkMAIDCapType_Generic )
	{
		return FALSE;
	}

	// check if this capability supports CapSet operation.
	if( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_SBAttrValue, kNkMAIDCapOperation_Set ))
	{
		return FALSE;
	}

	// ulSBHandle：Not use the Set.
	// ulSBAttrID：Not use the Set.

	// Open the file
	strcpy( cFileName, "SBAttrValue.dat" );
	fileStream = fopen( cFileName, "rb" );
	if( fileStream == NULL )
	{
		// abnomal.
		printf( "\nFile open error.\n" );

		return FALSE;
	}

	// To get the file size.
	struct stat stStatBuf;
#if defined( _WIN32 )
	if( fstat( _fileno( fileStream ), &stStatBuf ) == -1 )
#elif defined(__APPLE__)
	if( fstat( fileno( fileStream ), &stStatBuf ) == -1 )
#endif
	{
		// abnomal.
		printf( "\nFile status error.\n" );
		fclose( fileStream );

		return FALSE;
	}
	// Data size of SBAttributeList set to pData.
	pstSbAttrValue->ulSize = (ULONG)stStatBuf.st_size;

	// pData：A pointer to the SBAttributeList.
	pstSbAttrValue->pData = ( char* )malloc( pstSbAttrValue->ulSize );
	if( pstSbAttrValue->pData == NULL )
	{
		// abnomal.
		printf( "\nMemory allocation error.\n" );
		fclose( fileStream );

		return FALSE;
	}
	pData = ( char * )pstSbAttrValue->pData;

	// Read the file.
	while( !feof( fileStream ))
	{
		// Read file until eof.
		ulCount = ( ULONG )fread( pData, sizeof( char ), 100, fileStream );
		if( ferror( fileStream ) )
		{
			// abnomal.
			printf( "\nFile read error.\n" );
			fclose( fileStream );
			free( pstSbAttrValue->pData );

			return FALSE;
		}

		// Total count up actual bytes read.
		ulTotal += ulCount;
		pData += ulCount;
		if( pstSbAttrValue->ulSize < ulTotal )
		{
			printf( "\nThe size of \"SBAttrValue.dat\" is size over.\n" );
			fclose( fileStream );
			free( pstSbAttrValue->pData );
			return FALSE;
		}
	}

	// Set SBAttrValue.
	SLONG nResult = kNkMAIDResult_NoError;
	bRet = Command_CapSetSB(pRefObj->pObject, kNkMAIDCapability_SBAttrValue, kNkMAIDDataType_GenericPtr, (NKPARAM)pstSbAttrValue, NULL, NULL, &nResult);
	if( bRet == FALSE )
	{
		fclose( fileStream );
		free( pstSbAttrValue->pData );
		if (nResult == kNkMAIDResult_UnexpectedError)
		{
			printf("\nNot supported SB Attribute Value.\n");
			return TRUE;
		}
		else
		{
			// abnomal.
			printf("\nFailed in setting SB Attribute Value.\n");
		return FALSE;
	}
	}

	printf( "\nSucceed in setting SB Attribute Value.\n" );

	fclose( fileStream );
	free( pstSbAttrValue->pData );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Get SBAttrValueCapability
BOOL GetSBAttrValueCapability( LPRefObj pRefObj, NkMAIDSBAttrValue *pstSbAttrValue )
{
	BOOL				bRet = TRUE;
	char				buf[256] = { 0 };
	ULONG				ulSBHandleMenu = 0;
	ULONG				ulSBHandle = 0;
	ULONG				ulSBAttrID = 0;
	FILE				*fileStream;
	LPNkMAIDCapInfo		pCapInfo = NULL;
	SLONG nResult = kNkMAIDResult_NoError;

	// To display the menu.
	printf( "\nSelect SBHandle Menu(0-1, 2)\n" );
	printf( " 0. All\n" );
	printf( " 1. Any value\n" );
	printf( " 2. Exit\n>" );
	scanf( "%s", buf );
	ulSBHandleMenu = ( ULONG )atoi( buf );

	// Check the input value.
	if( ulSBHandleMenu >= 2 )
	{
		return TRUE;
	}
	else if( ulSBHandleMenu == 1 )
	{
		// To display the menu.
		printf( "\nEnter SBHandle Value(Hex)\n" );
		printf( "0x" );
		scanf( "%s", buf );

		char *pcErrbuf;
		ulSBHandle = (ULONG)strtol( buf, &pcErrbuf, 16 );
		if( *pcErrbuf != '\0' )
		{
			return TRUE;
		}
	}
	else
	{
		ulSBHandle = kNkMAIDSBHandle_ALL;
	}

	// To display the menu.
	printf( "\nSelect SBAttribute(1-5, 0)\n" );
	printf( " 1. SBAttribute ALL\n" );
	printf( " 2. SBAttribute Name\n" );
	printf( " 3. SBAttribute GroupID\n" );
	printf( " 4. SBAttribute Status\n" );
	printf( " 5. SBAttribute TestFlashDisable\n" );
	printf( " 0. Exit\n>" );
	scanf( "%s", buf );
	ulSBAttrID = ( ULONG )atoi( buf );

	// Check the input value.
	if(( ulSBAttrID == 0 ) || ( ulSBAttrID > 5 ))
	{
		return TRUE;
	}

	pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_SBAttrValue );
	if( pCapInfo == NULL )
	{
		return FALSE;
	}

	// check data type of the capability
	if( pCapInfo->ulType != kNkMAIDCapType_Generic )
	{
		return FALSE;
	}

	// check if this capability supports CapGet operation.
	if( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_SBAttrValue, kNkMAIDCapOperation_Get ) )
	{
		return FALSE;
	}

	// 1.To get the size in order to obtain the current value of the SB attribute.
	{
		// ulSBHandle：To specify the SBHandle to be acquisition target.
		pstSbAttrValue->ulSBHandle = ulSBHandle;

		// ulSBAttrID：To specify the SBAttrID to be acquisition target.
		switch( ulSBAttrID )
		{
			case 1:
				// SBAttribute_ALL
				pstSbAttrValue->ulSBAttrID = kNkMAIDSBAttribute_ALL;
				break;
			case 2:
				// SBAttribute_Name
				pstSbAttrValue->ulSBAttrID = kNkMAIDSBAttribute_Name;
				break;
			case 3:
				// SBAttribute_GroupID
				pstSbAttrValue->ulSBAttrID = kNkMAIDSBAttribute_GroupID;
				break;
			case 4:
				// SBAttribute_Status
				pstSbAttrValue->ulSBAttrID = kNkMAIDSBAttribute_Status;
				break;
			case 5:
				// SBAttribute_TestFlashDisable
				pstSbAttrValue->ulSBAttrID = kNkMAIDSBAttribute_TestFlashDisable;
				break;
			default:
				pstSbAttrValue->ulSBAttrID = 0;
				break;
		}

		// ulSize：0 is specified.
		pstSbAttrValue->ulSize = 0;

		// To get the size in order to obtain the current value of the SB attribute.
		bRet = Command_CapGetSB(pRefObj->pObject, kNkMAIDCapability_SBAttrValue, kNkMAIDDataType_GenericPtr, (NKPARAM)pstSbAttrValue, NULL, NULL, &nResult);
		if( bRet == FALSE )
		{
			if (nResult == kNkMAIDResult_UnexpectedError)
			{
				printf("\nNot supported SB Attribute Value.\n");
				return TRUE;
			}
			else
			{
			// abnomal.
			printf( "\nFailed in getting SB Attribute Value.\n" );

			return FALSE;
		}
	}
	}

	// 2.To get the current value of the SB attribute.
	{
		// ulSBHandle：To specify the SBHandle to be acquisition target. ( Already set )
		// ulSBAttrID：To specify the SBAttr to be acquisition target. ( Already set )
		// ulSize：Size of SBHandleAttributeDescList set to pData. ( Already set )
		// pData：Data pointer for SBHandleAttributeDescList acquisition.
		pstSbAttrValue->pData = ( char* )malloc( pstSbAttrValue->ulSize );
		if( pstSbAttrValue->pData == NULL )
		{
			// abnomal.
			printf( "\nMemory allocation error.\n" );

			return FALSE;
		}

		// To get the current value of the SB attribute.
		nResult = kNkMAIDResult_NoError;
		bRet = Command_CapGetSB(pRefObj->pObject, kNkMAIDCapability_SBAttrValue, kNkMAIDDataType_GenericPtr, (NKPARAM)pstSbAttrValue, NULL, NULL, &nResult);
		if( bRet == FALSE )
		{
			free(pstSbAttrValue->pData);
			if (nResult == kNkMAIDResult_UnexpectedError)
			{
				printf("\nNot supported SB Attribute Value.\n");
				return TRUE;
			}
			else
			{
			// abnomal.
				printf("\nFailed in getting SB Attribute Value.\n");
			return FALSE;
		}
	}
	}

	// Save to file
	// open file
	fileStream = fopen( "SBAttrValue.dat", "wb" );
	if( fileStream == NULL )
	{
		// abnomal.
		printf( "\nFile open error.\n" );
		free( pstSbAttrValue->pData );

		return FALSE;
	}

	// Get data pointer
	unsigned char* pucData = NULL;
	pucData = ( unsigned char* )pstSbAttrValue->pData;

	// write file
	fwrite( pucData, 1, pstSbAttrValue->ulSize, fileStream );
	printf( "\nSBAttrValue.dat was saved.\n" );

	// close file
	fclose( fileStream );
	free( pstSbAttrValue->pData );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Get SBGroupAttrDescCapability
BOOL GetSBGroupAttrDescCapability( LPRefObj pRefObj )
{
	BOOL						bRet = TRUE;
	char						buf[256] = { 0 };
	ULONG						ulSBGroupID;
	ULONG						ulSBGroupAttrID;
	FILE						*fileStream;
	NkMAIDGetSBGroupAttrDesc	stSbGroupAttrDesc;
	LPNkMAIDCapInfo				pCapInfo = NULL;

	// To clear the structure.
	memset( &stSbGroupAttrDesc, 0, sizeof( NkMAIDGetSBGroupAttrDesc ));

	// To display the menu.
	printf( "\nSelect SBGroupID(1-8, 0)\n" );
	printf( " 1. All\n" );
	printf( " 2. Master\n" );
	printf( " 3. A\n" );
	printf( " 4. B\n" );
	printf( " 5. C\n" );
	printf( " 6. D\n" );
	printf( " 7. E\n" );
	printf( " 8. F\n" );
	printf( " 0. Exit\n>" );
	scanf( "%s", buf );
	ulSBGroupID = ( ULONG )atoi( buf );

	// Check the input value.
	if(( ulSBGroupID == 0 ) || ( ulSBGroupID > 8 ))
	{
		return TRUE;
	}

	// To display the menu.
	printf( "\nSelect SBGroupAttrID(1-10, 0)\n" );
	printf( " 1. All\n" );
	printf( " 2. FlashMode\n" );
	printf( " 3. FlashCompensation\n" );
	printf( " 4. FlashRatio\n" );
	printf( " 5. FlashLevel\n" );
	printf( " 6. FlashRange\n" );
	printf( " 7. Repeat\n" );
	printf( " 8. RepeatCount\n" );
	printf( " 9. RepeatInterval\n" );
	printf( "10. Invalid\n" );
	printf( " 0. Exit\n>" );
	scanf( "%s", buf );
	ulSBGroupAttrID = ( ULONG )atoi( buf );

	// Check the input value.
	if(( ulSBGroupAttrID == 0 ) || ( ulSBGroupAttrID > 10 ))
	{
		return TRUE;
	}

	pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_GetSBGroupAttrDesc );
	if( pCapInfo == NULL )
	{
		return FALSE;
	}

	// check data type of the capability
	if( pCapInfo->ulType != kNkMAIDCapType_Generic )
	{
		return FALSE;
	}

	// check if this capability supports CapGet operation.
	if( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_GetSBGroupAttrDesc, kNkMAIDCapOperation_Get ) )
	{
		return FALSE;
	}

	// 1.To get the size of the order of the SB group attribute descriptor acquisition.
	{
		// ulSBGroupID：To specify the SBGroupID to be acquisition target.
		switch( ulSBGroupID )
		{
			case 1:
				// SBGroupID_All
				stSbGroupAttrDesc.ulSBGroupID = kNkMAIDSBGroupID_ALL;
				break;
			case 2:
				// SBGroupID_Master
				stSbGroupAttrDesc.ulSBGroupID = kNkMAIDSBGroupID_Master;
				break;
			case 3:
				// SBGroupID_A
				stSbGroupAttrDesc.ulSBGroupID = kNkMAIDSBGroupID_A;
				break;
			case 4:
				// SBGroupID_B
				stSbGroupAttrDesc.ulSBGroupID = kNkMAIDSBGroupID_B;
				break;
			case 5:
				// SBGroupID_C
				stSbGroupAttrDesc.ulSBGroupID = kNkMAIDSBGroupID_C;
				break;
			case 6:
				// SBGroupID_D
				stSbGroupAttrDesc.ulSBGroupID = kNkMAIDSBGroupID_D;
				break;
			case 7:
				// SBGroupID_E
				stSbGroupAttrDesc.ulSBGroupID = kNkMAIDSBGroupID_E;
				break;
			case 8:
				// SBGroupID_F
				stSbGroupAttrDesc.ulSBGroupID = kNkMAIDSBGroupID_F;
				break;
			default:
				break;
		}

		// ulSBGroupAttrID：To specify the SBGroupAttrID to be acquisition target.
		switch( ulSBGroupAttrID )
		{
			case 1:
				// SBGroupAttribute_All
				stSbGroupAttrDesc.ulSBGroupAttrID = kNkMAIDSBGroupAttribute_ALL;
				break;
			case 2:
				// SBGroupAttribute_FlashMode
				stSbGroupAttrDesc.ulSBGroupAttrID = kNkMAIDSBGroupAttribute_FlashMode;
				break;
			case 3:
				// SBGroupAttribute_FlashCompensation
				stSbGroupAttrDesc.ulSBGroupAttrID = kNkMAIDSBGroupAttribute_FlashCompensation;
				break;
			case 4:
				// SBGroupAttribute_FlashRatio
				stSbGroupAttrDesc.ulSBGroupAttrID = kNkMAIDSBGroupAttribute_FlashRatio;
				break;
			case 5:
				// SBGroupAttribute_FlashLevel
				stSbGroupAttrDesc.ulSBGroupAttrID = kNkMAIDSBGroupAttribute_FlashLevel;
				break;
			case 6:
				// SBGroupAttribute_FlashRange
				stSbGroupAttrDesc.ulSBGroupAttrID = kNkMAIDSBGroupAttribute_FlashRange;
				break;
			case 7:
				// SBGroupAttribute_Repeat
				stSbGroupAttrDesc.ulSBGroupAttrID = kNkMAIDSBGroupAttribute_Repeat;
				break;
			case 8:
				// SBGroupAttribute_RepeatCount
				stSbGroupAttrDesc.ulSBGroupAttrID = kNkMAIDSBGroupAttribute_RepeatCount;
				break;
			case 9:
				// SBGroupAttribute_RepeatInterval
				stSbGroupAttrDesc.ulSBGroupAttrID = kNkMAIDSBGroupAttribute_RepeatInterval;
				break;
			case 10:
				// SBGroupAttribute_Invalid
				stSbGroupAttrDesc.ulSBGroupAttrID = kNkMAIDSBGroupAttribute_Invalid;
				break;
			default:
				break;
		}

		// ulSize：0 is specified.
		stSbGroupAttrDesc.ulSize = 0;

		// To get the size of the order of the SB group attribute descriptor acquisition.
		bRet = Command_CapGet(pRefObj->pObject, kNkMAIDCapability_GetSBGroupAttrDesc, kNkMAIDDataType_GenericPtr, (NKPARAM)&stSbGroupAttrDesc, NULL, NULL);
		if( bRet == FALSE )
		{
			// abnomal.
			printf("\nFailed in getting SBGroupAttrDesc.\n");

			return FALSE;
		}
	}

	// 2.To get the SB group attribute descriptor.
	{
		// ulSBGroupID：To specify the SBGroupID to be acquisition target. ( Already set )
		// ulSBGroupAttrID：To specify the SBGroupAttrID to be acquisition target. ( Already set )
		// ulSize：Size of SBGroupAttributeDescList set to pData. ( Already set )
		// pData：Data pointer for SBGroupAttributeDescList acquisition.
		stSbGroupAttrDesc.pData = ( char* )malloc( stSbGroupAttrDesc.ulSize );
		if( stSbGroupAttrDesc.pData == NULL )
		{
			// abnomal.
			printf( "\nmemory allocation error.\n" );

			return FALSE;
		}

		// To get the SB group attribute descriptor.
		bRet = Command_CapGet(pRefObj->pObject, kNkMAIDCapability_GetSBGroupAttrDesc, kNkMAIDDataType_GenericPtr, (NKPARAM)&stSbGroupAttrDesc, NULL, NULL);
		if( bRet == FALSE )
		{
			// abnomal.
			printf("\nFailed in getting SBGroupAttrDesc.\n");
			free( stSbGroupAttrDesc.pData );

			return FALSE;
		}
	}

	// Save to file
	// open file
	fileStream = fopen( "SBGroupAttrDesc.dat", "wb" );
	if( fileStream == NULL )
	{
		// abnomal.
		printf( "\nfile open error.\n" );
		free( stSbGroupAttrDesc.pData );

		return FALSE;
	}

	// Get data pointer
	unsigned char* pucData = NULL;
	pucData = ( unsigned char* )stSbGroupAttrDesc.pData;

	// write file
	fwrite( pucData, 1, stSbGroupAttrDesc.ulSize, fileStream );
	printf( "\nSBGroupAttrDesc.dat was saved.\n" );

	// close file
	fclose( fileStream );
	free( stSbGroupAttrDesc.pData );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Set/Get SBGroupAttrValueCapability
BOOL SBGroupAttrValueCapability( LPRefObj pRefObj )
{
	BOOL					bRet = TRUE;
	char					buf[256] = { 0 };
	ULONG					ulSel;
	NkMAIDSBGroupAttrValue	stSbGroupAttrValue;

	do
	{
		// To clear the structure.
		memset( &stSbGroupAttrValue, 0, sizeof( NkMAIDSBGroupAttrValue ));

		// To display the menu.
		printf( "\nSelect (1-2, 0)\n" );
		printf( " 1. Set SB Group Attribute Value\n" );
		printf( " 2. Get SB Group Attribute Value\n" );
		printf( " 0. Exit\n>" );
		scanf( "%s", buf );
		ulSel = ( ULONG )atol( buf );
		switch( ulSel )
		{
			case 0:
				// Exit
				break;
			case 1:
				// Set
				bRet = SetSBGroupAttrValueCapability( pRefObj, &stSbGroupAttrValue );
				break;
			case 2:
				// Get
				bRet = GetSBGroupAttrValueCapability( pRefObj, &stSbGroupAttrValue );
				break;
			default:
				break;
		}

		if( bRet == FALSE )
		{
			// abnomal.
			printf( "An Error occured. \n" );
		}
	} while( ulSel != 0 );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Set SBGroupAttrValueCapability
BOOL SetSBGroupAttrValueCapability( LPRefObj pRefObj, NkMAIDSBGroupAttrValue *pstSbGroupAttrValue )
{
	BOOL				bRet = FALSE;
	char				cFileName[256] = { 0 };
	char				*pData = NULL;
	ULONG				ulCount = 0;
	ULONG				ulTotal = 0;
	FILE				*fileStream;
	LPNkMAIDCapInfo		pCapInfo = NULL;

	pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_SBGroupAttrValue );
	if( pCapInfo == NULL )
	{
		return FALSE;
	}

	// check data type of the capability
	if( pCapInfo->ulType != kNkMAIDCapType_Generic )
	{
		return FALSE;
	}

	// check if this capability supports CapGet operation.
	if( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_SBGroupAttrValue, kNkMAIDCapOperation_Set ) )
	{
		return FALSE;
	}

	// ulSBGroupID：Not use the Set.
	// ulSBGroupAttrID：Not use the Set.

	// Open the file
	strcpy( cFileName, "SBGroupAttrValue.dat" );
	fileStream = fopen( cFileName, "rb" );
	if( fileStream == NULL )
	{
		// abnomal.
		printf( "\nFile open error.\n" );

		return FALSE;
	}

	// To get the size.
	struct stat stStatBuf;
#if defined( _WIN32 )
	if( fstat( _fileno( fileStream ), &stStatBuf ) == -1 )
#elif defined(__APPLE__)
	if( fstat( fileno( fileStream ), &stStatBuf ) == -1 )
#endif
	{
		// abnomal.
		printf( "\nFile status error.\n" );
		fclose( fileStream );

		return FALSE;
	}

	// ulSize：Data size of SBGroupAttributeList set to pData.
	pstSbGroupAttrValue->ulSize = (ULONG)stStatBuf.st_size;

	// pData：A pointer to the SBGroupAttributeList.
	pstSbGroupAttrValue->pData = ( char* )malloc( pstSbGroupAttrValue->ulSize );
	if( pstSbGroupAttrValue->pData == NULL )
	{
		// abnomal.
		printf( "\nMemory allocation error.\n" );
		fclose( fileStream );

		return FALSE;
	}

	// To get the binary data .
	pData = ( char * )pstSbGroupAttrValue->pData;
	while( !feof( fileStream ) )
	{
		// Read file until eof.
		ulCount = ( ULONG )fread( pData, sizeof( char ), 100, fileStream );
		if( ferror( fileStream ) )
		{
			// abnomal.
			printf( "\nfile read error.\n" );
			fclose( fileStream );
			free( pstSbGroupAttrValue->pData );

			return FALSE;
		}

		// Total count up actual bytes read.
		ulTotal += ulCount;
		pData += ulCount;
		if( pstSbGroupAttrValue->ulSize < ulTotal )
		{
			printf( "\nThe size of \"SBGroupAttrValue.dat\" is size over.\n" );
			fclose( fileStream );
			free( pstSbGroupAttrValue->pData );
			return FALSE;
		}
	}

	// Set SB Group Attr Value.
	bRet = Command_CapSet(pRefObj->pObject, kNkMAIDCapability_SBGroupAttrValue, kNkMAIDDataType_GenericPtr, (NKPARAM)pstSbGroupAttrValue, NULL, NULL);
	if( bRet == FALSE )
	{
		// abnomal.
		printf("\nFailed in setting SB Group Attribute Value.\n");
		fclose( fileStream );
		free( pstSbGroupAttrValue->pData );

		return FALSE;
	}

	printf( "\nSucceed in setting SB Group Attribute Value.\n" );

	fclose( fileStream );
	free( pstSbGroupAttrValue->pData );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Get SBGroupAttrValueCapability
BOOL GetSBGroupAttrValueCapability( LPRefObj pRefObj, NkMAIDSBGroupAttrValue *pstSbGroupAttrValue )
{
	BOOL				bRet = TRUE;
	char				buf[256] = { 0 };
	ULONG				ulSBGroupID;
	ULONG				ulSBGroupAttrID;
	FILE						*fileStream;
	LPNkMAIDCapInfo		pCapInfo = NULL;

	// To display the menu.
	printf( "\nSelect SBGroupID(1-8, 0)\n" );
	printf( " 1. SBGroupID ALL\n" );
	printf( " 2. SBGroupID Master\n" );
	printf( " 3. SBGroupID A\n" );
	printf( " 4. SBGroupID B\n" );
	printf( " 5. SBGroupID C\n" );
	printf( " 6. SBGroupID D\n" );
	printf( " 7. SBGroupID E\n" );
	printf( " 8. SBGroupID F\n" );
	printf( " 0. Exit\n>" );
	scanf( "%s", buf );
	ulSBGroupID = ( ULONG )atoi( buf );

	// Check the input value.
	if(( ulSBGroupID == 0 ) || ( ulSBGroupID > 8 ))
	{
		return TRUE;
	}

	// To display the menu.
	printf( "\nSelect SBGroupAttrID(1-10, 0)\n" );
	printf( " 1. All\n" );
	printf( " 2. FlashMode\n" );
	printf( " 3. FlashCompensation\n" );
	printf( " 4. FlashRatio\n" );
	printf( " 5. FlashLevel\n" );
	printf( " 6. FlashRange\n" );
	printf( " 7. Repeat\n" );
	printf( " 8. RepeatCount\n" );
	printf( " 9. RepeatInterval\n" );
	printf( "10. Invalid\n" );
	printf( " 0. Exit\n>" );
	scanf( "%s", buf );
	ulSBGroupAttrID = ( ULONG )atoi( buf );

	// Check the input value.
	if(( ulSBGroupAttrID == 0 ) || ( ulSBGroupAttrID > 10))
	{
		return TRUE;
	}

	pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_SBGroupAttrValue );
	if( pCapInfo == NULL )
	{
		return FALSE;
	}

	// check data type of the capability
	if( pCapInfo->ulType != kNkMAIDCapType_Generic )
	{
		return FALSE;
	}

	// check if this capability supports CapGet operation.
	if( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_SBGroupAttrValue, kNkMAIDCapOperation_Get ) )
	{
		return FALSE;
	}

	// 1.To get the size in order to obtain the current value of SB group attributes.
	{
		// ulSBGroupID：To specify the SBGroupID to be acquisition target.
		switch( ulSBGroupID )
		{
			case 1:
				// SBGroupID_All
				pstSbGroupAttrValue->ulSBGroupID = kNkMAIDSBGroupID_ALL;
				break;
			case 2:
				// SBGroupID_Master
				pstSbGroupAttrValue->ulSBGroupID = kNkMAIDSBGroupID_Master;
				break;
			case 3:
				// SBGroupID_A
				pstSbGroupAttrValue->ulSBGroupID = kNkMAIDSBGroupID_A;
				break;
			case 4:
				// SBGroupID_B
				pstSbGroupAttrValue->ulSBGroupID = kNkMAIDSBGroupID_B;
				break;
			case 5:
				// SBGroupID_C
				pstSbGroupAttrValue->ulSBGroupID = kNkMAIDSBGroupID_C;
				break;
			case 6:
				// SBGroupID_D
				pstSbGroupAttrValue->ulSBGroupID = kNkMAIDSBGroupID_D;
				break;
			case 7:
				// SBGroupID_E
				pstSbGroupAttrValue->ulSBGroupID = kNkMAIDSBGroupID_E;
				break;
			case 8:
				// SBGroupID_F
				pstSbGroupAttrValue->ulSBGroupID = kNkMAIDSBGroupID_F;
				break;
			default:
				break;
		}

		// ulSBGroupAttrID：To specify the SBGroupAttrID to be acquisition target.
		switch( ulSBGroupAttrID )
		{
			case 0:
				break;
			case 1:
				// SBGroupAttribute_All
				pstSbGroupAttrValue->ulSBGroupAttrID = kNkMAIDSBGroupAttribute_ALL;
				break;
			case 2:
				// SBGroupAttribute_FlashMode
				pstSbGroupAttrValue->ulSBGroupAttrID = kNkMAIDSBGroupAttribute_FlashMode;
				break;
			case 3:
				// SBGroupAttribute_FlashCompensation
				pstSbGroupAttrValue->ulSBGroupAttrID = kNkMAIDSBGroupAttribute_FlashCompensation;
				break;
			case 4:
				// SBGroupAttribute_FlashRatio
				pstSbGroupAttrValue->ulSBGroupAttrID = kNkMAIDSBGroupAttribute_FlashRatio;
				break;
			case 5:
				// SBGroupAttribute_FlashLevel
				pstSbGroupAttrValue->ulSBGroupAttrID = kNkMAIDSBGroupAttribute_FlashLevel;
				break;
			case 6:
				// SBGroupAttribute_FlashRange
				pstSbGroupAttrValue->ulSBGroupAttrID = kNkMAIDSBGroupAttribute_FlashRange;
				break;
			case 7:
				// SBGroupAttribute_Repeat
				pstSbGroupAttrValue->ulSBGroupAttrID = kNkMAIDSBGroupAttribute_Repeat;
				break;
			case 8:
				// SBGroupAttribute_RepeatCount
				pstSbGroupAttrValue->ulSBGroupAttrID = kNkMAIDSBGroupAttribute_RepeatCount;
				break;
			case 9:
				// SBGroupAttribute_RepeatInterval
				pstSbGroupAttrValue->ulSBGroupAttrID = kNkMAIDSBGroupAttribute_RepeatInterval;
				break;
			case 10:
				// SBGroupAttribute_Invalid
				pstSbGroupAttrValue->ulSBGroupAttrID = kNkMAIDSBGroupAttribute_Invalid;
				break;
			default:
				break;
		}

		// ulSize：0 is specified.
		pstSbGroupAttrValue->ulSize = 0;

		// To get the size in order to obtain the current value of SB group attributes.
		bRet = Command_CapGet(pRefObj->pObject, kNkMAIDCapability_SBGroupAttrValue, kNkMAIDDataType_GenericPtr, (NKPARAM)pstSbGroupAttrValue, NULL, NULL);
		if( bRet == FALSE )
		{
			// abnomal.
			printf("\nFailed in getting SB Group Attribute Value.\n");

			return FALSE;
		}
	}

	// 2.To get the current value of the SB group attribute.
	{
		// ulSBGroupID：To specify the SBGroupID to be acquisition target. ( Already set )
		// ulSBGroupAttrID：To specify the SBGroupAttrID to be acquisition target. ( Already set )
		// ulSize：Size of SBGroupIDAttributeDescList set to pData. ( Already set )
		// pData：Data pointer for SBGroupIDAttributeDescList acquisition.
		pstSbGroupAttrValue->pData = ( char* )malloc( pstSbGroupAttrValue->ulSize );
		if( pstSbGroupAttrValue->pData == NULL )
		{
			// abnomal.
			printf( "\nMemory allocation error.\n" );

			return FALSE;
		}

		// To get the current value of the SB group attribute.
		bRet = Command_CapGet(pRefObj->pObject, kNkMAIDCapability_SBGroupAttrValue, kNkMAIDDataType_GenericPtr, (NKPARAM)pstSbGroupAttrValue, NULL, NULL);
		if( bRet == FALSE )
		{
			// abnomal.
			printf("\nFailed in getting SB Group Attribute Value.\n");
			free(pstSbGroupAttrValue->pData);

			return FALSE;
		}
	}

	// Save to file
	// open file
	fileStream = fopen( "SBGroupAttrValue.dat", "wb" );
	if( fileStream == NULL )
	{
		// abnomal.
		printf( "\nFile open error.\n" );
		free( pstSbGroupAttrValue->pData );

		return FALSE;
	}

	// Get data pointer
	unsigned char* pucData = NULL;
	pucData = ( unsigned char* )pstSbGroupAttrValue->pData;

	// write file
	fwrite( pucData, 1, pstSbGroupAttrValue->ulSize, fileStream );
	printf( "\nSBGroupAttrValue.dat was saved.\n" );

	// close file
	fclose( fileStream );
	free( pstSbGroupAttrValue->pData );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Set TestFlashCapability
BOOL TestFlashCapability( LPRefObj pRefObj )
{
	BOOL				bRet = TRUE;
	char				buf[256] = { 0 };
	ULONG				ulSBHandleMenu = 0;
	ULONG				ulSBHandle = 0;
	NkMAIDTestFlash		stTestFlash;
	LPNkMAIDCapInfo		pCapInfo = NULL;

	// To clear the structure.
	memset( &stTestFlash, 0, sizeof( NkMAIDTestFlash ) );

	// To display the menu.
	printf( "\nSelect SBHandle(0-1, 2)\n" );
	printf( " 0. SBHandle ALL\n" );
	printf( " 1. Any value\n" );
	printf( " 2. Exit\n>" );
	scanf( "%s", buf );
	ulSBHandleMenu = ( ULONG )atoi( buf );

	// Check the input value.
	if( ulSBHandleMenu >= 2 )
	{
		return TRUE;
	}
	else if( ulSBHandleMenu == 1 )
	{
		// To display the menu.
		printf( "\nEnter SBHandle Value(Hex)\n" );
		printf( "0x" );
		scanf( "%s", buf );

		char *pcErrbuf;
		ulSBHandle = (ULONG)strtol( buf, &pcErrbuf, 16 );
		if( *pcErrbuf != '\0' )
		{
			return TRUE;
		}
	}
	else
	{
		ulSBHandle = 0;
	}

	pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_TestFlash );
	if( pCapInfo == NULL )
	{
		return FALSE;
	}

	// check data type of the capability
	if( pCapInfo->ulType != kNkMAIDCapType_Generic )
	{
		return FALSE;
	}

	// check if this capability supports CapGet operation.
	if( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_TestFlash, kNkMAIDCapOperation_Start ) )
	{
		return FALSE;
	}

	// ulType：kNkMAIDTestFlash_Test is specified.
	stTestFlash.ulType = kNkMAIDTestFlash_Test;

	// ulParam：To specify the SBHandle to be acquisition target.
	stTestFlash.ulParam = ulSBHandle;
	bRet = Command_CapStartGeneric( pRefObj->pObject, kNkMAIDCapability_TestFlash, (NKPARAM)&stTestFlash, NULL, NULL, NULL );

	if( bRet == FALSE )
	{
		// abnomal.
		printf( "An Error occured. \n" );
		return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// read the array data from the camera and display it on the screen
BOOL ShowArrayCapability( LPRefObj pRefObj, ULONG ulCapID )
{
	BOOL	bRet = TRUE;
	NkMAIDArray	stArray;
	ULONG	ulSize, i, j;
	LPNkMAIDCapInfo	pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Array ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;
	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_ArrayPtr, (NKPARAM)&stArray, NULL, NULL );
	if( bRet == FALSE ) return FALSE;

	ulSize = stArray.ulElements * stArray.wPhysicalBytes;
	// allocate memory for array data
	stArray.pData = malloc( ulSize );
	if ( stArray.pData == NULL ) return FALSE;
	// get array data
	bRet = Command_CapGetArray( pRefObj->pObject, ulCapID, kNkMAIDDataType_ArrayPtr, (NKPARAM)&stArray, NULL, NULL );
	if( bRet == FALSE ) {
		free( stArray.pData );
		return FALSE;
	}

	// show selectable items for this capability and current setting
	printf( "[%s]\n", pCapInfo->szDescription );
	for ( i = 0, j = 0; i*16+j < ulSize; i++ ) {
		for ( ; j < 16 && i*16+j < ulSize; j++ ) {
			printf( " %02X", ((UCHAR*)stArray.pData)[i*16+j] );
		}
		j = 0;
		printf( "\n" );
	}

	if ( stArray.pData != NULL )
		free( stArray.pData );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// read the array data from the camera and save it on a storage(hard drive)
//  for kNkMAIDCapability_GetLiveViewImage
BOOL GetArrayCapability( LPRefObj pRefObj, ULONG ulCapID, LPNkMAIDArray pstArray )
{
	BOOL	bRet = TRUE;
	LPNkMAIDCapInfo	pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Array ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;
	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_ArrayPtr, (NKPARAM)pstArray, NULL, NULL );
	if( bRet == FALSE ) return FALSE;

	// check if this capability supports CapGetArray operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_GetArray ) ) return FALSE;
	// allocate memory for array data
	pstArray->pData = malloc( pstArray->ulElements * pstArray->wPhysicalBytes );
	if ( pstArray->pData == NULL ) return FALSE;
	// get array data
	bRet = Command_CapGetArray( pRefObj->pObject, ulCapID, kNkMAIDDataType_ArrayPtr, (NKPARAM)pstArray, NULL, NULL );
	if( bRet == FALSE ) {
		free( pstArray->pData );
		pstArray->pData = NULL;
		return FALSE;
	}

	// show selectable items for this capability and current setting
	printf( "[%s]\n", pCapInfo->szDescription );

	// Do not free( pstArray->pData )
	// Upper class use pstArray->pData to save file.
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// load the array data from a storage and send it to the camera
BOOL LoadArrayCapability( LPRefObj pRefObj, ULONG ulCapID, char* filename )
{
	BOOL	bRet = TRUE;
	NkMAIDArray	stArray;
	FILE *stream;
	LPNkMAIDCapInfo	pCapInfo = GetCapInfo( pRefObj, ulCapID );
	if ( pCapInfo == NULL ) return FALSE;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Array ) return FALSE;
	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Get ) ) return FALSE;
	bRet = Command_CapGet( pRefObj->pObject, ulCapID, kNkMAIDDataType_ArrayPtr, (NKPARAM)&stArray, NULL, NULL );
	if( bRet == FALSE ) return FALSE;

	// allocate memory for array data
	stArray.pData = malloc( stArray.ulElements * stArray.wPhysicalBytes );
	if ( stArray.pData == NULL ) return FALSE;

	// show selectable items for this capability and current setting
	printf( "[%s]\n", pCapInfo->szDescription );

	if ( (stream = fopen( filename, "rb" ) ) == NULL) {
		printf( "file not found\n" );
		if ( stArray.pData != NULL )
			free( stArray.pData );
		return FALSE;
	}
	fread( stArray.pData, 1, stArray.ulElements * stArray.wPhysicalBytes, stream );
	fclose( stream );

	// check if this capability supports CapSet operation.
	if ( CheckCapabilityOperation( pRefObj, ulCapID, kNkMAIDCapOperation_Set ) ) {
		// set array data
		bRet = Command_CapSet( pRefObj->pObject, ulCapID, kNkMAIDDataType_ArrayPtr, (NKPARAM)&stArray, NULL, NULL );
		if( bRet == FALSE ) {
			free( stArray.pData );
			return FALSE;
		}
	}
	if ( stArray.pData != NULL )
		free( stArray.pData );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL IssueProcess( LPRefObj pRefSrc, ULONG ulCapID )
{
	LPNkMAIDObject pSourceObject = pRefSrc->pObject;
	LPNkMAIDCapInfo pCapInfo;
	ULONG	ulCount = 0L;
	BOOL bRet;
	LPRefCompletionProc pRefCompletion;
	pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = NULL;

	// Confirm whether this capability is supported or not.
	pCapInfo =	GetCapInfo( pRefSrc, ulCapID );
	// check if the CapInfo is available.
	if ( pCapInfo == NULL ) return FALSE;

	printf( "[%s]\n", pCapInfo->szDescription );

	// Start the process
	bRet = Command_CapStart( pSourceObject, ulCapID, (LPNKFUNC)CompletionProc, (NKREF)pRefCompletion, NULL );
	if ( bRet == FALSE ) return FALSE;
	// Wait for end of the process and issue Command_Async.
	IdleLoop( pSourceObject, &ulCount, 1 );

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL TerminateCaptureCapability( LPRefObj pRefSrc )
{
	LPNkMAIDObject pSourceObject = pRefSrc->pObject;
	LPNkMAIDCapInfo pCapInfo;
	ULONG	ulCount = 0L;
	BOOL bRet;
	NkMAIDTerminateCapture Param;
	LPRefCompletionProc pRefCompletion;

	Param.ulParameter1 = 0;
	Param.ulParameter2 = 0;

	pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = NULL;

	// Confirm whether this capability is supported or not.
	pCapInfo =	GetCapInfo( pRefSrc, kNkMAIDCapability_TerminateCapture );
	// check if the CapInfo is available.
	if ( pCapInfo == NULL ) return FALSE;

	printf( "[%s]\n", pCapInfo->szDescription );

	// Start the process
	bRet = Command_CapStartGeneric( pSourceObject, kNkMAIDCapability_TerminateCapture, (NKPARAM)&Param,(LPNKFUNC)CompletionProc, (NKREF)pRefCompletion, NULL );
	if ( bRet == FALSE ) return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
BOOL GetRecordingInfoCapability(LPRefObj pRefObj)
{
	BOOL bRet = TRUE;
	NkMAIDGetRecordingInfo stGetRecordingInfo;
	LPNkMAIDCapInfo pCapInfo = NULL;

	pCapInfo = GetCapInfo(pRefObj, kNkMAIDCapability_GetRecordingInfo);
	if (pCapInfo == NULL)
	{
		return FALSE;
	}

	// check data type of the capability
	if (pCapInfo->ulType != kNkMAIDCapType_Generic)
	{
		return FALSE;
	}

	// check if this capability supports CapGet operation.
	if (!CheckCapabilityOperation(pRefObj, kNkMAIDCapability_GetRecordingInfo, kNkMAIDCapOperation_Get))
	{
		return FALSE;
	}

	bRet = Command_CapGet(pRefObj->pObject, kNkMAIDCapability_GetRecordingInfo, kNkMAIDDataType_GenericPtr, (NKPARAM)&stGetRecordingInfo, NULL, NULL);
	if (bRet == FALSE)
	{
		// abnomal.
		printf("\nFailed in getting RecordingInfo.\n");

		return FALSE;
	}

	printf("File Index                  %d\n", stGetRecordingInfo.ulIndexOfMov);
	printf("The number of divided files %d\n", stGetRecordingInfo.ulTotalMovCount);
	printf("Total Size                  %llu[byte]\n", stGetRecordingInfo.ullTotalMovSize);

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL IssueProcessSync( LPRefObj pRefSrc, ULONG ulCapID )
{
	LPNkMAIDObject pSourceObject = pRefSrc->pObject;
	LPNkMAIDCapInfo pCapInfo;
	BOOL bRet;
	// Confirm whether this capability is supported or not.
	pCapInfo =	GetCapInfo( pRefSrc, ulCapID );
	// check if the CapInfo is available.
	if ( pCapInfo == NULL ) return FALSE;

	printf( "[%s]\n", pCapInfo->szDescription );

	// Start the process
	bRet = Command_CapStart( pSourceObject, ulCapID, NULL, NULL, NULL );
	if ( bRet == FALSE ) return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL IssueAcquire( LPRefObj pRefDat )
{
	BOOL	bRet;
	LPRefObj	pRefItm = (LPRefObj)pRefDat->pRefParent;
	NkMAIDCallback	stProc;
	LPRefDataProc	pRefDeliver;
	LPRefCompletionProc	pRefCompletion;
	ULONG	ulCount = 0L;

	// set reference from DataProc
	pRefDeliver = (LPRefDataProc)malloc( sizeof(RefDataProc) );// this block will be freed in CompletionProc.
	pRefDeliver->pBuffer = NULL;
	pRefDeliver->ulOffset = 0L;
	pRefDeliver->ulTotalLines = 0L;
	pRefDeliver->lID = pRefItm->lMyID;
	// set reference from CompletionProc
	pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );// this block will be freed in CompletionProc.
	pRefCompletion->pulCount = &ulCount;
	pRefCompletion->pRef = pRefDeliver;
	// set reference from DataProc
	stProc.pProc = (LPNKFUNC)DataProc;
	stProc.refProc = (NKREF)pRefDeliver;

	// set DataProc as data delivery callback function
	if( CheckCapabilityOperation( pRefDat, kNkMAIDCapability_DataProc, kNkMAIDCapOperation_Set ) ) {
		bRet = Command_CapSet( pRefDat->pObject, kNkMAIDCapability_DataProc, kNkMAIDDataType_CallbackPtr, (NKPARAM)&stProc, NULL, NULL );
		if ( bRet == FALSE ) return FALSE;
	} else
		return FALSE;

	// start getting an image
	bRet = Command_CapStart( pRefDat->pObject, kNkMAIDCapability_Acquire, (LPNKFUNC)CompletionProc, (NKREF)pRefCompletion, NULL );
	if ( bRet == FALSE ) return FALSE;
	IdleLoop( pRefDat->pObject, &ulCount, 1 );

	// reset DataProc
	bRet = Command_CapSet( pRefDat->pObject, kNkMAIDCapability_DataProc, kNkMAIDDataType_Null, (NKPARAM)NULL, NULL, NULL );
	if ( bRet == FALSE ) return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Get Video image
BOOL GetVideoImageExCapability(LPRefObj pRefDat, ULONG ulCapID)
{
	BOOL	bRet = TRUE;
	char	MovieFileName[256];
	FILE*	hFileMovie = NULL;		// Movie file name
	unsigned char* pucData = NULL;	// Movie data pointer
	NK_UINT_64	ullTotalSize = 0;
	int i = 0;
	NkMAIDGetVideoImageEx	stVideoImage;
	NkMAIDEnum	stEnum;
	LPRefObj pobject;

#if defined( _WIN32 )
	SetConsoleCtrlHandler(cancelhandler, TRUE);
#elif defined(__APPLE__)
	struct sigaction action, oldaction;
	memset(&action, 0, sizeof(action));
	action.sa_handler = cancelhandler;
	action.sa_flags = SA_RESETHAND;
	sigaction(SIGINT, &action, &oldaction);
#endif

	memset(&stVideoImage, 0, sizeof(NkMAIDGetVideoImageEx));
	memset(&pobject, 0, sizeof(LPRefObj));

	// get total size
	stVideoImage.ullDataSize = 0;
	bRet = Command_CapGet(pRefDat->pObject, ulCapID, kNkMAIDDataType_GenericPtr, (NKPARAM)&stVideoImage, NULL, NULL);
	if (bRet == FALSE) return FALSE;

	ullTotalSize = stVideoImage.ullDataSize;
	if (ullTotalSize == 0) return FALSE;

	// get movie data
	stVideoImage.ullDataSize = VIDEO_SIZE_BLOCK;		// read block size : 5MB
	stVideoImage.ullReadSize = 0;
	stVideoImage.ullOffset = 0;

	// allocate memory for array data
	stVideoImage.pData = malloc(VIDEO_SIZE_BLOCK);
	if (stVideoImage.pData == NULL) return FALSE;

	// get movie file type
	LPRefObj pRefItem = (LPRefObj)pRefDat->pRefParent;
	if (!pRefItem) return FALSE;
	LPRefObj pRefSource = (LPRefObj)pRefItem->pRefParent;
	if (!pRefSource) return FALSE;
	bRet = Command_CapGet(pRefSource->pObject, kNkMAIDCapability_MovieFileType, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL);
	if (bRet == FALSE) return FALSE;

	// create file name
	while (TRUE)
	{
		if (stEnum.ulValue == 0){
			sprintf(MovieFileName, "MovieData%03d.%s", ++i, "mov");
		}
		else if (stEnum.ulValue == 1){
			sprintf(MovieFileName, "MovieData%03d.%s", ++i, "mp4");
		}
		if ((hFileMovie = fopen(MovieFileName, "r")) != NULL)
		{
			// this file name is already used.
			fclose(hFileMovie);
			hFileMovie = NULL;
		}
		else
		{
			break;
		}
	}

	// open file
	hFileMovie = fopen(MovieFileName, "wb");
	if (hFileMovie == NULL)
	{
		fclose(hFileMovie);
		printf("file open error.\n");
		return FALSE;
	}

	// Get data pointer
	pucData = (unsigned char*)stVideoImage.pData;

	printf("Please press the Ctrl+C to cancel.\n");

	// write file
	while ((stVideoImage.ullOffset < ullTotalSize) && (bRet == TRUE))
	{
		if (TRUE == g_bCancel)
		{
			stVideoImage.ullDataSize = 0;
			bRet = Command_CapGetArray(pRefDat->pObject, ulCapID, kNkMAIDDataType_GenericPtr, (NKPARAM)&stVideoImage, NULL, NULL);
			break;
		}

		bRet = Command_CapGetArray(pRefDat->pObject, ulCapID, kNkMAIDDataType_GenericPtr, (NKPARAM)&stVideoImage, NULL, NULL);

		stVideoImage.ullOffset += (stVideoImage.ullReadSize);

		fwrite(pucData, (ULONG)stVideoImage.ullReadSize, 1, hFileMovie);

		if (bRet == FALSE) {
			free(stVideoImage.pData);
			return FALSE;
		}

	}
#if defined( _WIN32 )
	SetConsoleCtrlHandler(cancelhandler, FALSE);
#elif defined(__APPLE__)
	sigaction(SIGINT, &oldaction, NULL);
#endif

	if (stVideoImage.ullOffset < ullTotalSize && TRUE == g_bCancel)
	{
		printf("Get Video image was canceled.\n");
	}
	else {
		printf("%s was saved.\n", MovieFileName);
	}
	g_bCancel = FALSE;

	// close file
	fclose(hFileMovie);
	free(stVideoImage.pData);

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL IssueThumbnail( LPRefObj pRefSrc )
{
	BOOL	bRet;
	LPRefObj	pRefItm, pRefDat;
	NkMAIDCallback	stProc;
	LPRefDataProc	pRefDeliver;
	LPRefCompletionProc	pRefCompletion;
	ULONG	ulItemID, ulFinishCount = 0L;
	ULONG	i, j;
	NkMAIDEnum	stEnum;
	LPNkMAIDCapInfo	pCapInfo;
	
	pCapInfo = GetCapInfo( pRefSrc, kNkMAIDCapability_Children );
	// check if the CapInfo is available.
	if ( pCapInfo == NULL )	return FALSE;

	// check if this capability supports CapGet operation.
	if ( !CheckCapabilityOperation( pRefSrc, kNkMAIDCapability_Children, kNkMAIDCapOperation_Get ) ) return FALSE;
	bRet = Command_CapGet( pRefSrc->pObject, kNkMAIDCapability_Children, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if ( bRet == FALSE ) return FALSE;

	// If the source object has no item, it does nothing and returns soon.
	if ( stEnum.ulElements == 0 )	return TRUE;

	// allocate memory for array data
	stEnum.pData = malloc( stEnum.ulElements * stEnum.wPhysicalBytes );
	if ( stEnum.pData == NULL ) return FALSE;
	// get array data
	bRet = Command_CapGetArray( pRefSrc->pObject, kNkMAIDCapability_Children, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if ( bRet == FALSE ) {
		free( stEnum.pData );
		return FALSE;
	}

	// Open all thumbnail objects in the current directory.
	for ( i = 0; i < stEnum.ulElements; i++ ) {
		ulItemID = ((ULONG*)stEnum.pData)[i];
		pRefItm = GetRefChildPtr_ID( pRefSrc, ulItemID );
		if ( pRefItm == NULL ) {
			// open the item object
			bRet = AddChild( pRefSrc, ulItemID );
			if ( bRet == FALSE ) {
				free( stEnum.pData );
				return FALSE;
			}
			pRefItm = GetRefChildPtr_ID( pRefSrc, ulItemID );
		}
		if ( pRefItm != NULL ) {
			pRefDat = GetRefChildPtr_ID( pRefItm, kNkMAIDDataObjType_Thumbnail );
			if ( pRefDat == NULL ) {
				// open the thumbnail object
				bRet = AddChild( pRefItm, kNkMAIDDataObjType_Thumbnail );
				if ( bRet == FALSE ) {
					free( stEnum.pData );
					return FALSE;
				}
				pRefDat = GetRefChildPtr_ID( pRefItm, kNkMAIDDataObjType_Thumbnail );
			}
		}
	}
	free ( stEnum.pData );

	// set NkMAIDCallback structure for DataProc
	stProc.pProc = (LPNKFUNC)DataProc;

	// acquire all thumbnail images.
	for ( i = 0; i < pRefSrc->ulChildCount; i++ ) {
		pRefItm = GetRefChildPtr_Index( pRefSrc, i );
		pRefDat = GetRefChildPtr_ID( pRefItm, kNkMAIDDataObjType_Thumbnail );

		if ( pRefDat != NULL ) {
			// set RefDeliver structure refered in DataProc
			pRefDeliver = (LPRefDataProc)malloc( sizeof(RefDataProc) );// this block will be freed in CompletionProc.
			pRefDeliver->pBuffer = NULL;
			pRefDeliver->ulOffset = 0L;
			pRefDeliver->ulTotalLines = 0L;
			pRefDeliver->lID = pRefItm->lMyID;

			// set DataProc as data delivery callback function
			stProc.refProc = (NKREF)pRefDeliver;
			if( CheckCapabilityOperation( pRefDat, kNkMAIDCapability_DataProc, kNkMAIDCapOperation_Set ) ) {
				bRet = Command_CapSet( pRefDat->pObject, kNkMAIDCapability_DataProc, kNkMAIDDataType_CallbackPtr, (NKPARAM)&stProc, NULL, NULL );
				if ( bRet == FALSE ) return FALSE;
			} else
				return FALSE;

			pRefCompletion = (LPRefCompletionProc)malloc( sizeof(RefCompletionProc) );// this block will be freed in CompletionProc.

			// Set RefCompletion structure refered from CompletionProc.
			pRefCompletion->pulCount = &ulFinishCount;
			pRefCompletion->pRef = pRefDeliver;

			// Starting Acquire Thumbnail
			bRet = Command_CapStart( pRefDat->pObject, kNkMAIDCapability_Acquire, (LPNKFUNC)CompletionProc, (NKREF)pRefCompletion, NULL );
			if ( bRet == FALSE ) return FALSE;
		} else {
			// This item doesn't have a thumbnail, so we count up ulFinishCount.
			ulFinishCount++;
		}

		// Send Async command to all DataObjects that have started acquire command.
		for ( j = 0; j <= i; j++ ) {
			bRet = Command_Async( GetRefChildPtr_ID(GetRefChildPtr_Index(pRefSrc, j ), kNkMAIDDataObjType_Thumbnail)->pObject );
			if ( bRet == FALSE ) return FALSE;
		}
	}

	// Send Async command to all DataObjects, untill all scanning complete.
	while ( ulFinishCount < pRefSrc->ulChildCount ) {
		for ( j = 0; j < pRefSrc->ulChildCount; j++ ) {
			bRet = Command_Async( GetRefChildPtr_ID(GetRefChildPtr_Index( pRefSrc, j), kNkMAIDDataObjType_Thumbnail )->pObject );
			if ( bRet == FALSE ) return FALSE;
		}
	}

	// Close all item objects(include image and thumbnail object).
	while ( pRefSrc->ulChildCount > 0 ) {
		pRefItm = GetRefChildPtr_Index( pRefSrc, 0 );
		ulItemID = pRefItm->lMyID;
		// reset DataProc
		bRet = Command_CapSet( pRefDat->pObject, kNkMAIDCapability_DataProc, kNkMAIDDataType_Null, (NKPARAM)NULL, NULL, NULL );
		if ( bRet == FALSE ) return FALSE;
		bRet = RemoveChild( pRefSrc, ulItemID );
		if ( bRet == FALSE ) return FALSE;
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// get pointer to CapInfo, the capability ID of that is 'ulID'
LPNkMAIDCapInfo GetCapInfo(LPRefObj pRef, ULONG ulID)
{
	ULONG i;
	LPNkMAIDCapInfo pCapInfo;

	if (pRef == NULL)
		return NULL;
	for ( i = 0; i < pRef->ulCapCount; i++ ){
		pCapInfo = (LPNkMAIDCapInfo)( (char*)pRef->pCapArray + i * sizeof(NkMAIDCapInfo) );
		if ( pCapInfo->ulID == ulID )
			break;
	}
	if ( i < pRef->ulCapCount )
		return pCapInfo;
	else
		return NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL CheckCapabilityOperation(LPRefObj pRef, ULONG ulID, ULONG ulOperations)
{
	SLONG nResult;
	LPNkMAIDCapInfo pCapInfo = GetCapInfo(pRef, ulID);

	if(pCapInfo != NULL){
		if(pCapInfo->ulOperations & ulOperations){
			nResult = kNkMAIDResult_NoError;
		}else{
			nResult = kNkMAIDResult_NotSupported;
		}
	}else{
		nResult = kNkMAIDResult_NotSupported;
	}

	return (nResult == kNkMAIDResult_NoError);
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL AddChild( LPRefObj pRefParent, SLONG lIDChild )
{
	SLONG lResult;
	ULONG ulCount = pRefParent->ulChildCount;
	LPVOID pNewMemblock = realloc( pRefParent->pRefChildArray, (ulCount + 1) * sizeof(LPRefObj) );
	LPRefObj pRefChild = (LPRefObj)malloc( sizeof(RefObj) );

	if(pNewMemblock == NULL || pRefChild == NULL) {
		puts( "There is not enough memory" );
		return FALSE;
	}
	pRefParent->pRefChildArray = pNewMemblock;
	((LPRefObj*)pRefParent->pRefChildArray)[ulCount] = pRefChild;
	InitRefObj(pRefChild);
	pRefChild->lMyID = lIDChild;
	pRefChild->pRefParent = pRefParent;
	pRefChild->pObject = (LPNkMAIDObject)malloc(sizeof(NkMAIDObject));
	if(pRefChild->pObject == NULL){
		puts( "There is not enough memory" );
		pRefParent->pRefChildArray = realloc( pRefParent->pRefChildArray, ulCount * sizeof(LPRefObj) );
		return FALSE;
	}

	pRefChild->pObject->refClient = (NKREF)pRefChild;
	lResult = Command_Open( pRefParent->pObject, pRefChild->pObject, lIDChild );
	if(lResult == TRUE)
		pRefParent->ulChildCount ++;
	else {
		puts( "Failed in Opening an object." );
		pRefParent->pRefChildArray = realloc( pRefParent->pRefChildArray, ulCount * sizeof(LPRefObj) );
		free(pRefChild->pObject);
		free(pRefChild);
		return FALSE;
	}

	lResult = EnumCapabilities( pRefChild->pObject, &(pRefChild->ulCapCount), &(pRefChild->pCapArray), NULL, NULL );
	
	// set callback functions to child object.
	SetProc( pRefChild );

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL RemoveChild( LPRefObj pRefParent, SLONG lIDChild )
{
	LPRefObj pRefChild = NULL, *pOldRefChildArray, *pNewRefChildArray;
	ULONG i, n;
	pRefChild = GetRefChildPtr_ID( pRefParent, lIDChild );
	if ( pRefChild == NULL ) return FALSE;

	while ( pRefChild->ulChildCount > 0 )
		RemoveChild( pRefChild, ((LPRefObj*)pRefChild->pRefChildArray)[0]->lMyID );

	if ( ResetProc( pRefChild ) == FALSE ) return FALSE;
	if ( Command_Close( pRefChild->pObject ) == FALSE ) return FALSE;
	pOldRefChildArray = (LPRefObj*)pRefParent->pRefChildArray;
	pNewRefChildArray = NULL;
	if( pRefParent->ulChildCount > 1 ){
		pNewRefChildArray = (LPRefObj*)malloc( (pRefParent->ulChildCount - 1) * sizeof(LPRefObj) );
		for( n = 0, i = 0; i < pRefParent->ulChildCount; i++ ){
			if( ((LPRefObj)pOldRefChildArray[i])->lMyID != lIDChild )
				memmove( &pNewRefChildArray[n++], &pOldRefChildArray[i], sizeof(LPRefObj) );
		}
	}
	pRefParent->pRefChildArray = pNewRefChildArray;
	pRefParent->ulChildCount--;
	if ( pRefChild->pObject != NULL )
		free( pRefChild->pObject );
	if ( pRefChild->pCapArray != NULL )
		free( pRefChild->pCapArray );
	if ( pRefChild->pRefChildArray != NULL )
		free( pRefChild->pRefChildArray );
	if ( pRefChild != NULL )
		free( pRefChild );
	if ( pOldRefChildArray != NULL )
		free( pOldRefChildArray );
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL SetProc( LPRefObj pRefObj )
{
	BOOL bRet;
	NkMAIDCallback	stProc;
	stProc.refProc = (NKREF)pRefObj;

	if( CheckCapabilityOperation( pRefObj, kNkMAIDCapability_ProgressProc, kNkMAIDCapOperation_Set ) ){
		stProc.pProc = (LPNKFUNC)ProgressProc;
		bRet = Command_CapSet( pRefObj->pObject, kNkMAIDCapability_ProgressProc, kNkMAIDDataType_CallbackPtr, (NKPARAM)&stProc, NULL, NULL );
		if ( bRet == FALSE ) return FALSE;
	}

	switch ( pRefObj->pObject->ulType  ) {
		case kNkMAIDObjectType_Module:
			// If Module object supports Cap_EventProc, set ModEventProc. 
			if( CheckCapabilityOperation( pRefObj, kNkMAIDCapability_EventProc, kNkMAIDCapOperation_Set ) ) {
				stProc.pProc = (LPNKFUNC)ModEventProc;
				bRet = Command_CapSet( pRefObj->pObject, kNkMAIDCapability_EventProc, kNkMAIDDataType_CallbackPtr, (NKPARAM)&stProc, NULL, NULL );
				if ( bRet == FALSE ) return FALSE;
			}
			// UIRequestProc is supported by Module object only.
			if( CheckCapabilityOperation( pRefObj, kNkMAIDCapability_UIRequestProc, kNkMAIDCapOperation_Set ) ) {
				stProc.pProc = (LPNKFUNC)UIRequestProc;
				bRet = Command_CapSet( pRefObj->pObject, kNkMAIDCapability_UIRequestProc, kNkMAIDDataType_CallbackPtr, (NKPARAM)&stProc, NULL, NULL );
				if ( bRet == FALSE ) return FALSE;
			}
			break;
		case kNkMAIDObjectType_Source:
			// If Source object supports Cap_EventProc, set SrcEventProc. 
			if( CheckCapabilityOperation( pRefObj, kNkMAIDCapability_EventProc, kNkMAIDCapOperation_Set ) ) {
				stProc.pProc = (LPNKFUNC)SrcEventProc;
				bRet = Command_CapSet( pRefObj->pObject, kNkMAIDCapability_EventProc, kNkMAIDDataType_CallbackPtr, (NKPARAM)&stProc, NULL, NULL );
				if ( bRet == FALSE ) return FALSE;
			}
			break;
		case kNkMAIDObjectType_Item:
			// If Item object supports Cap_EventProc, set ItmEventProc. 
			if( CheckCapabilityOperation( pRefObj, kNkMAIDCapability_EventProc, kNkMAIDCapOperation_Set ) ) {
				stProc.pProc = (LPNKFUNC)ItmEventProc;
				bRet = Command_CapSet( pRefObj->pObject, kNkMAIDCapability_EventProc, kNkMAIDDataType_CallbackPtr, (NKPARAM)&stProc, NULL, NULL );
				if ( bRet == FALSE ) return FALSE;
			}
			break;
		case kNkMAIDObjectType_DataObj:
			// if Data object supports Cap_EventProc, set DatEventProc. 
			if( CheckCapabilityOperation( pRefObj, kNkMAIDCapability_EventProc, kNkMAIDCapOperation_Set ) ) {
				stProc.pProc = (LPNKFUNC)DatEventProc;
				bRet = Command_CapSet( pRefObj->pObject, kNkMAIDCapability_EventProc, kNkMAIDDataType_CallbackPtr, (NKPARAM)&stProc, NULL, NULL );
				if ( bRet == FALSE ) return FALSE;
			}
			break;
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
//
BOOL ResetProc( LPRefObj pRefObj )
{
	BOOL bRet;

	if( CheckCapabilityOperation( pRefObj, kNkMAIDCapability_ProgressProc, kNkMAIDCapOperation_Set ) ) {
		bRet = Command_CapSet( pRefObj->pObject, kNkMAIDCapability_ProgressProc, kNkMAIDDataType_Null, (NKPARAM)NULL, NULL, NULL );
		if ( bRet == FALSE ) return FALSE;
	}
	if( CheckCapabilityOperation( pRefObj, kNkMAIDCapability_EventProc, kNkMAIDCapOperation_Set ) ) {
		bRet = Command_CapSet( pRefObj->pObject, kNkMAIDCapability_EventProc, kNkMAIDDataType_Null, (NKPARAM)NULL, NULL, NULL );
		if ( bRet == FALSE ) return FALSE;
	}

	if ( pRefObj->pObject->ulType == kNkMAIDObjectType_Module ) {
			// UIRequestProc is supported by Module object only.
			if( CheckCapabilityOperation( pRefObj, kNkMAIDCapability_UIRequestProc, kNkMAIDCapOperation_Set ) ) {
				bRet = Command_CapSet( pRefObj->pObject, kNkMAIDCapability_UIRequestProc, kNkMAIDDataType_Null, (NKPARAM)NULL, NULL, NULL );
				if ( bRet == FALSE ) return FALSE;
			}
	}

	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Get pointer to reference of child object by child's ID
LPRefObj GetRefChildPtr_ID( LPRefObj pRefParent, SLONG lIDChild )
{
	LPRefObj pRefChild;
	ULONG ulCount;

	if(pRefParent == NULL)
		return NULL;

	for( ulCount = 0; ulCount < pRefParent->ulChildCount; ulCount++ ){
		if ( (pRefChild = GetRefChildPtr_Index(pRefParent, ulCount)) != NULL ) {
			if (pRefChild->lMyID == lIDChild)
				return pRefChild;
		}
	}

	return NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------
// Get pointer to reference of child object by index
LPRefObj GetRefChildPtr_Index( LPRefObj pRefParent, ULONG ulIndex )
{
	if (pRefParent == NULL)
		return NULL;

	if( (pRefParent->pRefChildArray != NULL) && (ulIndex < pRefParent->ulChildCount) )
		return (LPRefObj)((LPRefObj*)pRefParent->pRefChildArray)[ulIndex];
	else
		return NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------

#if defined( _WIN32 )
BOOL WINAPI	cancelhandler(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_C_EVENT) {
		g_bCancel = TRUE;
		return TRUE;
	}
	return FALSE;
}
#elif defined(__APPLE__)
void	cancelhandler(int sig)
{
	g_bCancel = TRUE;
}
#endif
//------------------------------------------------------------------------------------------------------------------------------------
