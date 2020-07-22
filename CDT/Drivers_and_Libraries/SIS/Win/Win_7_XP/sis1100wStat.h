#ifndef SIS1100WSTAT_H_
#define SIS1100WSTAT_H_

#define API_RETURNCODE_START 0x0
#define API_RETURNCODE_LOCAL 0x100
#define API_RETURNCODE_REMOTE 0x200

typedef enum _SIS1100W_STATUS{
	// 0: general return codes
	Stat1100Success = API_RETURNCODE_START,
	Stat1100NullArgument,
	Stat1100InvalidDeviceIndex,
	Stat1100ErrorDeviceOpen,
	Stat1100ErrorDeviceReset,
	Stat1100ErrorDeviceClose,
	Stat1100ErrorBarMap,
	Stat1100ErrorBarUnmap,
	Stat1100ErrorAllocUserHandle,
	Stat1100ErrorRegisterAccess,
	Stat1100ErrorFifoFlush,
	Stat1100ErrorNoRemoteDevice,
	Stat1100InvalidSizeValue,
	Stat1100ErrorIrqSetup,
	Stat1100ErrorIrqWait,
	Stat1100ErrorIrqAlreadyRegistered,
	Stat1100ErrorIrqNoSetup,
	Stat1100ErrorIrqTimeout,
	Stat1100ErrorCommonBufferInfo,
	Stat1100ErrorCommonBufferSize,
	Stat1100ErrorCommonBufferMap,
	Stat1100ErrorCommonBufferUnmap,
	Stat1100ErrorNoLink,
	Stat1100ErrorNoRing,
	Stat1100ErrorRingTimeout,
	// 0x100: local side errors
	Stat1100LESynch = API_RETURNCODE_LOCAL + 0x1, // starts at 0x101
	Stat1100LENrdy,
	Stat1100LEXoff,
	Stat1100LEResource,
	Stat1100LEDlock, 
	Stat1100LETimeout = API_RETURNCODE_LOCAL + 0x7,
	// 0x200: remote side errors
	Stat1100RENrdy = API_RETURNCODE_REMOTE + 0x2, // starts at 0x202
	Stat1100REProt = API_RETURNCODE_REMOTE + 0x6,
	Stat1100RETimeout,
	Stat1100REBerr,
	Stat1100REFerr,
	Stat1100REBusErr = API_RETURNCODE_REMOTE + 0x11 // SIS310x Berr at 0x211
}SIS1100W_STATUS;

#endif