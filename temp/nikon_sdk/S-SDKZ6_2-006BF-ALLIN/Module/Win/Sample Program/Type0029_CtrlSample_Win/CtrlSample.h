//================================================================================================
// Copyright Nikon Corporation - All rights reserved
//
// View this file in a non-proportional font, tabs = 3
//================================================================================================

#if defined( _WIN32 )
#elif defined(__APPLE__)
//	#include	<CodeFragments.h>
#endif
/////////////////////////////////////////////////////////////////////////////
// Structures

#pragma pack(push, 2)

	typedef struct tagRefObj
	{
		LPNkMAIDObject	pObject;
		SLONG lMyID;
		LPVOID pRefParent;
		ULONG ulChildCount;
		LPVOID pRefChildArray;
		ULONG ulCapCount;
		LPNkMAIDCapInfo pCapArray;
	} RefObj, *LPRefObj;

	typedef struct tagRefCompletionProc
	{
//		BOOL bEnd;
		ULONG* pulCount;
		NKERROR nResult;
//		LPVOID pcProgressDlg;
		LPVOID pRef;
	} RefCompletionProc, *LPRefCompletionProc;

	typedef struct tagRefDataProc
	{
		LPVOID	pBuffer;
		ULONG	ulOffset;
		ULONG	ulTotalLines;
		SLONG	lID;
	} RefDataProc, *LPRefDataProc;

	typedef struct tagPSDFileHeader
	{
		char	type[5];
		char	space11[1];
		char	space01[6];
		short	Planecount; 	//0004 if RGB, this is 0003
		long	rowPixels;
		long	columnPixels;
		short	bits; 			//0008 means 8bit. 16bit also supported
		short	mode; 			//0004 means CMYK, Gray -- 1, RGB -- 3
		char	space02[14];
	} PSDFileHeader, *LPPSDFileHeader;

	typedef struct tagRefSpecialCap
	{
		ULONG ulCapID;
		ULONG ulCapValue;
//		ULONG ulCapType;
		ULONG ulUIID;
	} RefSpecialCap, *LPRefSpecialCap;

	typedef struct tagSettingLensData
	{
		UCHAR  ucFmmManualSetting;      // Fmm
		UCHAR  ucF0ManualSetting;       // F0
		short sReserved;              // Reserved
	} SettingLensData, FAR* LPSettingLensData;


	typedef struct tagGetLensDataInfo
	{
		ULONG  ulLensID;								// Lens Number, 0xFFFFFFFF means all Setting Data
		ULONG  ulCount;									// The amount of Setting Data
		SettingLensData  pSettingLensData[20];			// The pointer to Thumbnail Data
	} GetLensDataInfo, FAR* LPGetLensDataInfo;

#pragma pack(pop)


/////////////////////////////////////////////////////////////////////////////
// Prototype

SLONG	CallMAIDEntryPoint( 
		LPNkMAIDObject	pObject,				// module, source, item, or data object
		ULONG				ulCommand,			// Command, one of eNkMAIDCommand
		ULONG				ulParam,				// parameter for the command
		ULONG				ulDataType,			// Data type, one of eNkMAIDDataType
		NKPARAM			data,					// Pointer or long integer
		LPNKFUNC			pfnComplete,		// Completion function, may be NULL
		NKREF				refComplete );		// Value passed to pfnComplete
BOOL	Command_Async( LPNkMAIDObject pObject);
BOOL	Command_CapSet(LPNkMAIDObject pObject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete);
BOOL	Command_CapGet(LPNkMAIDObject pObject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete);
BOOL	Command_CapSetSB(LPNkMAIDObject pObject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete, SLONG* pnResult);
BOOL	Command_CapGetSB(LPNkMAIDObject pObject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete, SLONG* pnResult);
BOOL	Command_CapStart(LPNkMAIDObject pObject, ULONG ulParam, LPNKFUNC pfnComplete, NKREF refComplete, SLONG* pnResult);
BOOL	Command_CapStartGeneric( LPNkMAIDObject pObject, ULONG ulParam, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete, SLONG* pnResult );
BOOL	Command_CapGetArray( LPNkMAIDObject pObject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete );
BOOL	Command_CapGetDefault( LPNkMAIDObject pObject, ULONG ulParam, ULONG ulDataType, NKPARAM pData, LPNKFUNC pfnComplete, NKREF refComplete );
BOOL	Command_Abort(LPNkMAIDObject pobject, LPNKFUNC pfnComplete, NKREF refComplete);
BOOL	Command_Open( LPNkMAIDObject pParentObj, LPNkMAIDObject pChildObj, ULONG ulChildID );
BOOL	Command_Close( LPNkMAIDObject pObject );

void	CALLPASCAL CALLBACK ModEventProc( NKREF refProc, ULONG ulEvent, NKPARAM data );
void	CALLPASCAL CALLBACK SrcEventProc( NKREF refProc, ULONG ulEvent, NKPARAM data );
void	CALLPASCAL CALLBACK ItmEventProc( NKREF refProc, ULONG ulEvent, NKPARAM data );
void	CALLPASCAL CALLBACK DatEventProc( NKREF refProc, ULONG ulEvent, NKPARAM data );
void	CALLPASCAL CALLBACK ProgressProc( ULONG ulCommand, ULONG ulParam, NKREF refProc, ULONG ulDone, ULONG ulTotal );
ULONG	CALLPASCAL CALLBACK UIRequestProc( NKREF ref, LPNkMAIDUIRequestInfo pUIRequest );
void	CALLPASCAL CALLBACK CompletionProc( LPNkMAIDObject pObject, ULONG ulCommand, ULONG ulParam, ULONG ulDataType, NKPARAM data, NKREF refComplete, NKERROR nResult );
NKERROR	CALLPASCAL CALLBACK DataProc( NKREF ref, LPVOID pDataInfo, LPVOID pData );

void	InitRefObj( LPRefObj pRef );
BOOL	Search_Module( void* Path );
BOOL	Load_Module( void* Path );
BOOL	Close_Module( LPRefObj pRefMod );
BOOL	EnumCapabilities( LPNkMAIDObject pobject, ULONG* pulCapCount, LPNkMAIDCapInfo* ppCapArray, LPNKFUNC pfnComplete, NKREF refComplete );
BOOL	EnumChildrten( LPNkMAIDObject pobject );
BOOL	AddChild( LPRefObj pRefParent, SLONG lIDChild );
BOOL	RemoveChild( LPRefObj pRefParent, SLONG lIDChild );
BOOL	SetProc( LPRefObj pRefObj );
BOOL	ResetProc( LPRefObj pRefObj );
BOOL	IdleLoop( LPNkMAIDObject pObject, ULONG* pulCount, ULONG ulEndCount );
void WaitEvent(void);

BOOL	SourceCommandLoop( LPRefObj pRefMod, ULONG ulSrcID );
BOOL	ItemCommandLoop( LPRefObj pRefSrc, ULONG ulItemID );
BOOL	ImageCommandLoop( LPRefObj pRefItm, ULONG ulDatID );
BOOL	MovieCommandLoop( LPRefObj pRefItm, ULONG ulDatID );
BOOL	ThumbnailCommandLoop( LPRefObj pRefItm, ULONG ulDatID );
BOOL	SelectSource( LPRefObj pRefMod, ULONG *pulSrcID );
BOOL	SelectItem( LPRefObj pRefSrc, ULONG *pulItemID );
BOOL	SelectData( LPRefObj pRefItm, ULONG *pulDataType );
BOOL	CheckDataType( LPRefObj pRefItm, ULONG *pulDataType );
BOOL	SetUpCamera1( LPRefObj pRefSrc );
BOOL	SetUpCamera2( LPRefObj pRefSrc );
BOOL	SetShootingMenu( LPRefObj pRefSrc );
BOOL	SetMovieMenu(LPRefObj pRefSrc);
BOOL	SetLiveView(LPRefObj pRefSrc);
BOOL	SetCustomSettings( LPRefObj pRefSrc );
BOOL	SetSBMenu( LPRefObj pRefSrc );
BOOL	SetEnumCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	SetEnumUnsignedCapability( LPRefObj pRefObj, ULONG ulCapID, LPNkMAIDEnum pstEnum );
BOOL	SetEnumPackedStringCapability( LPRefObj pRefObj, ULONG ulCapID, LPNkMAIDEnum pstEnum );
BOOL	SetEnumStringCapability( LPRefObj pRefObj, ULONG ulCapID, LPNkMAIDEnum pstEnum );
BOOL	SetFloatCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	SetBoolCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	SetIntegerCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	SetUnsignedCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	GetUnsignedCapability( LPRefObj pRefObj, ULONG ulCapID, ULONG* pulValue );
BOOL	SetStringCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	SetSizeCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	SetDateTimeCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	SetRangeCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	TimeCodeOriginCapability(LPRefObj pRefSrc);
BOOL	GetManualSettingLensDataCapability(LPRefObj pRefSrc);
BOOL    ConvertLensDataString(ULONG ulLensID, LPGetLensDataInfo stGetLensDataInfo);
BOOL	SetTrackingAFAreaCapability(LPRefObj pRefSrc);
BOOL	DeleteDramCapability( LPRefObj pRefItem, ULONG ulItmID );
BOOL	GetLiveViewImageCapability( LPRefObj pRefSrc );
BOOL	PictureControlDataCapability(LPRefObj pRefSrc, ULONG ulCapID);
BOOL	SetPictureControlDataCapability(LPRefObj pRefObj, NkMAIDPicCtrlData* pPicCtrlData, char* filename, ULONG ulCapID);
BOOL	GetPictureControlDataCapability(LPRefObj pRefObj, NkMAIDPicCtrlData* pPicCtrlData, ULONG ulCapID);
BOOL	DeleteCustomPictureControlCapability(LPRefObj pRefSrc, ULONG ulCapID);
BOOL	GetSBHandlesCapability( LPRefObj pRefObj );
BOOL	GetSBAttrDescCapability( LPRefObj pRefObj );
BOOL	SBAttrValueCapability( LPRefObj pRefObj );
BOOL	SetSBAttrValueCapability( LPRefObj pRefObj, NkMAIDSBAttrValue *pstSbAttrValue );
BOOL	GetSBAttrValueCapability( LPRefObj pRefObj, NkMAIDSBAttrValue *pstSbAttrValue );
BOOL	GetSBGroupAttrDescCapability( LPRefObj pRefObj );
BOOL	SBGroupAttrValueCapability( LPRefObj pRefObj );
BOOL	SetSBGroupAttrValueCapability( LPRefObj pRefObj, NkMAIDSBGroupAttrValue *pstSbGroupAttrValue );
BOOL	GetSBGroupAttrValueCapability( LPRefObj pRefObj, NkMAIDSBGroupAttrValue *pstSbGroupAttrValue );
BOOL	TestFlashCapability( LPRefObj pRefObj );
BOOL	ShowArrayCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	GetArrayCapability( LPRefObj pRefObj, ULONG ulCapID, LPNkMAIDArray pstArray );
BOOL	LoadArrayCapability( LPRefObj pRefObj, ULONG ulCapID, char* filename );
char*	GetEnumString( ULONG ulCapID, ULONG ulValue, char *psString );
char*	GetUnsignedString( ULONG ulCapID, ULONG ulValue, char *psString );
BOOL	IssueProcess( LPRefObj pRefSrc, ULONG ulCapID );
BOOL	IssueProcessSync( LPRefObj pRefSrc, ULONG ulCapID );
BOOL	IssueAcquire( LPRefObj pRefDat );
BOOL	GetVideoImageExCapability(LPRefObj pRefDat, ULONG ulCapID);
BOOL	IssueThumbnail( LPRefObj pRefSrc );
BOOL	SetPointCapability( LPRefObj pRefObj, ULONG ulCapID );
BOOL	TerminateCaptureCapability( LPRefObj pRefSrc );
BOOL	GetRecordingInfoCapability(LPRefObj pRefObj);

LPNkMAIDCapInfo	GetCapInfo( LPRefObj pRef, ULONG ulID );
BOOL	CheckCapabilityOperation( LPRefObj pRef, ULONG ulID, ULONG ulOperations );
LPRefObj	GetRefChildPtr_Index( LPRefObj pRefParent, ULONG ulIndex );
LPRefObj	GetRefChildPtr_ID( LPRefObj pRefParent, SLONG lIDChild );


/////////////////////////////////////////////////////////////////////////////
// Static variables

extern LPMAIDEntryPointProc	g_pMAIDEntryPoint;
extern UCHAR	g_bFileRemoved;
extern BOOL		g_bFirstCall;	// used in ProgressProc, and DoDeleteDramImage
#if defined( _WIN32 )
	extern HINSTANCE	g_hInstModule;
#elif defined(__APPLE__)
	extern CFBundleRef gBundle;
#endif

