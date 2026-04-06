/*****************************************************************************/
//  MAID3D1.H    Rev. 2.73
//      Extra definition of Maid3.h
/*****************************************************************************/
#ifndef _MAID3D1_H
#define _MAID3D1_H

#ifndef _MAID3_
    #include "Maid3.h"
#endif

///////////////////////////////////////////////////////////////////////////////
#define kNkMAIDCapability_VendorBaseDX2 kNkMAIDCapability_VendorBase + 0x100
#define kNkMAIDResult_DX2Origin kNkMAIDResult_VendorBase + 129
#define kNkMAIDEvent_DX2Origin  kNkMAIDEvent_CapChangeValueOnly + 0x100

///////////////////////////////////////////////////////////////////////////////
enum eNkMAIDCapabilityD1
{
    kNkMAIDCapability_ModuleMode            = kNkMAIDCapability_VendorBaseDX2 + 0x01,// 0x8101
    kNkMAIDCapability_CurrentDirectory      = kNkMAIDCapability_VendorBaseDX2 + 0x02,
    kNkMAIDCapability_FormatStorage         = kNkMAIDCapability_VendorBaseDX2 + 0x03,
    kNkMAIDCapability_PreCapture            = kNkMAIDCapability_VendorBaseDX2 + 0x04,
    kNkMAIDCapability_LockFocus             = kNkMAIDCapability_VendorBaseDX2 + 0x05,
    kNkMAIDCapability_LockExposure          = kNkMAIDCapability_VendorBaseDX2 + 0x06,
    kNkMAIDCapability_LifeTime              = kNkMAIDCapability_VendorBaseDX2 + 0x07,
    kNkMAIDCapability_CFStatus              = kNkMAIDCapability_VendorBaseDX2 + 0x08,
    kNkMAIDCapability_ClockBatteryLevel     = kNkMAIDCapability_VendorBaseDX2 + 0x09,
    kNkMAIDCapability_FlashStatus           = kNkMAIDCapability_VendorBaseDX2 + 0x0a,
    kNkMAIDCapability_ExposureStatus        = kNkMAIDCapability_VendorBaseDX2 + 0x0b,
    kNkMAIDCapability_MediaType             = kNkMAIDCapability_VendorBaseDX2 + 0x0c,
    kNkMAIDCapability_FileType              = kNkMAIDCapability_VendorBaseDX2 + 0x0f,
    kNkMAIDCapability_CompressionLevel      = kNkMAIDCapability_VendorBaseDX2 + 0x10,
    kNkMAIDCapability_ExposureMode          = kNkMAIDCapability_VendorBaseDX2 + 0x11,
    kNkMAIDCapability_ShutterSpeed          = kNkMAIDCapability_VendorBaseDX2 + 0x12,
    kNkMAIDCapability_Aperture              = kNkMAIDCapability_VendorBaseDX2 + 0x13,
    kNkMAIDCapability_FlexibleProgram       = kNkMAIDCapability_VendorBaseDX2 + 0x14,
    kNkMAIDCapability_ExposureComp          = kNkMAIDCapability_VendorBaseDX2 + 0x15,
    kNkMAIDCapability_MeteringMode          = kNkMAIDCapability_VendorBaseDX2 + 0x16,
    kNkMAIDCapability_Sensitivity           = kNkMAIDCapability_VendorBaseDX2 + 0x17,
    kNkMAIDCapability_WBMode                = kNkMAIDCapability_VendorBaseDX2 + 0x18,
    kNkMAIDCapability_WBTuneAuto            = kNkMAIDCapability_VendorBaseDX2 + 0x19,
    kNkMAIDCapability_WBTuneIncandescent    = kNkMAIDCapability_VendorBaseDX2 + 0x1a,
    kNkMAIDCapability_WBTuneFluorescent     = kNkMAIDCapability_VendorBaseDX2 + 0x1b,
    kNkMAIDCapability_WBTuneSunny           = kNkMAIDCapability_VendorBaseDX2 + 0x1c,
    kNkMAIDCapability_WBTuneFlash           = kNkMAIDCapability_VendorBaseDX2 + 0x1d,
    kNkMAIDCapability_WBTuneShade           = kNkMAIDCapability_VendorBaseDX2 + 0x1e,
    kNkMAIDCapability_WBTuneCloudy          = kNkMAIDCapability_VendorBaseDX2 + 0x1f,
    kNkMAIDCapability_FocusMode             = kNkMAIDCapability_VendorBaseDX2 + 0x20,
    kNkMAIDCapability_FocusAreaMode         = kNkMAIDCapability_VendorBaseDX2 + 0x21,
    kNkMAIDCapability_FocusPreferredArea    = kNkMAIDCapability_VendorBaseDX2 + 0x22,
    kNkMAIDCapability_FocalLength           = kNkMAIDCapability_VendorBaseDX2 + 0x23,
    kNkMAIDCapability_ClockDateTime         = kNkMAIDCapability_VendorBaseDX2 + 0x24,
    kNkMAIDCapability_CustomSettings        = kNkMAIDCapability_VendorBaseDX2 + 0x25,
    kNkMAIDCapability_BracketingOrder       = kNkMAIDCapability_VendorBaseDX2 + 0x26,
    kNkMAIDCapability_BracketingVary        = kNkMAIDCapability_VendorBaseDX2 + 0x27,
    kNkMAIDCapability_AFonRelease           = kNkMAIDCapability_VendorBaseDX2 + 0x28,
    kNkMAIDCapability_AFAreaSelector        = kNkMAIDCapability_VendorBaseDX2 + 0x29,
    kNkMAIDCapability_AFsPriority           = kNkMAIDCapability_VendorBaseDX2 + 0x2a,
    kNkMAIDCapability_AFcPriority           = kNkMAIDCapability_VendorBaseDX2 + 0x2b,
    kNkMAIDCapability_EVInterval            = kNkMAIDCapability_VendorBaseDX2 + 0x00,
    kNkMAIDCapability_CWMeteringDiameter    = kNkMAIDCapability_VendorBaseDX2 + 0x2d,
    kNkMAIDCapability_AELockonRelease       = kNkMAIDCapability_VendorBaseDX2 + 0x2e,
    kNkMAIDCapability_ExchangeDials         = kNkMAIDCapability_VendorBaseDX2 + 0x2f,
    kNkMAIDCapability_EasyExposureComp      = kNkMAIDCapability_VendorBaseDX2 + 0x30,
    kNkMAIDCapability_Microscope            = kNkMAIDCapability_VendorBaseDX2 + 0x31,
    kNkMAIDCapability_AutoOffDelay          = kNkMAIDCapability_VendorBaseDX2 + 0x32,
    kNkMAIDCapability_SelfTimerDuration     = kNkMAIDCapability_VendorBaseDX2 + 0x33,
    kNkMAIDCapability_LCDBackLight          = kNkMAIDCapability_VendorBaseDX2 + 0x34,
    kNkMAIDCapability_PlayBackImage         = kNkMAIDCapability_VendorBaseDX2 + 0x35,
    kNkMAIDCapability_LimitImageDisplay     = kNkMAIDCapability_VendorBaseDX2 + 0x36,
    kNkMAIDCapability_BlinkTimerLED         = kNkMAIDCapability_VendorBaseDX2 + 0x37,
    kNkMAIDCapability_ApertureDial          = kNkMAIDCapability_VendorBaseDX2 + 0x38,
    kNkMAIDCapability_ZoomAperture          = kNkMAIDCapability_VendorBaseDX2 + 0x39,
    kNkMAIDCapability_AEAFLockButton        = kNkMAIDCapability_VendorBaseDX2 + 0x3a,
    kNkMAIDCapability_EdgeEnhancement       = kNkMAIDCapability_VendorBaseDX2 + 0x3b,
    kNkMAIDCapability_Curve                 = kNkMAIDCapability_VendorBaseDX2 + 0x3c,
    kNkMAIDCapability_ShootingSpeed         = kNkMAIDCapability_VendorBaseDX2 + 0x3d,
    kNkMAIDCapability_ShootingLimit         = kNkMAIDCapability_VendorBaseDX2 + 0x3e,
    kNkMAIDCapability_LutHeader             = kNkMAIDCapability_VendorBaseDX2 + 0x3f,
    kNkMAIDCapability_FileHeader            = kNkMAIDCapability_VendorBaseDX2 + 0x40,
    kNkMAIDCapability_LockCamera            = kNkMAIDCapability_VendorBaseDX2 + 0x41,
    kNkMAIDCapability_LockShutterSpeed      = kNkMAIDCapability_VendorBaseDX2 + 0x42,
    kNkMAIDCapability_LockAperture          = kNkMAIDCapability_VendorBaseDX2 + 0x43,
    kNkMAIDCapability_LensInfo              = kNkMAIDCapability_VendorBaseDX2 + 0x44,
    kNkMAIDCapability_MirrorUp              = kNkMAIDCapability_VendorBaseDX2 + 0x45,
    kNkMAIDCapability_EnableNIF             = kNkMAIDCapability_VendorBaseDX2 + 0x46,
    kNkMAIDCapability_PlaybackMode          = kNkMAIDCapability_VendorBaseDX2 + 0x47,
    kNkMAIDCapability_UserComment           = kNkMAIDCapability_VendorBaseDX2 + 0x48,
    kNkMAIDCapability_DeleteDirectory       = kNkMAIDCapability_VendorBaseDX2 + 0x2c,
    kNkMAIDCapability_NumberingMode         = kNkMAIDCapability_VendorBaseDX2 + 0x49,
    kNkMAIDCapability_ReadOnly              = kNkMAIDCapability_VendorBaseDX2 + 0x4a,
    kNkMAIDCapability_Invisible             = kNkMAIDCapability_VendorBaseDX2 + 0x4b,
    kNkMAIDCapability_DirCreatedByDx2       = kNkMAIDCapability_VendorBaseDX2 + 0x4c,
    kNkMAIDCapability_ContinuousInPCMode    = kNkMAIDCapability_VendorBaseDX2 + 0x4d,
    kNkMAIDCapability_CurrentDirID          = kNkMAIDCapability_VendorBaseDX2 + 0x4e,
    kNkMAIDCapability_SensitivityIncrease   = kNkMAIDCapability_VendorBaseDX2 + 0x4f,
    kNkMAIDCapability_WritingMedia          = kNkMAIDCapability_VendorBaseDX2 + 0x50,
    kNkMAIDCapability_WBPresetNumber        = kNkMAIDCapability_VendorBaseDX2 + 0x51,
    kNkMAIDCapability_ThumbnailSize         = kNkMAIDCapability_VendorBaseDX2 + 0x52,
    kNkMAIDCapability_SensitivityInterval   = kNkMAIDCapability_VendorBaseDX2 + 0x53,
    kNkMAIDCapability_ShootNoCard           = kNkMAIDCapability_VendorBaseDX2 + 0x54,
    kNkMAIDCapability_ColorReproduct        = kNkMAIDCapability_VendorBaseDX2 + 0x56,
    kNkMAIDCapability_ImageSize             = kNkMAIDCapability_VendorBaseDX2 + 0x57,
    kNkMAIDCapability_CompressRAW           = kNkMAIDCapability_VendorBaseDX2 + 0x58,
    kNkMAIDCapability_EnableMonitor         = kNkMAIDCapability_VendorBaseDX2 + 0x59,
    kNkMAIDCapability_WBGainRed             = kNkMAIDCapability_VendorBaseDX2 + 0x5a,
    kNkMAIDCapability_WBGainBlue            = kNkMAIDCapability_VendorBaseDX2 + 0x5b,
    kNkMAIDCapability_MakeDirectory         = kNkMAIDCapability_VendorBaseDX2 + 0x5c,
    kNkMAIDCapability_RearPanelDisplayMode  = kNkMAIDCapability_VendorBaseDX2 + 0x5d,
    kNkMAIDCapability_ColorAdjustment       = kNkMAIDCapability_VendorBaseDX2 + 0x5e,
    kNkMAIDCapability_SelectFUNC            = kNkMAIDCapability_VendorBaseDX2 + 0x5f,

    kNkMAIDCapability_TypicalFlashMode      = kNkMAIDCapability_VendorBaseDX2 + 0x60,
    kNkMAIDCapability_Converter             = kNkMAIDCapability_VendorBaseDX2 + 0x61,
    kNkMAIDCapability_ElectronicZoom        = kNkMAIDCapability_VendorBaseDX2 + 0x62,
    kNkMAIDCapability_DateFormat            = kNkMAIDCapability_VendorBaseDX2 + 0x63,
    kNkMAIDCapability_PreviewInterval       = kNkMAIDCapability_VendorBaseDX2 + 0x64,

    kNkMAIDCapability_MenuBank              = kNkMAIDCapability_VendorBaseDX2 + 0x65,
    kNkMAIDCapability_FlashComp             = kNkMAIDCapability_VendorBaseDX2 + 0x66,
    kNkMAIDCapability_NoAperture            = kNkMAIDCapability_VendorBaseDX2 + 0x67,
    kNkMAIDCapability_AntiVibration         = kNkMAIDCapability_VendorBaseDX2 + 0x68,
    kNkMAIDCapability_BatteryPack           = kNkMAIDCapability_VendorBaseDX2 + 0x69,
    kNkMAIDCapability_ResetCustomSetting    = kNkMAIDCapability_VendorBaseDX2 + 0x6a,
    kNkMAIDCapability_ImagePreview          = kNkMAIDCapability_VendorBaseDX2 + 0x6b,
    kNkMAIDCapability_IsoControl            = kNkMAIDCapability_VendorBaseDX2 + 0x6c,
    kNkMAIDCapability_NoiseReduction        = kNkMAIDCapability_VendorBaseDX2 + 0x6d,
    kNkMAIDCapability_FocusAreaLed          = kNkMAIDCapability_VendorBaseDX2 + 0x6e,
    kNkMAIDCapability_AfSubLight            = kNkMAIDCapability_VendorBaseDX2 + 0x6f,
    kNkMAIDCapability_AfButton              = kNkMAIDCapability_VendorBaseDX2 + 0x70,
    kNkMAIDCapability_SoundLevel            = kNkMAIDCapability_VendorBaseDX2 + 0x71,
    kNkMAIDCapability_FinderMode            = kNkMAIDCapability_VendorBaseDX2 + 0x72,
    kNkMAIDCapability_AeBracketNum          = kNkMAIDCapability_VendorBaseDX2 + 0x73,
    kNkMAIDCapability_WbBracketNum          = kNkMAIDCapability_VendorBaseDX2 + 0x74,
    kNkMAIDCapability_InternalSplMode       = kNkMAIDCapability_VendorBaseDX2 + 0x75,
    kNkMAIDCapability_EnableComment         = kNkMAIDCapability_VendorBaseDX2 + 0x76,
    kNkMAIDCapability_PresetExpMode         = kNkMAIDCapability_VendorBaseDX2 + 0x77,
    kNkMAIDCapability_PossibleToShoot       = kNkMAIDCapability_VendorBaseDX2 + 0x78,
    kNkMAIDCapability_ResetFileNumber       = kNkMAIDCapability_VendorBaseDX2 + 0x79,
    kNkMAIDCapability_ExpCompInterval           = kNkMAIDCapability_VendorBaseDX2 + 0x80,
    kNkMAIDCapability_FocusGroupPreferredArea   = kNkMAIDCapability_VendorBaseDX2 + 0x8a,
    kNkMAIDCapability_ResetMenuBank             = kNkMAIDCapability_VendorBaseDX2 + 0x8b,
    kNkMAIDCapability_WBTuneColorTemp           = kNkMAIDCapability_VendorBaseDX2 + 0x8c,
    kNkMAIDCapability_ShootingMode              = kNkMAIDCapability_VendorBaseDX2 + 0x8d,
    kNkMAIDCapability_LockFV                    = kNkMAIDCapability_VendorBaseDX2 + 0x8e,
    kNkMAIDCapability_RemainContinuousShooting  = kNkMAIDCapability_VendorBaseDX2 + 0x8f,
    kNkMAIDCapability_ShootingBankName          = kNkMAIDCapability_VendorBaseDX2 + 0x90,
    kNkMAIDCapability_WBPresetName              = kNkMAIDCapability_VendorBaseDX2 + 0x91,
    kNkMAIDCapability_FmmManual                 = kNkMAIDCapability_VendorBaseDX2 + 0x92,
    kNkMAIDCapability_F0Manual                  = kNkMAIDCapability_VendorBaseDX2 + 0x93,
    kNkMAIDCapability_CustomBankName            = kNkMAIDCapability_VendorBaseDX2 + 0x94,
    kNkMAIDCapability_AfGroupAreaPattern        = kNkMAIDCapability_VendorBaseDX2 + 0x95,
    kNkMAIDCapability_FocusFrameInMf            = kNkMAIDCapability_VendorBaseDX2 + 0x96,
    kNkMAIDCapability_FocusFrameInContinuousShooting= kNkMAIDCapability_VendorBaseDX2 + 0x97,
    kNkMAIDCapability_FocusFrameDisplayTime     = kNkMAIDCapability_VendorBaseDX2 + 0x98,
    kNkMAIDCapability_ExposureDelay             = kNkMAIDCapability_VendorBaseDX2 + 0x99,
    kNkMAIDCapability_AddIccProfile             = kNkMAIDCapability_VendorBaseDX2 + 0x9a,
    kNkMAIDCapability_ShootCounterInFinder      = kNkMAIDCapability_VendorBaseDX2 + 0x9b,
    kNkMAIDCapability_CenterButtonOnShooting    = kNkMAIDCapability_VendorBaseDX2 + 0x9c,
    kNkMAIDCapability_CenterButtonOnPlayback    = kNkMAIDCapability_VendorBaseDX2 + 0x9d,
    kNkMAIDCapability_MultiSelector             = kNkMAIDCapability_VendorBaseDX2 + 0x9e,
    kNkMAIDCapability_MultiSelectorDirection    = kNkMAIDCapability_VendorBaseDX2 + 0x9f,
    kNkMAIDCapability_CommandDialDirection          = kNkMAIDCapability_VendorBaseDX2 + 0xa0,
    kNkMAIDCapability_EnableCommandDialOnPlayback   = kNkMAIDCapability_VendorBaseDX2 + 0xa1,
    kNkMAIDCapability_UniversalMode             = kNkMAIDCapability_VendorBaseDX2 + 0xa2,
    kNkMAIDCapability_VerticalAfButton          = kNkMAIDCapability_VendorBaseDX2 + 0xa4,
    kNkMAIDCapability_FlashSyncTime             = kNkMAIDCapability_VendorBaseDX2 + 0xa5,
    kNkMAIDCapability_FlashSlowLimit            = kNkMAIDCapability_VendorBaseDX2 + 0xa6,
    kNkMAIDCapability_ExternalFlashMode         = kNkMAIDCapability_VendorBaseDX2 + 0xa7,
    kNkMAIDCapability_ModelingOnPreviewButton   = kNkMAIDCapability_VendorBaseDX2 + 0xa8,
    kNkMAIDCapability_BracketingFactor          = kNkMAIDCapability_VendorBaseDX2 + 0xa9,
    kNkMAIDCapability_BracketingMethod          = kNkMAIDCapability_VendorBaseDX2 + 0xaa,
    kNkMAIDCapability_RGBGain                   = kNkMAIDCapability_VendorBaseDX2 + 0xab,
    kNkMAIDCapability_USBSpeed                  = kNkMAIDCapability_VendorBaseDX2 + 0xac,
    kNkMAIDCapability_WBPresetData              = kNkMAIDCapability_VendorBaseDX2 + 0xad,
    kNkMAIDCapability_ContinuousShootingNum     = kNkMAIDCapability_VendorBaseDX2 + 0xae,
    kNkMAIDCapability_EnableBracketing          = kNkMAIDCapability_VendorBaseDX2 + 0xaf,
    kNkMAIDCapability_BracketingType            = kNkMAIDCapability_VendorBaseDX2 + 0xb0,
    kNkMAIDCapability_AEBracketingStep          = kNkMAIDCapability_VendorBaseDX2 + 0xb1,
    kNkMAIDCapability_WBBracketingStep          = kNkMAIDCapability_VendorBaseDX2 + 0xb2,
    kNkMAIDCapability_BracketingCount           = kNkMAIDCapability_VendorBaseDX2 + 0xb3,
    kNkMAIDCapability_CameraInclination         = kNkMAIDCapability_VendorBaseDX2 + 0xb4,
    kNkMAIDCapability_RawJpegImageStatus        = kNkMAIDCapability_VendorBaseDX2 + 0xb5,
    kNkMAIDCapability_CaptureDustImage          = kNkMAIDCapability_VendorBaseDX2 + 0xb6,
    kNkMAIDCapability_ZoomRateOnPlayback        = kNkMAIDCapability_VendorBaseDX2 + 0xb7,
    kNkMAIDCapability_AfGroupAreaPatternType    = kNkMAIDCapability_VendorBaseDX2 + 0xb8,
    kNkMAIDCapability_ExternalFlashSort         = kNkMAIDCapability_VendorBaseDX2 + 0xb9,
    kNkMAIDCapability_ExternalOldTypeFlashMode  = kNkMAIDCapability_VendorBaseDX2 + 0xba,
    kNkMAIDCapability_ExternalNewTypeFlashMode  = kNkMAIDCapability_VendorBaseDX2 + 0xbb,
    kNkMAIDCapability_InternalFlashStatus       = kNkMAIDCapability_VendorBaseDX2 + 0xbc,
    kNkMAIDCapability_ExternalFlashStatus       = kNkMAIDCapability_VendorBaseDX2 + 0xbd,
    kNkMAIDCapability_InternalFlashComp         = kNkMAIDCapability_VendorBaseDX2 + 0xbe,
    kNkMAIDCapability_ExternalFlashComp         = kNkMAIDCapability_VendorBaseDX2 + 0xbf,
    kNkMAIDCapability_ImageSetting              = kNkMAIDCapability_VendorBaseDX2 + 0xc0,
    kNkMAIDCapability_SaturationSetting         = kNkMAIDCapability_VendorBaseDX2 + 0xc1,
    kNkMAIDCapability_Beep                      = kNkMAIDCapability_VendorBaseDX2 + 0xc2,
    kNkMAIDCapability_AFMode                    = kNkMAIDCapability_VendorBaseDX2 + 0xc3,
    kNkMAIDCapability_ISOAutoShutterTime        = kNkMAIDCapability_VendorBaseDX2 + 0xc4,
    kNkMAIDCapability_InternalSplValue          = kNkMAIDCapability_VendorBaseDX2 + 0xc5,
    kNkMAIDCapability_InternalSplCommand        = kNkMAIDCapability_VendorBaseDX2 + 0xc6,
    kNkMAIDCapability_RecommendFlashDisp        = kNkMAIDCapability_VendorBaseDX2 + 0xc7,
    kNkMAIDCapability_RemoteTimer               = kNkMAIDCapability_VendorBaseDX2 + 0xc8,
    kNkMAIDCapability_CameraInclinationMode     = kNkMAIDCapability_VendorBaseDX2 + 0xc9,
    kNkMAIDCapability_InternalSplCommandValue   = kNkMAIDCapability_VendorBaseDX2 + 0xcb,
    kNkMAIDCapability_PreviewStatus             = kNkMAIDCapability_VendorBaseDX2 + 0xcc,
    kNkMAIDCapability_PreviewImage              = kNkMAIDCapability_VendorBaseDX2 + 0xcd,
    kNkMAIDCapability_CCDDataMode               = kNkMAIDCapability_VendorBaseDX2 + 0xd0,
    kNkMAIDCapability_JpegCompressionPolicy     = kNkMAIDCapability_VendorBaseDX2 + 0xd1,
    kNkMAIDCapability_AFLockOn                  = kNkMAIDCapability_VendorBaseDX2 + 0xd2,
    kNkMAIDCapability_FocalLengthControl        = kNkMAIDCapability_VendorBaseDX2 + 0xd3,
    kNkMAIDCapability_ExpBaseMatrix             = kNkMAIDCapability_VendorBaseDX2 + 0xd4,
    kNkMAIDCapability_ExpBaseCenter             = kNkMAIDCapability_VendorBaseDX2 + 0xd5,
    kNkMAIDCapability_ExpBaseSpot               = kNkMAIDCapability_VendorBaseDX2 + 0xd6,
    kNkMAIDCapability_CameraType                = kNkMAIDCapability_VendorBaseDX2 + 0xd7,
    kNkMAIDCapability_NoiseReductionHighISO     = kNkMAIDCapability_VendorBaseDX2 + 0xd8,
    kNkMAIDCapability_EasyExposureCompMode      = kNkMAIDCapability_VendorBaseDX2 + 0xd9,
    kNkMAIDCapability_DeviceNameList            = kNkMAIDCapability_VendorBaseDX2 + 0xda,
    kNkMAIDCapability_ImageColorSpace           = kNkMAIDCapability_VendorBaseDX2 + 0xdb,
    kNkMAIDCapability_ISOAutoSetting            = kNkMAIDCapability_VendorBaseDX2 + 0xdc,
    kNkMAIDCapability_ImageMode                 = kNkMAIDCapability_VendorBaseDX2 + 0xdd,
// kNkMAIDCapability_VendorBaseDX2 + 0xe0 <--> + 0xee are reserved.
    kNkMAIDCapability_FocusAreaFrame            = kNkMAIDCapability_VendorBaseDX2 + 0xf1,
    kNkMAIDCapability_ISOAutoHiLimit            = kNkMAIDCapability_VendorBaseDX2 + 0xf2,
    kNkMAIDCapability_BeepEx                    = kNkMAIDCapability_VendorBaseDX2 + 0xf3,
    kNkMAIDCapability_AFLockOnEx                = kNkMAIDCapability_VendorBaseDX2 + 0xf4,
    kNkMAIDCapability_WarningDisp               = kNkMAIDCapability_VendorBaseDX2 + 0xf5,
    kNkMAIDCapability_CellKind                  = kNkMAIDCapability_VendorBaseDX2 + 0xf6,
    kNkMAIDCapability_InternalSplMRPTValue      = kNkMAIDCapability_VendorBaseDX2 + 0xf7,
    kNkMAIDCapability_InternalSplMRPTCount      = kNkMAIDCapability_VendorBaseDX2 + 0xf8,
    kNkMAIDCapability_InternalSplMRPTInterval   = kNkMAIDCapability_VendorBaseDX2 + 0xf9,
    kNkMAIDCapability_InternalSplCommandChannel = kNkMAIDCapability_VendorBaseDX2 + 0xfa,
    kNkMAIDCapability_InternalSplCmdSelfComp    = kNkMAIDCapability_VendorBaseDX2 + 0xfb,
    kNkMAIDCapability_InternalSplCmdGroupAMode  = kNkMAIDCapability_VendorBaseDX2 + 0xfc,
    kNkMAIDCapability_InternalSplCmdGroupAComp  = kNkMAIDCapability_VendorBaseDX2 + 0xfd,
    kNkMAIDCapability_InternalSplCmdGroupAValue = kNkMAIDCapability_VendorBaseDX2 + 0xfe,
    kNkMAIDCapability_InternalSplCmdGroupBMode  = kNkMAIDCapability_VendorBaseDX2 + 0xff,
    kNkMAIDCapability_InternalSplCmdGroupBComp  = kNkMAIDCapability_VendorBaseDX2 + 0x100,
    kNkMAIDCapability_InternalSplCmdGroupBValue = kNkMAIDCapability_VendorBaseDX2 + 0x101,
    kNkMAIDCapability_InternalSplCmdSelfMode    = kNkMAIDCapability_VendorBaseDX2 + 0x102,
    kNkMAIDCapability_InternalSplCmdSelfValue   = kNkMAIDCapability_VendorBaseDX2 + 0x103,
    kNkMAIDCapability_SelectFUNC2               = kNkMAIDCapability_VendorBaseDX2 + 0x104,
    kNkMAIDCapability_LutIndexNumber            = kNkMAIDCapability_VendorBaseDX2 + 0x105,
    kNkMAIDCapability_MonochromeFilterEffect    = kNkMAIDCapability_VendorBaseDX2 + 0x120,
    kNkMAIDCapability_MonochromeEdgeEnhancement = kNkMAIDCapability_VendorBaseDX2 + 0x121,
    kNkMAIDCapability_MonochromeCurve           = kNkMAIDCapability_VendorBaseDX2 + 0x122,
    kNkMAIDCapability_AutoFPShoot               = kNkMAIDCapability_VendorBaseDX2 + 0x123,
    kNkMAIDCapability_MonochromeSettingType     = kNkMAIDCapability_VendorBaseDX2 + 0x124,
    kNkMAIDCapability_AFCapture                 = kNkMAIDCapability_VendorBaseDX2 + 0x125,
    kNkMAIDCapability_AutoOffTimer              = kNkMAIDCapability_VendorBaseDX2 + 0x126,
    kNkMAIDCapability_ImageConfirmTime          = kNkMAIDCapability_VendorBaseDX2 + 0x127,
    kNkMAIDCapability_InfoDisplayErrStatus      = kNkMAIDCapability_VendorBaseDX2 + 0x128,
    kNkMAIDCapability_ExternalSplMode           = kNkMAIDCapability_VendorBaseDX2 + 0x129,
    kNkMAIDCapability_ExternalSplValue          = kNkMAIDCapability_VendorBaseDX2 + 0x12a,
    kNkMAIDCapability_Slot2ImageSaveMode        = kNkMAIDCapability_VendorBaseDX2 + 0x12c,
    kNkMAIDCapability_CompressRAWBitMode        = kNkMAIDCapability_VendorBaseDX2 + 0x12d,
    kNkMAIDCapability_PictureControl            = kNkMAIDCapability_VendorBaseDX2 + 0x12e,
    kNkMAIDCapability_IntegratedLevel           = kNkMAIDCapability_VendorBaseDX2 + 0x12f,
    kNkMAIDCapability_Brightness                = kNkMAIDCapability_VendorBaseDX2 + 0x130,
    kNkMAIDCapability_MonochromeTuneColors      = kNkMAIDCapability_VendorBaseDX2 + 0x131,
    kNkMAIDCapability_Active_D_Lighting         = kNkMAIDCapability_VendorBaseDX2 + 0x132,
    kNkMAIDCapability_DynamicAFArea             = kNkMAIDCapability_VendorBaseDX2 + 0x133,
    kNkMAIDCapability_ShootingSpeedHigh         = kNkMAIDCapability_VendorBaseDX2 + 0x134,
    kNkMAIDCapability_InfoDispSetting           = kNkMAIDCapability_VendorBaseDX2 + 0x135,
    kNkMAIDCapability_PreviewButton             = kNkMAIDCapability_VendorBaseDX2 + 0x137,
    kNkMAIDCapability_PreviewButton2            = kNkMAIDCapability_VendorBaseDX2 + 0x138,
    kNkMAIDCapability_AEAFLockButton2           = kNkMAIDCapability_VendorBaseDX2 + 0x13a,
    kNkMAIDCapability_IndicatorDisplay          = kNkMAIDCapability_VendorBaseDX2 + 0x13b,
    kNkMAIDCapability_LiveViewMode              = kNkMAIDCapability_VendorBaseDX2 + 0x13c,
    kNkMAIDCapability_LiveViewDriveMode         = kNkMAIDCapability_VendorBaseDX2 + 0x13d,
    kNkMAIDCapability_LiveViewStatus            = kNkMAIDCapability_VendorBaseDX2 + 0x13e,
    kNkMAIDCapability_LiveViewImageZoomRate     = kNkMAIDCapability_VendorBaseDX2 + 0x13f,
    kNkMAIDCapability_ContrastAF                = kNkMAIDCapability_VendorBaseDX2 + 0x140,
    kNkMAIDCapability_DeleteDramImage           = kNkMAIDCapability_VendorBaseDX2 + 0x143,
    kNkMAIDCapability_CurrentPreviewID          = kNkMAIDCapability_VendorBaseDX2 + 0x144,
    kNkMAIDCapability_GetPreviewImageLow        = kNkMAIDCapability_VendorBaseDX2 + 0x145,
    kNkMAIDCapability_GetPreviewImageNormal     = kNkMAIDCapability_VendorBaseDX2 + 0x146,
    kNkMAIDCapability_GetLiveViewImage          = kNkMAIDCapability_VendorBaseDX2 + 0x147,
    kNkMAIDCapability_MFDriveStep               = kNkMAIDCapability_VendorBaseDX2 + 0x148,
    kNkMAIDCapability_MFDrive                   = kNkMAIDCapability_VendorBaseDX2 + 0x149,
    kNkMAIDCapability_ContrastAFArea            = kNkMAIDCapability_VendorBaseDX2 + 0x14a,
    kNkMAIDCapability_CompressRAWEx             = kNkMAIDCapability_VendorBaseDX2 + 0x14b,
    kNkMAIDCapability_CellKindPriority          = kNkMAIDCapability_VendorBaseDX2 + 0x14c,
    kNkMAIDCapability_WBFluorescentType         = kNkMAIDCapability_VendorBaseDX2 + 0x14d,
    kNkMAIDCapability_WBTuneColorAdjust         = kNkMAIDCapability_VendorBaseDX2 + 0x14e,
    kNkMAIDCapability_WBTunePreset1             = kNkMAIDCapability_VendorBaseDX2 + 0x14f,
    kNkMAIDCapability_WBTunePreset2             = kNkMAIDCapability_VendorBaseDX2 + 0x150,
    kNkMAIDCapability_WBTunePreset3             = kNkMAIDCapability_VendorBaseDX2 + 0x151,
    kNkMAIDCapability_WBTunePreset4             = kNkMAIDCapability_VendorBaseDX2 + 0x152,
    kNkMAIDCapability_WBTunePreset5             = kNkMAIDCapability_VendorBaseDX2 + 0x153,
    kNkMAIDCapability_AFAreaPoint               = kNkMAIDCapability_VendorBaseDX2 + 0x154,
    kNkMAIDCapability_NormalAfButton            = kNkMAIDCapability_VendorBaseDX2 + 0x155,
    kNkMAIDCapability_ManualSetLensNo           = kNkMAIDCapability_VendorBaseDX2 + 0x156,
    kNkMAIDCapability_AutoDXCrop                = kNkMAIDCapability_VendorBaseDX2 + 0x157,
    kNkMAIDCapability_PictureControlData        = kNkMAIDCapability_VendorBaseDX2 + 0x158,
    kNkMAIDCapability_GetPicCtrlInfo            = kNkMAIDCapability_VendorBaseDX2 + 0x159,
    kNkMAIDCapability_DeleteCustomPictureControl= kNkMAIDCapability_VendorBaseDX2 + 0x15a,

    kNkMAIDCapability_LensType                  = kNkMAIDCapability_VendorBaseDX2 + 0x15c,
    kNkMAIDCapability_ChangedPictureControl     = kNkMAIDCapability_VendorBaseDX2 + 0x15d,
    kNkMAIDCapability_LiveViewProhibit          = kNkMAIDCapability_VendorBaseDX2 + 0x15e,
    kNkMAIDCapability_DateImprintSetting        = kNkMAIDCapability_VendorBaseDX2 + 0x15f,
    kNkMAIDCapability_DateCounterSelect         = kNkMAIDCapability_VendorBaseDX2 + 0x160,
    kNkMAIDCapability_DateCounterData           = kNkMAIDCapability_VendorBaseDX2 + 0x161,
    kNkMAIDCapability_DateCounterDispSetting    = kNkMAIDCapability_VendorBaseDX2 + 0x162,
    kNkMAIDCapability_RangeFinderSetting        = kNkMAIDCapability_VendorBaseDX2 + 0x163,
    kNkMAIDCapability_RangeFinderStatus         = kNkMAIDCapability_VendorBaseDX2 + 0x164,
    kNkMAIDCapability_AutoOffPhoto              = kNkMAIDCapability_VendorBaseDX2 + 0x165,
    kNkMAIDCapability_AutoOffMenu               = kNkMAIDCapability_VendorBaseDX2 + 0x166,
    kNkMAIDCapability_AutoOffInfo               = kNkMAIDCapability_VendorBaseDX2 + 0x167,
    kNkMAIDCapability_ScreenTips                = kNkMAIDCapability_VendorBaseDX2 + 0x168,
    kNkMAIDCapability_IlluminationSetting       = kNkMAIDCapability_VendorBaseDX2 + 0x169,
    kNkMAIDCapability_ShutterSpeedLockSetting   = kNkMAIDCapability_VendorBaseDX2 + 0x16a,
    kNkMAIDCapability_ApertureLockSetting       = kNkMAIDCapability_VendorBaseDX2 + 0x16b,
    kNkMAIDCapability_VignetteControl           = kNkMAIDCapability_VendorBaseDX2 + 0x16c,
    kNkMAIDCapability_FocusPointBrightness      = kNkMAIDCapability_VendorBaseDX2 + 0x16d,
    kNkMAIDCapability_EnableCopyright           = kNkMAIDCapability_VendorBaseDX2 + 0x16e,
    kNkMAIDCapability_ArtistName                = kNkMAIDCapability_VendorBaseDX2 + 0x16f,
    kNkMAIDCapability_CopyrightInfo             = kNkMAIDCapability_VendorBaseDX2 + 0x170,
    kNkMAIDCapability_AngleLevel                = kNkMAIDCapability_VendorBaseDX2 + 0x171,
    kNkMAIDCapability_MovieScreenSize           = kNkMAIDCapability_VendorBaseDX2 + 0x172,
    kNkMAIDCapability_MovieVoice                = kNkMAIDCapability_VendorBaseDX2 + 0x173,
    kNkMAIDCapability_LiveViewAF                = kNkMAIDCapability_VendorBaseDX2 + 0x175,
    kNkMAIDCapability_SelfTimerShootNum         = kNkMAIDCapability_VendorBaseDX2 + 0x176,
    kNkMAIDCapability_FinderISODisplay          = kNkMAIDCapability_VendorBaseDX2 + 0x177,
    kNkMAIDCapability_EnableCommandDialOnPlaybackEx = kNkMAIDCapability_VendorBaseDX2 + 0x178,
    kNkMAIDCapability_ExchangeDialsEx           = kNkMAIDCapability_VendorBaseDX2 + 0x179,
    kNkMAIDCapability_CenterButtonOnLiveView    = kNkMAIDCapability_VendorBaseDX2 + 0x180,
    kNkMAIDCapability_ZoomRateOnLiveView        = kNkMAIDCapability_VendorBaseDX2 + 0x181,
    kNkMAIDCapability_AutoDistortion            = kNkMAIDCapability_VendorBaseDX2 + 0x182,
    kNkMAIDCapability_SceneMode                 = kNkMAIDCapability_VendorBaseDX2 + 0x183,
    kNkMAIDCapability_LiveViewScreenDispSetting = kNkMAIDCapability_VendorBaseDX2 + 0x184,
    kNkMAIDCapability_MovieRecMicrophone        = kNkMAIDCapability_VendorBaseDX2 + 0x185,
    kNkMAIDCapability_MovieRecDestination       = kNkMAIDCapability_VendorBaseDX2 + 0x186,
    kNkMAIDCapability_PrimarySlot               = kNkMAIDCapability_VendorBaseDX2 + 0x187,
    kNkMAIDCapability_ADLBracketingType         = kNkMAIDCapability_VendorBaseDX2 + 0x188,
    kNkMAIDCapability_SelectFUNC2CapAreaCrop    = kNkMAIDCapability_VendorBaseDX2 + 0x191,
    kNkMAIDCapability_PreviewButton2CapAreaCrop = kNkMAIDCapability_VendorBaseDX2 + 0x192,
    kNkMAIDCapability_AEAFLockButton2CapAreaCrop= kNkMAIDCapability_VendorBaseDX2 + 0x193,
    kNkMAIDCapability_BracketButton             = kNkMAIDCapability_VendorBaseDX2 + 0x194,
    kNkMAIDCapability_RemainCountInMedia        = kNkMAIDCapability_VendorBaseDX2 + 0x195,
    kNkMAIDCapability_AngleLevelPitch           = kNkMAIDCapability_VendorBaseDX2 + 0x196,
    kNkMAIDCapability_AngleLevelYaw             = kNkMAIDCapability_VendorBaseDX2 + 0x197,
    kNkMAIDCapability_MovRecInCardStatus        = kNkMAIDCapability_VendorBaseDX2 + 0x198,
    kNkMAIDCapability_MovRecInCardProhibit      = kNkMAIDCapability_VendorBaseDX2 + 0x199,
// kNkMAIDCapability_VendorBaseDX2 + 0x200 <--> + 0x203 are reserved.
    kNkMAIDCapability_ActiveSlot                = kNkMAIDCapability_VendorBaseDX2 + 0x204,
    kNkMAIDCapability_SaveMedia                 = kNkMAIDCapability_VendorBaseDX2 + 0x205,
    kNkMAIDCapability_MovieRecHiISO             = kNkMAIDCapability_VendorBaseDX2 + 0x206,

    kNkMAIDCapability_UserMode1                 = kNkMAIDCapability_VendorBaseDX2 + 0x207,
    kNkMAIDCapability_UserMode2                 = kNkMAIDCapability_VendorBaseDX2 + 0x208,
    kNkMAIDCapability_MovieManualSetting        = kNkMAIDCapability_VendorBaseDX2 + 0x209,
    kNkMAIDCapability_AFModeAtLiveView          = kNkMAIDCapability_VendorBaseDX2 + 0x210,
    kNkMAIDCapability_SelfTimerShootInterval    = kNkMAIDCapability_VendorBaseDX2 + 0x211,
    kNkMAIDCapability_AutoOffLiveView           = kNkMAIDCapability_VendorBaseDX2 + 0x212,
    kNkMAIDCapability_RemoteCtrlWaitTime        = kNkMAIDCapability_VendorBaseDX2 + 0x213,
    kNkMAIDCapability_BeepVolume                = kNkMAIDCapability_VendorBaseDX2 + 0x214,
    kNkMAIDCapability_VideoMode                 = kNkMAIDCapability_VendorBaseDX2 + 0x215,
    kNkMAIDCapability_WBAutoType                = kNkMAIDCapability_VendorBaseDX2 + 0x216,
    kNkMAIDCapability_GetVideoImage             = kNkMAIDCapability_VendorBaseDX2 + 0x217,
    kNkMAIDCapability_TerminateCapture          = kNkMAIDCapability_VendorBaseDX2 + 0x218,
    kNkMAIDCapability_EffectMode                = kNkMAIDCapability_VendorBaseDX2 + 0x219,
    kNkMAIDCapability_HDRMode                   = kNkMAIDCapability_VendorBaseDX2 + 0x220,
    kNkMAIDCapability_HDRExposure               = kNkMAIDCapability_VendorBaseDX2 + 0x221,
    kNkMAIDCapability_HDRSmoothing              = kNkMAIDCapability_VendorBaseDX2 + 0x222,
    kNkMAIDCapability_BlinkingStatus            = kNkMAIDCapability_VendorBaseDX2 + 0x223,
    kNkMAIDCapability_AutoSceneModeStatus       = kNkMAIDCapability_VendorBaseDX2 + 0x224,
    kNkMAIDCapability_WBTuneColorTempEx         = kNkMAIDCapability_VendorBaseDX2 + 0x225,
    kNkMAIDCapability_WBPresetProtect1          = kNkMAIDCapability_VendorBaseDX2 + 0x226,
    kNkMAIDCapability_WBPresetProtect2          = kNkMAIDCapability_VendorBaseDX2 + 0x227,
    kNkMAIDCapability_WBPresetProtect3          = kNkMAIDCapability_VendorBaseDX2 + 0x228,
    kNkMAIDCapability_WBPresetProtect4          = kNkMAIDCapability_VendorBaseDX2 + 0x229,
    kNkMAIDCapability_ISOAutoShutterTimeAutoValue = kNkMAIDCapability_VendorBaseDX2 + 0x230,
    kNkMAIDCapability_MovieImageQuality         = kNkMAIDCapability_VendorBaseDX2 + 0x231,
    kNkMAIDCapability_MovieRecMicrophoneValue   = kNkMAIDCapability_VendorBaseDX2 + 0x232,
    kNkMAIDCapability_LiveViewExposurePreview   = kNkMAIDCapability_VendorBaseDX2 + 0x233,
    kNkMAIDCapability_LiveViewSelector          = kNkMAIDCapability_VendorBaseDX2 + 0x234,
    kNkMAIDCapability_LiveViewWBMode            = kNkMAIDCapability_VendorBaseDX2 + 0x235,
    kNkMAIDCapability_MovieShutterSpeed         = kNkMAIDCapability_VendorBaseDX2 + 0x236,
    kNkMAIDCapability_MovieAperture             = kNkMAIDCapability_VendorBaseDX2 + 0x237,
    kNkMAIDCapability_MovieSensitivity          = kNkMAIDCapability_VendorBaseDX2 + 0x238,
    kNkMAIDCapability_MovieExposureComp         = kNkMAIDCapability_VendorBaseDX2 + 0x239,
    kNkMAIDCapability_ADLBracketingStep         = kNkMAIDCapability_VendorBaseDX2 + 0x243,
    kNkMAIDCapability_ResetWBMode               = kNkMAIDCapability_VendorBaseDX2 + 0x244,
    kNkMAIDCapability_ExpCompFlashUsed          = kNkMAIDCapability_VendorBaseDX2 + 0x245,
    kNkMAIDCapability_LiveViewPhotoShootingMode = kNkMAIDCapability_VendorBaseDX2 + 0x246,
    kNkMAIDCapability_ExposureDelayEx           = kNkMAIDCapability_VendorBaseDX2 + 0x247,
    kNkMAIDCapability_MovieISORange             = kNkMAIDCapability_VendorBaseDX2 + 0x248,
    kNkMAIDCapability_MovieReleaseButton        = kNkMAIDCapability_VendorBaseDX2 + 0x249,
    kNkMAIDCapability_MovieRecFrameCount        = kNkMAIDCapability_VendorBaseDX2 + 0x250,
    kNkMAIDCapability_CurrentItemID             = kNkMAIDCapability_VendorBaseDX2 + 0x251,
    kNkMAIDCapability_GetIPTCInfo               = kNkMAIDCapability_VendorBaseDX2 + 0x252,
    kNkMAIDCapability_LiveViewImageSize         = kNkMAIDCapability_VendorBaseDX2 + 0x253,
    kNkMAIDCapability_RemoteControlMode         = kNkMAIDCapability_VendorBaseDX2 + 0x254,
    kNkMAIDCapability_WBTunePreset6             = kNkMAIDCapability_VendorBaseDX2 + 0x255,
    kNkMAIDCapability_WBPresetProtect5          = kNkMAIDCapability_VendorBaseDX2 + 0x256,
    kNkMAIDCapability_WBPresetProtect6          = kNkMAIDCapability_VendorBaseDX2 + 0x257,
    kNkMAIDCapability_SpotWBMode                = kNkMAIDCapability_VendorBaseDX2 + 0x258,
    kNkMAIDCapability_SpotWBMeasure             = kNkMAIDCapability_VendorBaseDX2 + 0x259,
    kNkMAIDCapability_SpotWBChangeArea          = kNkMAIDCapability_VendorBaseDX2 + 0x260,
    kNkMAIDCapability_SpotWBResultDispEnd       = kNkMAIDCapability_VendorBaseDX2 + 0x261,
// kNkMAIDCapability_VendorBaseDX2 + 0x262 <--> + 0x319 are reserved.
    kNkMAIDCapability_MovieWindNoiseReduction   = kNkMAIDCapability_VendorBaseDX2 + 0x320,
    kNkMAIDCapability_RetractableLensWarningStatus  = kNkMAIDCapability_VendorBaseDX2 + 0x321,
    kNkMAIDCapability_ISOControlSensitivity     = kNkMAIDCapability_VendorBaseDX2 + 0x322,
    kNkMAIDCapability_ExposureMeterLinkage      = kNkMAIDCapability_VendorBaseDX2 + 0x330,
// kNkMAIDCapability_VendorBaseDX2 + 0x331 <--> + 0x340 are reserved.
    kNkMAIDCapability_MovieRecordingZone        = kNkMAIDCapability_VendorBaseDX2 + 0x350,
    kNkMAIDCapability_MovieISOControl           = kNkMAIDCapability_VendorBaseDX2 + 0x351,
    kNkMAIDCapability_MovieISOAutoHiLimit       = kNkMAIDCapability_VendorBaseDX2 + 0x352,
    kNkMAIDCapability_RawImageSize              = kNkMAIDCapability_VendorBaseDX2 + 0x353,
    kNkMAIDCapability_RawJpegTransferStatus     = kNkMAIDCapability_VendorBaseDX2 + 0x354,
    kNkMAIDCapability_LimitAFAreaMode           = kNkMAIDCapability_VendorBaseDX2 + 0x355,
    kNkMAIDCapability_AFModeRestrictions        = kNkMAIDCapability_VendorBaseDX2 + 0x356,
    kNkMAIDCapability_ExpBaseHighlight          = kNkMAIDCapability_VendorBaseDX2 + 0x357,
    kNkMAIDCapability_ElectronicFrontCurtainShutter = kNkMAIDCapability_VendorBaseDX2 + 0x358,
    kNkMAIDCapability_PictureControlDataEx      = kNkMAIDCapability_VendorBaseDX2 + 0x359,
    kNkMAIDCapability_MovieWBMode               = kNkMAIDCapability_VendorBaseDX2 + 0x360,
    kNkMAIDCapability_MovieWBTuneAuto           = kNkMAIDCapability_VendorBaseDX2 + 0x361,
    kNkMAIDCapability_MovieWBAutoType           = kNkMAIDCapability_VendorBaseDX2 + 0x362,
    kNkMAIDCapability_MovieWBTuneIncandescent   = kNkMAIDCapability_VendorBaseDX2 + 0x363,
    kNkMAIDCapability_MovieWBFluorescentType    = kNkMAIDCapability_VendorBaseDX2 + 0x364,
    kNkMAIDCapability_MovieWBTuneFluorescent    = kNkMAIDCapability_VendorBaseDX2 + 0x365,
    kNkMAIDCapability_MovieWBTuneSunny          = kNkMAIDCapability_VendorBaseDX2 + 0x366,

    kNkMAIDCapability_MovieWBTuneShade          = kNkMAIDCapability_VendorBaseDX2 + 0x368,
    kNkMAIDCapability_MovieWBTuneCloudy         = kNkMAIDCapability_VendorBaseDX2 + 0x369,
    kNkMAIDCapability_MovieWBTuneColorTempEx    = kNkMAIDCapability_VendorBaseDX2 + 0x36a,
    kNkMAIDCapability_MovieWBTuneColorAdjust    = kNkMAIDCapability_VendorBaseDX2 + 0x36b,
    kNkMAIDCapability_MovieWBTunePreset1        = kNkMAIDCapability_VendorBaseDX2 + 0x36c,
    kNkMAIDCapability_MovieWBTunePreset2        = kNkMAIDCapability_VendorBaseDX2 + 0x36d,
    kNkMAIDCapability_MovieWBTunePreset3        = kNkMAIDCapability_VendorBaseDX2 + 0x36e,
    kNkMAIDCapability_MovieWBTunePreset4        = kNkMAIDCapability_VendorBaseDX2 + 0x36f,
    kNkMAIDCapability_MovieWBTunePreset5        = kNkMAIDCapability_VendorBaseDX2 + 0x370,
    kNkMAIDCapability_MovieWBTunePreset6        = kNkMAIDCapability_VendorBaseDX2 + 0x371,
    kNkMAIDCapability_MovieWBPresetProtect1     = kNkMAIDCapability_VendorBaseDX2 + 0x372,
    kNkMAIDCapability_MovieWBPresetProtect2     = kNkMAIDCapability_VendorBaseDX2 + 0x373,
    kNkMAIDCapability_MovieWBPresetProtect3     = kNkMAIDCapability_VendorBaseDX2 + 0x374,
    kNkMAIDCapability_MovieWBPresetProtect4     = kNkMAIDCapability_VendorBaseDX2 + 0x375,
    kNkMAIDCapability_MovieWBPresetProtect5     = kNkMAIDCapability_VendorBaseDX2 + 0x376,
    kNkMAIDCapability_MovieWBPresetProtect6     = kNkMAIDCapability_VendorBaseDX2 + 0x377,
    kNkMAIDCapability_MovieWBPresetNumber       = kNkMAIDCapability_VendorBaseDX2 + 0x378,
    kNkMAIDCapability_MovieWBPresetName         = kNkMAIDCapability_VendorBaseDX2 + 0x379,
    kNkMAIDCapability_MovieWBGainRed            = kNkMAIDCapability_VendorBaseDX2 + 0x37a,
    kNkMAIDCapability_MovieWBGainBlue           = kNkMAIDCapability_VendorBaseDX2 + 0x37b,
    kNkMAIDCapability_MoviePictureControlData   = kNkMAIDCapability_VendorBaseDX2 + 0x37c,
    kNkMAIDCapability_MoviePictureControlDataEx = kNkMAIDCapability_VendorBaseDX2 + 0x37d,
    kNkMAIDCapability_MovieResetMenuBank        = kNkMAIDCapability_VendorBaseDX2 + 0x37e,
    kNkMAIDCapability_MovieCCDDataMode          = kNkMAIDCapability_VendorBaseDX2 + 0x37f,
    kNkMAIDCapability_MovieAutoDXCrop           = kNkMAIDCapability_VendorBaseDX2 + 0x380,
    kNkMAIDCapability_MovieNoiseReductionHighISO    = kNkMAIDCapability_VendorBaseDX2 + 0x381,
    kNkMAIDCapability_MoviePictureControl       = kNkMAIDCapability_VendorBaseDX2 + 0x382,
    kNkMAIDCapability_ChangedMoviePictureControl    = kNkMAIDCapability_VendorBaseDX2 + 0x383,
    kNkMAIDCapability_MovieResetWBMode          = kNkMAIDCapability_VendorBaseDX2 + 0x384,
    kNkMAIDCapability_MovieMeteringMode         = kNkMAIDCapability_VendorBaseDX2 + 0x385,
    kNkMAIDCapability_GetMoviePicCtrlInfo       = kNkMAIDCapability_VendorBaseDX2 + 0x386,
    kNkMAIDCapability_DeleteMovieCustomPictureControl   = kNkMAIDCapability_VendorBaseDX2 + 0x387,
    kNkMAIDCapability_MovieWBPresetData         = kNkMAIDCapability_VendorBaseDX2 + 0x388,

    kNkMAIDCapability_PowerZoomByFocalLength            = kNkMAIDCapability_VendorBaseDX2 + 0x389,
    kNkMAIDCapability_MovieCaptureMode                  = kNkMAIDCapability_VendorBaseDX2 + 0x38a,
    kNkMAIDCapability_SlowMotionMovieRecordScreenSize   = kNkMAIDCapability_VendorBaseDX2 + 0x38b,
    kNkMAIDCapability_HighSpeedStillCaptureRate         = kNkMAIDCapability_VendorBaseDX2 + 0x38c,
    kNkMAIDCapability_BestMomentCaptureMode             = kNkMAIDCapability_VendorBaseDX2 + 0x38d,
    kNkMAIDCapability_ActiveSelectionFrameSavedDefault  = kNkMAIDCapability_VendorBaseDX2 + 0x38e,
    kNkMAIDCapability_ActiveSelectionCapture40frameOver = kNkMAIDCapability_VendorBaseDX2 + 0x38f,
    kNkMAIDCapability_ActiveSelectionOnReleaseRecord    = kNkMAIDCapability_VendorBaseDX2 + 0x390,
    kNkMAIDCapability_ActiveSelectionControl            = kNkMAIDCapability_VendorBaseDX2 + 0x391,
    kNkMAIDCapability_ActiveSelectionSelectedPictures   = kNkMAIDCapability_VendorBaseDX2 + 0x392,
    kNkMAIDCapability_SaveSelectionPictures             = kNkMAIDCapability_VendorBaseDX2 + 0x393,
    kNkMAIDCapability_LensTypeNikon1                    = kNkMAIDCapability_VendorBaseDX2 + 0x394,
    kNkMAIDCapability_SilentPhotography                 = kNkMAIDCapability_VendorBaseDX2 + 0x395,
    kNkMAIDCapability_FacePriority                      = kNkMAIDCapability_VendorBaseDX2 + 0x396,
    kNkMAIDCapability_OpticalVR                         = kNkMAIDCapability_VendorBaseDX2 + 0x397,
    kNkMAIDCapability_ElectronicVR                      = kNkMAIDCapability_VendorBaseDX2 + 0x398,
	kNkMAIDCapability_CaptureLV                         = kNkMAIDCapability_VendorBaseDX2 + 0x399,
	kNkMAIDCapability_AFCaptureLV                       = kNkMAIDCapability_VendorBaseDX2 + 0x39a,
	kNkMAIDCapability_DeviceReadyLV                     = kNkMAIDCapability_VendorBaseDX2 + 0x39b,

    kNkMAIDCapability_AFLockOnAcross                    = kNkMAIDCapability_VendorBaseDX2 + 0x3bc,
    kNkMAIDCapability_AFLockOnMove                      = kNkMAIDCapability_VendorBaseDX2 + 0x3bd,
    kNkMAIDCapability_IPTCPresetSelect                  = kNkMAIDCapability_VendorBaseDX2 + 0x3be,
    kNkMAIDCapability_FlashISOAutoHighLimit             = kNkMAIDCapability_VendorBaseDX2 + 0x3c1,
    kNkMAIDCapability_SBWirelessMode                    = kNkMAIDCapability_VendorBaseDX2 + 0x3c2,
    kNkMAIDCapability_SBWirelessMultipleFlashMode       = kNkMAIDCapability_VendorBaseDX2 + 0x3c3,
    kNkMAIDCapability_SBUsableGroup                     = kNkMAIDCapability_VendorBaseDX2 + 0x3c4,
    kNkMAIDCapability_WirelessCLSEntryMode              = kNkMAIDCapability_VendorBaseDX2 + 0x3c5,
    kNkMAIDCapability_SBPINCode                         = kNkMAIDCapability_VendorBaseDX2 + 0x3c6,
    kNkMAIDCapability_RadioMultipleFlashChannel         = kNkMAIDCapability_VendorBaseDX2 + 0x3c7,
    kNkMAIDCapability_OpticalMultipleFlashChannel       = kNkMAIDCapability_VendorBaseDX2 + 0x3c8,
    kNkMAIDCapability_FlashRangeDisplay                 = kNkMAIDCapability_VendorBaseDX2 + 0x3c9,
    kNkMAIDCapability_AllTestFiringDisable              = kNkMAIDCapability_VendorBaseDX2 + 0x3ca,
    kNkMAIDCapability_IPTCPresetInfo                    = kNkMAIDCapability_VendorBaseDX2 + 0x3cc,
    kNkMAIDCapability_GetSBHandles                      = kNkMAIDCapability_VendorBaseDX2 + 0x3cd,
    kNkMAIDCapability_GetSBAttrDesc                     = kNkMAIDCapability_VendorBaseDX2 + 0x3ce,
    kNkMAIDCapability_SBAttrValue                       = kNkMAIDCapability_VendorBaseDX2 + 0x3cf,
    kNkMAIDCapability_GetSBGroupAttrDesc                = kNkMAIDCapability_VendorBaseDX2 + 0x3d0,
    kNkMAIDCapability_SBGroupAttrValue                  = kNkMAIDCapability_VendorBaseDX2 + 0x3d1,
    kNkMAIDCapability_TestFlash                         = kNkMAIDCapability_VendorBaseDX2 + 0x3d2,
    kNkMAIDCapability_FaceDetection                     = kNkMAIDCapability_VendorBaseDX2 + 0x3d3,
    kNkMAIDCapability_3DTrackingCaptuerArea             = kNkMAIDCapability_VendorBaseDX2 + 0x3d4,
    kNkMAIDCapability_SBSettingMemberLock				= kNkMAIDCapability_VendorBaseDX2 + 0x3d6,
	kNkMAIDCapability_MatrixMetering					= kNkMAIDCapability_VendorBaseDX2 + 0x3d7,
	kNkMAIDCapability_CaptureAsync						= kNkMAIDCapability_VendorBaseDX2 + 0x3d8,
	kNkMAIDCapability_AFCaptureAsync					= kNkMAIDCapability_VendorBaseDX2 + 0x3d9,
	kNkMAIDCapability_DeviceReady						= kNkMAIDCapability_VendorBaseDX2 + 0x3da,
    kNkMAIDCapability_SBIntegrationFlashReady           = kNkMAIDCapability_VendorBaseDX2 + 0x3db,

    kNkMAIDCapability_MirrorUpCancel                    = kNkMAIDCapability_VendorBaseDX2 + 0x3ef,
    kNkMAIDCapability_MirrorUpStatus                    = kNkMAIDCapability_VendorBaseDX2 + 0x3f0,
    kNkMAIDCapability_MirrorUpReleaseShootingCount      = kNkMAIDCapability_VendorBaseDX2 + 0x3f1,
	kNkMAIDCapability_MovieActive_D_Lighting			= kNkMAIDCapability_VendorBaseDX2 + 0x3f2,
	kNkMAIDCapability_FlickerReductionSetting			= kNkMAIDCapability_VendorBaseDX2 + 0x3f3,
	kNkMAIDCapability_ExposureCompFlashUsed				= kNkMAIDCapability_VendorBaseDX2 + 0x3f4,
	kNkMAIDCapability_LimitStillCaptureMode				= kNkMAIDCapability_VendorBaseDX2 + 0x3f5,
	kNkMAIDCapability_MovieFileType						= kNkMAIDCapability_VendorBaseDX2 + 0x3f6,
	kNkMAIDCapability_SaveCameraSetting					= kNkMAIDCapability_VendorBaseDX2 + 0x3f7,
	kNkMAIDCapability_LiveViewTFTStatus					= kNkMAIDCapability_VendorBaseDX2 + 0x3f8,
	kNkMAIDCapability_LiveViewButtonMode				= kNkMAIDCapability_VendorBaseDX2 + 0x3f9,
	kNkMAIDCapability_ExternalRecordingControl			= kNkMAIDCapability_VendorBaseDX2 + 0x3fa,
    kNkMAIDCapability_DetectionPeaking                  = kNkMAIDCapability_VendorBaseDX2 + 0x3fb,
    kNkMAIDCapability_WBTuneNatural                     = kNkMAIDCapability_VendorBaseDX2 + 0x3fc,
    kNkMAIDCapability_HighlightBrightness               = kNkMAIDCapability_VendorBaseDX2 + 0x3fd,
    kNkMAIDCapability_MovieWBTuneNatural                = kNkMAIDCapability_VendorBaseDX2 + 0x3fe,
    kNkMAIDCapability_MovieAttenuator                   = kNkMAIDCapability_VendorBaseDX2 + 0x3ff,
	kNkMAIDCapability_GetRecordingInfo                  = kNkMAIDCapability_VendorBaseDX2 + 0x400,
    kNkMAIDCapability_PictureControlDataEx2             = kNkMAIDCapability_VendorBaseDX2 + 0x401,
    kNkMAIDCapability_MoviePictureControlDataEx2        = kNkMAIDCapability_VendorBaseDX2 + 0x402,
    kNkMAIDCapability_GetVideoImageEx			        = kNkMAIDCapability_VendorBaseDX2 + 0x403,
	kNkMAIDCapability_DiffractionCompensation			= kNkMAIDCapability_VendorBaseDX2 + 0x404,
	kNkMAIDCapability_VibrationReduction				= kNkMAIDCapability_VendorBaseDX2 + 0x405,
	kNkMAIDCapability_MovieAutoDistortion				= kNkMAIDCapability_VendorBaseDX2 + 0x406,
	kNkMAIDCapability_MovieAfAreaMode					= kNkMAIDCapability_VendorBaseDX2 + 0x407,
	kNkMAIDCapability_MovieVibrationReduction			= kNkMAIDCapability_VendorBaseDX2 + 0x408,
	kNkMAIDCapability_MovieFocusMode					= kNkMAIDCapability_VendorBaseDX2 + 0x409,
	kNkMAIDCapability_MovieVignetteControl				= kNkMAIDCapability_VendorBaseDX2 + 0x40a,
	kNkMAIDCapability_MovieDiffractionCompensation		= kNkMAIDCapability_VendorBaseDX2 + 0x40b,
	kNkMAIDCapability_HDRSaveIndividualImages			= kNkMAIDCapability_VendorBaseDX2 + 0x40c,
	kNkMAIDCapability_UserMode3							= kNkMAIDCapability_VendorBaseDX2 + 0x40d,
	kNkMAIDCapability_RecordTimeCodes					= kNkMAIDCapability_VendorBaseDX2 + 0x40e,
	kNkMAIDCapability_CountUpMethod						= kNkMAIDCapability_VendorBaseDX2 + 0x40f,
	kNkMAIDCapability_TimeCodeOrigin					= kNkMAIDCapability_VendorBaseDX2 + 0x410,
	kNkMAIDCapability_DropFrame							= kNkMAIDCapability_VendorBaseDX2 + 0x411,
	kNkMAIDCapability_MovieLogOutput					= kNkMAIDCapability_VendorBaseDX2 + 0x412,
	kNkMAIDCapability_MovieLogSetting					= kNkMAIDCapability_VendorBaseDX2 + 0x413,
	kNkMAIDCapability_HDMIOutputDataDepth				= kNkMAIDCapability_VendorBaseDX2 + 0x414,
	kNkMAIDCapability_LowLightAF						= kNkMAIDCapability_VendorBaseDX2 + 0x415,
	kNkMAIDCapability_ApplyLiveViewSetting				= kNkMAIDCapability_VendorBaseDX2 + 0x416,
	kNkMAIDCapability_LiveViewImageStatus				= kNkMAIDCapability_VendorBaseDX2 + 0x417,
    kNkMAIDCapability_LiveViewZoomArea                  = kNkMAIDCapability_VendorBaseDX2 + 0x418,
    kNkMAIDCapability_TrackingAFArea                    = kNkMAIDCapability_VendorBaseDX2 + 0x419,
    kNkMAIDCapability_MovieAfSpeed                      = kNkMAIDCapability_VendorBaseDX2 + 0x41a,
    kNkMAIDCapability_MovieAfSpeedWhenToApply           = kNkMAIDCapability_VendorBaseDX2 + 0x41b,
    kNkMAIDCapability_MovieAfTrackingSensitivity        = kNkMAIDCapability_VendorBaseDX2 + 0x41c,
    kNkMAIDCapability_GetManualSettingLensData          = kNkMAIDCapability_VendorBaseDX2 + 0x41d,
	kNkMAIDCapability_StillCaptureModeSaveFrame         = kNkMAIDCapability_VendorBaseDX2 + 0x41e,
    kNkMAIDCapability_StillFocusAreaMode                = kNkMAIDCapability_VendorBaseDX2 + 0x41f,
    kNkMAIDCapability_ChangeMonitorOffStatus            = kNkMAIDCapability_VendorBaseDX2 + 0x420,
    kNkMAIDCapability_RemainingExposureTime             = kNkMAIDCapability_VendorBaseDX2 + 0x421,
	kNkMAIDCapability_ExtendedShutterSpeedsManual       = kNkMAIDCapability_VendorBaseDX2 + 0x422,
	kNkMAIDCapability_QuietReleaseMode					= kNkMAIDCapability_VendorBaseDX2 + 0x423,
	kNkMAIDCapability_AutoAreaAFFaceDetection			= kNkMAIDCapability_VendorBaseDX2 + 0x424,
	kNkMAIDCapability_Slot2JpegImageSize				= kNkMAIDCapability_VendorBaseDX2 + 0x425,
	kNkMAIDCapability_GroupAreaFaceDetection			= kNkMAIDCapability_VendorBaseDX2 + 0x426,
	kNkMAIDCapability_SinglePointAFWatchArea			= kNkMAIDCapability_VendorBaseDX2 + 0x427,
	kNkMAIDCapability_RearControlPanel					= kNkMAIDCapability_VendorBaseDX2 + 0x428,
	kNkMAIDCapability_FlushBurstPrioritize				= kNkMAIDCapability_VendorBaseDX2 + 0x429,
	kNkMAIDCapability_BracketingBurstOptions			= kNkMAIDCapability_VendorBaseDX2 + 0x42a,
	kNkMAIDCapability_3DTrackingFaceDetection			= kNkMAIDCapability_VendorBaseDX2 + 0x42b,
	kNkMAIDCapability_AutoAreaAFStartingPoint			= kNkMAIDCapability_VendorBaseDX2 + 0x42c,
	kNkMAIDCapability_CustomGroupingsC1					= kNkMAIDCapability_VendorBaseDX2 + 0x42d,
    kNkMAIDCapability_LiveViewWBTuneColorTemp           = kNkMAIDCapability_VendorBaseDX2 + 0x42e,
    kNkMAIDCapability_LiveViewWBTuneColorAdjust         = kNkMAIDCapability_VendorBaseDX2 + 0x42f,
	kNkMAIDCapability_LockExposureEx					= kNkMAIDCapability_VendorBaseDX2 + 0x430,
    kNkMAIDCapability_InitiatePixelMapping              = kNkMAIDCapability_VendorBaseDX2 + 0x431,
    kNkMAIDCapability_CustomGroupingsC2                 = kNkMAIDCapability_VendorBaseDX2 + 0x432,
    kNkMAIDCapability_ElectronicFrontCurtainShutterEx   = kNkMAIDCapability_VendorBaseDX2 + 0x433,
    kNkMAIDCapability_AutoFlashIsoSensitivityControl    = kNkMAIDCapability_VendorBaseDX2 + 0x434,
	kNkMAIDCapability_PreCaptureAsync                   = kNkMAIDCapability_VendorBaseDX2 + 0x435,
};

// The module will deliver one of these values to the nResult parameter of the client 
// completion callback function and return the same value from the entry point. 
// Errors will have smaller values than kNkMAIDResult_DX2Origin.
// Minimun value = kNkMAIDResult_DX2Origin - 128
// Maximun value = kNkMAIDResult_DX2Origin + 127
enum eNkMAIDResultD1
{
    kNkMAIDResult_ApertureFEE                   = kNkMAIDResult_DX2Origin - 128,// = 128
    kNkMAIDResult_BufferNotReady                = 129,
    kNkMAIDResult_NormalTTL                     = 130,
    kNkMAIDResult_MediaFull                     = 131,
    kNkMAIDResult_InvalidMedia                  = 132,
    kNkMAIDResult_EraseFailure                  = 133,
    kNkMAIDResult_CameraNotFound                = 134,
    kNkMAIDResult_BatteryDontWork               = 135,
    kNkMAIDResult_ShutterBulb                   = 136,
    kNkMAIDResult_OutOfFocus                    = 137,
    kNkMAIDResult_Protected                     = 138,
    kNkMAIDResult_FileExists                    = 139,
    kNkMAIDResult_SharingViolation              = 140,
    kNkMAIDResult_DataTransFailure              = 141,
    kNkMAIDResult_SessionFailure                = 142,
    kNkMAIDResult_FileRemoved                   = 143,
    kNkMAIDResult_BusReset                      = 144,
    kNkMAIDResult_NonCPULens                    = 145,
    kNkMAIDResult_ReleaseButtonPressed          = 146,
    kNkMAIDResult_BatteryExhausted              = 147,
    kNkMAIDResult_CaptureFailure                = 148,
    kNkMAIDResult_InvalidString                 = 149,
    kNkMAIDResult_NotInitialized                = 150,
    kNkMAIDResult_CaptureDisable                = 151,
    kNkMAIDResult_DeviceBusy                    = 152,
    kNkMAIDResult_CaptureDustFailure            = 153,
    kNkMAIDResult_ICADown                       = 154,
    kNkMAIDResult_CpxInvalidStatus              = 155,
    kNkMAIDResult_CpxInternalMemoryFull         = 156,
    kNkMAIDResult_CpxBatteryLow                 = 157,
    kNkMAIDResult_CpxPlaybackMode               = 158,
    kNkMAIDResult_NotLiveView                   = 159,
    kNkMAIDResult_MFDriveEnd                    = 160,
    kNkMAIDResult_UnformattedMedia              = 161,
    kNkMAIDResult_MediaReadOnly                 = 162,
    kNkMAIDResult_DuringUpdate                  = 163,
    kNkMAIDResult_BulbReleaseBusy               = 164,
    kNkMAIDResult_SilentReleaseBusy             = 165,
    kNkMAIDResult_MovieFrameReleaseBusy         = 166,
    kNkMAIDResult_ShutterTime                   = 167,
    kNkMAIDResult_Waiting_2ndRelease            = 168,
    kNkMAIDResult_MirrorUpCapture_Already_Start = 169,
    kNkMAIDResult_High_Temperature              = 170,
	kNkMAIDResult_InvalidSBAttributeValue		= 171,
	kNkMAIDResult_CameraModeNotAdjustFnumber	= 172,
	kNkMAIDResult_RecInCard						= 173,
	kNkMAIDResult_RecInExternalDevice			= 174,
	kNkMAIDResult_RecInCardAndExternalDevice	= 175,
    kNkMAIDResult_SBSettingError                = 176,
    kNkMAIDResult_InvalidData                   = 177,
	kNkMAIDResult_MountAdapterFWVer             = 178
};

// the verdor unique event code.
enum eNkMAIDEventDX2
{
    kNkMAIDEvent_AddPreviewImage        = kNkMAIDEvent_DX2Origin + 1, //0x107
    kNkMAIDEvent_CaptureComplete        = kNkMAIDEvent_DX2Origin + 2, //0x108
    kNkMAIDEvent_AddChildInCard         = kNkMAIDEvent_DX2Origin + 3, //0x109
    kNkMAIDEvent_RecordingInterrupted   = kNkMAIDEvent_DX2Origin + 4, //0x10A
    kNkMAIDEvent_CapChangeOperationOnly = kNkMAIDEvent_DX2Origin + 5, //0x10B
    kNkMAIDEvent_1stCaptureComplete     = kNkMAIDEvent_DX2Origin + 6, //0x10C
    kNkMAIDEvent_MirrorUpCancelComplete = kNkMAIDEvent_DX2Origin + 7, //0x10D
    kNkMAIDEvent_SBAdded                = kNkMAIDEvent_DX2Origin + 8, //0x10E
    kNkMAIDEvent_SBRemoved              = kNkMAIDEvent_DX2Origin + 9, //0x10F
    kNkMAIDEvent_SBAttrChanged          = kNkMAIDEvent_DX2Origin + 10, //0x110
    kNkMAIDEvent_SBGroupAttrChanged     = kNkMAIDEvent_DX2Origin + 11, //0x111
// kNkMAIDEvent_DX2Origin + 11(0x112) is  reserved.
	kNkMAIDEvent_ActiveSelectionInterrupted = kNkMAIDEvent_DX2Origin + 13, //0x113
	kNkMAIDEvent_StoreRemoved			= kNkMAIDEvent_DX2Origin + 14, //0x114
	kNkMAIDEvent_MovieRecordComplete    = kNkMAIDEvent_DX2Origin + 15, //0x115
	kNkMAIDEvent_RequestLiveViewStart   = kNkMAIDEvent_DX2Origin + 16, //0x116
	kNkMAIDEvent_StartMovieRecord		= kNkMAIDEvent_DX2Origin + 17, //0x117
//	kNkMAIDEvent_LiveViewStateChanged	= kNkMAIDEvent_DX2Origin + 18,  //0x118  not supported.
    kNkMAIDEvent_PictureControlAdjustChanged = kNkMAIDEvent_DX2Origin + 19 , //0x119
    kNkMAIDEvent_ManualSettingLensDataChanged = kNkMAIDEvent_DX2Origin + 20,  //0x11A
	kNkMAIDEvent_PixelMappingComplete   = kNkMAIDEvent_DX2Origin + 21 //0x11B
};

///////////////////////////////////////////////////////////////////////////////
//  CONSTANT DEFINITIONS
//

// kNkMAIDCapability_ModuleMode(0x8101)
enum eNkMAIDModuleMode
{
    kNkMAIDModuleMode_Browser,
    kNkMAIDModuleMode_Controller
};

// kNkMAIDCapability_CFStatus(0x8108)
enum eNkMAIDCFStatus
{
    kNkMAIDCFStatus_Good,
    kNkMAIDCFStatus_Full,
    kNkMAIDCFStatus_NotFormated,
    kNkMAIDCFStatus_NotExist,
    kNkMAIDCFStatus_NotAvailable
};

// kNkMAIDCapability_FlashStatus(0x810a)
enum eNkMAIDFlashStatus
{
    kNkMAIDFlashStatus_Ready,
    kNkMAIDFlashStatus_NotReady,
    kNkMAIDFlashStatus_NotExist,
    kNkMAIDFlashStatus_NormalTTL
};

// kNkMAIDCapability_MediaType(0x810c)
// kNkMAIDCapability_WritingMedia(0x8150)
enum eNkMAIDMediaType
{
    kNkMAIDMediaType_CFCard,
    kNkMAIDMediaType_1394
};

// kNkMAIDCapability_ExposureMode(0x8111)
enum eNkMAIDExposureMode
{
    kNkMAIDExposureMode_Program = 0,
    kNkMAIDExposureMode_AperturePriority,
    kNkMAIDExposureMode_SpeedPriority,
    kNkMAIDExposureMode_Manual,
    kNkMAIDExposureMode_Disable,
    kNkMAIDExposureMode_Auto,
    kNkMAIDExposureMode_Portrait,
    kNkMAIDExposureMode_Landscape,
    kNkMAIDExposureMode_Closeup,
    kNkMAIDExposureMode_Sports,
    kNkMAIDExposureMode_NightPortrait,
    kNkMAIDExposureMode_NightView,
    kNkMAIDExposureMode_Child,
    kNkMAIDExposureMode_FlashOff,
    kNkMAIDExposureMode_Scene,
    kNkMAIDExposureMode_UserMode1,
    kNkMAIDExposureMode_UserMode2,
    kNkMAIDExposureMode_Effects,
    kNkMAIDExposureMode_UserMode3
};

// kNkMAIDCapability_MeteringMode(0x8116)
enum eNkMAIDMeteringMode
{
    kNkMAIDMeteringMode_Matrix,
    kNkMAIDMeteringMode_CenterWeighted,
    kNkMAIDMeteringMode_Spot,
    kNkMAIDMeteringMode_AfSpot,
    kNkMAIDMeteringMode_HighLight
};

// kNkMAIDCapability_MovieMeteringMode(0x8385)
enum eNkMAIDMovieMeteringMode
{
    kNkMAIDMovieMeteringMode_Matrix,
    kNkMAIDMovieMeteringMode_CenterWeighted,
    kNkMAIDMovieMeteringMode_HighLight
};

// kNkMAIDCapability_FocusMode(0x8120)
enum eNkMAIDFocusMode
{
    kNkMAIDFocusMode_MF = 0,
    kNkMAIDFocusMode_AFs,
    kNkMAIDFocusMode_AFc,
    kNkMAIDFocusMode_AFa,
    kNkMAIDFocusMode_AFf,
    kNkMAIDFocusMode_AF = 0x10,
    kNkMAIDFocusMode_Macro,
    kNkMAIDFocusMode_Infinity
};

// kNkMAIDCapability_FocusPreferredArea(0x8122)
enum eNkMAIDFocusPreferred
{
    kNkMAIDFocusPreferred_Center,
    kNkMAIDFocusPreferred_Upper,
    kNkMAIDFocusPreferred_Bottom,
    kNkMAIDFocusPreferred_Left,
    kNkMAIDFocusPreferred_Right,
    kNkMAIDFocusPreferred_UpperLeft,
    kNkMAIDFocusPreferred_UpperRight,
    kNkMAIDFocusPreferred_BottomLeft,
    kNkMAIDFocusPreferred_BottomRight,
    kNkMAIDFocusPreferred_LeftEnd,
    kNkMAIDFocusPreferred_RightEnd
};

// kNkMAIDCapability_FocusPreferredArea(0x8122)
enum eNkMAIDFocusPreferred2
{
    kNkMAIDFocusPreferred2_Unknown = 0,
    kNkMAIDFocusPreferred2_C = 1,
    kNkMAIDFocusPreferred2_CT,
    kNkMAIDFocusPreferred2_CTT,
    kNkMAIDFocusPreferred2_CB,
    kNkMAIDFocusPreferred2_CBB,
    kNkMAIDFocusPreferred2_CR,
    kNkMAIDFocusPreferred2_CRT,
    kNkMAIDFocusPreferred2_CRTT,
    kNkMAIDFocusPreferred2_CRB,
    kNkMAIDFocusPreferred2_CRBB,
    kNkMAIDFocusPreferred2_CL,
    kNkMAIDFocusPreferred2_CLT,
    kNkMAIDFocusPreferred2_CLTT,
    kNkMAIDFocusPreferred2_CLB,
    kNkMAIDFocusPreferred2_CLBB,
    kNkMAIDFocusPreferred2_RA,
    kNkMAIDFocusPreferred2_RAT,
    kNkMAIDFocusPreferred2_RATT,
    kNkMAIDFocusPreferred2_RAB,
    kNkMAIDFocusPreferred2_RABB,
    kNkMAIDFocusPreferred2_RB,
    kNkMAIDFocusPreferred2_RBT,
    kNkMAIDFocusPreferred2_RBTT,
    kNkMAIDFocusPreferred2_RBB,
    kNkMAIDFocusPreferred2_RBBB,
    kNkMAIDFocusPreferred2_RC,
    kNkMAIDFocusPreferred2_RCT,
    kNkMAIDFocusPreferred2_RCTT,
    kNkMAIDFocusPreferred2_RCB,
    kNkMAIDFocusPreferred2_RCBB,
    kNkMAIDFocusPreferred2_RD,
    kNkMAIDFocusPreferred2_RDT,
    kNkMAIDFocusPreferred2_RDB,
    kNkMAIDFocusPreferred2_LA,
    kNkMAIDFocusPreferred2_LAT,
    kNkMAIDFocusPreferred2_LATT,
    kNkMAIDFocusPreferred2_LAB,
    kNkMAIDFocusPreferred2_LABB,
    kNkMAIDFocusPreferred2_LB,
    kNkMAIDFocusPreferred2_LBT,
    kNkMAIDFocusPreferred2_LBTT,
    kNkMAIDFocusPreferred2_LBB,
    kNkMAIDFocusPreferred2_LBBB,
    kNkMAIDFocusPreferred2_LC,
    kNkMAIDFocusPreferred2_LCT,
    kNkMAIDFocusPreferred2_LCTT,
    kNkMAIDFocusPreferred2_LCB,
    kNkMAIDFocusPreferred2_LCBB,
    kNkMAIDFocusPreferred2_LD,
    kNkMAIDFocusPreferred2_LDT,
    kNkMAIDFocusPreferred2_LDB
};

// kNkMAIDCapability_FocusPreferredArea(0x8122)
enum eNkMAIDFocusPreferred3
{
    kNkMAIDFocusPreferred3_Unknown = 0,
    kNkMAIDFocusPreferred3_Center,
    kNkMAIDFocusPreferred3_Upper,
    kNkMAIDFocusPreferred3_Bottom,
    kNkMAIDFocusPreferred3_Left,
    kNkMAIDFocusPreferred3_UpperLeft,
    kNkMAIDFocusPreferred3_BottomLeft,
    kNkMAIDFocusPreferred3_LeftEnd,
    kNkMAIDFocusPreferred3_Right,
    kNkMAIDFocusPreferred3_UpperRight,
    kNkMAIDFocusPreferred3_BottomRight,
    kNkMAIDFocusPreferred3_RightEnd
};

// kNkMAIDCapability_FocusPreferredArea(0x8122)
enum eNkMAIDFocusPreferred4
{
    kNkMAIDFocusPreferred4_Unknown = 0,
    kNkMAIDFocusPreferred4_C = 1,
    kNkMAIDFocusPreferred4_CT,
    kNkMAIDFocusPreferred4_CTT,
    kNkMAIDFocusPreferred4_CB,
    kNkMAIDFocusPreferred4_CBB,
    kNkMAIDFocusPreferred4_CR,
    kNkMAIDFocusPreferred4_CRT,
    kNkMAIDFocusPreferred4_CRTT,
    kNkMAIDFocusPreferred4_CRB,
    kNkMAIDFocusPreferred4_CRBB,
    kNkMAIDFocusPreferred4_CL,
    kNkMAIDFocusPreferred4_CLT,
    kNkMAIDFocusPreferred4_CLTT,
    kNkMAIDFocusPreferred4_CLB,
    kNkMAIDFocusPreferred4_CLBB,
    kNkMAIDFocusPreferred4_RA,
    kNkMAIDFocusPreferred4_RAT,
    kNkMAIDFocusPreferred4_RAB,
    kNkMAIDFocusPreferred4_RB,
    kNkMAIDFocusPreferred4_RBT,
    kNkMAIDFocusPreferred4_RBB,
    kNkMAIDFocusPreferred4_RC,
    kNkMAIDFocusPreferred4_RCT,
    kNkMAIDFocusPreferred4_RCB,
    kNkMAIDFocusPreferred4_RD,
    kNkMAIDFocusPreferred4_RDT,
    kNkMAIDFocusPreferred4_RDB,
    kNkMAIDFocusPreferred4_LA,
    kNkMAIDFocusPreferred4_LAT,
    kNkMAIDFocusPreferred4_LAB,
    kNkMAIDFocusPreferred4_LB,
    kNkMAIDFocusPreferred4_LBT,
    kNkMAIDFocusPreferred4_LBB,
    kNkMAIDFocusPreferred4_LC,
    kNkMAIDFocusPreferred4_LCT,
    kNkMAIDFocusPreferred4_LCB,
    kNkMAIDFocusPreferred4_LD,
    kNkMAIDFocusPreferred4_LDT,
    kNkMAIDFocusPreferred4_LDB
};

// kNkMAIDCapability_FocusPreferredArea(0x8122)
enum eNkMAIDFocusPreferred5
{
    kNkMAIDFocusPreferred5_Unknown = 0,
    kNkMAIDFocusPreferred5_C = 1,
    kNkMAIDFocusPreferred5_CT1,
    kNkMAIDFocusPreferred5_CT2,
    kNkMAIDFocusPreferred5_CT3,
    kNkMAIDFocusPreferred5_CT4,
    kNkMAIDFocusPreferred5_CB1,
    kNkMAIDFocusPreferred5_CB2,
    kNkMAIDFocusPreferred5_CB3,
    kNkMAIDFocusPreferred5_CB4,
    kNkMAIDFocusPreferred5_CR1,
    kNkMAIDFocusPreferred5_CR1T1,
    kNkMAIDFocusPreferred5_CR1T2,
    kNkMAIDFocusPreferred5_CR1T3,
    kNkMAIDFocusPreferred5_CR1T4,
    kNkMAIDFocusPreferred5_CR1B1,
    kNkMAIDFocusPreferred5_CR1B2,
    kNkMAIDFocusPreferred5_CR1B3,
    kNkMAIDFocusPreferred5_CR1B4,
    kNkMAIDFocusPreferred5_CR2,
    kNkMAIDFocusPreferred5_CR2T1,
    kNkMAIDFocusPreferred5_CR2T2,
    kNkMAIDFocusPreferred5_CR2T3,
    kNkMAIDFocusPreferred5_CR2T4,
    kNkMAIDFocusPreferred5_CR2B1,
    kNkMAIDFocusPreferred5_CR2B2,
    kNkMAIDFocusPreferred5_CR2B3,
    kNkMAIDFocusPreferred5_CR2B4,
    kNkMAIDFocusPreferred5_CL1,
    kNkMAIDFocusPreferred5_CL1T1,
    kNkMAIDFocusPreferred5_CL1T2,
    kNkMAIDFocusPreferred5_CL1T3,
    kNkMAIDFocusPreferred5_CL1T4,
    kNkMAIDFocusPreferred5_CL1B1,
    kNkMAIDFocusPreferred5_CL1B2,
    kNkMAIDFocusPreferred5_CL1B3,
    kNkMAIDFocusPreferred5_CL1B4,
    kNkMAIDFocusPreferred5_CL2,
    kNkMAIDFocusPreferred5_CL2T1,
    kNkMAIDFocusPreferred5_CL2T2,
    kNkMAIDFocusPreferred5_CL2T3,
    kNkMAIDFocusPreferred5_CL2T4,
    kNkMAIDFocusPreferred5_CL2B1,
    kNkMAIDFocusPreferred5_CL2B2,
    kNkMAIDFocusPreferred5_CL2B3,
    kNkMAIDFocusPreferred5_CL2B4,
    kNkMAIDFocusPreferred5_R1,
    kNkMAIDFocusPreferred5_R1T1,
    kNkMAIDFocusPreferred5_R1T2,
    kNkMAIDFocusPreferred5_R1T3,
    kNkMAIDFocusPreferred5_R1T4,
    kNkMAIDFocusPreferred5_R1B1,
    kNkMAIDFocusPreferred5_R1B2,
    kNkMAIDFocusPreferred5_R1B3,
    kNkMAIDFocusPreferred5_R1B4,
    kNkMAIDFocusPreferred5_R2,
    kNkMAIDFocusPreferred5_R2T1,
    kNkMAIDFocusPreferred5_R2T2,
    kNkMAIDFocusPreferred5_R2T3,
    kNkMAIDFocusPreferred5_R2T4,
    kNkMAIDFocusPreferred5_R2B1,
    kNkMAIDFocusPreferred5_R2B2,
    kNkMAIDFocusPreferred5_R2B3,
    kNkMAIDFocusPreferred5_R2B4,
    kNkMAIDFocusPreferred5_R3,
    kNkMAIDFocusPreferred5_R3T1,
    kNkMAIDFocusPreferred5_R3T2,
    kNkMAIDFocusPreferred5_R3T3,
    kNkMAIDFocusPreferred5_R3T4,
    kNkMAIDFocusPreferred5_R3B1,
    kNkMAIDFocusPreferred5_R3B2,
    kNkMAIDFocusPreferred5_R3B3,
    kNkMAIDFocusPreferred5_R3B4,
    kNkMAIDFocusPreferred5_RS1,
    kNkMAIDFocusPreferred5_RS1T1,
    kNkMAIDFocusPreferred5_RS1T2,
    kNkMAIDFocusPreferred5_RS1T3,
    kNkMAIDFocusPreferred5_RS1T4,
    kNkMAIDFocusPreferred5_RS1B1,
    kNkMAIDFocusPreferred5_RS1B2,
    kNkMAIDFocusPreferred5_RS1B3,
    kNkMAIDFocusPreferred5_RS1B4,
    kNkMAIDFocusPreferred5_RS2,
    kNkMAIDFocusPreferred5_RS2T1,
    kNkMAIDFocusPreferred5_RS2T2,
    kNkMAIDFocusPreferred5_RS2T3,
    kNkMAIDFocusPreferred5_RS2T4,
    kNkMAIDFocusPreferred5_RS2B1,
    kNkMAIDFocusPreferred5_RS2B2,
    kNkMAIDFocusPreferred5_RS2B3,
    kNkMAIDFocusPreferred5_RS2B4,
    kNkMAIDFocusPreferred5_RS3,
    kNkMAIDFocusPreferred5_RS3T1,
    kNkMAIDFocusPreferred5_RS3T2,
    kNkMAIDFocusPreferred5_RS3T3,
    kNkMAIDFocusPreferred5_RS3T4,
    kNkMAIDFocusPreferred5_RS3B1,
    kNkMAIDFocusPreferred5_RS3B2,
    kNkMAIDFocusPreferred5_RS3B3,
    kNkMAIDFocusPreferred5_RS3B4,
    kNkMAIDFocusPreferred5_L1,
    kNkMAIDFocusPreferred5_L1T1,
    kNkMAIDFocusPreferred5_L1T2,
    kNkMAIDFocusPreferred5_L1T3,
    kNkMAIDFocusPreferred5_L1T4,
    kNkMAIDFocusPreferred5_L1B1,
    kNkMAIDFocusPreferred5_L1B2,
    kNkMAIDFocusPreferred5_L1B3,
    kNkMAIDFocusPreferred5_L1B4,
    kNkMAIDFocusPreferred5_L2,
    kNkMAIDFocusPreferred5_L2T1,
    kNkMAIDFocusPreferred5_L2T2,
    kNkMAIDFocusPreferred5_L2T3,
    kNkMAIDFocusPreferred5_L2T4,
    kNkMAIDFocusPreferred5_L2B1,
    kNkMAIDFocusPreferred5_L2B2,
    kNkMAIDFocusPreferred5_L2B3,
    kNkMAIDFocusPreferred5_L2B4,
    kNkMAIDFocusPreferred5_L3,
    kNkMAIDFocusPreferred5_L3T1,
    kNkMAIDFocusPreferred5_L3T2,
    kNkMAIDFocusPreferred5_L3T3,
    kNkMAIDFocusPreferred5_L3T4,
    kNkMAIDFocusPreferred5_L3B1,
    kNkMAIDFocusPreferred5_L3B2,
    kNkMAIDFocusPreferred5_L3B3,
    kNkMAIDFocusPreferred5_L3B4,
    kNkMAIDFocusPreferred5_LS1,
    kNkMAIDFocusPreferred5_LS1T1,
    kNkMAIDFocusPreferred5_LS1T2,
    kNkMAIDFocusPreferred5_LS1T3,
    kNkMAIDFocusPreferred5_LS1T4,
    kNkMAIDFocusPreferred5_LS1B1,
    kNkMAIDFocusPreferred5_LS1B2,
    kNkMAIDFocusPreferred5_LS1B3,
    kNkMAIDFocusPreferred5_LS1B4,
    kNkMAIDFocusPreferred5_LS2,
    kNkMAIDFocusPreferred5_LS2T1,
    kNkMAIDFocusPreferred5_LS2T2,
    kNkMAIDFocusPreferred5_LS2T3,
    kNkMAIDFocusPreferred5_LS2T4,
    kNkMAIDFocusPreferred5_LS2B1,
    kNkMAIDFocusPreferred5_LS2B2,
    kNkMAIDFocusPreferred5_LS2B3,
    kNkMAIDFocusPreferred5_LS2B4,
    kNkMAIDFocusPreferred5_LS3,
    kNkMAIDFocusPreferred5_LS3T1,
    kNkMAIDFocusPreferred5_LS3T2,
    kNkMAIDFocusPreferred5_LS3T3,
    kNkMAIDFocusPreferred5_LS3T4,
    kNkMAIDFocusPreferred5_LS3B1,
    kNkMAIDFocusPreferred5_LS3B2,
    kNkMAIDFocusPreferred5_LS3B3,
    kNkMAIDFocusPreferred5_LS3B4
};

// kNkMAIDCapability_FocusPreferredArea(0x8122)
enum eNkMAIDFocusPreferred6
{
	kNkMAIDFocusPreferred6_Unknown = 0,
	kNkMAIDFocusPreferred6_C,
	kNkMAIDFocusPreferred6_CT1,
	kNkMAIDFocusPreferred6_CT2,
	kNkMAIDFocusPreferred6_CT3,
	kNkMAIDFocusPreferred6_CB1,
	kNkMAIDFocusPreferred6_CB2,
	kNkMAIDFocusPreferred6_CB3,
	kNkMAIDFocusPreferred6_CR1,
	kNkMAIDFocusPreferred6_CR1T1,
	kNkMAIDFocusPreferred6_CR1T2,
	kNkMAIDFocusPreferred6_CR1T3,
	kNkMAIDFocusPreferred6_CR1B1,
	kNkMAIDFocusPreferred6_CR1B2,
	kNkMAIDFocusPreferred6_CR1B3,
	kNkMAIDFocusPreferred6_CR2,
	kNkMAIDFocusPreferred6_CR2T1,
	kNkMAIDFocusPreferred6_CR2T2,
	kNkMAIDFocusPreferred6_CR2T3,
	kNkMAIDFocusPreferred6_CR2B1,
	kNkMAIDFocusPreferred6_CR2B2,
	kNkMAIDFocusPreferred6_CR2B3,
	kNkMAIDFocusPreferred6_CL1,
	kNkMAIDFocusPreferred6_CL1T1,
	kNkMAIDFocusPreferred6_CL1T2,
	kNkMAIDFocusPreferred6_CL1T3,
	kNkMAIDFocusPreferred6_CL1B1,
	kNkMAIDFocusPreferred6_CL1B2,
	kNkMAIDFocusPreferred6_CL1B3,
	kNkMAIDFocusPreferred6_CL2,
	kNkMAIDFocusPreferred6_CL2T1,
	kNkMAIDFocusPreferred6_CL2T2,
	kNkMAIDFocusPreferred6_CL2T3,
	kNkMAIDFocusPreferred6_CL2B1,
	kNkMAIDFocusPreferred6_CL2B2,
	kNkMAIDFocusPreferred6_CL2B3,
	kNkMAIDFocusPreferred6_R1,
	kNkMAIDFocusPreferred6_R1T1,
	kNkMAIDFocusPreferred6_R1T2,
	kNkMAIDFocusPreferred6_R1T3,
	kNkMAIDFocusPreferred6_R1B1,
	kNkMAIDFocusPreferred6_R1B2,
	kNkMAIDFocusPreferred6_R1B3,
	kNkMAIDFocusPreferred6_R2,
	kNkMAIDFocusPreferred6_R2T1,
	kNkMAIDFocusPreferred6_R2T2,
	kNkMAIDFocusPreferred6_R2T3,
	kNkMAIDFocusPreferred6_R2B1,
	kNkMAIDFocusPreferred6_R2B2,
	kNkMAIDFocusPreferred6_R2B3,
	kNkMAIDFocusPreferred6_R3,
	kNkMAIDFocusPreferred6_R3T1,
	kNkMAIDFocusPreferred6_R3T2,
	kNkMAIDFocusPreferred6_R3T3,
	kNkMAIDFocusPreferred6_R3B1,
	kNkMAIDFocusPreferred6_R3B2,
	kNkMAIDFocusPreferred6_R3B3,
	kNkMAIDFocusPreferred6_RS1,
	kNkMAIDFocusPreferred6_RS1T1,
	kNkMAIDFocusPreferred6_RS1T2,
	kNkMAIDFocusPreferred6_RS1T3,
	kNkMAIDFocusPreferred6_RS1B1,
	kNkMAIDFocusPreferred6_RS1B2,
	kNkMAIDFocusPreferred6_RS1B3,
	kNkMAIDFocusPreferred6_RS2,
	kNkMAIDFocusPreferred6_RS2T1,
	kNkMAIDFocusPreferred6_RS2T2,
	kNkMAIDFocusPreferred6_RS2T3,
	kNkMAIDFocusPreferred6_RS2B1,
	kNkMAIDFocusPreferred6_RS2B2,
	kNkMAIDFocusPreferred6_RS2B3,
	kNkMAIDFocusPreferred6_L1,
	kNkMAIDFocusPreferred6_L1T1,
	kNkMAIDFocusPreferred6_L1T2,
	kNkMAIDFocusPreferred6_L1T3,
	kNkMAIDFocusPreferred6_L1B1,
	kNkMAIDFocusPreferred6_L1B2,
	kNkMAIDFocusPreferred6_L1B3,
	kNkMAIDFocusPreferred6_L2,
	kNkMAIDFocusPreferred6_L2T1,
	kNkMAIDFocusPreferred6_L2T2,
	kNkMAIDFocusPreferred6_L2T3,
	kNkMAIDFocusPreferred6_L2B1,
	kNkMAIDFocusPreferred6_L2B2,
	kNkMAIDFocusPreferred6_L2B3,
	kNkMAIDFocusPreferred6_L3,
	kNkMAIDFocusPreferred6_L3T1,
	kNkMAIDFocusPreferred6_L3T2,
	kNkMAIDFocusPreferred6_L3T3,
	kNkMAIDFocusPreferred6_L3B1,
	kNkMAIDFocusPreferred6_L3B2,
	kNkMAIDFocusPreferred6_L3B3,
	kNkMAIDFocusPreferred6_LS1,
	kNkMAIDFocusPreferred6_LS1T1,
	kNkMAIDFocusPreferred6_LS1T2,
	kNkMAIDFocusPreferred6_LS1T3,
	kNkMAIDFocusPreferred6_LS1B1,
	kNkMAIDFocusPreferred6_LS1B2,
	kNkMAIDFocusPreferred6_LS1B3,
	kNkMAIDFocusPreferred6_LS2,
	kNkMAIDFocusPreferred6_LS2T1,
	kNkMAIDFocusPreferred6_LS2T2,
	kNkMAIDFocusPreferred6_LS2T3,
	kNkMAIDFocusPreferred6_LS2B1,
	kNkMAIDFocusPreferred6_LS2B2,
	kNkMAIDFocusPreferred6_LS2B3,

};

// kNkMAIDCapability_TypicalFlashMode(0x8160)
enum eNkMAIDTypicalFlashMode
{
    kNkMAIDTypicalFlashMode_Auto,
    kNkMAIDTypicalFlashMode_ForcedOn,
    kNkMAIDTypicalFlashMode_Off,
    kNkMAIDTypicalFlashMode_RedEyeReduction,
    kNkMAIDTypicalFlashMode_SlowSync
};

// kNkMAIDCapability_Converter(0x8161)
enum eNkMAIDConverter
{
    kNkMAIDConverter_None,
    kNkMAIDConverter_Wide,
    kNkMAIDConverter_FishEye
};

// kNkMAIDCapability_ElectronicZoom(0x8162)
enum eNkMAIDElectronicZoom
{
    kNkMAIDElectronicZoom_100,
    kNkMAIDElectronicZoom_120,
    kNkMAIDElectronicZoom_140,
    kNkMAIDElectronicZoom_160,
    kNkMAIDElectronicZoom_180,
    kNkMAIDElectronicZoom_200,
    kNkMAIDElectronicZoom_220,
    kNkMAIDElectronicZoom_240,
    kNkMAIDElectronicZoom_260,
    kNkMAIDElectronicZoom_280,
    kNkMAIDElectronicZoom_300,
    kNkMAIDElectronicZoom_320,
    kNkMAIDElectronicZoom_340,
    kNkMAIDElectronicZoom_360,
    kNkMAIDElectronicZoom_380,
    kNkMAIDElectronicZoom_400
};

// kNkMAIDCapability_DateFormat(0x8163)
enum eNkMAIDDateFormat
{
    kNkMAIDDateFormat_Off,
    kNkMAIDDateFormat_YMD,
    kNkMAIDDateFormat_MDY,
    kNkMAIDDateFormat_DMY
};

// kNkMAIDCapability_ColorAdjustment(0x815e)
enum eNkMAIDColorAdjustment
{
    eNkMAIDColorAdjustment_Minus9,
    eNkMAIDColorAdjustment_Minus6,
    eNkMAIDColorAdjustment_Minus3,
    eNkMAIDColorAdjustment_0,
    eNkMAIDColorAdjustment_Plus3,
    eNkMAIDColorAdjustment_Plus6,
    eNkMAIDColorAdjustment_Plus9
};

// kNkMAIDCapability_WBTuneColorTemp(0x818c)
enum eNkMAIDWBTuneColorTemp
{
    eNkMAIDWBTuneColorTemp_Unknown = 0,
    eNkMAIDWBTuneColorTemp_2500 = 2500,
    eNkMAIDWBTuneColorTemp_2550 = 2550,
    eNkMAIDWBTuneColorTemp_2560 = 2560,
    eNkMAIDWBTuneColorTemp_2630 = 2630,
    eNkMAIDWBTuneColorTemp_2650 = 2650,
    eNkMAIDWBTuneColorTemp_2700 = 2700,
    eNkMAIDWBTuneColorTemp_2780 = 2780,
    eNkMAIDWBTuneColorTemp_2800 = 2800,
    eNkMAIDWBTuneColorTemp_2850 = 2850,
    eNkMAIDWBTuneColorTemp_2860 = 2860,
    eNkMAIDWBTuneColorTemp_2940 = 2940,
    eNkMAIDWBTuneColorTemp_2950 = 2950,
    eNkMAIDWBTuneColorTemp_3000 = 3000,
    eNkMAIDWBTuneColorTemp_3030 = 3030,
    eNkMAIDWBTuneColorTemp_3100 = 3100,
    eNkMAIDWBTuneColorTemp_3130 = 3130,
    eNkMAIDWBTuneColorTemp_3200 = 3200,
    eNkMAIDWBTuneColorTemp_3230 = 3230,
    eNkMAIDWBTuneColorTemp_3300 = 3300,
    eNkMAIDWBTuneColorTemp_3330 = 3330,
    eNkMAIDWBTuneColorTemp_3400 = 3400,
    eNkMAIDWBTuneColorTemp_3450 = 3450,
    eNkMAIDWBTuneColorTemp_3570 = 3570,
    eNkMAIDWBTuneColorTemp_3600 = 3600,
    eNkMAIDWBTuneColorTemp_3700 = 3700,
    eNkMAIDWBTuneColorTemp_3800 = 3800,
    eNkMAIDWBTuneColorTemp_3850 = 3850,
    eNkMAIDWBTuneColorTemp_4000 = 4000,
    eNkMAIDWBTuneColorTemp_4170 = 4170,
    eNkMAIDWBTuneColorTemp_4200 = 4200,
    eNkMAIDWBTuneColorTemp_4300 = 4300,
    eNkMAIDWBTuneColorTemp_4350 = 4350,
    eNkMAIDWBTuneColorTemp_4500 = 4500,
    eNkMAIDWBTuneColorTemp_4550 = 4550,
    eNkMAIDWBTuneColorTemp_4760 = 4760,
    eNkMAIDWBTuneColorTemp_4800 = 4800,
    eNkMAIDWBTuneColorTemp_5000 = 5000,
    eNkMAIDWBTuneColorTemp_5260 = 5260,
    eNkMAIDWBTuneColorTemp_5300 = 5300,
    eNkMAIDWBTuneColorTemp_5560 = 5560,
    eNkMAIDWBTuneColorTemp_5600 = 5600,
    eNkMAIDWBTuneColorTemp_5880 = 5880,
    eNkMAIDWBTuneColorTemp_5900 = 5900,
    eNkMAIDWBTuneColorTemp_6250 = 6250,
    eNkMAIDWBTuneColorTemp_6300 = 6300,
    eNkMAIDWBTuneColorTemp_6670 = 6670,
    eNkMAIDWBTuneColorTemp_6700 = 6700,
    eNkMAIDWBTuneColorTemp_7100 = 7100,
    eNkMAIDWBTuneColorTemp_7140 = 7140,
    eNkMAIDWBTuneColorTemp_7690 = 7690,
    eNkMAIDWBTuneColorTemp_7700 = 7700,
    eNkMAIDWBTuneColorTemp_8300 = 8300,
    eNkMAIDWBTuneColorTemp_8330 = 8330,
    eNkMAIDWBTuneColorTemp_9090 = 9090,
    eNkMAIDWBTuneColorTemp_9100 = 9100,
    eNkMAIDWBTuneColorTemp_9900 = 9900,
    eNkMAIDWBTuneColorTemp_10000 = 10000
};

//kNkMAIDCapability_ShootingMode(0x818d)
enum eNkMAIDShootingMode
{
    eNkMAIDShootingMode_S = 0,
    eNkMAIDShootingMode_C,
    eNkMAIDShootingMode_CH,
    eNkMAIDShootingMode_SelfTimer,
    eNkMAIDShootingMode_MirrorUp,
    eNkMAIDShootingMode_RemoteTimer_Instant,
    eNkMAIDShootingMode_RemoteTimer_2sec,
    eNkMAIDShootingMode_LiveView,
    eNkMAIDShootingMode_Quiet,
    eNkMAIDShootingMode_RemoteCtrl,
    eNkMAIDShootingMode_QuietC,
    eNkMAIDShootingMode_CHPriorityFrame,
    eNkMAIDShootingMode_Unknown = 0xff
};

//kNkMAIDCapability_FmmManual(0x8192)
enum eNkMAIDFmmManual
{
    eNkMAIDFmmManual_0 = 0,
    eNkMAIDFmmManual_6 = 6,
    eNkMAIDFmmManual_8 = 8,
    eNkMAIDFmmManual_13 = 13,
    eNkMAIDFmmManual_15 = 15,
    eNkMAIDFmmManual_16 = 16,
    eNkMAIDFmmManual_18 = 18,
    eNkMAIDFmmManual_20 = 20,
    eNkMAIDFmmManual_24 = 24,
    eNkMAIDFmmManual_25 = 25,
    eNkMAIDFmmManual_28 = 28,
    eNkMAIDFmmManual_35 = 35,
    eNkMAIDFmmManual_43 = 43,
    eNkMAIDFmmManual_45 = 45,
    eNkMAIDFmmManual_50 = 50,
    eNkMAIDFmmManual_55 = 55,
    eNkMAIDFmmManual_58 = 58,
    eNkMAIDFmmManual_70 = 70,
    eNkMAIDFmmManual_80 = 80,
    eNkMAIDFmmManual_85 = 85,
    eNkMAIDFmmManual_86 = 86,
    eNkMAIDFmmManual_100 = 100,
    eNkMAIDFmmManual_105 = 105,
    eNkMAIDFmmManual_135 = 135,
    eNkMAIDFmmManual_180 = 180,
    eNkMAIDFmmManual_200 = 200,
    eNkMAIDFmmManual_300 = 300,
    eNkMAIDFmmManual_360 = 360,
    eNkMAIDFmmManual_400 = 400,
    eNkMAIDFmmManual_500 = 500,
    eNkMAIDFmmManual_600 = 600,
    eNkMAIDFmmManual_800 = 800,
    eNkMAIDFmmManual_1000 = 1000,
    eNkMAIDFmmManual_1200 = 1200,
    eNkMAIDFmmManual_1400 = 1400,
    eNkMAIDFmmManual_1600 = 1600,
    eNkMAIDFmmManual_2000 = 2000,
    eNkMAIDFmmManual_2400 = 2400,
    eNkMAIDFmmManual_2800 = 2800,
    eNkMAIDFmmManual_3200 = 3200,
    eNkMAIDFmmManual_4000 = 4000
};

//kNkMAIDCapability_USBSpeed(0x81ac)
enum eNkMAIDUSBSpeed
{
    eNkMAIDUSBSpeed_FullSpeed,
    eNkMAIDUSBSpeed_HighSpeed,
    eNkMAIDUSBSpeed_SuperSpeed
};

//kNkMAIDCapability_FocusGroupPreferredArea(0x818a)
enum eNkMAIDFocusGroupPreferred
{
    kNkMAIDFocusGroupPreferred_Center,
    kNkMAIDFocusGroupPreferred_Upper,
    kNkMAIDFocusGroupPreferred_Bottom,
    kNkMAIDFocusGroupPreferred_Left,
    kNkMAIDFocusGroupPreferred_Right
};

//kNkMAIDCapability_BracketingType(0x81b0)
enum eNkMAIDBracketingType
{
    eNkMAIDBracketingType_Minus_2,
    eNkMAIDBracketingType_Puls_2,
    eNkMAIDBracketingType_Minus_3,
    eNkMAIDBracketingType_Puls_3,
    eNkMAIDBracketingType_Both_3,
    eNkMAIDBracketingType_Both_5,
    eNkMAIDBracketingType_Both_7,
    eNkMAIDBracketingType_Both_9,
    eNkMAIDBracketingType_None_0
};

//kNkMAIDCapability_AEBracketingStep(0x81b1)
enum eNkMAIDAEBracketingStep
{
    eNkMAIDAEBracketingStep_1_3EV,
    eNkMAIDAEBracketingStep_1_2EV,
    eNkMAIDAEBracketingStep_2_3EV,
    eNkMAIDAEBracketingStep_1EV,
    eNkMAIDAEBracketingStep_4_3EV,
    eNkMAIDAEBracketingStep_3_2EV,
    eNkMAIDAEBracketingStep_5_3EV,
    eNkMAIDAEBracketingStep_2EV,
    eNkMAIDAEBracketingStep_3EV
};

//kNkMAIDCapability_WBBracketingStep(0x81b2)
enum eNkMAIDWBBracketingStep
{
    eNkMAIDWBBracketingStep_1STEP,
    eNkMAIDWBBracketingStep_2STEP,
    eNkMAIDWBBracketingStep_3STEP
};

//kNkMAIDCapability_CameraInclination(0x81b4)
enum eNkMAIDCameraInclination
{
    eNkMAIDCameraInclination_Level = 0,
    eNkMAIDCameraInclination_GripTop,
    eNkMAIDCameraInclination_GripBottom,
    eNkMAIDCameraInclination_LevelUpDown
};

//kNkMAIDCapability_RawJpegImageStatus(0x81b5)
enum eNkMAIDRawJpegImageStatus
{
    eNkMAIDRawJpegImageStatus_Single,
    eNkMAIDRawJpegImageStatus_RawJpeg
};

// kNkMAIDCapability_CaptureDustImage(0x81b6)
enum eNkMAIDFileDataTypesDX2
{
    kNkMAIDFileDataType_VendorBaseDx2 = kNkMAIDFileDataType_UserType, //0x100
    kNkMAIDFileDataType_NDF,
    kNkMAIDFileDataType_AVI,
    kNkMAIDFileDataType_MOV,
    kNkMAIDFileDataType_MP4
};

//	kNkMAIDCapability_AfGroupAreaPatternType(0x81b8)
enum eNkMAIDAfGroupAreaPatternType
{
    eNkMAIDAfGroupAreaPatternType_Horizontal,
    eNkMAIDAfGroupAreaPatternType_Vertical
};

//	kNkMAIDCapability_ExternalFlashSort(0x81b9)
enum eNkMAIDExternalFlashSort
{
    eNkMAIDExternalFlashSort_NoCommunicate = 0,
    eNkMAIDExternalFlashSort_OldType,
    eNkMAIDExternalFlashSort_NewType,
    eNkMAIDExternalFlashSort_NotExist,
	eNkMAIDExternalFlashSort_NewTypeControl

};

//	kNkMAIDCapability_ExternalOldTypeFlashMode(0x81ba)
enum eNkMAIDExternalOldTypeFlashMode
{
    eNkMAIDExternalOldTypeFlashMode_TTL = 0,
    eNkMAIDExternalOldTypeFlashMode_DTTL,
    eNkMAIDExternalOldTypeFlashMode_AA,
    eNkMAIDExternalOldTypeFlashMode_ExtAutoFlash,
    eNkMAIDExternalOldTypeFlashMode_Manual,
    eNkMAIDExternalOldTypeFlashMode_FP,
    eNkMAIDExternalOldTypeFlashMode_NotExist,
	eNkMAIDExternalOldTypeFlashMode_NonTTL
};

//	kNkMAIDCapability_ExternalNewTypeFlashMode(0x81bb)
enum eNkMAIDExternalNewTypeFlashMode
{
    eNkMAIDExternalNewTypeFlashMode_OFF = 0,
    eNkMAIDExternalNewTypeFlashMode_CompTTL,
    eNkMAIDExternalNewTypeFlashMode_NoCompTTL,
    eNkMAIDExternalNewTypeFlashMode_AA,
    eNkMAIDExternalNewTypeFlashMode_ExtAutoFlash,
    eNkMAIDExternalNewTypeFlashMode_ManualLengthPriority,
    eNkMAIDExternalNewTypeFlashMode_Manual,
    eNkMAIDExternalNewTypeFlashMode_Multi,
    eNkMAIDExternalNewTypeFlashMode_NotExist
};

// kNkMAIDCapability_InternalFlashStatus(0x81bc)
enum eNkMAIDInternalFlashStatus
{
    kNkMAIDInternalFlashStatus_Ready = 0,
    kNkMAIDInternalFlashStatus_NotReady,
    kNkMAIDInternalFlashStatus_Close,
    kNkMAIDInternalFlashStatus_TTL
};

// kNkMAIDCapability_ExternalFlashStatus(0x81bd)
enum eNkMAIDExternalFlashStatus
{
    kNkMAIDExternalFlashStatus_Ready = 0,
    kNkMAIDExternalFlashStatus_NotReady,
    kNkMAIDExternalFlashStatus_NotExist
};

// kNkMAIDCapability_ImageSetting(0x81c0)
enum eNkMAIDImageSetting
{
    kNkMAIDImageSetting_Normal = 0,
    kNkMAIDImageSetting_Clear,
    kNkMAIDImageSetting_Sharp,
    kNkMAIDImageSetting_Soft,
    kNkMAIDImageSetting_DirectPrint,
    kNkMAIDImageSetting_Portrait,
    kNkMAIDImageSetting_Landscape,
    kNkMAIDImageSetting_Custom,
    kNkMAIDImageSetting_Black_and_White, 
    kNkMAIDImageSetting_MoreClear  
};

// kNkMAIDCapability_SaturationSetting(0x81c1)
enum eNkMAIDSaturationSetting
{
    kNkMAIDSaturationSetting_Normal = 0,
    kNkMAIDSaturationSetting_Low,
    kNkMAIDSaturationSetting_High,
    kNkMAIDSaturationSetting_Auto
};

// kNkMAIDCapability_AFMode(0x81c3)
enum eNkMAIDAFMode
{
    kNkMAIDAFMode_S = 0,
    kNkMAIDAFMode_C,
    kNkMAIDAFMode_A,    
    kNkMAIDAFMode_M_FIX,
    kNkMAIDAFMode_M_SEL,
    kNkMAIDAFMode_F     
};

// kNkMAIDCapability_AFModeAtLiveView(0x8310)
enum eNkMAIDAFModeAtLiveView 
{
    kNkMAIDAFModeAtLiveView_S = 0,
    kNkMAIDAFModeAtLiveView_C,
    kNkMAIDAFModeAtLiveView_F,
    kNkMAIDAFModeAtLiveView_M_FIX,
    kNkMAIDAFModeAtLiveView_M_SEL,
    kNkMAIDAFModeAtLiveView_A
};

//	kNkMAIDCapability_ISOAutoShutterTime(0x81c4)
enum eNkMAIDISOAutoShutterTime
{
    kNkMAIDISOAutoShutterTime_0 = 0,// 1/125
    kNkMAIDISOAutoShutterTime_1,    // 1/60
    kNkMAIDISOAutoShutterTime_2,    // 1/30
    kNkMAIDISOAutoShutterTime_3,    // 1/15
    kNkMAIDISOAutoShutterTime_4,    // 1/8
    kNkMAIDISOAutoShutterTime_5,    // 1/4
    kNkMAIDISOAutoShutterTime_6,    // 1/2
    kNkMAIDISOAutoShutterTime_7,    // 1
    kNkMAIDISOAutoShutterTime_8,    // 2
    kNkMAIDISOAutoShutterTime_9,    // 4
    kNkMAIDISOAutoShutterTime_10,   // 8
    kNkMAIDISOAutoShutterTime_11,   // 15
    kNkMAIDISOAutoShutterTime_12,   // 30
    kNkMAIDISOAutoShutterTime_13,   // 1/250
    kNkMAIDISOAutoShutterTime_14,   // 1/200
    kNkMAIDISOAutoShutterTime_15,   // 1/160
    kNkMAIDISOAutoShutterTime_16,   // 1/100
    kNkMAIDISOAutoShutterTime_17,   // 1/80 
    kNkMAIDISOAutoShutterTime_18,   // 1/40 
    kNkMAIDISOAutoShutterTime_19,   // 1/50
    kNkMAIDISOAutoShutterTime_20,   // 1/4000
    kNkMAIDISOAutoShutterTime_21,   // 1/3200 
    kNkMAIDISOAutoShutterTime_22,   // 1/2500 
    kNkMAIDISOAutoShutterTime_23,   // 1/2000
    kNkMAIDISOAutoShutterTime_24,   // 1/1600
    kNkMAIDISOAutoShutterTime_25,   // 1/1250
    kNkMAIDISOAutoShutterTime_26,   // 1/1000
    kNkMAIDISOAutoShutterTime_27,   // 1/800
    kNkMAIDISOAutoShutterTime_28,   // 1/640
    kNkMAIDISOAutoShutterTime_29,   // 1/500
    kNkMAIDISOAutoShutterTime_30,   // 1/400
    kNkMAIDISOAutoShutterTime_31,   // 1/320
    kNkMAIDISOAutoShutterTime_32,   // Auto
	kNkMAIDISOAutoShutterTime_33,   // 1/25
	kNkMAIDISOAutoShutterTime_34,   // 1/20
	kNkMAIDISOAutoShutterTime_35,   // 1/13
	kNkMAIDISOAutoShutterTime_36,   // 1/10
	kNkMAIDISOAutoShutterTime_37,   // 1/6
	kNkMAIDISOAutoShutterTime_38,   // 1/5
	kNkMAIDISOAutoShutterTime_39,   // 1/3
	kNkMAIDISOAutoShutterTime_40,   // 1/2.5
	kNkMAIDISOAutoShutterTime_41,   // 1/1.6
	kNkMAIDISOAutoShutterTime_42,   // 1/1.3
	kNkMAIDISOAutoShutterTime_43,   // 1.3
	kNkMAIDISOAutoShutterTime_44,   // 1.6
	kNkMAIDISOAutoShutterTime_45,   // 2.5
	kNkMAIDISOAutoShutterTime_46,   // 3
	kNkMAIDISOAutoShutterTime_47,   // 5
	kNkMAIDISOAutoShutterTime_48,   // 6
	kNkMAIDISOAutoShutterTime_49,   // 10
	kNkMAIDISOAutoShutterTime_50,   // 13
	kNkMAIDISOAutoShutterTime_51,   // 20
	kNkMAIDISOAutoShutterTime_52    // 25
};

// kNkMAIDCapability_InternalSplValue(0x81c5)
enum eNkMAIDInternalSplValue
{
    kNkMAIDInternalSplValue_Full =0,    // Full
    kNkMAIDInternalSplValue_2,          // 1/2
    kNkMAIDInternalSplValue_4,          // 1/4
    kNkMAIDInternalSplValue_8,          // 1/8
    kNkMAIDInternalSplValue_16,         // 1/16
    kNkMAIDInternalSplValue_32,         // 1/32 
    kNkMAIDInternalSplValue_64,         // 1/64 
    kNkMAIDInternalSplValue_128,        // 1/128
    kNkMAIDInternalSplValue_1_3,        // 1/1.3
    kNkMAIDInternalSplValue_1_7,        // 1/1.7
    kNkMAIDInternalSplValue_2_5,        // 1/2.5
    kNkMAIDInternalSplValue_3_2,        // 1/3.2
    kNkMAIDInternalSplValue_5,          // 1/5
    kNkMAIDInternalSplValue_6_4,        // 1/6.4
    kNkMAIDInternalSplValue_10,         // 1/10
    kNkMAIDInternalSplValue_13,         // 1/13
    kNkMAIDInternalSplValue_20,         // 1/20
    kNkMAIDInternalSplValue_25,         // 1/25
    kNkMAIDInternalSplValue_40,         // 1/40
    kNkMAIDInternalSplValue_50,         // 1/50
    kNkMAIDInternalSplValue_80,         // 1/80
    kNkMAIDInternalSplValue_100,        // 1/100
    kNkMAIDInternalSplValue_256         // 1/256
};

// kNkMAIDCapability_InternalSplCommand(0x81c6)
enum eNkMAIDInternalSplCommand
{
    kNkMAIDInternalSplCommand_TTL = 0,
    kNkMAIDInternalSplCommand_AA,
    kNkMAIDInternalSplCommand_Manual,
    kNkMAIDInternalSplCommand_Off  
};

// kNkMAIDCapability_RemoteTimer(0x81c8)
enum eNkMAIDRemoteTimer
{
    kNkMAIDRemoteTimer_1min = 0,
    kNkMAIDRemoteTimer_5min,
    kNkMAIDRemoteTimer_10min,
    kNkMAIDRemoteTimer_15min
};

// kNkMAIDCapability_FlashMode(0x0035)
enum eNkMAIDFlashModeDX2
{
    kNkMAIDFlashMode_VendorBaseDx2 = kNkMAIDFlashMode_SlowSyncRearCurtain + 0x100,
    kNkMAIDFlashMode_Off,
    kNkMAIDFlashMode_Auto,
    kNkMAIDFlashMode_On,
    kNkMAIDFlashMode_FillFlash = 6,
    kNkMAIDFlashMode_RearCurtainSync ,
    kNkMAIDFlashMode_SlowSyncFillFlash ,
    kNkMAIDFlashMode_RedEyeReductionLighting,
    kNkMAIDFlashMode_SlowSyncRedEye 
};

// used for ulThumbnailRotate, member of NkMAIDWBPresetData.
enum eNkMAIDThumbnailRotate
{
    kNkMAIDThumbnailRotate_0 = 0,
    kNkMAIDThumbnailRotate_90,
    kNkMAIDThumbnailRotate_270
};

enum eNkMAIDExposureDisplayStatus
{
    kNkMAIDExposureDisplayStatus_None = 0,
    kNkMAIDExposureDisplayStatus_ShutterSpeedLo,
    kNkMAIDExposureDisplayStatus_ShutterSpeedHi,
    kNkMAIDExposureDisplayStatus_ApertureLo,
    kNkMAIDExposureDisplayStatus_ApertureHi,
    kNkMAIDExposureDisplayStatus_SpLo_ApLo,
    kNkMAIDExposureDisplayStatus_SpLo_ApHi,
    kNkMAIDExposureDisplayStatus_SpHi_ApLo,
    kNkMAIDExposureDisplayStatus_SpHi_ApHi
};

//kNkMAIDCapability_InternalSplCommandValue(0x81cb)
enum eNkMAIDInternalSplCommandValue
{
    kNkMAIDInternalSplCommandValue_Full = 0,
    kNkMAIDInternalSplCommandValue_2,
    kNkMAIDInternalSplCommandValue_4,
    kNkMAIDInternalSplCommandValue_8,
    kNkMAIDInternalSplCommandValue_16,
    kNkMAIDInternalSplCommandValue_32,
    kNkMAIDInternalSplCommandValue_64,
    kNkMAIDInternalSplCommandValue_128
};

//	kNkMAIDCapability_CCDDataMode(0x81d0)
enum eNkMAIDCCDDataMode
{
    kNkMAIDCCDDataMode_Full = 0,
    kNkMAIDCCDDataMode_HiSpeedCrop,
    kNkMAIDCCDDataMode_AutoDXCrop,
    kNkMAIDCCDDataMode_FXFormat,
    kNkMAIDCCDDataMode_DXFormat,
    kNkMAIDCCDDataMode_5_4,
    kNkMAIDCCDDataMode_12x,
	kNkMAIDCCDDataMode_13x,
	kNkMAIDCCDDataMode_1_1,
    kNkMAIDCCDDataMode_16_9
};

//	kNkMAIDCapability_JpegCompressionPolicy(0x81d1)
enum eNkMAIDJpegCompressionPolicy
{
    kNkMAIDJpegCompressionPolicy_Size = 0,
    kNkMAIDJpegCompressionPolicy_Quality
};

//	kNkMAIDCapability_CameraType(0x81d7)
enum eNkMAIDCameraType
{
    kNkMAIDCameraType_D2H_V1 = 0,   // D2H Ver.1
    kNkMAIDCameraType_D2H_V2,       // D2H Ver.2
    kNkMAIDCameraType_D2X,          // D2X
    kNkMAIDCameraType_D2HS,         // D2HS
    kNkMAIDCameraType_D2X_V2,       // D2X Ver.2
    kNkMAIDCameraType_D2XS,         // D2XS
    kNkMAIDCameraType_D2HS_V2,      // D2HS Ver.2
    kNkMAIDCameraType_D40 = 0x10,   // D40
    kNkMAIDCameraType_D40X,         // D40X
    kNkMAIDCameraType_D3 = 0x20,    // D3
    kNkMAIDCameraType_D300,         // D300
    kNkMAIDCameraType_D60,          // D60
    kNkMAIDCameraType_D3X,          // D3X
    kNkMAIDCameraType_D700,         // D700
    kNkMAIDCameraType_D3_FU1,       // D3 Firmup 1
    kNkMAIDCameraType_D3_FU2,       // D3 Firmup 2
    kNkMAIDCameraType_D300_FU,      // D300 Firmup
    kNkMAIDCameraType_D90,          // D90
    kNkMAIDCameraType_D5000,        // D5000
    kNkMAIDCameraType_D300S,        // D300S
    kNkMAIDCameraType_D3S,          // D3S
    kNkMAIDCameraType_D3_FU3,       // D3 Firmup 3
    kNkMAIDCameraType_D700_FU1,     // D700 Firmup 1
    kNkMAIDCameraType_D7000,        // D7000
    kNkMAIDCameraType_D5100,        // D5100
    kNkMAIDCameraType_D800,         // D800
    kNkMAIDCameraType_D4,           // D4
    kNkMAIDCameraType_D800E,        // D800E
    kNkMAIDCameraType_D600,         // D600
    kNkMAIDCameraType_D5200,        // D5200
    kNkMAIDCameraType_D7100,        // D7100
    kNkMAIDCameraType_D5300,        // D5300
    kNkMAIDCameraType_Df,           // Df
    kNkMAIDCameraType_D610,         // D610
    kNkMAIDCameraType_D4S,          // D4S
    kNkMAIDCameraType_D810,         // D810
    kNkMAIDCameraType_D750,         // D750
    kNkMAIDCameraType_D5500,        // D5500
    kNkMAIDCameraType_D7200,        // D7200
    kNkMAIDCameraType_D810A,        // D810
    kNkMAIDCameraType_Nikon1V3,     // Nikon1V3
    kNkMAIDCameraType_D5,           // D5
    kNkMAIDCameraType_Nikon1V3_NotSupported,     // Nikon1V3 Unsupported firm
    kNkMAIDCameraType_D500,			// D500
    kNkMAIDCameraType_D5_FU1,		// D5 Firmup 1
    kNkMAIDCameraType_D5600, 		// D5600
	kNkMAIDCameraType_D7500,		// D7500
    kNkMAIDCameraType_D5_FU2,       // D5 Firmup 2
	kNkMAIDCameraType_D850,			// D850
    kNkMAIDCameraType_Z_7,			// Z_7
    kNkMAIDCameraType_Z_6,			// Z_6
	kNkMAIDCameraType_Z_50,			// Z_50
    kNkMAIDCameraType_D780,         // D780
    kNkMAIDCameraType_Z_7_FU1 = 77, // Z_7 Firmup 1
    kNkMAIDCameraType_Z_6_FU1,      // Z_6 Firmup 1
    kNkMAIDCameraType_D6,           // D6
	kNkMAIDCameraType_Z_5,          // Z_5
    kNkMAIDCameraType_Z_7_FU2,      // Z_7 Firmup 2
    kNkMAIDCameraType_Z_6_FU2,      // Z_6 Firmup 2
	kNkMAIDCameraType_Z_6II,        // Z_6II
	kNkMAIDCameraType_Z_7_FU3 = 84, // Z_7 Firmup 3
	kNkMAIDCameraType_Z_6_FU3 = 85, // Z_6 Firmup 3
	kNkMAIDCameraType_Z_7II,         // Z_7II
	kNkMAIDCameraType_Z_6II_FU1 = 88 // Z_6II Firmup 1
};

//	kNkMAIDCapability_EasyExposureCompMode(0x81d9)
enum eNkMAIDEasyExposureCompMode
{
    kNkMAIDEasyExposureCompMode_Off = 0,
    kNkMAIDEasyExposureCompMode_On,
    kNkMAIDEasyExposureCompMode_AutoReset
};

//	kNkMAIDCapability_ImageColorSpace(0x81db)
enum eNkMAIDImageColorSpace
{
    kNkMAIDImageColorSpace_sRGB = 0,
    kNkMAIDImageColorSpace_AdobeRGB
};

//	kNkMAIDCapability_NoiseReductionHighISO(0x81d8)
enum eNkMAIDNoiseReductionHighISO
{
    kNkMAIDNoiseReductionHighISO_Off    = 0,
    kNkMAIDNoiseReductionHighISO_Normal = 1,
    kNkMAIDNoiseReductionHighISO_High   = 2,
    kNkMAIDNoiseReductionHighISO_Low    = 3
};

enum eNkMAIDCPXCameraType
{
    kNkMAIDCPXCameraType_E8400 = 0,
    kNkMAIDCPXCameraType_E8800,
    kNkMAIDCPXCameraType_S4,
    kNkMAIDCPXCameraType_L1
};

enum eNkMAIDImageMode
{
    kNkMAIDImageMode_6M_High = 0,   //6M(2816*2112)
    kNkMAIDImageMode_6M_Normal,     //6M(2816*2112)
    kNkMAIDImageMode_3M,            //3M(2048*1536)
    kNkMAIDImageMode_PC,            //PC(1024*768)
    kNkMAIDImageMode_TV             //TV(640*480)
};

// kNkMAIDCapability_FocusAreaFrame(0x81f1)
enum eNkMAIDFocusAreaFrame
{
    kNkMAIDFocusAreaFrame_Normal = 0,
    kNkMAIDFocusAreaFrame_Wide
};

//	kNkMAIDCapability_ISOAutoHiLimit(0x81f2)
enum eNkMAIDISOAutoHiLimit
{
    kNkMAIDISOAutoHiLimit_ISO200 = 0,
    kNkMAIDISOAutoHiLimit_ISO400,
    kNkMAIDISOAutoHiLimit_ISO800,
    kNkMAIDISOAutoHiLimit_ISO1600,
    kNkMAIDISOAutoHiLimit_ISO3200,
    kNkMAIDISOAutoHiLimit_ISO6400,
    kNkMAIDISOAutoHiLimit_Hi1,
    kNkMAIDISOAutoHiLimit_Hi2,
    kNkMAIDISOAutoHiLimit_ISO12800,
    kNkMAIDISOAutoHiLimit_ISO25600
};

//	kNkMAIDCapability_ISOAutoHiLimit(0x81f2)
enum eNkMAIDISOAutoHiLimit2
{
    kNkMAIDISOAutoHiLimit2_ISO400 = 0,
    kNkMAIDISOAutoHiLimit2_ISO500,
    kNkMAIDISOAutoHiLimit2_ISO560,
    kNkMAIDISOAutoHiLimit2_ISO640,
    kNkMAIDISOAutoHiLimit2_ISO800,
    kNkMAIDISOAutoHiLimit2_ISO1000,
    kNkMAIDISOAutoHiLimit2_ISO1100,
    kNkMAIDISOAutoHiLimit2_ISO1250,
    kNkMAIDISOAutoHiLimit2_ISO1600,
    kNkMAIDISOAutoHiLimit2_ISO2000,
    kNkMAIDISOAutoHiLimit2_ISO2200,
    kNkMAIDISOAutoHiLimit2_ISO2500,
    kNkMAIDISOAutoHiLimit2_ISO3200,
    kNkMAIDISOAutoHiLimit2_ISO4000,
    kNkMAIDISOAutoHiLimit2_ISO4500,
    kNkMAIDISOAutoHiLimit2_ISO5000,
    kNkMAIDISOAutoHiLimit2_ISO6400,
    kNkMAIDISOAutoHiLimit2_ISO8000,
    kNkMAIDISOAutoHiLimit2_ISO9000,
    kNkMAIDISOAutoHiLimit2_ISO10000,
    kNkMAIDISOAutoHiLimit2_ISO12800,
    kNkMAIDISOAutoHiLimit2_ISOHi03,
    kNkMAIDISOAutoHiLimit2_ISOHi05,
    kNkMAIDISOAutoHiLimit2_ISOHi07,
    kNkMAIDISOAutoHiLimit2_ISOHi10,
    kNkMAIDISOAutoHiLimit2_ISOHi20,
    kNkMAIDISOAutoHiLimit2_ISOHi30
};

//	kNkMAIDCapability_ISOAutoHiLimit(0x81f2)
enum eNkMAIDISOAutoHiLimit3
{
    kNkMAIDISOAutoHiLimit3_ISO200 = 0,
    kNkMAIDISOAutoHiLimit3_ISO250,
    kNkMAIDISOAutoHiLimit3_ISO280,
    kNkMAIDISOAutoHiLimit3_ISO320,
    kNkMAIDISOAutoHiLimit3_ISO400,
    kNkMAIDISOAutoHiLimit3_ISO500,
    kNkMAIDISOAutoHiLimit3_ISO560,
    kNkMAIDISOAutoHiLimit3_ISO640,
    kNkMAIDISOAutoHiLimit3_ISO800,
    kNkMAIDISOAutoHiLimit3_ISO1000,
    kNkMAIDISOAutoHiLimit3_ISO1100,
    kNkMAIDISOAutoHiLimit3_ISO1250,
    kNkMAIDISOAutoHiLimit3_ISO1600,
    kNkMAIDISOAutoHiLimit3_ISO2000,
    kNkMAIDISOAutoHiLimit3_ISO2200,
    kNkMAIDISOAutoHiLimit3_ISO2500,
    kNkMAIDISOAutoHiLimit3_ISO3200,
    kNkMAIDISOAutoHiLimit3_ISO4000,
    kNkMAIDISOAutoHiLimit3_ISO4500,
    kNkMAIDISOAutoHiLimit3_ISO5000,
    kNkMAIDISOAutoHiLimit3_ISO6400,
    kNkMAIDISOAutoHiLimit3_ISOHi03,
    kNkMAIDISOAutoHiLimit3_ISOHi05,
    kNkMAIDISOAutoHiLimit3_ISOHi07,
    kNkMAIDISOAutoHiLimit3_ISOHi10,
    kNkMAIDISOAutoHiLimit3_ISOHi20,
    kNkMAIDISOAutoHiLimit3_ISO8000,
    kNkMAIDISOAutoHiLimit3_ISO9000,
    kNkMAIDISOAutoHiLimit3_ISO10000,
    kNkMAIDISOAutoHiLimit3_ISO12800,
    kNkMAIDISOAutoHiLimit3_ISOHi30,
    kNkMAIDISOAutoHiLimit3_ISOHi40,
    kNkMAIDISOAutoHiLimit3_ISO16000,
    kNkMAIDISOAutoHiLimit3_ISO18000,
    kNkMAIDISOAutoHiLimit3_ISO20000,
    kNkMAIDISOAutoHiLimit3_ISO25600,
    kNkMAIDISOAutoHiLimit3_ISO32000,
    kNkMAIDISOAutoHiLimit3_ISO36000,
    kNkMAIDISOAutoHiLimit3_ISO40000,
    kNkMAIDISOAutoHiLimit3_ISO51200,
    kNkMAIDISOAutoHiLimit3_ISO64000,
    kNkMAIDISOAutoHiLimit3_ISO72000,
    kNkMAIDISOAutoHiLimit3_ISO81200,
    kNkMAIDISOAutoHiLimit3_ISO102400,
    kNkMAIDISOAutoHiLimit3_ISOHi50
};

enum eNkMAIDISOAutoHiLimit4
{
    kNkMAIDISOAutoHiLimit4_ISO72 = 0,
    kNkMAIDISOAutoHiLimit4_ISO80,
    kNkMAIDISOAutoHiLimit4_ISO100,
    kNkMAIDISOAutoHiLimit4_ISO125,
    kNkMAIDISOAutoHiLimit4_ISO140,
    kNkMAIDISOAutoHiLimit4_ISO160,
    kNkMAIDISOAutoHiLimit4_ISO200,
    kNkMAIDISOAutoHiLimit4_ISO250,
    kNkMAIDISOAutoHiLimit4_ISO280,
    kNkMAIDISOAutoHiLimit4_ISO320,
    kNkMAIDISOAutoHiLimit4_ISO400,
    kNkMAIDISOAutoHiLimit4_ISO500,
    kNkMAIDISOAutoHiLimit4_ISO560,
    kNkMAIDISOAutoHiLimit4_ISO640,
    kNkMAIDISOAutoHiLimit4_ISO800,
    kNkMAIDISOAutoHiLimit4_ISO1000,
    kNkMAIDISOAutoHiLimit4_ISO1100,
    kNkMAIDISOAutoHiLimit4_ISO1250,
    kNkMAIDISOAutoHiLimit4_ISO1600,
    kNkMAIDISOAutoHiLimit4_ISO2000,
    kNkMAIDISOAutoHiLimit4_ISO2200,
    kNkMAIDISOAutoHiLimit4_ISO2500,
    kNkMAIDISOAutoHiLimit4_ISO3200,
    kNkMAIDISOAutoHiLimit4_ISO4000,
    kNkMAIDISOAutoHiLimit4_ISO4500,
    kNkMAIDISOAutoHiLimit4_ISO5000,
    kNkMAIDISOAutoHiLimit4_ISO6400,
    kNkMAIDISOAutoHiLimit4_ISO8000,
    kNkMAIDISOAutoHiLimit4_ISO9000,
    kNkMAIDISOAutoHiLimit4_ISO10000,
    kNkMAIDISOAutoHiLimit4_ISO12800,
    kNkMAIDISOAutoHiLimit4_ISOHi03,
    kNkMAIDISOAutoHiLimit4_ISOHi05,
    kNkMAIDISOAutoHiLimit4_ISOHi07,
    kNkMAIDISOAutoHiLimit4_ISOHi10,
	kNkMAIDISOAutoHiLimit4_ISOHi20,
	kNkMAIDISOAutoHiLimit4_ISO16000,
	kNkMAIDISOAutoHiLimit4_ISO18000,
	kNkMAIDISOAutoHiLimit4_ISO20000,
	kNkMAIDISOAutoHiLimit4_ISO25600,
};

// kNkMAIDCapability_BeepEx(0x81f3)
enum eNkMAIDBeepEx
{
    kNkMAIDBeepEx_High = 0,
    kNkMAIDBeepEx_Low,
    kNkMAIDBeepEx_Off,
    kNkMAIDBeepEx_On
};

// kNkMAIDCapability_BeepVolume(0x8314)
enum eNkMAIDBeepVolume
{
    kNkMAIDBeepVolume_Off = 0,
    kNkMAIDBeepVolume_1,
    kNkMAIDBeepVolume_2,
    kNkMAIDBeepVolume_3
};

//	kNkMAIDCapability_AFLockOnEx(0x81f4)
enum eNkMAIDAFLockOnEx
{
    kNkMAIDAFLockOnEx_High = 0,
    kNkMAIDAFLockOnEx_Normal,
    kNkMAIDAFLockOnEx_Low,
    kNkMAIDAFLockOnEx_Off,
    kNkMAIDAFLockOnEx_LittleHigh,
    kNkMAIDAFLockOnEx_LittleLow
};

// kNkMAIDCapability_WarningDisp(0x81f5)
enum eNkMAIDWarningDisp
{
    kNkMAIDWarningDisp_On = 0,
    kNkMAIDWarningDisp_Off
};

// kNkMAIDCapability_CellKind(0x81f6)
enum eNkMAIDCellKind
{
    kNkMAIDCellKind_Alkaline = 0,
    kNkMAIDCellKind_NiMH,
    kNkMAIDCellKind_Lithium,
    kNkMAIDCellKind_NiMn
};

// kNkMAIDCapability_InternalSplMRPTValue(0x81f7)
enum eNkMAIDInternalSplMRPTValue
{
    kNkMAIDInternalSplMRPTValue_4 = 0,
    kNkMAIDInternalSplMRPTValue_8,
    kNkMAIDInternalSplMRPTValue_16,
    kNkMAIDInternalSplMRPTValue_32,
    kNkMAIDInternalSplMRPTValue_64,
    kNkMAIDInternalSplMRPTValue_128,
    kNkMAIDInternalSplMRPTValue_Full
};

// kNkMAIDCapability_InternalSplMRPTCount(0x81f8)
enum eNkMAIDInternalSplMRPTCount
{
    kNkMAIDInternalSplMRPTCount_2 = 0,
    kNkMAIDInternalSplMRPTCount_3,
    kNkMAIDInternalSplMRPTCount_4,
    kNkMAIDInternalSplMRPTCount_5,
    kNkMAIDInternalSplMRPTCount_6,
    kNkMAIDInternalSplMRPTCount_7,
    kNkMAIDInternalSplMRPTCount_8,
    kNkMAIDInternalSplMRPTCount_9,
    kNkMAIDInternalSplMRPTCount_10,
    kNkMAIDInternalSplMRPTCount_15,
    kNkMAIDInternalSplMRPTCount_20,
    kNkMAIDInternalSplMRPTCount_25,
    kNkMAIDInternalSplMRPTCount_30,
    kNkMAIDInternalSplMRPTCount_35
};

// kNkMAIDCapability_InternalSplMRPTInterval(0x81f9)
enum eNkMAIDInternalSplMRPTInterval
{
    kNkMAIDInternalSplMRPTInterval_1 = 0,
    kNkMAIDInternalSplMRPTInterval_2,
    kNkMAIDInternalSplMRPTInterval_3,
    kNkMAIDInternalSplMRPTInterval_4,
    kNkMAIDInternalSplMRPTInterval_5,
    kNkMAIDInternalSplMRPTInterval_6,
    kNkMAIDInternalSplMRPTInterval_7,
    kNkMAIDInternalSplMRPTInterval_8,
    kNkMAIDInternalSplMRPTInterval_9,
    kNkMAIDInternalSplMRPTInterval_10,
    kNkMAIDInternalSplMRPTInterval_20,
    kNkMAIDInternalSplMRPTInterval_30,
    kNkMAIDInternalSplMRPTInterval_40,
    kNkMAIDInternalSplMRPTInterval_50
};

// kNkMAIDCapability_InternalSplCommandChannel(0x81fa)
enum eNkMAIDInternalSplCommandChannel
{
    kNkMAIDInternalSplCommandChannel_1 = 0,
    kNkMAIDInternalSplCommandChannel_2,
    kNkMAIDInternalSplCommandChannel_3,
    kNkMAIDInternalSplCommandChannel_4
};

// kNkMAIDCapability_InternalSplCmdGroupAComp(0x81fd)
// kNkMAIDCapability_InternalSplCmdGroupBComp(0x8200)
enum eNkMAIDInternalSplCmdSelfComp
{
    kNkMAIDInternalSplCmdSelfComp_M30 = 0,
    kNkMAIDInternalSplCmdSelfComp_M27,
    kNkMAIDInternalSplCmdSelfComp_M23,
    kNkMAIDInternalSplCmdSelfComp_M20,
    kNkMAIDInternalSplCmdSelfComp_M17,
    kNkMAIDInternalSplCmdSelfComp_M13,
    kNkMAIDInternalSplCmdSelfComp_M10,
    kNkMAIDInternalSplCmdSelfComp_M07,
    kNkMAIDInternalSplCmdSelfComp_M03,
    kNkMAIDInternalSplCmdSelfComp_0,
    kNkMAIDInternalSplCmdSelfComp_P03,
    kNkMAIDInternalSplCmdSelfComp_P07,
    kNkMAIDInternalSplCmdSelfComp_P10,
    kNkMAIDInternalSplCmdSelfComp_P13,
    kNkMAIDInternalSplCmdSelfComp_P17,
    kNkMAIDInternalSplCmdSelfComp_P20,
    kNkMAIDInternalSplCmdSelfComp_P23,
    kNkMAIDInternalSplCmdSelfComp_P27,
    kNkMAIDInternalSplCmdSelfComp_P30
};

// kNkMAIDCapability_InternalSplCmdGroupAMode(0x81fc)
// kNkMAIDCapability_InternalSplCmdGroupBMode(0x81ff)
enum eNkMAIDInternalSplCmdGroupMode 
{
    kNkMAIDInternalSplCmdGroupMode_TTL = 0,
    kNkMAIDInternalSplCmdGroupMode_AA,
    kNkMAIDInternalSplCmdGroupMode_Manual,
    kNkMAIDInternalSplCmdGroupMode_Off
};

// kNkMAIDCapability_InternalSplCmdGroupAComp(0x81fd)
// kNkMAIDCapability_InternalSplCmdGroupBComp(0x8200)
enum eNkMAIDInternalSplCmdGroupComp
{
    kNkMAIDInternalSplCmdGroupComp_M30 = 0, // -3.0
    kNkMAIDInternalSplCmdGroupComp_M27,     // -2.7
    kNkMAIDInternalSplCmdGroupComp_M23,     // -2.3
    kNkMAIDInternalSplCmdGroupComp_M20,     // -2.0
    kNkMAIDInternalSplCmdGroupComp_M17,     // -1.7
    kNkMAIDInternalSplCmdGroupComp_M13,     // -1.3
    kNkMAIDInternalSplCmdGroupComp_M10,     // -1.0
    kNkMAIDInternalSplCmdGroupComp_M07,     // -0.7
    kNkMAIDInternalSplCmdGroupComp_M03,     // -0.3
    kNkMAIDInternalSplCmdGroupComp_0,       // 0
    kNkMAIDInternalSplCmdGroupComp_P03,     // +0.3
    kNkMAIDInternalSplCmdGroupComp_P07,     // +0.7
    kNkMAIDInternalSplCmdGroupComp_P10,     // +1.0
    kNkMAIDInternalSplCmdGroupComp_P13,     // +1.3
    kNkMAIDInternalSplCmdGroupComp_P17,     // +1.7
    kNkMAIDInternalSplCmdGroupComp_P20,     // +2.0
    kNkMAIDInternalSplCmdGroupComp_P23,     // +2.3
    kNkMAIDInternalSplCmdGroupComp_P27,     // +2.7
    kNkMAIDInternalSplCmdGroupComp_P30      // +3.0
};

// kNkMAIDCapability_InternalSplCmdGroupAValue(0x81fe)
// kNkMAIDCapability_InternalSplCmdGroupBValue(0x8201)
enum eNkMAIDInternalSplCmdGroupValue
{
    kNkMAIDInternalSplCmdGroupValue_1 = 0,  // 1/1
    kNkMAIDInternalSplCmdGroupValue_2,      // 1/2
    kNkMAIDInternalSplCmdGroupValue_4,      // 1/4
    kNkMAIDInternalSplCmdGroupValue_8,      // 1/8
    kNkMAIDInternalSplCmdGroupValue_16,     // 1/16
    kNkMAIDInternalSplCmdGroupValue_32,     // 1/32
    kNkMAIDInternalSplCmdGroupValue_64,     // 1/64
    kNkMAIDInternalSplCmdGroupValue_128,    // 1/128
    kNkMAIDInternalSplCmdGroupValue_1_3,    // 1/1.3
    kNkMAIDInternalSplCmdGroupValue_1_7,    // 1/1.7
    kNkMAIDInternalSplCmdGroupValue_2_5,    // 1/2.5
    kNkMAIDInternalSplCmdGroupValue_3_2,    // 1/3.2
    kNkMAIDInternalSplCmdGroupValue_5,      // 1/5
    kNkMAIDInternalSplCmdGroupValue_6_4,    // 1/6.4
    kNkMAIDInternalSplCmdGroupValue_10,     // 1/10
    kNkMAIDInternalSplCmdGroupValue_13,     // 1/13
    kNkMAIDInternalSplCmdGroupValue_20,     // 1/20
    kNkMAIDInternalSplCmdGroupValue_25,     // 1/25
    kNkMAIDInternalSplCmdGroupValue_40,     // 1/40
    kNkMAIDInternalSplCmdGroupValue_50,     // 1/50
    kNkMAIDInternalSplCmdGroupValue_80,     // 1/80
    kNkMAIDInternalSplCmdGroupValue_100     // 1/100
};

// kNkMAIDCapability_InternalSplCmdSelfMode(0x8202)
enum eNkMAIDInternalSplCmdSelfMode
{
    kNkMAIDInternalSplCmdSelfMode_TTL = 0,
    kNkMAIDInternalSplCmdSelfMode_Manual,
    kNkMAIDInternalSplCmdSelfMode_Off
};

// kNkMAIDCapability_InternalSplCmdSelfValue(0x8203)
enum eNkMAIDInternalSplCmdSelfValue
{
    kNkMAIDInternalSplCmdSelfValue_1 = 0,   // 1/1
    kNkMAIDInternalSplCmdSelfValue_2,       // 1/2
    kNkMAIDInternalSplCmdSelfValue_4,       // 1/4
    kNkMAIDInternalSplCmdSelfValue_8,       // 1/8
    kNkMAIDInternalSplCmdSelfValue_16,      // 1/16
    kNkMAIDInternalSplCmdSelfValue_32,      // 1/32
    kNkMAIDInternalSplCmdSelfValue_64,      // 1/64
    kNkMAIDInternalSplCmdSelfValue_128,     // 1/128
    kNkMAIDInternalSplCmdSelfValue_1_3,     // 1/1.3
    kNkMAIDInternalSplCmdSelfValue_1_7,     // 1/1.7
    kNkMAIDInternalSplCmdSelfValue_2_5,     // 1/2.5
    kNkMAIDInternalSplCmdSelfValue_3_2,     // 1/3.2
    kNkMAIDInternalSplCmdSelfValue_5,       // 1/5
    kNkMAIDInternalSplCmdSelfValue_6_4,     // 1/6.4
    kNkMAIDInternalSplCmdSelfValue_10,      // 1/10
    kNkMAIDInternalSplCmdSelfValue_13,      // 1/13
    kNkMAIDInternalSplCmdSelfValue_20,      // 1/20
    kNkMAIDInternalSplCmdSelfValue_25,      // 1/25
    kNkMAIDInternalSplCmdSelfValue_40,      // 1/40
    kNkMAIDInternalSplCmdSelfValue_50,      // 1/50
    kNkMAIDInternalSplCmdSelfValue_80,      // 1/80
    kNkMAIDInternalSplCmdSelfValue_100      // 1/100
};

// kNkMAIDCapability_MonochromeFilterEffect(0x8220)
enum eNkMAIDMonochromeFilterEffect
{
    kNkMAIDMonochromeFilterEffect_NoBorder = 0,
    kNkMAIDMonochromeFilterEffect_Yellow,
    kNkMAIDMonochromeFilterEffect_Orange,
    kNkMAIDMonochromeFilterEffect_Red,
    kNkMAIDMonochromeFilterEffect_Green
};

// kNkMAIDCapability_MonochromeSettingType(0x8224)
enum eNkMAIDMonochromeSettingType
{
    kNkMAIDMonochromeSettingType_Standard = 0,
    kNkMAIDMonochromeSettingType_Custom
};

// kNkMAIDCapability_AutoOffTimer(0x8226)
enum eNkMAIDAutoOffTimer
{
    kNkMAIDAutoOffTimer_Short = 0,
    kNkMAIDAutoOffTimer_Normal,
    kNkMAIDAutoOffTimer_Long,
    kNkMAIDAutoOffTimer_Custom
};

// kNkMAIDCapability_ExternalSplValue(0x822a)
enum eNkMAIDExternalSplValue
{
    kNkMAIDExternalSplValue_Full =0,    // Full
    kNkMAIDExternalSplValue_2,          // 1/2
    kNkMAIDExternalSplValue_4,          // 1/4
    kNkMAIDExternalSplValue_8,          // 1/8
    kNkMAIDExternalSplValue_16,         // 1/16
    kNkMAIDExternalSplValue_32,         // 1/32
    kNkMAIDExternalSplValue_64,         // 1/64
    kNkMAIDExternalSplValue_128,        // 1/128
    kNkMAIDExternalSplValue_256         // 1/256
};

// kNkMAIDCapability_CompressRAWEx(0x824b)
enum eNkMAIDCompressRAWEx
{
    kNkMAIDCompressRAWEx_Uncompressed       = 0,//Uncompressed
    kNkMAIDCompressRAWEx_Compressed         = 1,//Compressed
    kNkMAIDCompressRAWEx_LosslessCompressed = 2 //Lossless compressed
};

// kNkMAIDCapability_Slot2ImageSaveMode(0x822c)
enum eNkMAIDSlot2ImageSaveMode 
{
    kNkMAIDSlot2ImageSaveMode_Overflow = 0,     //Overflow
    kNkMAIDSlot2ImageSaveMode_Backup = 1,       //Backup
    kNkMAIDSlot2ImageSaveMode_Jpeg = 2,         //RAW primary & JPEG secondary
    kNkMAIDSlot2ImageSaveMode_SplitlJpeg  = 3,  //Jpeg & Smaller Jpeg
};

// kNkMAIDCapability_CompressRAWBitMode(0x822d)
enum eNkMAIDCompressRAWBitMode 
{
    kNkMAIDCompressRAWBitMode_12bit = 0, //12-bit depth recording
    kNkMAIDCompressRAWBitMode_14bit = 1  //14-bit depth recording
};

// kNkMAIDCapability_PictureControl(0x822e)
enum eNkMAIDPictureControl
{
    kNkMAIDPictureControl_Undefined  = 0,       //Undefined
    kNkMAIDPictureControl_Standard   = 1,       //Standard
    kNkMAIDPictureControl_Neutral    = 2,       //Neutral
    kNkMAIDPictureControl_Vivid      = 3,       //Vivid
    kNkMAIDPictureControl_Monochrome = 4,       //Monochrome
    kNkMAIDPictureControl_Portrait   = 5,       //Portrait
    kNkMAIDPictureControl_Landscape  = 6,       //Landscape
    kNkMAIDPictureControl_Flat       = 7,       //Flat
    kNkMAIDPictureControl_Auto       = 8,       //Auto
    kNkMAIDPictureControl_Option1    = 101,     //Option picture control1
    kNkMAIDPictureControl_Option2    = 102,     //Option picture control2
    kNkMAIDPictureControl_Option3    = 103,     //Option picture control3
    kNkMAIDPictureControl_Option4    = 104,     //Option picture control4
	kNkMAIDPictureControl_Dream      = 101,		//Dream
	kNkMAIDPictureControl_Morning    = 102,		//Morning
	kNkMAIDPictureControl_Pop        = 103,		//Popt
	kNkMAIDPictureControl_Sunday     = 104,		//Sunday
	kNkMAIDPictureControl_Somber     = 105,		//Somber
	kNkMAIDPictureControl_Dramatic   = 106,		//Dramatic
	kNkMAIDPictureControl_Silence    = 107,		//Silence
	kNkMAIDPictureControl_Breached   = 108,		//Breached
	kNkMAIDPictureControl_Melancholic = 109,    //Melancholic
	kNkMAIDPictureControl_Pure       = 110,		//Pure
	kNkMAIDPictureControl_Denim      = 111,		//Denim
	kNkMAIDPictureControl_Toy        = 112,		//Toy
	kNkMAIDPictureControl_Sepia      = 113,		//Sepia
	kNkMAIDPictureControl_Blue       = 114,		//Blue
	kNkMAIDPictureControl_Red        = 115,		//Read
	kNkMAIDPictureControl_Pink       = 116,		//Pink
	kNkMAIDPictureControl_Charcoal   = 117,		//Charcorl
	kNkMAIDPictureControl_Graphite   = 118,		//Graphite
	kNkMAIDPictureControl_Binary     = 119,		//Binary
	kNkMAIDPictureControl_Carbon     = 120,		//Carbon
    kNkMAIDPictureControl_Custom1    = 201,     //Custom picture control1
    kNkMAIDPictureControl_Custom2    = 202,     //Custom picture control2
    kNkMAIDPictureControl_Custom3    = 203,     //Custom picture control3
    kNkMAIDPictureControl_Custom4    = 204,     //Custom picture control4
    kNkMAIDPictureControl_Custom5    = 205,     //Custom picture control5
    kNkMAIDPictureControl_Custom6    = 206,     //Custom picture control6
    kNkMAIDPictureControl_Custom7    = 207,     //Custom picture control7
    kNkMAIDPictureControl_Custom8    = 208,     //Custom picture control8
    kNkMAIDPictureControl_Custom9    = 209      //Custom picture control9
};

// kNkMAIDCapability_Active_D_Lighting(0x8232)
enum eNkMAIDActive_D_Lighting
{
    kNkMAIDActive_D_Lighting_High       = 0, //High
    kNkMAIDActive_D_Lighting_Normal     = 1, //Normal
    kNkMAIDActive_D_Lighting_Low        = 2, //Low
    kNkMAIDActive_D_Lighting_Off        = 3, //Off
    kNkMAIDActive_D_Lighting_On         = 4, //On
    kNkMAIDActive_D_Lighting_ExtraHigh  = 5, //Extra high(Extra high 1)
    kNkMAIDActive_D_Lighting_Auto       = 6, //Auto
    kNkMAIDActive_D_Lighting_ExtraHigh2 = 7  //Extra high 2
};

// kNkMAIDCapability_LiveViewMode(0x823c)
enum eNkMAIDLiveViewMode
{
    kNkMAIDLiveViewMode_Handheld    = 0, //Handheld
    kNkMAIDLiveViewMode_Tripod      = 1  //Tripod
};

// kNkMAIDCapability_LiveViewDriveMode(0x823d)
enum eNkMAIDLiveViewDriveMode
{
    kNkMAIDLiveViewDriveMode_Single         = 0, //Single
    kNkMAIDLiveViewDriveMode_ContinuousLow  = 1, //Continuous low
    kNkMAIDLiveViewDriveMode_ContinuousHigh = 2  //Continuous high
};

// kNkMAIDCapability_DynamicAFArea(0x8233)
enum eNkMAIDDynamicAFArea
{
    kNkMAIDDynamicAFArea_9              = 0,//Dynamic AF mode (9 points)
    kNkMAIDDynamicAFArea_21             = 1,//Dynamic AF mode (21 points)
    kNkMAIDDynamicAFArea_51             = 2,//Dynamic AF mode (51 points)
    kNkMAIDDynamicAFArea_51_3DTtracking = 3 //3D-tracking
};

// kNkMAIDCapability_InfoDispSetting(0x8235)
enum eNkMAIDInfoDispSetting
{
    kNkMAIDInfoDispSetting_Auto             = 0, //Auto
    kNkMAIDInfoDispSetting_M_DarkOnLight    = 1, //Manual (black letters)
    kNkMAIDInfoDispSetting_M_LightOnDark    = 2  //Manual (black letters)
};

// kNkMAIDCapability_IndicatorDisplay(0x823b)
enum eNkMAIDIndicatorDisplay
{
    kNkMAIDIndicatorDisplay_Plus    = 0, //+
    kNkMAIDIndicatorDisplay_Minus   = 1  //-
};

// kNkMAIDCapability_LiveViewImageZoomRate(0x823f)
enum eNkMAIDLiveViewImageZoomRate
{
    kNkMAIDLiveViewImageZoomRate_All    = 0, //Entire display
    kNkMAIDLiveViewImageZoomRate_25     = 1, //25%
    kNkMAIDLiveViewImageZoomRate_33     = 2, //33%
    kNkMAIDLiveViewImageZoomRate_50     = 3, //50%
    kNkMAIDLiveViewImageZoomRate_66     = 4, //66.7%
    kNkMAIDLiveViewImageZoomRate_100    = 5, //100%
    kNkMAIDLiveViewImageZoomRate_200    = 6, //200%
    kNkMAIDLiveViewImageZoomRate_13     = 7, //13%
    kNkMAIDLiveViewImageZoomRate_17     = 8, //17%
};

// kNkMAIDCapability_MFDrive(0x8249)
enum eNkMAIDMFDrive
{
    kNkMAIDMFDrive_InfinityToClosest = 0, //No limit -> Closest
    kNkMAIDMFDrive_ClosestToInfinity = 1  //Closest -> No limit
};

// kNkMAIDCapability_LiveViewStatus(0x823e)
enum eNkMAIDLiveViewStatus
{
    kNkMAIDLiveViewStatus_OFF = 0,
    kNkMAIDLiveViewStatus_ON,
	kNkMAIDLiveViewStatus_ON_Menu,
    kNkMAIDLiveViewStatus_ON_RemoteLV,
    kNkMAIDLiveViewStatus_ON_CameraLV
};

// kNkMAIDCapability_CellKindPriority(0x824c)
enum eNkMAIDCellKindPriority
{
    kNkMAIDCellKindPriority_MB_D10 = 0,
    kNkMAIDCellKindPriority_Camera,
    kNkMAIDCellKindPriority_MB_D11 = 0
};

// kNkMAIDCapability_AFAreaPoint(0x8254)
enum eNkMAIDAFAreaPoint
{
    kNkMAIDAFAreaPoint_51 = 0,
    kNkMAIDAFAreaPoint_11,
    kNkMAIDAFAreaPoint_39, 
    kNkMAIDAFAreaPoint_55,
    kNkMAIDAFAreaPoint_15,
    kNkMAIDAFAreaPoint_All,
    kNkMAIDAFAreaPoint_Skip,
    kNkMAIDAFAreaPoint_105,
    kNkMAIDAFAreaPoint_27
};

// kNkMAIDCapability_WBAutoType(0x8316)
enum eNkWBAutoType
{
    kNkWBAutoType_Normal = 0,
    kNkWBAutoType_WarmWhite,
	kNkWBAutoType_KeepWhite,
	kNkWBAutoType_KeepAtmosphere
};

// kNkMAIDCapability_WBFluorescentType(0x824d)
enum eNkWBFluorescentType
{
    kNkWBFluorescentType_SodiumVapor = 0,
    kNkWBFluorescentType_WarmWhite, 
    kNkWBFluorescentType_White, 
    kNkWBFluorescentType_CoolWhite, 
    kNkWBFluorescentType_DayWhite, 
    kNkWBFluorescentType_Daylight, 
    kNkWBFluorescentType_HiTempMercuryVapor
};

// kNkMAIDCapability_ContrastAF(0x8240)
enum eNkMAIDContrastAF
{
    kNkMAIDContrastAF_Start         = 0x00, //Start the AF driving
    kNkMAIDContrastAF_Stop          = 0x01, //Stop the AF driving
    kNkMAIDContrastAF_OK            = 0x10, //Completed in the focused status
    kNkMAIDContrastAF_OutOfFocus    = 0x11, //Completed in the non-focused status
    kNkMAIDContrastAF_Busy          = 0x12  //AF driving
};

// kNkMAIDCapability_LensType(0x825c)
enum eNkMAIDLensType
{
    kNkMAIDLensType_D       = 0x00000001,   //D type
    kNkMAIDLensType_G       = 0x00000010,   //G type
    kNkMAIDLensType_VR      = 0x00000100,   //VR
    kNkMAIDLensType_DX      = 0x00001000,   //DX
    kNkMAIDLensType_AFS     = 0x00010000,   //AF-S lens
    kNkMAIDLensType_AD      = 0x00100000,   //Auto Distortion
    kNkMAIDLensType_RET     = 0x01000000,   //Retractable lens
    kNkMAIDLensType_E       = 0x00000020,   //E type
    kNkMAIDLensType_STM     = 0x00000040,   //STM
	kNkMAIDLensType_CD      = 0x00000080    //constantly distortion
};

// kNkMAIDCapability_LiveViewProhibit(0x825e)
enum eNkMAIDLiveViewProhibit
{
    kNkMAIDLiveViewProhibit_ExpModeScene    = 0x80000000,   //Bit31:ExposureMode is SCENE.
    kNkMAIDLiveViewProhibit_MountAdapterFWVer = 0x10000000, //Bit28:Version upgrade notification warning when mount adapter is attached.
    kNkMAIDLiveViewProhibit_RecordingImage  = 0x4000000,    //Bit26:During image recording.
    kNkMAIDLiveViewProhibit_Retractable     = 0x1000000,    //Bit24:The lens is retracting.
    kNkMAIDLiveViewProhibit_DuringMirrorup  = 0x400000, //Bit22:Release mode is mirror up and during cleaning mirror-up operation.
    kNkMAIDLiveViewProhibit_BulbWarning     = 0x200000, //Bit21:During bulb warning.
    kNkMAIDLiveViewProhibit_CardUnformat    = 0x100000, //Bit20:Card unformatted
    kNkMAIDLiveViewProhibit_CardError       = 0x80000,  //Bit19:Card error
    kNkMAIDLiveViewProhibit_CardProtect     = 0x40000,  //Bit18:Card protected
    kNkMAIDLiveViewProhibit_TempRise        = 0x20000,  //Bit17:The live view cannot be started when the temperature rises.
    kNkMAIDLiveViewProhibit_EffectMode      = 0x10000,  //Bit16:ExposureMode is EFFECTS.
    kNkMAIDLiveViewProhibit_Capture         = 0x8000,   //Bit15:During processing by the shooting command(*Until the shooting operation ends.)
    kNkMAIDLiveViewProhibit_NoCardLock      = 0x4000,   //Bit14:The recording destination is the card or the card & SDRAM,
                                                        //      and the card is not inserted with the release disabled without a card.
    kNkMAIDLiveViewProhibit_MirrorMode      = 0x2000,   //Bit13:The release mode is [Mirror-up].
    kNkMAIDLiveViewProhibit_SdramImg        = 0x1000,   //Bit12:There is an image whose recording destination is SDRAM.
    kNkMAIDLiveViewProhibit_NonCPU          = 0x0800,   //Bit11:The CPU lens is not mounted and the exposure mode is not M.
    kNkMAIDLiveViewProhibit_ApertureRing    = 0x0400,   //Bit10:While the aperture value operation by the lens aperture ring is valid
    kNkMAIDLiveViewProhibit_TTL             = 0x0200,   //Bit9:TTL error or SB-related error	(0: Invalid, 1: Valid)
    kNkMAIDLiveViewProhibit_Battery         = 0x0100,   //Bit8:During insufficiency of battery	(0: Invalid, 1: Valid)
    kNkMAIDLiveViewProhibit_Mirrorup        = 0x0080,   //Bit7:During cleaning mirror-up operation
    kNkMAIDLiveViewProhibit_Bulb            = 0x0040,   //Bit6:Bulb error	(0: Invalid, 1: Valid)
    kNkMAIDLiveViewProhibit_FEE             = 0x0020,   //Bit5:The aperture value is being set by the lens aperture ring.
    kNkMAIDLiveViewProhibit_Button          = 0x0010,   //Bit4:Fully pressed button error
    kNkMAIDLiveViewProhibit_ImageSensor     = 0x0008,   //Bit3:Image sensor factor
    kNkMAIDLiveViewProhibit_Sequence        = 0x0004,   //Bit2:Sequence error
    kNkMAIDLiveViewProhibit_CF              = 0x0001    //Bit0:The recording destination is the CF.
};

// kNkMAIDCapability_DateImprintSetting(0x825f)
enum eNkMAIDDateImprintSetting
{
    kNkMAIDDateImprintSetting_Off = 0,      //OFF
    kNkMAIDDateImprintSetting_Date,         //Year/month/date
    kNkMAIDDateImprintSetting_Date_Time,    //Year/month/date/time
    kNkMAIDDateImprintSetting_DateCounter   //Birthday counter
};

// kNkMAIDCapability_DateCounterSelect(0x8260)
enum eNkMAIDDateCounterSelect
{
    kNkMAIDDateCounterSelect_1 = 0, //First
    kNkMAIDDateCounterSelect_2,     //Second
    kNkMAIDDateCounterSelect_3      //Third
};

// kNkMAIDCapability_DateCounterDispSetting(0x8262)
enum eNkMAIDDateCounterDispSetting
{
    kNkMAIDDateCounterDispSetting_NumberOfDays = 0, //Days
    kNkMAIDDateCounterDispSetting_Years_Days,       //Year/date
    kNkMAIDDateCounterDispSetting_Years_Months_Days //Year/month/date
};

// kNkMAIDCapability_RangeFinderSetting(0x8263)
enum eNkMAIDRangeFinderSetting
{
    kNkMAIDRangeFinderSetting_Off = 0,  //OFF
    kNkMAIDRangeFinderSetting_On        //ON
};

// kNkMAIDCapability_RangeFinderStatus(0x8264)
enum eNkMAIDRangeFinderStatus
{
    kNkMAIDRangeFinderStatus_OutOfFocus = 0,// out of focus
    kNkMAIDRangeFinderStatus_FrontFocusL,   // in front of the subject(large distance)
    kNkMAIDRangeFinderStatus_FrontFocusS,   // in front of the subject (small distance)
    kNkMAIDRangeFinderStatus_InFocus,       // In focus
    kNkMAIDRangeFinderStatus_RearFocusS,    // behind the subject (small distance)
    kNkMAIDRangeFinderStatus_RearFocusL     // behind the subject (large distance)
};

// kNkMAIDCapability_ScreenTips(0x8268)
enum eNkMAIDScreenTips
{
    kNkMAIDScreenTips_On = 0,   // Guide display ON
    kNkMAIDScreenTips_Off       // Guide display OFF
};

// kNkMAIDCapability_IlluminationSetting(0x8269)
enum eNkMAIDIlluminationSetting
{
    kNkMAIDIlluminationSetting_LCDBacklight = 0,    // Illuminator ON/OFF
    kNkMAIDIlluminationSetting_Both                 // ON/OFF of the illuminator and Info screen
};

// kNkMAIDCapability_ShutterSpeedLockSetting(0x826a)
enum eNkMAIDShutterSpeedLockSetting
{
    kNkMAIDShutterSpeedLockSetting_Off = 0, // Off
    kNkMAIDShutterSpeedLockSetting_On       // On
};

// kNkMAIDCapability_ApertureLockSetting(0x826b)
enum eNkMAIDApertureLockSetting
{
    kNkMAIDApertureLockSetting_Off = 0, // Off
    kNkMAIDApertureLockSetting_On       // On
};

// kNkMAIDCapability_VignetteControl(0x826c)
enum eNkMAIDVignetteControl
{
    kNkMAIDVignetteControl_High   = 0, //High
    kNkMAIDVignetteControl_Normal = 1, //Normal
    kNkMAIDVignetteControl_Low    = 2, //Low
    kNkMAIDVignetteControl_Off    = 3  //Off
};

// kNkMAIDCapability_FocusPointBrightness(0x826d)
enum eNkMAIDFocusPointBrightness
{
    kNkMAIDFocusPointBrightness_Low       = 0,  //Dark
    kNkMAIDFocusPointBrightness_Normal    = 1,  //Normal
    kNkMAIDFocusPointBrightness_High      = 2,  //Bright
    kNkMAIDFocusPointBrightness_ExtraHigh = 3   //Extra bright
};

// kNkMAIDCapability_EnableCommandDialOnPlaybackEx(0x8278)
enum eNkMAIDEnableCommandDialOnPlaybackEx
{
    kNkMAIDEnableCommandDialOnPlaybackEx_Off = 0,//OFF
    kNkMAIDEnableCommandDialOnPlaybackEx_On  = 1,//ON
    kNkMAIDEnableCommandDialOnPlaybackEx_On_WithoutImageConf = 2//ON (except during image review)
};

// kNkMAIDCapability_MovieScreenSize(0x8272)
enum eNkMAIDMovieScreenSize
{
    kNkMAIDMovieScreenSize_QVGA = 0,                // QVGA (320x216)
    kNkMAIDMovieScreenSize_VGA,                     // VGA  (640x424)
    kNkMAIDMovieScreenSize_720p,                    // 720p (1280x720)
    kNkMAIDMovieScreenSize_VGA_Normal,              // VGA  (640x424):Normal
    kNkMAIDMovieScreenSize_VGA_Fine,                // VGA  (640x424):High image quality
    kNkMAIDMovieScreenSize_720p_Normal_Lowfps,      // 720p (1280x720):Normal, 24fps
    kNkMAIDMovieScreenSize_720p_Fine_Lowfps,        // 720p (1280x720):High image quality, 24fps
    kNkMAIDMovieScreenSize_720p_Normal_Highfps,     // 720p (1280x720):Normal, 30fps(NTSC)/25fps(PAL)
    kNkMAIDMovieScreenSize_720p_Fine_Highfps,       // 720p (1280x720):High image quality, 30fps(NTSC)/25fps(PAL)
    kNkMAIDMovieScreenSize_FullHD_Normal,           // FullHD (1920x1080):Normal
    kNkMAIDMovieScreenSize_FullHD_Fine,             // FullHD (1920x1080):High image quality
    kNkMAIDMovieScreenSize_FullHD_Normal_Highfps,   // FullHD (1920x1080):Normal, 30fps(NTSC)/25fps(PAL)
    kNkMAIDMovieScreenSize_FullHD_Fine_Highfps      // FullHD (1920x1080):High image quality, 30fps(NTSC)/25fps(PAL)
};

// kNkMAIDCapability_MovieScreenSize(0x8272)
enum eNkMAIDMovieScreenSize2
{
    kNkMAIDMovieScreenSize2_FullHD_30fps = 0,     // FullHD (1920x1080):30fps
    kNkMAIDMovieScreenSize2_FullHD_25fps,         // FullHD (1920x1080):25fps
    kNkMAIDMovieScreenSize2_FullHD_24fps,         // FullHD (1920x1080):24fps
    kNkMAIDMovieScreenSize2_720p_60fps,           // 720p (1280x720):60fps
    kNkMAIDMovieScreenSize2_720p_50fps,           // 720p (1280x720):50fps
    kNkMAIDMovieScreenSize2_720p_30fps,           // 720p (1280x720):30fps
    kNkMAIDMovieScreenSize2_720p_25fps,           // 720p (1280x720):25fps
    kNkMAIDMovieScreenSize_VGA_30fps,             // VGA  (640x424):30fps
    kNkMAIDMovieScreenSize_VGA_25fps,             // VGA  (640x424):25fps
    kNkMAIDMovieScreenSize2_FullHD_30fps_Crop,    // FullHD (1920x1080):30fps Crop
    kNkMAIDMovieScreenSize2_FullHD_25fps_Crop,    // FullHD (1920x1080):25fps Crop
    kNkMAIDMovieScreenSize2_FullHD_24fps_Crop     // FullHD (1920x1080):24fps Crop
};

// kNkMAIDCapability_MovieScreenSize(0x8272)
// Each definition of eNkMAIDMovieScreenSize3 has plural meanings by a camera and other setting.
// The comment below shows "camera1 setting1/camera1 setting2/camera2".
// (The details appear in MAID specifications of Module using eNkMAIDMovieScreenSize3.)
enum eNkMAIDMovieScreenSize3
{
    kNkMAIDMovieScreenSize3_FullHD_1 = 0,   // FullHD (1920x1080):60p/50p/60p
    kNkMAIDMovieScreenSize2_FullHD_2 = 1,   // FullHD (1920x1080):30p/25p/50p
    kNkMAIDMovieScreenSize3_FullHD_3 = 2,   // FullHD (1920x1080):24p/24p/30p
    kNkMAIDMovieScreenSize3_720p = 3,       // 720p (1280x720):60p/50p/60p
    kNkMAIDMovieScreenSize3_VGA = 4,        // VGA  (640x424) :30p/25p/unsupported
    kNkMAIDMovieScreenSize3_FullHD_4 = 5,   // FullHD (1920x1080):unsupported/unsupported/25p
    kNkMAIDMovieScreenSize3_FullHD_5 = 6,   // FullHD (1920x1080):unsupported/unsupported/24p
    kNkMAIDMovieScreenSize3_720p_2 = 7      // 720p (1280x720):unsupported/unsupported/50p
};

// kNkMAIDCapability_MovieScreenSize(0x8272)
enum eNkMAIDMovieScreenSize4
{
    kNkMAIDMovieScreenSize4_FullHD_60i = 0,   // FullHD (1920x1080):60i
    kNkMAIDMovieScreenSize4_FullHD_50i,       // FullHD (1920x1080):50i
    kNkMAIDMovieScreenSize4_FullHD_30p,       // FullHD (1920x1080):30p
    kNkMAIDMovieScreenSize4_FullHD_25p,       // FullHD (1920x1080):25p
    kNkMAIDMovieScreenSize4_FullHD_24p,       // FullHD (1920x1080):24p
    kNkMAIDMovieScreenSize4_720p_60p,         // 720p (1280x720):60p
    kNkMAIDMovieScreenSize4_720p_50p          // 720p (1280x720):50p
};

// kNkMAIDCapability_MovieScreenSize(0x8272)
enum eNkMAIDMovieScreenSize5
{
    kNkMAIDMovieScreenSize5_FullHD_1,         // FullHD (1920x1080):60p/50p
    kNkMAIDMovieScreenSize5_FullHD_2,         // FullHD (1920x1080):60i/50i
    kNkMAIDMovieScreenSize5_FullHD_3,         // FullHD (1920x1080):30p/25p
    kNkMAIDMovieScreenSize5_FullHD_4,         // FullHD (1920x1080):24p/24p
    kNkMAIDMovieScreenSize5_720p,             // 720p (1280x720):60p/50p
    kNkMAIDMovieScreenSize5_VGA               // VGA  (640x424) :30p/25p
};

// kNkMAIDCapability_MovieScreenSize(0x8272)
enum eNkMAIDMovieScreenSize6
{
    kNkMAIDMovieScreenSize6_FullHD_60p,       // FullHD (1920x1080):60p
    kNkMAIDMovieScreenSize6_FullHD_50p,       // FullHD (1920x1080):50p
    kNkMAIDMovieScreenSize6_FullHD_30p,       // FullHD (1920x1080):30p
    kNkMAIDMovieScreenSize6_FullHD_25p,       // FullHD (1920x1080):25p
    kNkMAIDMovieScreenSize6_FullHD_24p,       // FullHD (1920x1080):24p
    kNkMAIDMovieScreenSize6_720p_60p,         // 720p (1280x720):60p
    kNkMAIDMovieScreenSize6_720p_50p,         // 720p (1280x720):50p
    kNkMAIDMovieScreenSize6_VGA_30p,          // VGA  (640x424) :30p
    kNkMAIDMovieScreenSize6_VGA_25p,          // VGA  (640x424) :25p
    kNkMAIDMovieScreenSize6_FullHD_30p_Crop,  // FullHD (1920x1080):30p Crop
    kNkMAIDMovieScreenSize6_FullHD_25p_Crop,  // FullHD (1920x1080):25p Crop
    kNkMAIDMovieScreenSize6_FullHD_24p_Crop   // FullHD (1920x1080):24p Crop
};

// kNkMAIDCapability_MovieScreenSize(0x8272)
enum eNkMAIDMovieScreenSize7
{
    kNkMAIDMovieScreenSize7_FullHD_60p,
    kNkMAIDMovieScreenSize7_FullHD_30p,
    kNkMAIDMovieScreenSize7_720p_60p,
    kNkMAIDMovieScreenSize7_720p_30p,
};

// kNkMAIDCapability_MovieScreenSize(0x8272)
enum eNkMAIDMovieScreenSize8
{
    kNkMAIDMovieScreenSize8_QFHD_30p = 0,
    kNkMAIDMovieScreenSize8_QFHD_25p,
    kNkMAIDMovieScreenSize8_QFHD_24p,
    kNkMAIDMovieScreenSize8_FullHD_60p,
    kNkMAIDMovieScreenSize8_FullHD_50p,
    kNkMAIDMovieScreenSize8_FullHD_30p,
    kNkMAIDMovieScreenSize8_FullHD_25p,
    kNkMAIDMovieScreenSize8_FullHD_24p,
    kNkMAIDMovieScreenSize8_820p_60p,
    kNkMAIDMovieScreenSize8_820p_50p,
    kNkMAIDMovieScreenSize8_FullHD_60p_Crop,
    kNkMAIDMovieScreenSize8_FullHD_50p_Crop,
    kNkMAIDMovieScreenSize8_FullHD_30p_Crop,
    kNkMAIDMovieScreenSize8_FullHD_25p_Crop,
	kNkMAIDMovieScreenSize8_FullHD_24p_Crop,
	kNkMAIDMovieScreenSize8_FullHD_30p_x4_Slow,
	kNkMAIDMovieScreenSize8_FullHD_25p_x4_Slow,
	kNkMAIDMovieScreenSize8_FullHD_24p_x5_Slow,
    kNkMAIDMovieScreenSize8_FullHD_120p,
    kNkMAIDMovieScreenSize8_FullHD_100p,
    kNkMAIDMovieScreenSize8_720p_60p,
    kNkMAIDMovieScreenSize8_720p_50p,
    kNkMAIDMovieScreenSize8_QFHD_60p,
    kNkMAIDMovieScreenSize8_QFHD_50p
};

// kNkMAIDCapability_MovieVoice(0x8273)
enum eNkMAIDMovieVoice
{
    kNkMAIDMovieVoice_Off = 0,  //Off
    kNkMAIDMovieVoice_On        //On
};

// kNkMAIDCapability_LiveViewAF(0x8275)
enum eNkMAIDLiveViewAF
{
    kNkMAIDLiveViewAF_Face = 0,         //Face detection system AF
    kNkMAIDLiveViewAF_Wide,             //Wide area AF
    kNkMAIDLiveViewAF_Normal,           //Normal area AF
    kNkMAIDLiveViewAF_SubjectTracking,  //Target tracking AF
	kNkMAIDLiveViewAF_Spot				//Spot area AF
};

// kNkMAIDCapability_SelfTimerShootNum(0x8276)
enum eNkMAIDSelfTimerShootNum
{
    kNkMAIDSelfTimerShootNum_1 = 0,
    kNkMAIDSelfTimerShootNum_2,
    kNkMAIDSelfTimerShootNum_3,
    kNkMAIDSelfTimerShootNum_4,
    kNkMAIDSelfTimerShootNum_5,
    kNkMAIDSelfTimerShootNum_6,
    kNkMAIDSelfTimerShootNum_7,
    kNkMAIDSelfTimerShootNum_8,
    kNkMAIDSelfTimerShootNum_9
};

// kNkMAIDCapability_FinderISODisplay(0x8277)
enum eNkMAIDFinderISODisplay
{
    kNkMAIDFinderISODisplay_ISO = 0,    //Sensitivity display ON
    kNkMAIDFinderISODisplay_EasyISO,    //Sensitivity display/easy setting ON
    kNkMAIDFinderISODisplay_FrameCount  //Off
};

// kNkMAIDCapability_ExchangeDialsEx(0x8279)
enum eNkMAIDExchangeDialsEx
{
    kNkMAIDExchangeDialsEx_Off = 0,
    kNkMAIDExchangeDialsEx_On,
    kNkMAIDExchangeDialsEx_On_Amode
};

// kNkMAIDCapability_AutoDistortion(0x8282)
enum eNkMAIDAutoDistortion
{
    kNkMAIDAutoDistortion_Off = 0,
    kNkMAIDAutoDistortion_On
};

// kNkMAIDCapability_SceneMode(0x8283)
enum eNkMAIDSceneMode
{
    kNkMAIDSceneMode_NightLandscape = 0,    // 0: Night landscape
    kNkMAIDSceneMode_PartyIndoor,           // 1: Party/indoor
    kNkMAIDSceneMode_BeachSnow,             // 2: Beach/snow
    kNkMAIDSceneMode_Sunset,                // 3: Sunset
    kNkMAIDSceneMode_Duskdawn,              // 4: Dusk/dawn
    kNkMAIDSceneMode_Petportrait,           // 5: Pet portrait
    kNkMAIDSceneMode_Candlelight,           // 6: Candlelight
    kNkMAIDSceneMode_Blossom,               // 7: Blossom
    kNkMAIDSceneMode_AutumnColors,          // 8: Autumn colors
    kNkMAIDSceneMode_Food,                  // 9: Food
    kNkMAIDSceneMode_Silhouette,            //10: Silhouette
    kNkMAIDSceneMode_Highkey,               //11: High key
    kNkMAIDSceneMode_Lowkey,                //12: Low key
    kNkMAIDSceneMode_Portrait,              //13: Portrait
    kNkMAIDSceneMode_Landscape,             //14: Landscape
    kNkMAIDSceneMode_Child,                 //15: Child
    kNkMAIDSceneMode_Sports,                //16: Sports
    kNkMAIDSceneMode_Closeup,               //17: Close up
    kNkMAIDSceneMode_NightPortrait          //18: Night portrait
};

// kNkMAIDCapability_UserMode1(0x8307)
// kNkMAIDCapability_UserMode2(0x8308)
// kNkMAIDCapability_UserMode3(0x850d)
enum eNkMAIDUserMode
{
    kNkMAIDUserMode_NightLandscape = 0,     // 0: Night landscape
    kNkMAIDUserMode_PartyIndoor,            // 1: Party/indoor
    kNkMAIDUserMode_BeachSnow,              // 2: Beach/snow
    kNkMAIDUserMode_Sunset,                 // 3: Sunset
    kNkMAIDUserMode_Duskdawn,               // 4: Dusk/dawn
    kNkMAIDUserMode_Petportrait,            // 5: Pet portrait
    kNkMAIDUserMode_Candlelight,            // 6: Candlelight
    kNkMAIDUserMode_Blossom,                // 7: Blossom
    kNkMAIDUserMode_AutumnColors,           // 8: Autumn colors
    kNkMAIDUserMode_Food,                   // 9: Food
    kNkMAIDUserMode_Silhouette,             //10: Silhouette
    kNkMAIDUserMode_Highkey,                //11: High key
    kNkMAIDUserMode_Lowkey,                 //12: Low key
    kNkMAIDUserMode_Portrait,               //13: Portrait
    kNkMAIDUserMode_Landscape,              //14: Landscape
    kNkMAIDUserMode_Child,                  //15: Child
    kNkMAIDUserMode_Sports,                 //16: Sports
    kNkMAIDUserMode_Closeup,                //17: Close up
    kNkMAIDUserMode_NightPortrait,          //18: Night portrait
    kNkMAIDUserMode_Program,                //19: Program auto
    kNkMAIDUserMode_SpeedPriority,          //20: Shutter speed priority
    kNkMAIDUserMode_AperturePriority,       //21: Aperture priority
    kNkMAIDUserMode_Manual,                 //22: Manual
    kNkMAIDUserMode_Auto,                   //23: Auto
    kNkMAIDUserMode_FlashOff,               //24: Flash prohibition Auto
    kNkMAIDUserMode_NightVision,            //25: Night vision
    kNkMAIDUserMode_ColorSketch,            //26: Color sketch
    kNkMAIDUserMode_Miniature,              //27: Miniature effect
    kNkMAIDUserMode_SelectColor,            //28: Selective color
    kNkMAIDUserMode_ToyCamera,              //29: Toy camera
    kNkMAIDUserMode_SuperVivid,             //30: Super vivid
    kNkMAIDUserMode_Pop,                    //31: Pop 
    kNkMAIDUserMode_PhotoIllustration       //32: Photo Illustration
};

// kNkMAIDCapability_EffectMode(0x8319)
enum eNkMAIDEffectMode
{
    kNkMAIDEffectMode_NightVision = 0,      // 0: Night vision
    kNkMAIDEffectMode_ColorSketch,          // 1: Color sketch
    kNkMAIDEffectMode_Miniature,            // 2: Miniature effect
    kNkMAIDEffectMode_SelectColor,          // 3: Selective color
    kNkMAIDEffectMode_Silhouette,           // 4: Silhouette
    kNkMAIDEffectMode_Highkey,              // 5: High key
    kNkMAIDEffectMode_Lowkey,               // 6: Low key
    kNkMAIDEffectMode_ToyCamera,            // 7: Toy camera
    kNkMAIDEffectMode_HDRPainting,          // 8: HDR painting
    kNkMAIDEffectMode_SuperVivid,           // 9: Super vivid
    kNkMAIDEffectMode_Pop,                  //10: Pop 
    kNkMAIDEffectMode_PhotoIllustration     //11: Photo Illustration
};

//kNkMAIDCapability_LiveViewScreenDispSetting(0x8284)
enum eNkMAIDLiveViewScreenDispSetting
{
    kNkMAIDLiveViewScreenDispSetting_ShowShootingInfo = 0x00000008,
    kNkMAIDLiveViewScreenDispSetting_FramingGrid      = 0x00000004,
    kNkMAIDLiveViewScreenDispSetting_HideIndicators   = 0x00000002,
    kNkMAIDLiveViewScreenDispSetting_ShowIndicators   = 0x00000001,
    kNkMAIDLiveViewScreenDispSetting_ALL              = 0x0000000F
};

// kNkMAIDCapability_MovieRecMicrophone(0x8285)
enum eNkMAIDMovRecMicrophone
{
    kNkMAIDMovRecMicrophone_Auto = 0,
    kNkMAIDMovRecMicrophone_High,
    kNkMAIDMovRecMicrophone_Medium,
    kNkMAIDMovRecMicrophone_Low,
    kNkMAIDMovRecMicrophone_Off,
    kNkMAIDMovRecMicrophone_Manual
};

// kNkMAIDCapability_MovieRecDestination(0x8286)
enum eNkMAIDMovRecDestination
{
    kNkMAIDMovRecDestination_CF = 0,
    kNkMAIDMovRecDestination_SD,
    kNkMAIDMovRecDestination_XQD
};

// kNkMAIDCapability_MovieRecDestination(0x8286)
enum eNkMAIDMovRecDestination2
{
    kNkMAIDMovRecDestination2_Slot1 = 0,
    kNkMAIDMovRecDestination2_Slot2
};

// kNkMAIDCapability_MovieManualSetting(0x8309)
enum eNkMAIDMovManualSetting
{
    kNkMAIDMovManualSetting_OFF = 0,
    kNkMAIDMovManualSetting_ON
};

// kNkMAIDCapability_PrimarySlot(0x8287)
enum eNkMAIDPrimarySlot
{
    kNkMAIDPrimarySlot_CF = 0,
    kNkMAIDPrimarySlot_SD,
    kNkMAIDPrimarySlot_XQD
};

// kNkMAIDCapability_PrimarySlot(0x8287)
enum eNkMAIDPrimarySlot2
{
    kNkMAIDPrimarySlot2_Slot1 = 0,
    kNkMAIDPrimarySlot2_Slot2
};

// kNkMAIDCapability_ADLBracketingType(0x8288)
enum eNkMAIDADLBracketingType
{
    kNkMAIDADLBracketingType_2 = 0,             // 2: none -> user
    kNkMAIDADLBracketingType_3,                 // 3: none -> week   -> normal 
    kNkMAIDADLBracketingType_4,                 // 4: none -> week   -> normal -> strong
    kNkMAIDADLBracketingType_5,                 // 5: none -> week   -> normal -> strong      -> morestrong(1)
    kNkMAIDADLBracketingType_5_Max_ExHi2,       // 5: week -> normal -> strong -> morestrong1 -> morestrong2
    kNkMAIDADLBracketingType_0,                 // 0: -
};

// kNkMAIDCapability_MovieRecHiISO(0x8306)
enum eNkMAIDMovRecHiISO
{
    kNkMAIDMovRecHiISO_Off = 0,
    kNkMAIDMovRecHiISO_On
};

// kNkMAIDCapability_WBPresetProtect1(0x8326)
enum eNkMAIDWBPresetProtect1
{
    kNkMAIDWBPresetProtect1_Off = 0,
    kNkMAIDWBPresetProtect1_On
};

// kNkMAIDCapability_WBPresetProtect2(0x8327)
enum eNkMAIDWBPresetProtect2
{
    kNkMAIDWBPresetProtect2_Off = 0,
    kNkMAIDWBPresetProtect2_On
};

// kNkMAIDCapability_WBPresetProtect3(0x8328)
enum eNkMAIDWBPresetProtect3
{
    kNkMAIDWBPresetProtect3_Off = 0,
    kNkMAIDWBPresetProtect3_On
};

// kNkMAIDCapability_WBPresetProtect4(0x8329)
enum eNkMAIDWBPresetProtect4
{
    kNkMAIDWBPresetProtect4_Off = 0,
    kNkMAIDWBPresetProtect4_On
};

// kNkMAIDCapability_WBPresetProtect5(0x8356)
enum eNkMAIDWBPresetProtect5
{
    kNkMAIDWBPresetProtect5_Off = 0,
    kNkMAIDWBPresetProtect5_On
};

// kNkMAIDCapability_WBPresetProtect6(0x8357)
enum eNkMAIDWBPresetProtect6
{
    kNkMAIDWBPresetProtect6_Off = 0,
    kNkMAIDWBPresetProtect6_On
};

// kNkMAIDCapability_MovieImageQuality(0x8331)
enum eNkMAIDMovieImageQuality
{
    kNkMAIDMovieImageQuality_Normal = 0,
    kNkMAIDMovieImageQuality_Fine
};

// kNkMAIDCapability_MovieISORange(0x8348)
enum eNkMAIDMovieISORange
{
    kNkMAIDMovieISORange_200to12800 = 0,
    kNkMAIDMovieISORange_200toHi40
};

// kNkMAIDCapability_SelectFUNC2CapAreaCrop(0x8291)
// kNkMAIDCapability_PreviewButton2CapAreaCrop(0x8292)
// kNkMAIDCapability_AEAFLockButton2CapAreaCrop(0x8293)
enum eNkMAIDCapAreaCrop
{
    kNkMAIDCapAreaCrop_12x = 0x00000008,
    kNkMAIDCapAreaCrop_DX  = 0x00000002,
    kNkMAIDCapAreaCrop_54  = 0x00000004,
    kNkMAIDCapAreaCrop_FX  = 0x00000001
};

// kNkMAIDCapability_BracketButton(0x8294)
enum eNkMAIDBracketButton
{
    kNkMAIDBracketButton_AutoBracketing = 0,
    kNkMAIDBracketButton_MultipleExposur
};

// kNkMAIDCapability_MovRecInCardStatus(0x8298)
enum eNkMAIDMovRecInCardStatus
{
    kNkMAIDMovRecInCardStatus_Off = 0,
    kNkMAIDMovRecInCardStatus_On
};

// kNkMAIDCapability_MovRecInCardProhibit(0x8299)
enum eNkMAIDMovRecInCardProhibit
{
    kNkMAIDMovRecInCardProhibit_ErrorByLens               = 0x00080000,//Bit:19
    kNkMAIDMovRecInCardProhibit_SequenceError             = 0x00040000,//Bit:18
    kNkMAIDMovRecInCardProhibit_HighLoad                  = 0x00020000,//Bit:17
    kNkMAIDMovRecInCardProhibit_WaitingExtRecWithoutCard  = 0x00010000,//Bit:16
    kNkMAIDMovRecInCardProhibit_MovieLogOutput            = 0x00008000,//Bit:15
    kNkMAIDMovRecInCardProhibit_LVPhoto                   = 0x00002000,//Bit:13
    kNkMAIDMovRecInCardProhibit_LVImageZoom               = 0x00001000,//Bit:12
    kNkMAIDMovRecInCardProhibit_CardProtect               = 0x00000800,//Bit:11
    kNkMAIDMovRecInCardProhibit_RecMov                    = 0x00000400,//Bit:10
    kNkMAIDMovRecInCardProhibit_MovInBuf                  = 0x00000200,//Bit:9
    kNkMAIDMovRecInCardProhibit_PCInBuf                   = 0x00000100,//Bit:8
    kNkMAIDMovRecInCardProhibit_CardInBuf                 = 0x00000080,//Bit:7
    kNkMAIDMovRecInCardProhibit_CardFull                  = 0x00000008,//Bit:3
    kNkMAIDMovRecInCardProhibit_NoFormat                  = 0x00000004,//Bit:2
    kNkMAIDMovRecInCardProhibit_CardErr                   = 0x00000002,//Bit:1
    kNkMAIDMovRecInCardProhibit_NoCard                    = 0x00000001 //Bit:0
};

// kNkMAIDCapability_ActiveSlot(0x8304)
enum eNkMAIDActiveSlot
{
    kNkMAIDActiveSlot_NoCard = 0,
    kNkMAIDActiveSlot_Slot1,
    kNkMAIDActiveSlot_Slot2,
    kNkMAIDActiveSlot_Slot1_Slot2
};

// kNkMAIDCapability_SaveMedia(0x8305)
enum eNkMAIDSaveMedia
{
    kNkMAIDSaveMedia_Card = 0,
    kNkMAIDSaveMedia_SDRAM,
    kNkMAIDSaveMedia_Card_SDRAM
};

// kNkMAIDCapability_VideoMode(0x8315)
enum eNkMAIDVideoMode
{
    kNkMAIDVideoMode_NTSC = 0,
    kNkMAIDVideoMode_PAL
};

// kNkMAIDCapability_HDRMode(0x8320)
enum eNkMAIDHDRMode
{
    kNkMAIDHDRMode_Off = 0,     // Off
    kNkMAIDHDRMode_On,          // On (single photo)
    kNkMAIDHDRMode_Continuous   // On (series)
};

// kNkMAIDCapability_HDRMode(0x8320)
enum eNkMAIDHDRMode2
{
    kNkMAIDHDRMode2_Off = 0,    // Off
    kNkMAIDHDRMode2_Low,        // Low
    kNkMAIDHDRMode2_Normal,     // Normal
    kNkMAIDHDRMode2_High,       // High
    kNkMAIDHDRMode2_ExtraHigh,  // Extra high
    kNkMAIDHDRMode2_Auto        // Auto
};

// kNkMAIDCapability_HDRExposure(0x8321)
enum eNkMAIDHDRExposure
{
    kNkMAIDHDRExposure_Auto = 0,
    kNkMAIDHDRExposure_1EV,
    kNkMAIDHDRExposure_2EV,
    kNkMAIDHDRExposure_3EV
};

// kNkMAIDCapability_HDRSmoothing(0x8322)
enum eNkMAIDHDRSmoothing
{
    kNkMAIDHDRSmoothing_High = 0,   // High
    kNkMAIDHDRSmoothing_Normal,     // Normal
    kNkMAIDHDRSmoothing_Low,        // Low
    kNkMAIDHDRSmoothing_Auto,       // Auto
    kNkMAIDHDRSmoothing_ExtraHigh   // Extra high
};

// kNkMAIDCapability_SpotWBMode(0x8358)
enum eNkMAIDSpotWBMode
{
    kNkMAIDSpotWBMode_OFF = 0,
    kNkMAIDSpotWBMode_ON
};

// kNkMAIDCapability_BlinkingStatus(0x8323)
enum eNkMAIDBlinkingStatus
{
    kNkMAIDBlinkingStatus_None = 0,
    kNkMAIDBlinkingStatus_Sp_Blink,
    kNkMAIDBlinkingStatus_Ap_Blink,
    kNkMAIDBlinkingStatus_Both
};

// kNkMAIDCapability_LiveViewExposurePreview(0x8333)
enum eNkMAIDLiveViewExposurePreview
{
    kNkMAIDLiveViewExposurePreview_Off = 0,
    kNkMAIDLiveViewExposurePreview_On
};

// kNkMAIDCapability_MovieReleaseButton(0x8349)
enum eNkMAIDMovieReleaseButton
{
    kNkMAIDMovieReleaseButton_Photo = 0,    //Take photos
    kNkMAIDMovieReleaseButton_Movie,        //Record movies
    kNkMAIDMovieReleaseButton_SaveFrame     //Live frame grab
};

// kNkMAIDCapability_LiveViewSelector(0x8334)
enum eNkMAIDLiveViewSelector
{
    kNkMAIDLiveViewSelector_Photo = 0,
    kNkMAIDLiveViewSelector_Movie
};

// kNkMAIDCapability_LiveViewPhotoShootingMode(0x8346)
enum eNkMAIDLiveViewPhotoShootingMode
{
    kNkMAIDLiveViewPhotoShootingMode_Quiet  = 0,    //Quiet shooting(Off)
    kNkMAIDLiveViewPhotoShootingMode_Silent = 1,    //Silent shooting(On)
};

// kNkMAIDCapability_LiveViewPhotoShootingMode(0x8346)
enum eNkMAIDLiveViewPhotoShootingMode2
{
	kNkMAIDLiveViewPhotoShootingMode2_OFF = 0,    // OFF
	kNkMAIDLiveViewPhotoShootingMode2_Mode1 = 1,  // Mode1
	kNkMAIDLiveViewPhotoShootingMode2_Mode2 = 2   // Mode2

};

// kNkMAIDCapability_ADLBracketingStep(0x8340)
enum eNkMAIDADLBracketingStep
{
    kNkMAIDADLBracketingStep_Auto        = 0, //AUTO
    kNkMAIDADLBracketingStep_Low         = 1, //Low
    kNkMAIDADLBracketingStep_Normal      = 2, //Normal
    kNkMAIDADLBracketingStep_High        = 3, //High
    kNkMAIDADLBracketingStep_ExtraHigh_1 = 4, //Extra high(Extra high 1)
    kNkMAIDADLBracketingStep_ExtraHigh_2 = 5  //Extra high 2
};

// kNkMAIDCapability_ExposureDelayEx(0x8347)
enum eNkMAIDExposureDelayEx
{
    kNkMAIDExposureDelayEx_3sec = 0,
    kNkMAIDExposureDelayEx_2sec,
    kNkMAIDExposureDelayEx_1sec,
    kNkMAIDExposureDelayEx_Off,
	kNkMAIDExposureDelayEx_05sec,
	kNkMAIDExposureDelayEx_02sec
};

// kNkMAIDCapability_GetIPTCInfo(0x8352)
enum eNkMAIDGetIPTCInfo
{
    kNkMAIDGetIPTCInfo_None     = 0,
    kNkMAIDGetIPTCInfo_Attached
};

// kNkMAIDCapability_LiveViewImageSize(0x8353)
enum eNkMAIDLiveViewImageSize
{
    kNkMAIDLiveViewImageSize_QVGA = 1,
    kNkMAIDLiveViewImageSize_VGA,
    kNkMAIDLiveViewImageSize_XGA
};

// kNkMAIDCapability_RemoteControlMode(0x8354)
enum eNkMAIDRemoteControlMode
{
    kNkMAIDRemoteControlMode_Delayed = 0,
    kNkMAIDRemoteControlMode_QuickResponse,
    kNkMAIDRemoteControlMode_MirrorUp,
    kNkMAIDRemoteControlMode_Off
};

// kNkMAIDMovieWindNoiseReduction(0x8420)
enum eNkMAIDMovieWindNoiseReduction
{
    kNkMAIDMovieWindNoiseReduction_OFF = 0,
    kNkMAIDMovieWindNoiseReduction_ON
};

// kNkMAIDISOControlSensitivity(0x8422)
enum eNkMAIDISOControlSensitivity
{
    kNkMAIDISOControlSensitivity_Lo10 = 50,
    kNkMAIDISOControlSensitivity_Lo08 = 56,
    kNkMAIDISOControlSensitivity_Lo07 = 64,
    kNkMAIDISOControlSensitivity_Lo05 = 72,
    kNkMAIDISOControlSensitivity_Lo03 = 80,
    kNkMAIDISOControlSensitivity_Lo02 = 90,
    kNkMAIDISOControlSensitivity_ISO100 = 100,
    kNkMAIDISOControlSensitivity_ISO110 = 110,
    kNkMAIDISOControlSensitivity_ISO125 = 125,
    kNkMAIDISOControlSensitivity_ISO140 = 140,
    kNkMAIDISOControlSensitivity_ISO160 = 160,
    kNkMAIDISOControlSensitivity_ISO180 = 180,
    kNkMAIDISOControlSensitivity_ISO200 = 200,
    kNkMAIDISOControlSensitivity_ISO220 = 220,
    kNkMAIDISOControlSensitivity_ISO250 = 250,
    kNkMAIDISOControlSensitivity_ISO280 = 280,
    kNkMAIDISOControlSensitivity_ISO320 = 320,
    kNkMAIDISOControlSensitivity_ISO360 = 360,
    kNkMAIDISOControlSensitivity_ISO400 = 400,
    kNkMAIDISOControlSensitivity_ISO450 = 450,
    kNkMAIDISOControlSensitivity_ISO500 = 500,
    kNkMAIDISOControlSensitivity_ISO560 = 560,
    kNkMAIDISOControlSensitivity_ISO640 = 640,
    kNkMAIDISOControlSensitivity_ISO720 = 720,
    kNkMAIDISOControlSensitivity_ISO800 = 800,
    kNkMAIDISOControlSensitivity_ISO900 = 900,
    kNkMAIDISOControlSensitivity_ISO1000 = 1000,
    kNkMAIDISOControlSensitivity_ISO1100 = 1100,
    kNkMAIDISOControlSensitivity_ISO1250 = 1250,
    kNkMAIDISOControlSensitivity_ISO1400 = 1400,
    kNkMAIDISOControlSensitivity_ISO1600 = 1600,
    kNkMAIDISOControlSensitivity_ISO1800 = 1800,
    kNkMAIDISOControlSensitivity_ISO2000 = 2000,
    kNkMAIDISOControlSensitivity_ISO2200 = 2200,
    kNkMAIDISOControlSensitivity_ISO2500 = 2500,
    kNkMAIDISOControlSensitivity_ISO2800 = 2800,
    kNkMAIDISOControlSensitivity_ISO3200 = 3200,
    kNkMAIDISOControlSensitivity_ISO3600 = 3600,
    kNkMAIDISOControlSensitivity_ISO4000 = 4000,
    kNkMAIDISOControlSensitivity_ISO4500 = 4500,
    kNkMAIDISOControlSensitivity_ISO5000 = 5000,
    kNkMAIDISOControlSensitivity_ISO5600 = 5600,
    kNkMAIDISOControlSensitivity_ISO6400 = 6400,
    kNkMAIDISOControlSensitivity_ISO7200 = 7200,
    kNkMAIDISOControlSensitivity_ISO8000 = 8000,
    kNkMAIDISOControlSensitivity_ISO9000 = 9000,
    kNkMAIDISOControlSensitivity_ISO10000 = 10000,
    kNkMAIDISOControlSensitivity_ISO11000 = 11000,
    kNkMAIDISOControlSensitivity_ISO12800 = 12800,
    kNkMAIDISOControlSensitivity_Hi02 = 14400,
    kNkMAIDISOControlSensitivity_Hi03 = 16000,
    kNkMAIDISOControlSensitivity_Hi05 = 18000,
    kNkMAIDISOControlSensitivity_Hi07 = 20000,
    kNkMAIDISOControlSensitivity_Hi08 = 22000,
    kNkMAIDISOControlSensitivity_Hi10 = 25600,
    kNkMAIDISOControlSensitivity_Hi12 = 28800,
    kNkMAIDISOControlSensitivity_Hi13 = 32000,
    kNkMAIDISOControlSensitivity_Hi15 = 36000,
    kNkMAIDISOControlSensitivity_Hi17 = 40000,
    kNkMAIDISOControlSensitivity_Hi18 = 45600,
    kNkMAIDISOControlSensitivity_Hi20 = 51200,
    kNkMAIDISOControlSensitivity_Hi22 = 57600,
    kNkMAIDISOControlSensitivity_Hi23 = 64000,
    kNkMAIDISOControlSensitivity_Hi25 = 72000,
    kNkMAIDISOControlSensitivity_Hi27 = 81200,
    kNkMAIDISOControlSensitivity_Hi28 = 91200,
    kNkMAIDISOControlSensitivity_Hi30 = 102400,
    kNkMAIDISOControlSensitivity_Hi32 = 115000,
    kNkMAIDISOControlSensitivity_Hi33 = 128000,
    kNkMAIDISOControlSensitivity_Hi35 = 144000,
    kNkMAIDISOControlSensitivity_Hi37 = 162000,
    kNkMAIDISOControlSensitivity_Hi38 = 182000,
    kNkMAIDISOControlSensitivity_Hi40 = 204800,
};

// kNkMAIDISOControlSensitivity(0x8422)
enum eNkMAIDISOControlSensitivity2
{
    kNkMAIDISOControlSensitivity2_Lo10 = 50,
    kNkMAIDISOControlSensitivity2_Lo08 = 56,
    kNkMAIDISOControlSensitivity2_Lo07 = 64,
    kNkMAIDISOControlSensitivity2_Lo05 = 72,
    kNkMAIDISOControlSensitivity2_Lo03 = 80,
    kNkMAIDISOControlSensitivity2_Lo02 = 90,
    kNkMAIDISOControlSensitivity2_ISO100 = 100,
    kNkMAIDISOControlSensitivity2_ISO110 = 110,
    kNkMAIDISOControlSensitivity2_ISO125 = 125,
    kNkMAIDISOControlSensitivity2_ISO140 = 140,
    kNkMAIDISOControlSensitivity2_ISO160 = 160,
    kNkMAIDISOControlSensitivity2_ISO180 = 180,
    kNkMAIDISOControlSensitivity2_ISO200 = 200,
    kNkMAIDISOControlSensitivity2_ISO220 = 220,
    kNkMAIDISOControlSensitivity2_ISO250 = 250,
    kNkMAIDISOControlSensitivity2_ISO280 = 280,
    kNkMAIDISOControlSensitivity2_ISO320 = 320,
    kNkMAIDISOControlSensitivity2_ISO360 = 360,
    kNkMAIDISOControlSensitivity2_ISO400 = 400,
    kNkMAIDISOControlSensitivity2_ISO450 = 450,
    kNkMAIDISOControlSensitivity2_ISO500 = 500,
    kNkMAIDISOControlSensitivity2_ISO560 = 560,
    kNkMAIDISOControlSensitivity2_ISO640 = 640,
    kNkMAIDISOControlSensitivity2_ISO720 = 720,
    kNkMAIDISOControlSensitivity2_ISO800 = 800,
    kNkMAIDISOControlSensitivity2_ISO900 = 900,
    kNkMAIDISOControlSensitivity2_ISO1000 = 1000,
    kNkMAIDISOControlSensitivity2_ISO1100 = 1100,
    kNkMAIDISOControlSensitivity2_ISO1250 = 1250,
    kNkMAIDISOControlSensitivity2_ISO1400 = 1400,
    kNkMAIDISOControlSensitivity2_ISO1600 = 1600,
    kNkMAIDISOControlSensitivity2_ISO1800 = 1800,
    kNkMAIDISOControlSensitivity2_ISO2000 = 2000,
    kNkMAIDISOControlSensitivity2_ISO2200 = 2200,
    kNkMAIDISOControlSensitivity2_ISO2500 = 2500,
    kNkMAIDISOControlSensitivity2_ISO2800 = 2800,
    kNkMAIDISOControlSensitivity2_ISO3200 = 3200,
    kNkMAIDISOControlSensitivity2_ISO3600 = 3600,
    kNkMAIDISOControlSensitivity2_ISO4000 = 4000,
    kNkMAIDISOControlSensitivity2_ISO4500 = 4500,
    kNkMAIDISOControlSensitivity2_ISO5000 = 5000,
    kNkMAIDISOControlSensitivity2_ISO5600 = 5600,
    kNkMAIDISOControlSensitivity2_ISO6400 = 6400,
    kNkMAIDISOControlSensitivity2_ISO7200 = 7200,
    kNkMAIDISOControlSensitivity2_ISO8000 = 8000,
    kNkMAIDISOControlSensitivity2_ISO9000 = 9000,
    kNkMAIDISOControlSensitivity2_ISO10000 = 10000,
    kNkMAIDISOControlSensitivity2_ISO11000 = 11000,
    kNkMAIDISOControlSensitivity2_ISO12800 = 12800,
    kNkMAIDISOControlSensitivity2_ISO14400 = 14400,
    kNkMAIDISOControlSensitivity2_ISO16000 = 16000,
    kNkMAIDISOControlSensitivity2_ISO18000 = 18000,
    kNkMAIDISOControlSensitivity2_ISO20000 = 20000,
    kNkMAIDISOControlSensitivity2_ISO22000 = 22000,
    kNkMAIDISOControlSensitivity2_ISO25600 = 25600,
    kNkMAIDISOControlSensitivity2_Hi02 = 28800,
    kNkMAIDISOControlSensitivity2_Hi03 = 32000,
    kNkMAIDISOControlSensitivity2_Hi05 = 36000,
    kNkMAIDISOControlSensitivity2_Hi07 = 40000,
    kNkMAIDISOControlSensitivity2_Hi08 = 45600,
    kNkMAIDISOControlSensitivity2_Hi10 = 51200,
    kNkMAIDISOControlSensitivity2_Hi12 = 57600,
    kNkMAIDISOControlSensitivity2_Hi13 = 64000,
    kNkMAIDISOControlSensitivity2_Hi15 = 72000,
    kNkMAIDISOControlSensitivity2_Hi17 = 81200,
    kNkMAIDISOControlSensitivity2_Hi18 = 91200,
    kNkMAIDISOControlSensitivity2_Hi20 = 102400,
    kNkMAIDISOControlSensitivity2_Hi22 = 115000,
    kNkMAIDISOControlSensitivity2_Hi23 = 128000,
    kNkMAIDISOControlSensitivity2_Hi25 = 144000,
    kNkMAIDISOControlSensitivity2_Hi27 = 162000,
    kNkMAIDISOControlSensitivity2_Hi28 = 182000,
    kNkMAIDISOControlSensitivity2_Hi30 = 204800,
    kNkMAIDISOControlSensitivity2_Hi32 = 230000,
    kNkMAIDISOControlSensitivity2_Hi33 = 256000,
    kNkMAIDISOControlSensitivity2_Hi35 = 288000,
    kNkMAIDISOControlSensitivity2_Hi37 = 324000,
    kNkMAIDISOControlSensitivity2_Hi38 = 364000,
    kNkMAIDISOControlSensitivity2_Hi40 = 409600
};

// kNkMAIDISOControlSensitivity(0x8422)
enum eNkMAIDISOControlSensitivity3
{
    kNkMAIDISOControlSensitivity3_Lo10 = 32,
    kNkMAIDISOControlSensitivity3_Lo07 = 40,
    kNkMAIDISOControlSensitivity3_Lo05 = 45,
    kNkMAIDISOControlSensitivity3_Lo03 = 56,
    kNkMAIDISOControlSensitivity3_ISO64 = 64,
    kNkMAIDISOControlSensitivity3_ISO72 = 72,
    kNkMAIDISOControlSensitivity3_ISO80 = 80,
    kNkMAIDISOControlSensitivity3_ISO100 = 100,
    kNkMAIDISOControlSensitivity3_ISO110 = 110,
    kNkMAIDISOControlSensitivity3_ISO125 = 125,
    kNkMAIDISOControlSensitivity3_ISO140 = 140,
    kNkMAIDISOControlSensitivity3_ISO160 = 160,
    kNkMAIDISOControlSensitivity3_ISO180 = 180,
    kNkMAIDISOControlSensitivity3_ISO200 = 200,
    kNkMAIDISOControlSensitivity3_ISO220 = 220,
    kNkMAIDISOControlSensitivity3_ISO250 = 250,
    kNkMAIDISOControlSensitivity3_ISO280 = 280,
    kNkMAIDISOControlSensitivity3_ISO320 = 320,
    kNkMAIDISOControlSensitivity3_ISO360 = 360,
    kNkMAIDISOControlSensitivity3_ISO400 = 400,
    kNkMAIDISOControlSensitivity3_ISO450 = 450,
    kNkMAIDISOControlSensitivity3_ISO500 = 500,
    kNkMAIDISOControlSensitivity3_ISO560 = 560,
    kNkMAIDISOControlSensitivity3_ISO640 = 640,
    kNkMAIDISOControlSensitivity3_ISO720 = 720,
    kNkMAIDISOControlSensitivity3_ISO800 = 800,
    kNkMAIDISOControlSensitivity3_ISO900 = 900,
    kNkMAIDISOControlSensitivity3_ISO1000 = 1000,
    kNkMAIDISOControlSensitivity3_ISO1100 = 1100,
    kNkMAIDISOControlSensitivity3_ISO1250 = 1250,
    kNkMAIDISOControlSensitivity3_ISO1400 = 1400,
    kNkMAIDISOControlSensitivity3_ISO1600 = 1600,
    kNkMAIDISOControlSensitivity3_ISO1800 = 1800,
    kNkMAIDISOControlSensitivity3_ISO2000 = 2000,
    kNkMAIDISOControlSensitivity3_ISO2200 = 2200,
    kNkMAIDISOControlSensitivity3_ISO2500 = 2500,
    kNkMAIDISOControlSensitivity3_ISO2800 = 2800,
    kNkMAIDISOControlSensitivity3_ISO3200 = 3200,
    kNkMAIDISOControlSensitivity3_ISO3600 = 3600,
    kNkMAIDISOControlSensitivity3_ISO4000 = 4000,
    kNkMAIDISOControlSensitivity3_ISO4500 = 4500,
    kNkMAIDISOControlSensitivity3_ISO5000 = 5000,
    kNkMAIDISOControlSensitivity3_ISO5600 = 5600,
    kNkMAIDISOControlSensitivity3_ISO6400 = 6400,
    kNkMAIDISOControlSensitivity3_ISO7200 = 7200,
    kNkMAIDISOControlSensitivity3_ISO8000 = 8000,
    kNkMAIDISOControlSensitivity3_ISO9000 = 9000,
    kNkMAIDISOControlSensitivity3_ISO10000 = 10000,
    kNkMAIDISOControlSensitivity3_ISO11000 = 11000,
    kNkMAIDISOControlSensitivity3_ISO12800 = 12800,
    kNkMAIDISOControlSensitivity3_Hi02 = 14400,
    kNkMAIDISOControlSensitivity3_Hi03 = 16000,
    kNkMAIDISOControlSensitivity3_Hi05 = 18000,
    kNkMAIDISOControlSensitivity3_Hi07 = 20000,
    kNkMAIDISOControlSensitivity3_Hi08 = 22000,
    kNkMAIDISOControlSensitivity3_Hi10 = 25600,
    kNkMAIDISOControlSensitivity3_Hi12 = 28800,
    kNkMAIDISOControlSensitivity3_Hi13 = 32000,
    kNkMAIDISOControlSensitivity3_Hi15 = 36000,
    kNkMAIDISOControlSensitivity3_Hi17 = 40000,
    kNkMAIDISOControlSensitivity3_Hi18 = 45600,
    kNkMAIDISOControlSensitivity3_Hi20 = 51200
};

// kNkMAIDISOControlSensitivity(0x8422)
enum eNkMAIDISOControlSensitivity4
{
    kNkMAIDISOControlSensitivity4_Lo10 = 100,
    kNkMAIDISOControlSensitivity4_Lo08 = 110,
    kNkMAIDISOControlSensitivity4_Lo07 = 125,
    kNkMAIDISOControlSensitivity4_Lo05 = 140,
    kNkMAIDISOControlSensitivity4_Lo03 = 160,
    kNkMAIDISOControlSensitivity4_Lo02 = 180,
    kNkMAIDISOControlSensitivity4_ISO200 = 200,
    kNkMAIDISOControlSensitivity4_ISO220 = 220,
    kNkMAIDISOControlSensitivity4_ISO250 = 250,
    kNkMAIDISOControlSensitivity4_ISO280 = 280,
    kNkMAIDISOControlSensitivity4_ISO320 = 320,
    kNkMAIDISOControlSensitivity4_ISO360 = 360,
    kNkMAIDISOControlSensitivity4_ISO400 = 400,
    kNkMAIDISOControlSensitivity4_ISO450 = 450,
    kNkMAIDISOControlSensitivity4_ISO500 = 500,
    kNkMAIDISOControlSensitivity4_ISO560 = 560,
    kNkMAIDISOControlSensitivity4_ISO640 = 640,
    kNkMAIDISOControlSensitivity4_ISO720 = 720,
    kNkMAIDISOControlSensitivity4_ISO800 = 800,
    kNkMAIDISOControlSensitivity4_ISO900 = 900,
    kNkMAIDISOControlSensitivity4_ISO1000 = 1000,
    kNkMAIDISOControlSensitivity4_ISO1100 = 1100,
    kNkMAIDISOControlSensitivity4_ISO1250 = 1250,
    kNkMAIDISOControlSensitivity4_ISO1400 = 1400,
    kNkMAIDISOControlSensitivity4_ISO1600 = 1600,
    kNkMAIDISOControlSensitivity4_ISO1800 = 1800,
    kNkMAIDISOControlSensitivity4_ISO2000 = 2000,
    kNkMAIDISOControlSensitivity4_ISO2200 = 2200,
    kNkMAIDISOControlSensitivity4_ISO2500 = 2500,
    kNkMAIDISOControlSensitivity4_ISO2800 = 2800,
    kNkMAIDISOControlSensitivity4_ISO3200 = 3200,
    kNkMAIDISOControlSensitivity4_ISO3600 = 3600,
    kNkMAIDISOControlSensitivity4_ISO4000 = 4000,
    kNkMAIDISOControlSensitivity4_ISO4500 = 4500,
    kNkMAIDISOControlSensitivity4_ISO5000 = 5000,
    kNkMAIDISOControlSensitivity4_ISO5600 = 5600,
    kNkMAIDISOControlSensitivity4_ISO6400 = 6400,
    kNkMAIDISOControlSensitivity4_ISO7200 = 7200,
    kNkMAIDISOControlSensitivity4_ISO8000 = 8000,
    kNkMAIDISOControlSensitivity4_ISO9000 = 9000,
    kNkMAIDISOControlSensitivity4_ISO10000 = 10000,
    kNkMAIDISOControlSensitivity4_ISO11000 = 11000,
    kNkMAIDISOControlSensitivity4_ISO12800 = 12800,
    kNkMAIDISOControlSensitivity4_Hi02 = 14400,
    kNkMAIDISOControlSensitivity4_Hi03 = 16000,
    kNkMAIDISOControlSensitivity4_Hi05 = 18000,
    kNkMAIDISOControlSensitivity4_Hi07 = 20000,
    kNkMAIDISOControlSensitivity4_Hi08 = 22000,
    kNkMAIDISOControlSensitivity4_Hi10 = 25600,
    kNkMAIDISOControlSensitivity4_Hi12 = 28800,
    kNkMAIDISOControlSensitivity4_Hi13 = 32000,
    kNkMAIDISOControlSensitivity4_Hi15 = 36000,
    kNkMAIDISOControlSensitivity4_Hi17 = 40000,
    kNkMAIDISOControlSensitivity4_Hi18 = 45600,
    kNkMAIDISOControlSensitivity4_Hi20 = 51200
};

// kNkMAIDISOControlSensitivity(0x8422)
enum eNkMAIDISOControlSensitivity5
{
    kNkMAIDISOControlSensitivity5_Lo10 = 50,
    kNkMAIDISOControlSensitivity5_Lo08 = 56,
    kNkMAIDISOControlSensitivity5_Lo07 = 64,
    kNkMAIDISOControlSensitivity5_Lo05 = 72,
    kNkMAIDISOControlSensitivity5_Lo03 = 80,
    kNkMAIDISOControlSensitivity5_Lo02 = 90,
    kNkMAIDISOControlSensitivity5_ISO100 = 100,
    kNkMAIDISOControlSensitivity5_ISO110 = 110,
    kNkMAIDISOControlSensitivity5_ISO125 = 125,
    kNkMAIDISOControlSensitivity5_ISO140 = 140,
    kNkMAIDISOControlSensitivity5_ISO160 = 160,
    kNkMAIDISOControlSensitivity5_ISO180 = 180,
    kNkMAIDISOControlSensitivity5_ISO200 = 200,
    kNkMAIDISOControlSensitivity5_ISO220 = 220,
    kNkMAIDISOControlSensitivity5_ISO250 = 250,
    kNkMAIDISOControlSensitivity5_ISO280 = 280,
    kNkMAIDISOControlSensitivity5_ISO320 = 320,
    kNkMAIDISOControlSensitivity5_ISO360 = 360,
    kNkMAIDISOControlSensitivity5_ISO400 = 400,
    kNkMAIDISOControlSensitivity5_ISO450 = 450,
    kNkMAIDISOControlSensitivity5_ISO500 = 500,
    kNkMAIDISOControlSensitivity5_ISO560 = 560,
    kNkMAIDISOControlSensitivity5_ISO640 = 640,
    kNkMAIDISOControlSensitivity5_ISO720 = 720,
    kNkMAIDISOControlSensitivity5_ISO800 = 800,
    kNkMAIDISOControlSensitivity5_ISO900 = 900,
    kNkMAIDISOControlSensitivity5_ISO1000 = 1000,
    kNkMAIDISOControlSensitivity5_ISO1100 = 1100,
    kNkMAIDISOControlSensitivity5_ISO1250 = 1250,
    kNkMAIDISOControlSensitivity5_ISO1400 = 1400,
    kNkMAIDISOControlSensitivity5_ISO1600 = 1600,
    kNkMAIDISOControlSensitivity5_ISO1800 = 1800,
    kNkMAIDISOControlSensitivity5_ISO2000 = 2000,
    kNkMAIDISOControlSensitivity5_ISO2200 = 2200,
    kNkMAIDISOControlSensitivity5_ISO2500 = 2500,
    kNkMAIDISOControlSensitivity5_ISO2800 = 2800,
    kNkMAIDISOControlSensitivity5_ISO3200 = 3200,
    kNkMAIDISOControlSensitivity5_ISO3600 = 3600,
    kNkMAIDISOControlSensitivity5_ISO4000 = 4000,
    kNkMAIDISOControlSensitivity5_ISO4500 = 4500,
    kNkMAIDISOControlSensitivity5_ISO5000 = 5000,
    kNkMAIDISOControlSensitivity5_ISO5600 = 5600,
    kNkMAIDISOControlSensitivity5_ISO6400 = 6400,
    kNkMAIDISOControlSensitivity5_ISO7200 = 7200,
    kNkMAIDISOControlSensitivity5_ISO8000 = 8000,
    kNkMAIDISOControlSensitivity5_ISO9000 = 9000,
    kNkMAIDISOControlSensitivity5_ISO10000 = 10000,
    kNkMAIDISOControlSensitivity5_ISO11000 = 11000,
    kNkMAIDISOControlSensitivity5_ISO11400 = 11400,
    kNkMAIDISOControlSensitivity5_ISO12800 = 12800,
    kNkMAIDISOControlSensitivity5_ISO14400 = 14400,
    kNkMAIDISOControlSensitivity5_ISO16000 = 16000,
    kNkMAIDISOControlSensitivity5_ISO18000 = 18000,
    kNkMAIDISOControlSensitivity5_ISO20000 = 20000,
    kNkMAIDISOControlSensitivity5_ISO22000 = 22000,
    kNkMAIDISOControlSensitivity5_ISO22800 = 22800,
    kNkMAIDISOControlSensitivity5_ISO25600 = 25600,
    kNkMAIDISOControlSensitivity5_ISO28800 = 28800,
    kNkMAIDISOControlSensitivity5_ISO32000 = 32000,
    kNkMAIDISOControlSensitivity5_ISO36000 = 36000,
    kNkMAIDISOControlSensitivity5_ISO40000 = 40000,
    kNkMAIDISOControlSensitivity5_ISO45600 = 45600,
    kNkMAIDISOControlSensitivity5_ISO51200 = 51200,
    kNkMAIDISOControlSensitivity5_ISO57600 = 57600,
    kNkMAIDISOControlSensitivity5_ISO64000 = 64000,
    kNkMAIDISOControlSensitivity5_ISO72000 = 72000,
    kNkMAIDISOControlSensitivity5_ISO81200 = 81200,
    kNkMAIDISOControlSensitivity5_ISO91200 = 91200,
    kNkMAIDISOControlSensitivity5_ISO102400 = 102400,
    kNkMAIDISOControlSensitivity5_Hi02 = 115000,
    kNkMAIDISOControlSensitivity5_Hi03 = 128000,
    kNkMAIDISOControlSensitivity5_Hi05 = 144000,
    kNkMAIDISOControlSensitivity5_Hi07 = 162000,
    kNkMAIDISOControlSensitivity5_Hi08 = 182000,
    kNkMAIDISOControlSensitivity5_Hi10 = 204800,
    kNkMAIDISOControlSensitivity5_Hi12 = 230000,
    kNkMAIDISOControlSensitivity5_Hi13 = 256000,
    kNkMAIDISOControlSensitivity5_Hi15 = 288000,
    kNkMAIDISOControlSensitivity5_Hi17 = 324000,
    kNkMAIDISOControlSensitivity5_Hi18 = 364000,
    kNkMAIDISOControlSensitivity5_Hi20 = 409600,
    kNkMAIDISOControlSensitivity5_Hi22 = 460000,
    kNkMAIDISOControlSensitivity5_Hi23 = 512000,
    kNkMAIDISOControlSensitivity5_Hi25 = 576000,
    kNkMAIDISOControlSensitivity5_Hi27 = 648000,
    kNkMAIDISOControlSensitivity5_Hi28 = 728000,
    kNkMAIDISOControlSensitivity5_Hi30 = 820000,
    kNkMAIDISOControlSensitivity5_Hi32 = 920000,
    kNkMAIDISOControlSensitivity5_Hi33 = 1030000,
    kNkMAIDISOControlSensitivity5_Hi35 = 1160000,
    kNkMAIDISOControlSensitivity5_Hi37 = 1300000,
    kNkMAIDISOControlSensitivity5_Hi38 = 1460000,
    kNkMAIDISOControlSensitivity5_Hi40 = 1640000,
    kNkMAIDISOControlSensitivity5_Hi42 = 1840000,
    kNkMAIDISOControlSensitivity5_Hi43 = 2060000,
    kNkMAIDISOControlSensitivity5_Hi45 = 2320000,
    kNkMAIDISOControlSensitivity5_Hi47 = 2600000,
    kNkMAIDISOControlSensitivity5_Hi48 = 2920000,
    kNkMAIDISOControlSensitivity5_Hi50 = 3280000
};

// kNkMAIDISOControlSensitivity(0x8422)
enum eNkMAIDISOControlSensitivity6
{
	kNkMAIDISOControlSensitivity6_Lo10 = 50,
	kNkMAIDISOControlSensitivity6_Lo08 = 56,
	kNkMAIDISOControlSensitivity6_Lo07 = 64,
	kNkMAIDISOControlSensitivity6_Lo05 = 72,
	kNkMAIDISOControlSensitivity6_Lo03 = 80,
	kNkMAIDISOControlSensitivity6_Lo02 = 90,
	kNkMAIDISOControlSensitivity6_ISO100 = 100,
	kNkMAIDISOControlSensitivity6_ISO110 = 110,
	kNkMAIDISOControlSensitivity6_ISO125 = 125,
	kNkMAIDISOControlSensitivity6_ISO140 = 140,
	kNkMAIDISOControlSensitivity6_ISO160 = 160,
	kNkMAIDISOControlSensitivity6_ISO180 = 180,
	kNkMAIDISOControlSensitivity6_ISO200 = 200,
	kNkMAIDISOControlSensitivity6_ISO220 = 220,
	kNkMAIDISOControlSensitivity6_ISO250 = 250,
	kNkMAIDISOControlSensitivity6_ISO280 = 280,
	kNkMAIDISOControlSensitivity6_ISO320 = 320,
	kNkMAIDISOControlSensitivity6_ISO360 = 360,
	kNkMAIDISOControlSensitivity6_ISO400 = 400,
	kNkMAIDISOControlSensitivity6_ISO450 = 450,
	kNkMAIDISOControlSensitivity6_ISO500 = 500,
	kNkMAIDISOControlSensitivity6_ISO560 = 560,
	kNkMAIDISOControlSensitivity6_ISO640 = 640,
	kNkMAIDISOControlSensitivity6_ISO720 = 720,
	kNkMAIDISOControlSensitivity6_ISO800 = 800,
	kNkMAIDISOControlSensitivity6_ISO900 = 900,
	kNkMAIDISOControlSensitivity6_ISO1000 = 1000,
	kNkMAIDISOControlSensitivity6_ISO1100 = 1100,
	kNkMAIDISOControlSensitivity6_ISO1250 = 1250,
	kNkMAIDISOControlSensitivity6_ISO1400 = 1400,
	kNkMAIDISOControlSensitivity6_ISO1600 = 1600,
	kNkMAIDISOControlSensitivity6_ISO1800 = 1800,
	kNkMAIDISOControlSensitivity6_ISO2000 = 2000,
	kNkMAIDISOControlSensitivity6_ISO2200 = 2200,
	kNkMAIDISOControlSensitivity6_ISO2500 = 2500,
	kNkMAIDISOControlSensitivity6_ISO2800 = 2800,
	kNkMAIDISOControlSensitivity6_ISO3200 = 3200,
	kNkMAIDISOControlSensitivity6_ISO3600 = 3600,
	kNkMAIDISOControlSensitivity6_ISO4000 = 4000,
	kNkMAIDISOControlSensitivity6_ISO4500 = 4500,
	kNkMAIDISOControlSensitivity6_ISO5000 = 5000,
	kNkMAIDISOControlSensitivity6_ISO5600 = 5600,
	kNkMAIDISOControlSensitivity6_ISO6400 = 6400,
	kNkMAIDISOControlSensitivity6_ISO7200 = 7200,
	kNkMAIDISOControlSensitivity6_ISO8000 = 8000,
	kNkMAIDISOControlSensitivity6_ISO9000 = 9000,
	kNkMAIDISOControlSensitivity6_ISO10000 = 10000,
	kNkMAIDISOControlSensitivity6_ISO11000 = 11000,
	kNkMAIDISOControlSensitivity6_ISO11400 = 11400,
	kNkMAIDISOControlSensitivity6_ISO12800 = 12800,
	kNkMAIDISOControlSensitivity6_ISO14400 = 14400,
	kNkMAIDISOControlSensitivity6_ISO16000 = 16000,
	kNkMAIDISOControlSensitivity6_ISO18000 = 18000,
	kNkMAIDISOControlSensitivity6_ISO20000 = 20000,
	kNkMAIDISOControlSensitivity6_ISO22000 = 22000,
	kNkMAIDISOControlSensitivity6_ISO22800 = 22800,
	kNkMAIDISOControlSensitivity6_ISO25600 = 25600,
	kNkMAIDISOControlSensitivity6_ISO28800 = 28800,
	kNkMAIDISOControlSensitivity6_ISO32000 = 32000,
	kNkMAIDISOControlSensitivity6_ISO36000 = 36000,
	kNkMAIDISOControlSensitivity6_ISO40000 = 40000,
	kNkMAIDISOControlSensitivity6_ISO45600 = 45600,
	kNkMAIDISOControlSensitivity6_ISO51200 = 51200,
	kNkMAIDISOControlSensitivity6_ISO57600 = 57600,
	kNkMAIDISOControlSensitivity6_Hi02 = 57600,
    kNkMAIDISOControlSensitivity6_ISO64000 = 64000,
	kNkMAIDISOControlSensitivity6_Hi03 = 64000,
    kNkMAIDISOControlSensitivity6_ISO72000 = 72000,
	kNkMAIDISOControlSensitivity6_Hi05 = 72000,
    kNkMAIDISOControlSensitivity6_ISO81200 = 81200,
	kNkMAIDISOControlSensitivity6_Hi07 = 81200,
    kNkMAIDISOControlSensitivity6_ISO91200 = 91200,
	kNkMAIDISOControlSensitivity6_Hi08 = 91200,
    kNkMAIDISOControlSensitivity6_ISO102400 = 102400,
	kNkMAIDISOControlSensitivity6_Hi10 = 102400,
    kNkMAIDISOControlSensitivity6_Hi12 = 115000,
    kNkMAIDISOControlSensitivity6_Hi13 = 128000,
    kNkMAIDISOControlSensitivity6_Hi15 = 144000,
    kNkMAIDISOControlSensitivity6_Hi17 = 162000,
    kNkMAIDISOControlSensitivity6_Hi18 = 182000,
	kNkMAIDISOControlSensitivity6_Hi20 = 204800,
    kNkMAIDISOControlSensitivity6_Hi22 = 230000,
	kNkMAIDISOControlSensitivity6_Hi23 = 256000,
	kNkMAIDISOControlSensitivity6_Hi25 = 288000,
	kNkMAIDISOControlSensitivity6_Hi27 = 324000,
	kNkMAIDISOControlSensitivity6_Hi28 = 364000,
	kNkMAIDISOControlSensitivity6_Hi30 = 409600,
	kNkMAIDISOControlSensitivity6_Hi32 = 460000,
	kNkMAIDISOControlSensitivity6_Hi33 = 512000,
	kNkMAIDISOControlSensitivity6_Hi35 = 576000,
	kNkMAIDISOControlSensitivity6_Hi37 = 648000,
	kNkMAIDISOControlSensitivity6_Hi38 = 728000,
	kNkMAIDISOControlSensitivity6_Hi40 = 820000,
	kNkMAIDISOControlSensitivity6_Hi42 = 920000,
	kNkMAIDISOControlSensitivity6_Hi43 = 1030000,
	kNkMAIDISOControlSensitivity6_Hi45 = 1160000,
	kNkMAIDISOControlSensitivity6_Hi47 = 1300000,
	kNkMAIDISOControlSensitivity6_Hi48 = 1460000,
	kNkMAIDISOControlSensitivity6_Hi50 = 1640000
};

// kNkMAIDISOControlSensitivity(0x8422)
enum eNkMAIDISOControlSensitivity7
{
	kNkMAIDISOControlSensitivity7_Lo10 = 32,
	kNkMAIDISOControlSensitivity7_Lo07 = 40,
	kNkMAIDISOControlSensitivity7_Lo05 = 45,
	kNkMAIDISOControlSensitivity7_Lo03 = 50,
	kNkMAIDISOControlSensitivity7_ISO64 = 64,
	kNkMAIDISOControlSensitivity7_ISO72 = 72,
	kNkMAIDISOControlSensitivity7_ISO80 = 80,
	kNkMAIDISOControlSensitivity7_ISO90 = 90,
	kNkMAIDISOControlSensitivity7_ISO100 = 100,
	kNkMAIDISOControlSensitivity7_ISO110 = 110,
	kNkMAIDISOControlSensitivity7_ISO125 = 125,
	kNkMAIDISOControlSensitivity7_ISO140 = 140,
	kNkMAIDISOControlSensitivity7_ISO160 = 160,
	kNkMAIDISOControlSensitivity7_ISO180 = 180,
	kNkMAIDISOControlSensitivity7_ISO200 = 200,
	kNkMAIDISOControlSensitivity7_ISO220 = 220,
	kNkMAIDISOControlSensitivity7_ISO250 = 250,
	kNkMAIDISOControlSensitivity7_ISO280 = 280,
	kNkMAIDISOControlSensitivity7_ISO320 = 320,
	kNkMAIDISOControlSensitivity7_ISO360 = 360,
	kNkMAIDISOControlSensitivity7_ISO400 = 400,
	kNkMAIDISOControlSensitivity7_ISO450 = 450,
	kNkMAIDISOControlSensitivity7_ISO500 = 500,
	kNkMAIDISOControlSensitivity7_ISO560 = 560,
	kNkMAIDISOControlSensitivity7_ISO640 = 640,
	kNkMAIDISOControlSensitivity7_ISO720 = 720,
	kNkMAIDISOControlSensitivity7_ISO800 = 800,
	kNkMAIDISOControlSensitivity7_ISO900 = 900,
	kNkMAIDISOControlSensitivity7_ISO1000 = 1000,
	kNkMAIDISOControlSensitivity7_ISO2200 = 2200,
	kNkMAIDISOControlSensitivity7_ISO2500 = 2500,
	kNkMAIDISOControlSensitivity7_ISO2800 = 2800,
	kNkMAIDISOControlSensitivity7_ISO3200 = 3200,
	kNkMAIDISOControlSensitivity7_ISO3600 = 3600,
	kNkMAIDISOControlSensitivity7_ISO4000 = 4000,
	kNkMAIDISOControlSensitivity7_ISO4500 = 4500,
	kNkMAIDISOControlSensitivity7_ISO5000 = 5000,
	kNkMAIDISOControlSensitivity7_ISO5600 = 5600,
	kNkMAIDISOControlSensitivity7_ISO6400 = 6400,
	kNkMAIDISOControlSensitivity7_ISO7200 = 7200,
	kNkMAIDISOControlSensitivity7_ISO8000 = 8000,
	kNkMAIDISOControlSensitivity7_ISO9000 = 9000,
	kNkMAIDISOControlSensitivity7_ISO10000 = 10000,
	kNkMAIDISOControlSensitivity7_ISO11000 = 11000,
	kNkMAIDISOControlSensitivity7_ISO12800 = 12800,
	kNkMAIDISOControlSensitivity7_ISO14400 = 14400,
	kNkMAIDISOControlSensitivity7_ISO16000 = 16000,
	kNkMAIDISOControlSensitivity7_ISO18000 = 18000,
	kNkMAIDISOControlSensitivity7_ISO20000 = 20000,
	kNkMAIDISOControlSensitivity7_ISO22000 = 22000,
	kNkMAIDISOControlSensitivity7_ISO25600 = 25600,
	kNkMAIDISOControlSensitivity7_Hi02 = 28800,
	kNkMAIDISOControlSensitivity7_Hi03 = 32000,
	kNkMAIDISOControlSensitivity7_Hi05 = 36000,
	kNkMAIDISOControlSensitivity7_Hi07 = 40000,
	kNkMAIDISOControlSensitivity7_Hi08 = 45600,
	kNkMAIDISOControlSensitivity7_Hi10 = 51200,
	kNkMAIDISOControlSensitivity7_Hi12 = 57600,
	kNkMAIDISOControlSensitivity7_Hi13 = 64000,
	kNkMAIDISOControlSensitivity7_Hi15 = 72000,
	kNkMAIDISOControlSensitivity7_Hi17 = 81200,
	kNkMAIDISOControlSensitivity7_Hi18 = 91200,
	kNkMAIDISOControlSensitivity7_Hi20 = 102400
};

// kNkMAIDISOControlSensitivity(0x8422)
enum eNkMAIDISOControlSensitivity8
{
	kNkMAIDISOControlSensitivity8_Lo10 = 32,
	kNkMAIDISOControlSensitivity8_Lo07 = 40,
	kNkMAIDISOControlSensitivity8_Lo05 = 45,
	kNkMAIDISOControlSensitivity8_Lo03 = 50,
	kNkMAIDISOControlSensitivity8_ISO64 = 64,
	kNkMAIDISOControlSensitivity8_ISO72 = 72,
	kNkMAIDISOControlSensitivity8_ISO80 = 80,
	kNkMAIDISOControlSensitivity8_ISO90 = 90,
	kNkMAIDISOControlSensitivity8_ISO100 = 100,
	kNkMAIDISOControlSensitivity8_ISO110 = 110,
	kNkMAIDISOControlSensitivity8_ISO125 = 125,
	kNkMAIDISOControlSensitivity8_ISO140 = 140,
	kNkMAIDISOControlSensitivity8_ISO160 = 160,
	kNkMAIDISOControlSensitivity8_ISO180 = 180,
	kNkMAIDISOControlSensitivity8_ISO200 = 200,
	kNkMAIDISOControlSensitivity8_ISO220 = 220,
	kNkMAIDISOControlSensitivity8_ISO250 = 250,
	kNkMAIDISOControlSensitivity8_ISO280 = 280,
	kNkMAIDISOControlSensitivity8_ISO320 = 320,
	kNkMAIDISOControlSensitivity8_ISO360 = 360,
	kNkMAIDISOControlSensitivity8_ISO400 = 400,
	kNkMAIDISOControlSensitivity8_ISO450 = 450,
	kNkMAIDISOControlSensitivity8_ISO500 = 500,
	kNkMAIDISOControlSensitivity8_ISO560 = 560,
	kNkMAIDISOControlSensitivity8_ISO640 = 640,
	kNkMAIDISOControlSensitivity8_ISO720 = 720,
	kNkMAIDISOControlSensitivity8_ISO800 = 800,
	kNkMAIDISOControlSensitivity8_ISO900 = 900,
	kNkMAIDISOControlSensitivity8_ISO1000 = 1000,
	kNkMAIDISOControlSensitivity8_ISO1100 = 1100,
	kNkMAIDISOControlSensitivity8_ISO1250 = 1250,
	kNkMAIDISOControlSensitivity8_ISO1400 = 1400,
	kNkMAIDISOControlSensitivity8_ISO1600 = 1600,
	kNkMAIDISOControlSensitivity8_ISO1800 = 1800,
	kNkMAIDISOControlSensitivity8_ISO2000 = 2000,
    kNkMAIDISOControlSensitivity8_ISO2200 = 2200,
	kNkMAIDISOControlSensitivity8_ISO2500 = 2500,
	kNkMAIDISOControlSensitivity8_ISO2800 = 2800,
	kNkMAIDISOControlSensitivity8_ISO3200 = 3200,
	kNkMAIDISOControlSensitivity8_ISO3600 = 3600,
	kNkMAIDISOControlSensitivity8_ISO4000 = 4000,
	kNkMAIDISOControlSensitivity8_ISO4500 = 4500,
	kNkMAIDISOControlSensitivity8_ISO5000 = 5000,
	kNkMAIDISOControlSensitivity8_ISO5600 = 5600,
	kNkMAIDISOControlSensitivity8_ISO6400 = 6400,
	kNkMAIDISOControlSensitivity8_ISO7200 = 7200,
	kNkMAIDISOControlSensitivity8_ISO8000 = 8000,
	kNkMAIDISOControlSensitivity8_ISO9000 = 9000,
	kNkMAIDISOControlSensitivity8_ISO10000 = 10000,
	kNkMAIDISOControlSensitivity8_ISO11000 = 11000,
	kNkMAIDISOControlSensitivity8_ISO12800 = 12800,
	kNkMAIDISOControlSensitivity8_ISO14400 = 14400,
	kNkMAIDISOControlSensitivity8_ISO16000 = 16000,
	kNkMAIDISOControlSensitivity8_ISO18000 = 18000,
	kNkMAIDISOControlSensitivity8_ISO20000 = 20000,
	kNkMAIDISOControlSensitivity8_ISO22000 = 22000,
	kNkMAIDISOControlSensitivity8_ISO25600 = 25600,
	kNkMAIDISOControlSensitivity8_Hi02 = 28800,
	kNkMAIDISOControlSensitivity8_Hi03 = 32000,
	kNkMAIDISOControlSensitivity8_Hi05 = 36000,
	kNkMAIDISOControlSensitivity8_Hi07 = 40000,
	kNkMAIDISOControlSensitivity8_Hi08 = 45600,
	kNkMAIDISOControlSensitivity8_Hi10 = 51200,
	kNkMAIDISOControlSensitivity8_Hi12 = 57600,
	kNkMAIDISOControlSensitivity8_Hi13 = 64000,
	kNkMAIDISOControlSensitivity8_Hi15 = 72000,
	kNkMAIDISOControlSensitivity8_Hi17 = 81200,
	kNkMAIDISOControlSensitivity8_Hi18 = 91200,
	kNkMAIDISOControlSensitivity8_Hi20 = 102400
};

//kNkMAIDCapability_ExposureMeterLinkage(0x8430)
enum eNkMAIDExposureMeterLinkage
{
    kNkMAIDExposureMeterLinkage_AI = 0,
    kNkMAIDExposureMeterLinkage_NON_AI
};

//kNkMAIDCapability_MovieRecordingZone(0x8450)
enum eNkMAIDMovieRecordingZone
{
    kNkMAIDMovieRecordingZone_WideRange = 0,
    kNkMAIDMovieRecordingZone_VocalRange
};

//kNkMAIDCapability_MovieISOAutoHiLimit(0x8452)
enum eNkMAIDMovieISOAutoHiLimit
{
    kNkMAIDMovieISOAutoHiLimit_ISO400 = 0,
    kNkMAIDMovieISOAutoHiLimit_ISO500,
    kNkMAIDMovieISOAutoHiLimit_ISO560,
    kNkMAIDMovieISOAutoHiLimit_ISO640,
    kNkMAIDMovieISOAutoHiLimit_ISO800,
    kNkMAIDMovieISOAutoHiLimit_ISO1000,
    kNkMAIDMovieISOAutoHiLimit_ISO1100,
    kNkMAIDMovieISOAutoHiLimit_ISO1250,
    kNkMAIDMovieISOAutoHiLimit_ISO1600,
    kNkMAIDMovieISOAutoHiLimit_ISO2000,
    kNkMAIDMovieISOAutoHiLimit_ISO2200,
    kNkMAIDMovieISOAutoHiLimit_ISO2500,
    kNkMAIDMovieISOAutoHiLimit_ISO3200,
    kNkMAIDMovieISOAutoHiLimit_ISO4000,
    kNkMAIDMovieISOAutoHiLimit_ISO4500,
    kNkMAIDMovieISOAutoHiLimit_ISO5000,
    kNkMAIDMovieISOAutoHiLimit_ISO6400,
    kNkMAIDMovieISOAutoHiLimit_ISO8000,
    kNkMAIDMovieISOAutoHiLimit_ISO9000,
    kNkMAIDMovieISOAutoHiLimit_ISO10000,
    kNkMAIDMovieISOAutoHiLimit_ISO12800,
    kNkMAIDMovieISOAutoHiLimit_ISO16000,
    kNkMAIDMovieISOAutoHiLimit_ISO18000,
    kNkMAIDMovieISOAutoHiLimit_ISO20000,
    kNkMAIDMovieISOAutoHiLimit_ISO25600,
    kNkMAIDMovieISOAutoHiLimit_ISOHi03,
    kNkMAIDMovieISOAutoHiLimit_ISOHi05,
    kNkMAIDMovieISOAutoHiLimit_ISOHi07,
    kNkMAIDMovieISOAutoHiLimit_ISOHi10,
    kNkMAIDMovieISOAutoHiLimit_ISOHi20,
    kNkMAIDMovieISOAutoHiLimit_ISOHi30,
    kNkMAIDMovieISOAutoHiLimit_ISOHi40
};

//kNkMAIDCapability_MovieISOAutoHiLimit(0x8452)
enum eNkMAIDMovieISOAutoHiLimit2
{
    kNkMAIDMovieISOAutoHiLimit2_ISO72 = 0,
    kNkMAIDMovieISOAutoHiLimit2_ISO80,
    kNkMAIDMovieISOAutoHiLimit2_ISO100,
    kNkMAIDMovieISOAutoHiLimit2_ISO125,
    kNkMAIDMovieISOAutoHiLimit2_ISO140,
    kNkMAIDMovieISOAutoHiLimit2_ISO160,
    kNkMAIDMovieISOAutoHiLimit2_ISO200,
    kNkMAIDMovieISOAutoHiLimit2_ISO250,
    kNkMAIDMovieISOAutoHiLimit2_ISO280,
    kNkMAIDMovieISOAutoHiLimit2_ISO320,
    kNkMAIDMovieISOAutoHiLimit2_ISO400,
    kNkMAIDMovieISOAutoHiLimit2_ISO500,
    kNkMAIDMovieISOAutoHiLimit2_ISO560,
    kNkMAIDMovieISOAutoHiLimit2_ISO640,
    kNkMAIDMovieISOAutoHiLimit2_ISO800,
    kNkMAIDMovieISOAutoHiLimit2_ISO1000,
    kNkMAIDMovieISOAutoHiLimit2_ISO1100,
    kNkMAIDMovieISOAutoHiLimit2_ISO1250,
    kNkMAIDMovieISOAutoHiLimit2_ISO1600,
    kNkMAIDMovieISOAutoHiLimit2_ISO2000,
    kNkMAIDMovieISOAutoHiLimit2_ISO2200,
    kNkMAIDMovieISOAutoHiLimit2_ISO2500,
    kNkMAIDMovieISOAutoHiLimit2_ISO3200,
    kNkMAIDMovieISOAutoHiLimit2_ISO4000,
    kNkMAIDMovieISOAutoHiLimit2_ISO4500,
    kNkMAIDMovieISOAutoHiLimit2_ISO5000,
    kNkMAIDMovieISOAutoHiLimit2_ISO6400,
    kNkMAIDMovieISOAutoHiLimit2_ISO8000,
    kNkMAIDMovieISOAutoHiLimit2_ISO9000,
    kNkMAIDMovieISOAutoHiLimit2_ISO10000,
    kNkMAIDMovieISOAutoHiLimit2_ISO12800,
    kNkMAIDMovieISOAutoHiLimit2_ISOHi03,
    kNkMAIDMovieISOAutoHiLimit2_ISOHi05,
    kNkMAIDMovieISOAutoHiLimit2_ISOHi07,
    kNkMAIDMovieISOAutoHiLimit2_ISOHi10,
    kNkMAIDMovieISOAutoHiLimit2_ISOHi20,
	kNkMAIDMovieISOAutoHiLimit2_ISO16000,
	kNkMAIDMovieISOAutoHiLimit2_ISO18000,
	kNkMAIDMovieISOAutoHiLimit2_ISO20000,
	kNkMAIDMovieISOAutoHiLimit2_ISO25600,
};

//kNkMAIDCapability_MovieISOAutoHiLimit(0x8452)
enum eNkMAIDMovieISOAutoHiLimit3
{
    kNkMAIDMovieISOAutoHiLimit3_ISO200 = 0,
    kNkMAIDMovieISOAutoHiLimit3_ISO250,
    kNkMAIDMovieISOAutoHiLimit3_ISO280,
    kNkMAIDMovieISOAutoHiLimit3_ISO320,
    kNkMAIDMovieISOAutoHiLimit3_ISO400,
    kNkMAIDMovieISOAutoHiLimit3_ISO500,
    kNkMAIDMovieISOAutoHiLimit3_ISO560,
    kNkMAIDMovieISOAutoHiLimit3_ISO640,
    kNkMAIDMovieISOAutoHiLimit3_ISO800,
    kNkMAIDMovieISOAutoHiLimit3_ISO1000,
    kNkMAIDMovieISOAutoHiLimit3_ISO1100,
    kNkMAIDMovieISOAutoHiLimit3_ISO1250,
    kNkMAIDMovieISOAutoHiLimit3_ISO1600,
    kNkMAIDMovieISOAutoHiLimit3_ISO2000,
    kNkMAIDMovieISOAutoHiLimit3_ISO2200,
    kNkMAIDMovieISOAutoHiLimit3_ISO2500,
    kNkMAIDMovieISOAutoHiLimit3_ISO3200,
    kNkMAIDMovieISOAutoHiLimit3_ISO4000,
    kNkMAIDMovieISOAutoHiLimit3_ISO4500,
    kNkMAIDMovieISOAutoHiLimit3_ISO5000,
    kNkMAIDMovieISOAutoHiLimit3_ISO6400,
    kNkMAIDMovieISOAutoHiLimit3_ISO8000,
    kNkMAIDMovieISOAutoHiLimit3_ISO9000,
    kNkMAIDMovieISOAutoHiLimit3_ISO10000,
    kNkMAIDMovieISOAutoHiLimit3_ISO12800,
    kNkMAIDMovieISOAutoHiLimit3_ISOHi03,
    kNkMAIDMovieISOAutoHiLimit3_ISOHi05,
    kNkMAIDMovieISOAutoHiLimit3_ISOHi07,
    kNkMAIDMovieISOAutoHiLimit3_ISOHi10,
    kNkMAIDMovieISOAutoHiLimit3_ISOHi20,
    kNkMAIDMovieISOAutoHiLimit3_ISO16000,
    kNkMAIDMovieISOAutoHiLimit3_ISO18000,
    kNkMAIDMovieISOAutoHiLimit3_ISO20000,
    kNkMAIDMovieISOAutoHiLimit3_ISO25600,
    kNkMAIDMovieISOAutoHiLimit3_ISO32000,
    kNkMAIDMovieISOAutoHiLimit3_ISO36000,
    kNkMAIDMovieISOAutoHiLimit3_ISO40000,
    kNkMAIDMovieISOAutoHiLimit3_ISO51200,
    kNkMAIDMovieISOAutoHiLimit3_ISO64000,
    kNkMAIDMovieISOAutoHiLimit3_ISO72000,
    kNkMAIDMovieISOAutoHiLimit3_ISO81200,
    kNkMAIDMovieISOAutoHiLimit3_ISO102400,
    kNkMAIDMovieISOAutoHiLimit3_ISOHi30,
    kNkMAIDMovieISOAutoHiLimit3_ISOHi40,
    kNkMAIDMovieISOAutoHiLimit3_ISOHi50
};

//kNkMAIDCapability_MovieISOAutoHiLimit(0x8452)
enum eNkMAIDMovieISOAutoHiLimit4
{
    kNkMAIDMovieISOAutoHiLimit4_ISO400 = 0,
    kNkMAIDMovieISOAutoHiLimit4_ISO500,
    kNkMAIDMovieISOAutoHiLimit4_ISO560,
    kNkMAIDMovieISOAutoHiLimit4_ISO640,
    kNkMAIDMovieISOAutoHiLimit4_ISO800,
    kNkMAIDMovieISOAutoHiLimit4_ISO1000,
    kNkMAIDMovieISOAutoHiLimit4_ISO1100,
    kNkMAIDMovieISOAutoHiLimit4_ISO1250,
    kNkMAIDMovieISOAutoHiLimit4_ISO1600,
    kNkMAIDMovieISOAutoHiLimit4_ISO2000,
    kNkMAIDMovieISOAutoHiLimit4_ISO2200,
    kNkMAIDMovieISOAutoHiLimit4_ISO2500,
    kNkMAIDMovieISOAutoHiLimit4_ISO3200,
    kNkMAIDMovieISOAutoHiLimit4_ISO4000,
    kNkMAIDMovieISOAutoHiLimit4_ISO4500,
    kNkMAIDMovieISOAutoHiLimit4_ISO5000,
    kNkMAIDMovieISOAutoHiLimit4_ISO6400,
    kNkMAIDMovieISOAutoHiLimit4_ISO8000,
    kNkMAIDMovieISOAutoHiLimit4_ISO9000,
    kNkMAIDMovieISOAutoHiLimit4_ISO10000,
    kNkMAIDMovieISOAutoHiLimit4_ISO12800,
    kNkMAIDMovieISOAutoHiLimit4_ISOHi03,
    kNkMAIDMovieISOAutoHiLimit4_ISOHi05,
    kNkMAIDMovieISOAutoHiLimit4_ISOHi07,
    kNkMAIDMovieISOAutoHiLimit4_ISOHi10,
    kNkMAIDMovieISOAutoHiLimit4_ISOHi20,
};

//kNkMAIDCapability_LimitAFAreaMode(0x8455)
enum eNkMAIDLimitAFAreaMode
{
    kNkMAIDLimitAFAreaMode_Auto         = 0x0080,   //Bit7:Auto-area AF mode
    kNkMAIDLimitAFAreaMode_Group        = 0x0040,   //Bit6:Group-area AF mode
    kNkMAIDLimitAFAreaMode_3DTtracking  = 0x0020,   //Bit5:3D-tracking
    kNkMAIDLimitAFAreaMode_Dynamic51    = 0x0010,   //Bit4:Dynamic AF mode (51 points)
    kNkMAIDLimitAFAreaMode_Dynamic21    = 0x0004,   //Bit2:Dynamic AF mode (21 points)
    kNkMAIDLimitAFAreaMode_Dynamic9     = 0x0002    //Bit1:Dynamic AF mode (9 points)
};

//kNkMAIDCapability_LimitAFAreaMode(0x8455)
enum eNkMAIDLimitAFAreaMode2
{
    kNkMAIDLimitAFAreaMode2_Group_VL     = 0x0200,   //Bit9:Group-area AF(Virtual) mode
    kNkMAIDLimitAFAreaMode2_Group_HL     = 0x0100,   //Bit8:Group-area AF(Horizontal) mode
    kNkMAIDLimitAFAreaMode2_Auto         = 0x0080,   //Bit7:Auto-area AF mode
    kNkMAIDLimitAFAreaMode2_Group        = 0x0040,   //Bit6:Group-area AF mode
    kNkMAIDLimitAFAreaMode2_3DTtracking  = 0x0020,   //Bit5:3D-tracking
    kNkMAIDLimitAFAreaMode2_Dynamic153   = 0x0010,   //Bit4:Dynamic AF mode (153 points)
    kNkMAIDLimitAFAreaMode2_Dynamic72    = 0x0004,   //Bit2:Dynamic AF mode (72 points)
    kNkMAIDLimitAFAreaMode2_Dynamic25    = 0x0002,   //Bit1:Dynamic AF mode (25 points)
	kNkMAIDLimitAFAreaMode2_Dynamic9	 = 0x0001    //Bit0:Dynamic AF mode (9 points)
};

//kNkMAIDCapability_AFModeRestrictions(0x8456)
enum eNkMAIDAFModeRestrictions
{
    kNkMAIDAFModeRestrictions_OFF       = 0,    //No restrictions
    kNkMAIDAFModeRestrictions_AFs       = 1,    //Restrict AF-S
    kNkMAIDAFModeRestrictions_AFc       = 2     //Restrict AF-C
};

//kNkMAIDCapability_MovieWBAutoType(0x8462)
enum eNkMAIDMovieWBAutoType
{
    kNkMovieWBAutoType_Normal = 0,
    kNkMovieWBAutoType_WarmWhite,
	kNkMovieWBAutoType_KeepWhite,
	kNkMovieWBAutoType_KeepAtmosphere
};

//kNkMAIDCapability_MovieWBFluorescentType(0x8464)
enum eNkMAIDMovieWBFluorescentType
{
    kNkMovieWBFluorescentType_SodiumVapor = 0,
    kNkMovieWBFluorescentType_WarmWhite, 
    kNkMovieWBFluorescentType_White, 
    kNkMovieWBFluorescentType_CoolWhite, 
    kNkMovieWBFluorescentType_DayWhite, 
    kNkMovieWBFluorescentType_Daylight, 
    kNkMovieWBFluorescentType_HiTempMercuryVapor
};

//kNkMAIDCapability_MovieWBPresetProtect1(0x8472)
enum eNkMAIDMovieWBPresetProtect1
{
    kNkMAIDMovieWBPresetProtect1_Off = 0,
    kNkMAIDMovieWBPresetProtect1_On
};

//kNkMAIDCapability_MovieWBPresetProtect2(0x8473)
enum eNkMAIDMovieWBPresetProtect2
{
    kNkMAIDMovieWBPresetProtect2_Off = 0,
    kNkMAIDMovieWBPresetProtect2_On
};

//kNkMAIDCapability_MovieWBPresetProtect3(0x8474)
enum eNkMAIDMovieWBPresetProtect3
{
    kNkMAIDMovieWBPresetProtect3_Off = 0,
    kNkMAIDMovieWBPresetProtect3_On
};

//kNkMAIDCapability_MovieWBPresetProtect4(0x8475)
enum eNkMAIDMovieWBPresetProtect4
{
    kNkMAIDMovieWBPresetProtect4_Off = 0,
    kNkMAIDMovieWBPresetProtect4_On
};

//kNkMAIDCapability_MovieWBPresetProtect5(0x8476)
enum eNkMAIDMovieWBPresetProtect5
{
    kNkMAIDMovieWBPresetProtect5_Off = 0,
    kNkMAIDMovieWBPresetProtect5_On
};

//kNkMAIDCapability_MovieWBPresetProtect6(0x8477)
enum eNkMAIDMovieWBPresetProtect6
{
    kNkMAIDMovieWBPresetProtect6_Off = 0,
    kNkMAIDMovieWBPresetProtect6_On
};

//kNkMAIDCapability_MovieCCDDataMode(0x847f)
enum eNkMAIDMovieCCDDataMode
{
    kNkMAIDMovieCCDDataMode_FXFormat = 0,
    kNkMAIDMovieCCDDataMode_DXFormat,
    kNkMAIDMovieCCDDataMode_13x
};

//kNkMAIDCapability_MovieNoiseReductionHighISO(0x8481)
enum eNkMAIDMovieNoiseReductionHighISO
{
    kNkMAIDMovieNoiseReductionHighISO_Off   = 0,
    kNkMAIDMovieNoiseReductionHighISO_Normal    = 1, 
    kNkMAIDMovieNoiseReductionHighISO_High  = 2,
    kNkMAIDMovieNoiseReductionHighISO_Low   = 3
};

//kNkMAIDCapability_MoviePictureControl(0x8482)
enum eNkMAIDMoviePictureControl
{
    kNkMAIDMoviePictureControl_Undefined  = 0,      //Undefined
    kNkMAIDMoviePictureControl_Standard   = 1,      //Standard
    kNkMAIDMoviePictureControl_Neutral    = 2,      //Neutral
    kNkMAIDMoviePictureControl_Vivid      = 3,      //Vivid
    kNkMAIDMoviePictureControl_Monochrome = 4,      //Monochrome
    kNkMAIDMoviePictureControl_Portrait   = 5,      //Portrait
    kNkMAIDMoviePictureControl_Landscape  = 6,      //Landscape
    kNkMAIDMoviePictureControl_Flat       = 7,      //Flat
    kNkMAIDMoviePictureControl_Auto       = 8,      //Auto
    kNkMAIDMoviePictureControl_SamePhoto  = 100,    //Same as photo
    kNkMAIDMoviePictureControl_Option1   = 101,     //Option picture control1
    kNkMAIDMoviePictureControl_Option2   = 102,     //Option picture control2
    kNkMAIDMoviePictureControl_Option3   = 103,     //Option picture control3
    kNkMAIDMoviePictureControl_Option4   = 104,     //Option picture control4
	kNkMAIDMoviePictureControl_Dream     = 101,
	kNkMAIDMoviePictureControl_Morning   = 102,
	kNkMAIDMoviePictureControl_Pop       = 103,
	kNkMAIDMoviePictureControl_Sunday    = 104,
	kNkMAIDMoviePictureControl_Somber    = 105,
	kNkMAIDMoviePictureControl_Dramatic  = 106,
	kNkMAIDMoviePictureControl_Silence   = 107,
	kNkMAIDMoviePictureControl_Breached  = 108,
	kNkMAIDMoviePictureControl_Melancholic = 109,
	kNkMAIDMoviePictureControl_Pure      = 110,
	kNkMAIDMoviePictureControl_Denim     = 111,
	kNkMAIDMoviePictureControl_Toy       = 112,
	kNkMAIDMoviePictureControl_Sepia     = 113,
	kNkMAIDMoviePictureControl_Blue      = 114,
	kNkMAIDMoviePictureControl_Red       = 115,
	kNkMAIDMoviePictureControl_Pink      = 116,
	kNkMAIDMoviePictureControl_Charcoal  = 117,
	kNkMAIDMoviePictureControl_Graphite  = 118,
	kNkMAIDMoviePictureControl_Binary    = 119,
	kNkMAIDMoviePictureControl_Carbon    = 120,
    kNkMAIDMoviePictureControl_Custom1   = 201,     //Custom picture control1
    kNkMAIDMoviePictureControl_Custom2   = 202,     //Custom picture control2
    kNkMAIDMoviePictureControl_Custom3   = 203,     //Custom picture control3
    kNkMAIDMoviePictureControl_Custom4   = 204,     //Custom picture control4
    kNkMAIDMoviePictureControl_Custom5   = 205,     //Custom picture control5
    kNkMAIDMoviePictureControl_Custom6   = 206,     //Custom picture control6
    kNkMAIDMoviePictureControl_Custom7   = 207,     //Custom picture control7
    kNkMAIDMoviePictureControl_Custom8   = 208,     //Custom picture control8
    kNkMAIDMoviePictureControl_Custom9   = 209      //Custom picture control9
};

//kNkMAIDCapability_MovieCaptureMode(0x848a)
enum eNkMAIDMovieCaptureMode
{
    kMAIDMovieCaptureMode_HD = 0,       // Normal Movie
    kMAIDMovieCaptureMode_SlowMotion    // Slow Motion Movie
};

//kNkMAIDCapability_HighSpeedStillCaptureRate(0x848c)
enum eNkMAIDHighSpeedStillCaptureRate
{
    kMAIDHighSpeedStillCaptureRate_CL    = 0,   // Continuous shot
    kMAIDHighSpeedStillCaptureRate_10fps,       // Continuous high-speed shot 10 frames/second
    kMAIDHighSpeedStillCaptureRate_20fps,       // Continuous high-speed shot 20 frames/second
    kMAIDHighSpeedStillCaptureRate_30fps,       // Continuous high-speed shot 30 frames/second
    kMAIDHighSpeedStillCaptureRate_60fps        // Continuous high-speed shot 60 frames/second
};

//kNkMAIDCapability_BestMomentCaptureMode(0x848d)
enum eNkMAIDBestMomentCaptureMode
{
    kMAIDBestMomentCaptureMode_OFF = 0,         //Except Best Moment Capture
    kMAIDBestMomentCaptureMode_ActiveSelection  //Active Selection
};

//kNkMAIDCapability_ActiveSelectionFrameSavedDefault(0x848e)
enum eNkMAIDActiveSelectionFrameSavedDefault
{
    kMAIDActiveSelectionFrameSavedDefault_1 = 0,   // 1frame
    kMAIDActiveSelectionFrameSavedDefault_40       //40frame
};

//kNkMAIDCapability_ActiveSelectionCapture40frameOver(0x848f)
enum eNkMAIDActiveSelectionCapture40frameOver
{
    kMAIDActiveSelectionCapture40frameOver_60fps = 0,  //2/3 second
    kMAIDActiveSelectionCapture40frameOver_30fps,      //4/3 second
    kMAIDActiveSelectionCapture40frameOver_20fps       //2 second
};

//kNkMAIDCapability_ActiveSelectionOnReleaseRecord(0x8490)
enum eNkMAIDActiveSelectionOnReleaseRecord
{
    kMAIDActiveSelectionOnReleaseRecord_PrecedingCap = 0,            //Before the shutter-release button is pressed all the way down
    kMAIDActiveSelectionOnReleaseRecord_PrecedingAndFollowingCap,    //Before and after the shutter-release button is pressed all the way down
    kMAIDActiveSelectionOnReleaseRecord_FollowingCap                 //After the shutter-release button is pressed all the way down
};

//kNkMAIDCapability_ActiveSelectionControl(0x8490)
enum eNkMAIDActiveSelectionControl
{
    kMAIDActiveSelectionControl_OFF = 0,  //OFF
    kMAIDActiveSelectionControl_ON        //ON
};

//kNkMAIDCapability_IPTCPresetInfo(0x84cc)
enum eNkMAIDIPTCPresetInfo
{
    kNkMAIDIPTCPresetInfo_ALL  = 0, // All Preset.
    kNkMAIDIPTCPresetInfo_1    = 1, // Preset No.1.
    kNkMAIDIPTCPresetInfo_2    = 2, // Preset No.2.
    kNkMAIDIPTCPresetInfo_3    = 3, // Preset No.3.
    kNkMAIDIPTCPresetInfo_4    = 4, // Preset No.4.
    kNkMAIDIPTCPresetInfo_5    = 5, // Preset No.5.
    kNkMAIDIPTCPresetInfo_6    = 6, // Preset No.6.
    kNkMAIDIPTCPresetInfo_7    = 7, // Preset No.7.
    kNkMAIDIPTCPresetInfo_8    = 8, // Preset No.8.
    kNkMAIDIPTCPresetInfo_9    = 9, // Preset No.9.
    kNkMAIDIPTCPresetInfo_10   = 10, // Preset No.10.
    kNkMAIDIPTCPresetInfo_11   = 11, // Preset No.11.
    kNkMAIDIPTCPresetInfo_12   = 12, // Preset No.12.
    kNkMAIDIPTCPresetInfo_13   = 13, // Preset No.13.

};

//kNkMAIDCapability_GetSBAttrDesc(0x84ce)
//kNkMAIDCapability_SBAttrValue(0x84cf)
enum eNkMAIDSBHandle
{
    kNkMAIDSBHandle_ALL = 0 // All Handle
};

//kNkMAIDCapability_SBUsableGroup(0x84c4)
//kNkMAIDCapability_GetSBHandles(0x84cd)
//kNkMAIDCapability_GetSBGroupAttrDesc(0x84d0)
//kNkMAIDCapability_SBGroupAttrValue(0x84d1)
enum eNkMAIDSBGroupID
{
    kNkMAIDSBGroupID_ALL    = 0,
    kNkMAIDSBGroupID_Master = 0x00000001,
    kNkMAIDSBGroupID_A      = 0x00000002,
    kNkMAIDSBGroupID_B      = 0x00000004,
    kNkMAIDSBGroupID_C      = 0x00000008,
    kNkMAIDSBGroupID_D      = 0x00000010,
    kNkMAIDSBGroupID_E      = 0x00000020,
    kNkMAIDSBGroupID_F      = 0x00000040
};

//kNkMAIDCapability_GetSBAttrDesc(0x84ce)
//kNkMAIDCapability_SBAttrValue(0x84cf)
enum eNkMAIDSBAttribute
{
    kNkMAIDSBAttribute_ALL              = 0,
    kNkMAIDSBAttribute_Name             = 0x1001,
    kNkMAIDSBAttribute_GroupID          = 0x1002,
    kNkMAIDSBAttribute_Status           = 0x1003,
    kNkMAIDSBAttribute_TestFlashDisable = 0x1005,
    kNkMAIDSBAttribute_MaxNum           = 4,
};

//kNkMAIDCapability_GetSBAttrDesc(0x84ce)
//kNkMAIDCapability_SBAttrValue(0x84cf)
//kNkMAIDCapability_GetSBGroupAttrDesc(0x84d0)
//kNkMAIDCapability_SBGroupAttrValue(0x84d1)
enum eNkMAIDSBGroupAttribute
{
    kNkMAIDSBGroupAttribute_ALL               = 0,
    kNkMAIDSBGroupAttribute_FlashMode         = 0x4001,
    kNkMAIDSBGroupAttribute_FlashCompensation = 0x4002,
    kNkMAIDSBGroupAttribute_FlashRatio        = 0x4003,
    kNkMAIDSBGroupAttribute_FlashLevel        = 0x4004,
    kNkMAIDSBGroupAttribute_FlashRange        = 0x4005,
    kNkMAIDSBGroupAttribute_Repeat            = 0x4006,
    kNkMAIDSBGroupAttribute_RepeatCount       = 0x4007,
    kNkMAIDSBGroupAttribute_RepeatInterval    = 0x4008,
    kNkMAIDSBGroupAttribute_Invalid           = 0x4009,
    kNkMAIDSBGroupAttribute_MaxNum            = 9,
};

//kNkMAIDCapability_TestFlash(0x84d2)
enum eNkMAIDTestFlash
{
    kNkMAIDTestFlash_Test     = 0
};

//kNkMAIDCapability_IPTCPresetSelect(0x84be)
enum eNkMAIDIPTCPresetSelect
{
    kNkMAIDIPTCPresetSelect_OFF = 0,
    kNkMAIDIPTCPresetSelect_1,
    kNkMAIDIPTCPresetSelect_2,
    kNkMAIDIPTCPresetSelect_3,
    kNkMAIDIPTCPresetSelect_4,
    kNkMAIDIPTCPresetSelect_5,
    kNkMAIDIPTCPresetSelect_6,
    kNkMAIDIPTCPresetSelect_7,
    kNkMAIDIPTCPresetSelect_8,
    kNkMAIDIPTCPresetSelect_9,
    kNkMAIDIPTCPresetSelect_10,
	kNkMAIDIPTCPresetSelect_11,
	kNkMAIDIPTCPresetSelect_12,
	kNkMAIDIPTCPresetSelect_13,
};

//kNkMAIDCapability_FlashISOAutoHighLimit(0x84c1)
enum eNkMAIDFlashISOAutoHighLimit
{
    kNkMAIDFlashISOAutoHighLimit_ISO200    = 0,
    kNkMAIDFlashISOAutoHighLimit_ISO250,
    kNkMAIDFlashISOAutoHighLimit_ISO280,
    kNkMAIDFlashISOAutoHighLimit_ISO320,
    kNkMAIDFlashISOAutoHighLimit_ISO400,
    kNkMAIDFlashISOAutoHighLimit_ISO500,
    kNkMAIDFlashISOAutoHighLimit_ISO560,
    kNkMAIDFlashISOAutoHighLimit_ISO640,
    kNkMAIDFlashISOAutoHighLimit_ISO800,
    kNkMAIDFlashISOAutoHighLimit_ISO1000,
    kNkMAIDFlashISOAutoHighLimit_ISO1100,
    kNkMAIDFlashISOAutoHighLimit_ISO1250,
    kNkMAIDFlashISOAutoHighLimit_ISO1600,
    kNkMAIDFlashISOAutoHighLimit_ISO2000,
    kNkMAIDFlashISOAutoHighLimit_ISO2200,
    kNkMAIDFlashISOAutoHighLimit_ISO2500,
    kNkMAIDFlashISOAutoHighLimit_ISO3200,
    kNkMAIDFlashISOAutoHighLimit_ISO4000,
    kNkMAIDFlashISOAutoHighLimit_ISO4500,
    kNkMAIDFlashISOAutoHighLimit_ISO5000,
    kNkMAIDFlashISOAutoHighLimit_ISO6400,
    kNkMAIDFlashISOAutoHighLimit_ISO8000,
    kNkMAIDFlashISOAutoHighLimit_ISO9000,
    kNkMAIDFlashISOAutoHighLimit_ISO10000,
    kNkMAIDFlashISOAutoHighLimit_ISO12800,
    kNkMAIDFlashISOAutoHighLimit_ISO16000,
    kNkMAIDFlashISOAutoHighLimit_ISO18000,
    kNkMAIDFlashISOAutoHighLimit_ISO20000,
    kNkMAIDFlashISOAutoHighLimit_ISO25600,
    kNkMAIDFlashISOAutoHighLimit_ISO32000,
    kNkMAIDFlashISOAutoHighLimit_ISO36000,
    kNkMAIDFlashISOAutoHighLimit_ISO40000,
    kNkMAIDFlashISOAutoHighLimit_ISO51200,
    kNkMAIDFlashISOAutoHighLimit_ISO64000,
    kNkMAIDFlashISOAutoHighLimit_ISO72000,
    kNkMAIDFlashISOAutoHighLimit_ISO81200,
    kNkMAIDFlashISOAutoHighLimit_ISO102400,
    kNkMAIDFlashISOAutoHighLimit_Hi03,
    kNkMAIDFlashISOAutoHighLimit_Hi05,
    kNkMAIDFlashISOAutoHighLimit_Hi07,
    kNkMAIDFlashISOAutoHighLimit_Hi1,
    kNkMAIDFlashISOAutoHighLimit_Hi2,
    kNkMAIDFlashISOAutoHighLimit_Hi3,
    kNkMAIDFlashISOAutoHighLimit_Hi4,
    kNkMAIDFlashISOAutoHighLimit_Hi5,
    kNkMAIDFlashISOAutoHighLimit_NoneFlash = 45,
	kNkMAIDFlashISOAutoHighLimit_ISO72,
	kNkMAIDFlashISOAutoHighLimit_ISO80,
	kNkMAIDFlashISOAutoHighLimit_ISO100,
	kNkMAIDFlashISOAutoHighLimit_ISO125,
	kNkMAIDFlashISOAutoHighLimit_ISO140,
	kNkMAIDFlashISOAutoHighLimit_ISO160
};

//kNkMAIDCapability_SBWirelessMode(0x84c2)
enum eNkMAIDSBWirelessMode
{
    kNkMAIDSBWirelessMode_Off = 0,
    kNkMAIDSBWirelessMode_Radio,
    kNkMAIDSBWirelessMode_Optical,
    kNkMAIDSBWirelessMode_OpticalandRadio
};

//kNkMAIDCapability_SBWirelessMultipleFlashMode(0x84c3)
enum eNkMAIDSBWirelessMultipleFlashMode
{
    kNkMAIDSBWirelessMultipleFlashMode_Group = 0,
    kNkMAIDSBWirelessMultipleFlashMode_QuickWireless,
    kNkMAIDSBWirelessMultipleFlashMode_Repeat
};

//kNkMAIDCapability_SBUsableGroup(0x84c4)
enum eNkMAIDSBUsableGroup 
{
    kNkMAIDSBUsableGroup_Disable = 0
};

//kNkMAIDCapability_WirelessCLSEntryMode(0x84c5)
enum eNkMAIDWirelessCLSEntryMode
{
    kNkMAIDWirelessCLSEntryMode_Peering = 0,
    kNkMAIDWirelessCLSEntryMode_PINCode
};

//kNkMAIDCapability_RadioMultipleFlashChannel(0x84c7)
enum eNkMAIDRadioMultipleFlashChannel
{
    kNkMAIDRadioMultipleFlashChannel_Unknown = 0,
    kNkMAIDRadioMultipleFlashChannel_5ch     = 5,
    kNkMAIDRadioMultipleFlashChannel_10ch    = 10,
    kNkMAIDRadioMultipleFlashChannel_15ch    = 15
};

//kNkMAIDCapability_OpticalMultipleFlashChannel(0x84c8)
enum eNkMAIDOpticalMultipleFlashChannel
{
    kNkMAIDOpticalMultipleFlashChannel_1ch = 1,
    kNkMAIDOpticalMultipleFlashChannel_2ch,
    kNkMAIDOpticalMultipleFlashChannel_3ch,
    kNkMAIDOpticalMultipleFlashChannel_4ch
};

//kNkMAIDCapability_FlashRangeDisplay(0x84c9)
enum eNkMAIDFlashRangeDisplay
{
    kNkMAIDFlashRangeDisplay_m = 0,
    kNkMAIDFlashRangeDisplay_ft
};

//kNkMAIDCapability_AllTestFiringDisable(0x84ca)
enum eNkMAIDAllTestFiringDisable
{
    kNkMAIDAllTestFiringDisable_Enable = 0,
    kNkMAIDAllTestFiringDisable_Disable
};

//kNkMAIDCapability_FaceDetection(0x84d3)
enum eNkMAIDFaceDetection
{
    kNkMAIDFaceDetection_Off = 0,
    kNkMAIDFaceDetection_On,
    kNkMAIDFaceDetection_FaceAndEye,
	kNkMAIDFaceDetection_Animal
};

//kNkMAIDCapability_3DTrackingCaptuerArea(0x84d4)
enum eNkMAID3DTrackingCaptuerArea
{
    kNkMAID3DTrackingCaptuerArea_Wide = 0,
	kNkMAID3DTrackingCaptuerArea_Normal
};

//kNkMAIDCapability_SBSettingMemberLock(0x84d6)
enum eNkMAIDDSBSettingMemberLock
{
    kNkMAIDSBSettingMemberLock_Off = 0,
    kNkMAIDSBSettingMemberLock_On
};

//kNkMAIDCapability_SBIntegrationFlashReady(0x84db)
enum eNkMAIDSBIntegrationFlashReady
{
	kNkMAIDSBIntegrationFlashReady_NotReady = 0,
	kNkMAIDSBIntegrationFlashReady_Ready
};

//kNkMAIDCapability_MirrorUpStatus(0x84f0)
enum eNkMAIDMirrorUpStatus
{
    kNkMAIDMirrorUpStatus_Down = 0,
    kNkMAIDMirrorUpStatus_Up
};

//kNkMAIDCapability_MirrorUpReleaseShootingCount(0x84f1)
enum eNkMAIDMirrorUpReleaseShootingCount
{
    kNkMAIDMirrorUpReleaseShootingCount_0 = 0,
    kNkMAIDMirrorUpReleaseShootingCount_1,
    kNkMAIDMirrorUpReleaseShootingCount_2
};

//kNkMAIDCapability_LensTypeNikon1(0x8494)
enum eNkMAIDLensTypeNikon1
{
    kMAIDLensTypeNikon1_MountAdapter = 0x00000001, // (bit:0) Mount adapter
    kMAIDLensTypeNikon1_ShrinkLens   = 0x00000100, // (bit:8) Power retractable lens
    kMAIDLensTypeNikon1_PowerZoom    = 0x00000200, // (bit:9) Power zoom lens
    kMAIDLensTypeNikon1_VR           = 0x20000000  // (bit:29)Anti-vibration mechanism
};

//kNkMAIDCapability_SilentPhotography(0x8495)
enum eNkMAIDSilentPhotography
{
    kMAIDSilentPhotography_Off = 0,
    kMAIDSilentPhotography_On
};

//kNkMAIDCapability_FacePriority(0x8496)
enum eNkMAIDFacePriority
{
    kMAIDFacePriority_Off = 0,
    kMAIDFacePriority_On
};

//kNkMAIDCapability_SlowMotionMovieRecordScreenSize(0x848b)
enum eNkMAIDSlowMotionMovieRecordScreenSize
{
    kMAIDSlowMotionMovieRecordScreenSize_1280x720_120fps = 0,   //1280x720 Recording120fps Playback 30fps
    kMAIDSlowMotionMovieRecordScreenSize_768x288_400fps,        //768x288 Recording 400fps Playback 30fps
    kMAIDSlowMotionMovieRecordScreenSize_416x144_1200fps        //416x144 Recording 1200fps Playback 30fps
};

//kNkMAIDCapability_OpticalVR(0x8497)
enum eNkMAIDOpticalVR
{
    kMAIDOpticalVR_NORMAL = 0,
    kMAIDOpticalVR_ACTIVE,
    kMAIDOpticalVR_OFF
};

//kNkMAIDCapability_ElectronicVR(0x8498)
enum eNkMAIDElectronicVR
{
    kMAIDElectronicVR_OFF = 0,
    kMAIDElectronicVR_ON
};

//kNkMAIDCapability_ExposureCompFlashUsed(0x84f4)
enum eNkMAIDExposureCompFlashUsed
{
	kNkMAIDExposureCompFlashUsed_Entireframe = 0,
	kNkMAIDExposureCompFlashUsed_Backgroundonly
};

//kNkMAIDCapability_ElectronicFrontCurtainShutter(0x8458)
enum eNkMAIDElectronicFrontCurtainShutter
{
  kNkMAIDElectronicFrontCurtainShutter_Off = 0,
  kNkMAIDElectronicFrontCurtainShutter_On,
  kNkMAIDElectronicFrontCurtainShutter_Auto
};


//kNkMAIDCapability_AFLockOnAcross(0x84bc)
enum eNkMAIDAFLockOnAcross
{
    kNkMAIDAFLockOnAcross_1 = 1,//Quick[Sensitive]
    kNkMAIDAFLockOnAcross_2,
    kNkMAIDAFLockOnAcross_3,
    kNkMAIDAFLockOnAcross_4,
    kNkMAIDAFLockOnAcross_5     //Slow[Insensitive]
};

//kNkMAIDCapability_AFLockOnMove(0x84bd)
enum eNkMAIDAFLockOnMove
{
    kNkMAIDAFLockOnMove_1 = 1, //Steady[Smooth]
    kNkMAIDAFLockOnMove_2,
    kNkMAIDAFLockOnMove_3       //Erratic[Random]
};

//kNkMAIDCapability_MatrixMetering(0x84d7)
enum eNkMAIDMatrixMetering
{
  kNkMAIDMatrixMetering_Off = 0,
  kNkMAIDMatrixMetering_On
};

//kNkMAIDCapability_MovieActive_D_Lighting(0x84f2)
enum eNkMAIDMovieActive_D_Lighting
{
	kNkMAIDMovieActive_D_Lighting_Off = 0,        //Off
	kNkMAIDMovieActive_D_Lighting_Low,            //Low
	kNkMAIDMovieActive_D_Lighting_Normal,         //Normal
	kNkMAIDMovieActive_D_Lighting_High,           //High
	kNkMAIDMovieActive_D_Lighting_ExtraHigh,      //Extra high
	kNkMAIDMovieActive_D_Lighting_SamePhoto,      //Same as photo
	kNkMAIDMovieActive_D_Lighting_ExtraHigh1,     //Extra high1
	kNkMAIDMovieActive_D_Lighting_ExtraHigh2      //Extra high2
};

//kNkMAIDCapability_FlickerReductionSetting(0x84f3)
enum eNkMAIDFlickerReductionSetting
{
	kNkMAIDFlickerReductionSetting_Off = 0,		//Off
	kNkMAIDFlickerReductionSetting_On			//On
};


//kNkMAIDCapability_LimitStillCaptureMode(0x84f5)
enum eNkMAIDLimitStillCaptureMode
{
	kNkMAIDLimitStillCaptureMode_MirrorUp	= 0x0040,   //Bit6:MirrorUp
	kNkMAIDLimitStillCaptureMode_SelfTimer	= 0x0020,   //Bit5:SelfTimer
	kNkMAIDLimitStillCaptureMode_Quiet		= 0x0008,   //Bit3:Quiet
	kNkMAIDLimitStillCaptureMode_CH			= 0x0004,   //Bit2:CH
	kNkMAIDLimitStillCaptureMode_C			= 0x0002    //Bit1:C
};

//kNkMAIDCapability_MovieFileType(0x84f6)
enum eNkMAIDMovieFileType
{
	kNkMAIDMovieFileType_MOV = 0,
	kNkMAIDMovieFileType_MP4
};

//kNkMAIDCapability_LiveViewTFTStatus(0x84f8)
enum eNkMAIDLiveViewTFTStatus
{
	kNkMAIDLiveViewTFTStatus_Off,
	kNkMAIDLiveViewTFTStatus_On
};

//kNkMAIDCapability_LiveViewButtonMode(0x84f9)
enum eNkMAIDLiveViewButtonMode
{
	kNkMAIDLiveViewButtonMode_TFTOnOff,
	kNkMAIDLiveViewButtonMode_LiveView
};

//kNkMAIDCapability_ExternalRecordingControl(0x84fa)
enum eNkMAIDExternalRecordingControl
{
	kNkMAIDExternalRecordingControl_Off,
	kNkMAIDExternalRecordingControl_On
};

//kNkMAIDCapability_DetectionPeaking(0x84fb)
enum eNkMAIDDetectionPeaking
{
	kNkMAIDDetectionPeaking_Off,
	kNkMAIDDetectionPeaking_Low,
	kNkMAIDDetectionPeaking_Normal,
	kNkMAIDDetectionPeaking_High,
};

//kNkMAIDCapability_HighlightBrightness(0x84fd)
enum eNkMAIDHighlightBrightness
{
	kNkMAIDHighlightBrightness_180 = 0,
	kNkMAIDHighlightBrightness_191,
	kNkMAIDHighlightBrightness_202,
	kNkMAIDHighlightBrightness_213,
	kNkMAIDHighlightBrightness_224,
	kNkMAIDHighlightBrightness_235,
	kNkMAIDHighlightBrightness_248,
	kNkMAIDHighlightBrightness_255
};

//kNkMAIDCapability_MovieAttenuator(0x84ff)
enum eNkMAIDMovieAttenuator
{
    kNkMAIDMovieAttenuator_disable,
    kNkMAIDMovieAttenuator_enable,
};

//kNkMAIDCapability_DiffractionCompensation(0x8504)
enum eNkMAIDDiffractionCompensation
{
	kNkMAIDDiffractionCompensation_Off,
	kNkMAIDDiffractionCompensation_On
};

//kNkMAIDCapability_VibrationReduction(0x8505)
enum eNkMAIDVibrationReduction
{
	kNkMAIDVibrationReduction_Off,      // Off
	kNkMAIDVibrationReduction_Normal,   // Normal (On)
	kNkMAIDVibrationReduction_Sport     // Sport
};

//kNkMAIDCapability_MovieAutoDistortion(0x8506)
enum eNkMAIDMovieAutoDistortion
{
	kNkMAIDMovieAutoDistortion_Off,
	kNkMAIDMovieAutoDistortion_On
};

//kNkMAIDCapability_MovieVibrationReduction(0x8508)
enum eNkMAIDMovieVibrationReduction
{
	kNkMAIDMovieVibrationReduction_Off		= 0,
	kNkMAIDMovieVibrationReduction_Normal,
	kNkMAIDMovieVibrationReduction_Sport,
	kNkMAIDMovieVibrationReduction_SamePhoto = 100
};

//kNkMAIDCapability_MovieFocusMode(0x8509)
enum eNkMAIDMovieFocusMode
{
	kNkMAIDMovieFocusMode_AFs,
	kNkMAIDMovieFocusMode_AFc,
	kNkMAIDMovieFocusMode_AFf,
	kNkMAIDMovieFocusMode_MF_FIX,
	kNkMAIDMovieFocusMode_MF_SEL
};

//kNkMAIDCapability_MovieVignetteControl(0x850a)
enum eNkMAIDMovieVignetteControl
{
	kNkMAIDMovieVignetteControl_Off		= 0,
	kNkMAIDMovieVignetteControl_Low,
	kNkMAIDMovieVignetteControl_Normal,
	kNkMAIDMovieVignetteControl_High,
	kNkMAIDMovieVignetteControl_SamePhoto = 100
};

//kNkMAIDCapability_MovieDiffractionCompensation(0x850b)
enum eNkMAIDMovieDiffractionCompensation
{
	kNkMAIDMovieDiffractionCompensation_Off,
	kNkMAIDMovieDiffractionCompensation_On
};

//kNkMAIDCapability_HDRSaveIndividualImages(0x850c)
enum eNkMAIDHDRSaveIndividualImages
{
	kNkMAIDHDRSaveIndividualImages_Off,
	kNkMAIDHDRSaveIndividualImages_On
};

//kNkMAIDCapability_RecordTimeCodes(0x850e)
enum eNkMAIDRecordTimeCodes
{
    kNkMAIDRecordTimeCodes_Off,
    kNkMAIDRecordTimeCodes_On,
    kNkMAIDRecordTimeCodes_OnWithHDMIOutput
};

//kNkMAIDCapability_CountUpMethod(0x850f)
enum eNkMAIDCountUpMethod
{
    kNkMAIDCountUpMethod_RecordRun,
    kNkMAIDCountUpMethod_FreeRun
};

//kNkMAIDCapability_DropFrame(0x8511)
enum eNkMAIDDropFrame
{
    kNkMAIDDropFrame_Off,
    kNkMAIDDropFrame_On
};

//kNkMAIDCapability_MovieLogOutput(0x8512)
enum eNkMAIDMovieLogOutput
{
    kNkMAIDMovieLogOutput_NotOutputting,
    kNkMAIDMovieLogOutput_Outputting,
};

//kNkMAIDCapability_MovieLogSetting(0x8513)
enum eNkMAIDMovieLogSetting
{
    kNkMAIDMovieLogSetting_Off,
    kNkMAIDMovieLogSetting_OnCannotRecToCard,
    kNkMAIDMovieLogSetting_NLog,
    kNkMAIDMovieLogSetting_HDR
};

//kNkMAIDCapability_HDMIOutputDataDepth(0x8514)
enum eNkMAIDHDMIOutputDataDepth
{
    kNkMAIDHDMIOutputDataDepth_8bit,
    kNkMAIDHDMIOutputDataDepth_10bit
};

//kNkMAIDCapability_LowLightAF(0x8515)
enum eNkMAIDLowLightAF
{
    kNkMAIDLowLightAF_Off,
    kNkMAIDLowLightAF_On
};

//kNkMAIDCapability_ApplyLiveViewSetting(0x8516)
enum eNkMAIDApplyLiveViewSetting
{
    kNkMAIDApplyLiveViewSetting_Off,
    kNkMAIDApplyLiveViewSetting_On
};

//kNkMAIDCapability_LiveViewImageStatus(0x8517)
enum eNkMAIDLiveViewImageStatus
{
    kNkMAIDLiveViewImageStatus_CannotAcquire,
    kNkMAIDLiveViewImageStatus_CanAcquire,
};

//kNkMAIDCapability_LiveViewZoomArea(0x8518)
enum eNkMAIDLiveViewZoomArea
{
    kNkMAIDLiveViewZoomArea_0    =    0,
    kNkMAIDLiveViewZoomArea_360  =  360,
    kNkMAIDLiveViewZoomArea_512  =  512,
    kNkMAIDLiveViewZoomArea_640  =  640,
    kNkMAIDLiveViewZoomArea_720  =  720,
    kNkMAIDLiveViewZoomArea_1024 = 1024,
    kNkMAIDLiveViewZoomArea_1280 = 1280,
    kNkMAIDLiveViewZoomArea_1440 = 1440,
    kNkMAIDLiveViewZoomArea_1448 = 1448,
    kNkMAIDLiveViewZoomArea_2048 = 2048,
    kNkMAIDLiveViewZoomArea_2560 = 2560,
    kNkMAIDLiveViewZoomArea_2896 = 2896,
};

//kNkMAIDCapability_MovieAfSpeedWhenToApply(0x851B)
enum eNkMAIDMovieAfSpeedWhenToApply
{
    kNkMAIDMovieAfSpeedWhenToApply_Always,
    kNkMAIDMovieAfSpeedWhenToApply_RecOnly
};

//kNkMAIDCapability_GetManualSettingLensData(0x851D)
enum eNkMAIDManualSettingLensData
{
    eNkMAIDManualSettingLensData_ALL  = 0xFFFFFFFF , // All .
    eNkMAIDManualSettingLensData_1    = 0  , // LensID No.1.
    eNkMAIDManualSettingLensData_2    = 1  , // LensID No.2.
    eNkMAIDManualSettingLensData_3    = 2  , // LensID No.3.
    eNkMAIDManualSettingLensData_4    = 3  , // LensID No.4.
    eNkMAIDManualSettingLensData_5    = 4  , // LensID No.5.
    eNkMAIDManualSettingLensData_6    = 5  , // LensID No.6.
    eNkMAIDManualSettingLensData_7    = 6  , // LensID No.7.
    eNkMAIDManualSettingLensData_8    = 7  , // LensID No.8.
    eNkMAIDManualSettingLensData_9    = 8  , // LensID No.9.
    eNkMAIDManualSettingLensData_10   = 9  , // LensID No.10.
    eNkMAIDManualSettingLensData_11   = 10 , // LensID No.11.
    eNkMAIDManualSettingLensData_12   = 11 , // LensID No.12.
    eNkMAIDManualSettingLensData_13   = 12 , // LensID No.13.
    eNkMAIDManualSettingLensData_14   = 13 , // LensID No.14.
    eNkMAIDManualSettingLensData_15   = 14 , // LensID No.15.
    eNkMAIDManualSettingLensData_16   = 15 , // LensID No.16.
    eNkMAIDManualSettingLensData_17   = 16 , // LensID No.17.
    eNkMAIDManualSettingLensData_18   = 17 , // LensID No.18.
    eNkMAIDManualSettingLensData_19   = 18 , // LensID No.19.
    eNkMAIDManualSettingLensData_20   = 19 , // LensID No.20.

};

//kNkMAIDCapability_StillCaptureModeSaveFrame(0x851E)
enum  eNkMAIDStillCaptureModeSaveFrame
{
	eNkMAIDStillCaptureModeSaveFrame_S  = 1,
	eNkMAIDStillCaptureModeSaveFrame_CH = 2,
};

//kNkMAIDCapability_ChangeMonitorOffStatus(0x8520)
enum eNkMAIDChangeMonitorOffStatus
{
    kNkMAIDChangeMonitorOffStatus_CancelForcedTurnOff,
    kNkMAIDChangeMonitorOffStatus_ForcedTurnOff
};

//kNkMAIDCapability_ExtendedShutterSpeedsManual(0x8522)
enum eNkMAIDExtendedShutterSpeedsManual
{
	kNkMAIDExtendedShutterSpeedsManual_Off,
	kNkMAIDExtendedShutterSpeedsManual_On
};

// kNkMAIDCapability_QuietReleaseMode(0x8523)
enum eNkMAIDQuietReleaseMode
{
	kNkMAIDQuietReleaseMode_Singleframe = 0,
	kNkMAIDQuietReleaseMode_1fps,
	kNkMAIDQuietReleaseMode_2fps,
	kNkMAIDQuietReleaseMode_3fps,
	kNkMAIDQuietReleaseMode_4fps,
	kNkMAIDQuietReleaseMode_5fps
};

// kNkMAIDCapability_AutoAreaAFFaceDetection(0x8524)
enum eNkMAIDAutoAreaAFFaceDetection
{
	kNkMAIDAutoAreaAFFaceDetection_Off,
	kNkMAIDAutoAreaAFFaceDetection_On
};

// kNkMAIDCapability_GroupAreaFaceDetection(0x8526)
enum eNkMAIDGroupAreaFaceDetection
{
	kNkMAIDGroupAreaFaceDetection_Off = 0,
	kNkMAIDGroupAreaFaceDetection_On,
};

// kNkMAIDCapability_SinglePointAFWatchArea(0x8527)
enum eNkMAIDSinglePointAFWatchArea
{
	kNkMAIDSinglePointAFWatchArea_Normal = 0,
	kNkMAIDSinglePointAFWatchArea_Wide,
};



// kNkMAIDCapability_RearControlPanel(0x8528)
enum eNkMAIDRearControlPanel
{
	kNkMAIDRearControlPanel_ReleaseMode = 0,
	kNkMAIDRearControlPanel_FrameCount,
};

// kNkMAIDCapability_FlushBurstPrioritize(0x8529)
enum eNkMAIDFlushBurstPrioritize
{
	kNkMAIDFlushBurstPrioritize_FrameAdvanceRate = 0,
	kNkMAIDFlushBurstPrioritize_PreciseFlashControl,
};

// kNkMAIDCapability_BracketingBurstOptions(0x852a)
enum eNkMAIDBracketingBurstOptions
{
	kNkMAIDBracketingBurstOptions_Disable = 0,
	kNkMAIDBracketingBurstOptions_Enable,
};

// kNkMAIDCapability_3DTrackingFaceDetection(0x852b)
enum eNkMAID3DTrackingFaceDetection
{
	kNkMAID3DTrackingFaceDetection_Off = 0,
	kNkMAID3DTrackingFaceDetection_On,
};

//kNkMAIDCapability_AutoAreaAFStartingPoint(0x852c)
enum eNkMAIDAutoAreaAFStartingPoint
{
	kNkMAIDAutoAreaAFStartingPoint_Off = 0,
	kNkMAIDAutoAreaAFStartingPoint_On,
};

//kNkMAIDCapability_CustomGroupingsC1(0x852d)
//kNkMAIDCapability_CustomGroupingsC2(0x8532)
enum eNkMAIDCustomGroupings
{
	kNkMAIDCustomGroupings_1x5 = 0x0105,
	kNkMAIDCustomGroupings_1x7 = 0x0107,
	kNkMAIDCustomGroupings_3x3 = 0x0303,
	kNkMAIDCustomGroupings_3x5 = 0x0305,
	kNkMAIDCustomGroupings_3x7 = 0x0307,
	kNkMAIDCustomGroupings_5x1 = 0x0501,
	kNkMAIDCustomGroupings_5x3 = 0x0503,
	kNkMAIDCustomGroupings_5x5 = 0x0505,
	kNkMAIDCustomGroupings_5x7 = 0x0507,
	kNkMAIDCustomGroupings_7x1 = 0x0701,
	kNkMAIDCustomGroupings_7x3 = 0x0703,
	kNkMAIDCustomGroupings_7x5 = 0x0705,
	kNkMAIDCustomGroupings_7x7 = 0x0707,
	kNkMAIDCustomGroupings_11x1 = 0x0B01,
	kNkMAIDCustomGroupings_11x3 = 0x0B03,
	kNkMAIDCustomGroupings_15x1 = 0x0F01,
	kNkMAIDCustomGroupings_15x3 = 0x0F03,
};

//kNkMAIDCapability_LockExposureEx(0x8530)
enum eNkMAIDLockExposureEx
{
	kNkMAIDLockExposureEx_Unlock = 0,
	kNkMAIDLockExposureEx_AELock,
	kNkMAIDLockExposureEx_AELock_AWBLock
};

//kNkMAIDCapability_AutoFlashIsoSensitivityControl(0x8534)
enum eNkMAIDAutoFlashIsoSensitivityControl
{
    kNkMAIDAutoFlashIsoSensitivityControl_SubjectAndBackground = 0,
    kNkMAIDAutoFlashIsoSensitivityControl_Subject,
};


///////////////////////////////////////////////////////////////////////////////
// Definition of structure

// for kNkMAIDCapability_WBPresetData
typedef struct tagNkMAIDWBPresetData
{
    ULONG  ulPresetNumber;      // Preset Number
    ULONG  ulPresetGain;        // Preset Gain
    ULONG  ulThumbnailSize;     // Thumbnail size of pThumbnailData
    ULONG  ulThumbnailRotate;   // One of eNkMAIDThumbnailRotate
    void*  pThumbnailData;      // The pointer to Thumbnail Data
} NkMAIDWBPresetData, FAR* LPNkMAIDWBPresetData;

// for kNkMAIDCapability_PictureControlData
typedef struct tagNkMAIDPicCtrlData
{
    ULONG  ulPicCtrlItem;   // picture control item
    ULONG  ulSize;          // the data sizer of pData
    bool   bModifiedFlag;   // Flag to set New or current 
    void*  pData;           // The pointer to picture control data
} NkMAIDPicCtrlData, FAR* LPNkMAIDPicCtrlData;

// for kNkMAIDCapability_GetPicCtrlInfo
typedef struct tagNkMAIDGetPicCtrlInfo
{
    ULONG  ulPicCtrlItem;   // picture control item
    ULONG  ulSize;          // the data sizer of pData
    void*  pData;           // The pointer to Quick Adjust Param
} NkMAIDGetPicCtrlInfo, FAR* LPNkMAIDGetPicCtrlInfo;

// for kNkMAIDCapability_DateCounterData
typedef struct tagNkMAIDDateCounterData
{
    wchar_t wcDate1[9]; // First date
    wchar_t wcDate2[9]; // Second date
    wchar_t wcDate3[9]; // Third date
} NkMAIDDateCounterData, FAR* LPNkMAIDDateCounterData;

// for kNkMAIDCapability_GetVideoImage
typedef struct tagNkMAIDGetVideoImage
{
    ULONG  ulType;          // one of eNkMAIDArrayType
    ULONG  ulOffset;        // Offset
    ULONG  ulReadSize;      // size of get data
    ULONG  ulDataSize;      // size of "pData" 
    LPVOID pData;           // allocated by the client
} NkMAIDGetVideoImage, FAR* LPNkMAIDGetVideoImage;

#ifdef _MAID3D1_H
// for kNkMAIDCapability_TerminateCapture
typedef struct tagNkMAIDTerminateCapture
{
    ULONG  ulParameter1;    // unused(set 0)
    ULONG  ulParameter2;    // unused(set 0)
} NkMAIDTerminateCapture, FAR* LPNkMAIDTerminateCapture;

#else
// for kNkMAIDCapability_TerminateCapture
typedef struct tagNkMAIDTerminateCapture
{
    ULONG  ulKeyCode;       // Recognize client
    ULONG  ulReleaseTime;   // The shooting time specified 100msec unit
} NkMAIDTerminateCapture, FAR* LPNkMAIDTerminateCapture;
#endif

// for kNkMAIDCapability_ActiveSelectionSelectedPictures
typedef struct tagNkMAIDActiveSelectionSelectedPictures
{
    ULONG  ulPicIDNum;
    ULONG  *pulSelPicIDs;
} NkMAIDActiveSelectionSelectedPictures, FAR* LPNkMAIDActiveSelectionSelectedPictures;

// for kNkMAIDCapability_IPTCPresetInfo.
typedef struct tagNkMAIDIPTCPresetInfo
{
    ULONG  ulPresetNo;
    ULONG  ulSize;
    void*  pData;
} NkMAIDIPTCPresetInfo, FAR* LPNkMAIDIPTCPresetInfo;

// for kNkMAIDCapability_GetSBHandles.
typedef struct tagNkMAIDGetSBHandles
{
    ULONG  ulSBGroupID;
    ULONG  ulNumber;
    ULONG  ulSize;
    void*  pData;
} NkMAIDGetSBHandles, FAR* LPNkMAIDGetSBHandles;

// for kNkMAIDCapability_GetSBAttrDesc.
typedef struct tagNkMAIDGetSBAttrDesc
{ 
    ULONG  ulSBHandle;
    ULONG  ulSBAttrID;
    ULONG  ulSize;
    void*  pData;
} NkMAIDGetSBAttrDesc, FAR* LPNkMAIDGetSBAttrDesc;

// for kNkMAIDCapability_SBAttrValue.
typedef struct tagNkMAIDSBAttrValue
{
    ULONG  ulSBHandle;
    ULONG  ulSBAttrID;
    ULONG  ulSize;
    void*  pData;
} NkMAIDSBAttrValue, FAR* LPNkMAIDSBAttrValue;

// for kNkMAIDCapability_GetSBGroupAttrDesc.
typedef struct tagNkMAIDGetSBGroupAttrDesc
{
    ULONG  ulSBGroupID;
    ULONG  ulSBGroupAttrID;
    ULONG  ulSize;
    void*  pData;
} NkMAIDGetSBGroupAttrDesc, FAR* LPNkMAIDGetSBGroupAttrDesc;

// for kNkMAIDCapability_SBGroupAttrValue.
typedef struct tagNkMAIDSBGroupAttrValue
{
    ULONG  ulSBGroupID;
    ULONG  ulSBGroupAttrID;
    ULONG  ulSize;
    void*  pData;
} NkMAIDSBGroupAttrValue, FAR* LPNkMAIDSBGroupAttrValue;

// for kNkMAIDCapability_TestFlash.
typedef struct tagNkMAIDTestFlash
{
    ULONG  ulType;
    ULONG  ulParam;
} NkMAIDTestFlash, FAR* LPNkMAIDTestFlash;

// for kNkMAIDCapability_LensTypeNikon1.
typedef struct tagNkMAIDLensTypeNikon1
{
    ULONG  ulLensType1;
    ULONG  ulLensType2;
} NkMAIDLensTypeNikon1, FAR* LPNkMAIDLensTypeNikon1;

// for kNkMAIDCapability_GetRecordingInfo.
typedef struct tagNkMAIDGetRecordingInfo
{
	ULONG		ulIndexOfMov;
	ULONG		ulTotalMovCount;
	NK_UINT_64  ullTotalMovSize;
} NkMAIDGetRecordingInfo, FAR* LPNkMAIDGetRecordingInfo;

// for kNkMAIDCapability_GetVideoImageEx.
typedef struct tagNkMAIDGetVideoImageEx
{
	NK_UINT_64  ullOffset;
	NK_UINT_64  ullReadSize;
	NK_UINT_64  ullDataSize;
	LPVOID  pData;
} NkMAIDGetVideoImageEx, FAR* LPNkMAIDGetVideoImageEx;

// for kNkMAIDCapability_TimeCodeOrigin.
typedef struct tagNkMAIDTimeCodeOrigin
{
    UCHAR  ucTimeCodeInfo;
    UCHAR  ucTimeCode[3];
    ULONG  ulFrame;
} NkMAIDTimeCodeOrigin, FAR* LPNkMAIDTimeCodeOrigin;

//for kNkMAIDCapability_TrackingAFArea.
typedef struct tagNkMAIDTrackingAFArea
{
    ULONG  ulTrackingStatus;
    NkMAIDPoint stAfPoint;
} NkMAIDTrackingAFArea, FAR* LPNkMAIDTrackingAFArea;

//for kNkMAIDCapability_GetManualSettingLensData.
typedef struct tagGetManualSettingLensData
 {
    ULONG  ulLensID;
    ULONG  ulSize;
    void*  pData;
} NkMAIDGetManualSettingLensData, FAR* LPNkMAIDGetManualSettingLensData;

// for Event call back parameter
typedef struct tagNkMAIDEventParam
{
    ULONG  ulEvent;     // one of eNkMAIDEvent
    ULONG  ulElements;  // total number of valid params
    ULONG  ulParam[2];  // event parameter 
} NkMAIDEventParam, FAR* LPNkMAIDEventParam;

///////////////////////////////////////////////////////////////////////////////
#endif
