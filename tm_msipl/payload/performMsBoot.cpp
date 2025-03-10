#include "performBoot.h"

#include <psptypes.h>
#include <type_traits>

#include <cache.h>
#include <serial.h>
#include <noBootromPatch.h>
#include <cstring>

#include <tm_common.h>

//For reference, code that boots original format TM IPLs

// void performMsBoot(FIL &fp) {
// 	constexpr u32 BUFFER_START_ADDR = 0x40E0000;
// 	constexpr u32 IPL_START_ADDR = 0x40F0000;

// 	constexpr u32 PAYLOAD_MAX_SIZE = 0x3000;
// 	constexpr u32 PAYLOAD_BSS_SIZE = 0x3000;
// 	constexpr u32 IPL_MAX_SIZE = 0x1000000;

// 	auto const g_PayloadLocation = reinterpret_cast<u8*>(BUFFER_START_ADDR);
// 	auto const g_PayloadDataLocation = reinterpret_cast<u8*>(BUFFER_START_ADDR+PAYLOAD_MAX_SIZE);
// 	auto const g_IplLocation = reinterpret_cast<u8*>(IPL_START_ADDR);

// 	using v_v_function_t = std::add_pointer_t<void()>;
// 	auto const jumpToIpl = reinterpret_cast<v_v_function_t const>(IPL_START_ADDR);

// 	u32 bytes_read;

// 	f_read(&fp, g_PayloadLocation, PAYLOAD_MAX_SIZE, &bytes_read);
// 	memset(g_PayloadDataLocation, 0, PAYLOAD_BSS_SIZE);
// 	f_read(&fp, g_IplLocation, IPL_MAX_SIZE, &bytes_read);

// 	f_close(&fp);

// #ifdef RESET_EXPLOIT
// 	noBootromPatch(g_IplLocation);
// #endif
	
// 	if constexpr (isDebug) {
// 		printf("Clear caches\n");
// 	}

// 	iplKernelDcacheWritebackInvalidateAll();
// 	iplKernelIcacheInvalidateAll();

// 	if constexpr (isDebug) {
// 		printf("Boot\n");
// 	}

// 	jumpToIpl();

// 	__builtin_unreachable();
// }

constexpr inline u32 BUFFER_START_ADDR = 0x40E0000;
constexpr inline u32 IPL_MAX_SIZE = 0x1000000;

auto const g_PayloadLocation = reinterpret_cast<u8*>(BUFFER_START_ADDR);

using v_v_function_t = std::add_pointer_t<void()>;
auto const jumpToPayload = reinterpret_cast<v_v_function_t const>(BUFFER_START_ADDR);

void performMsBoot(FIL &fp) {
	u32 bytes_read;

	f_read(&fp, g_PayloadLocation, IPL_MAX_SIZE, &bytes_read);

	f_close(&fp);
	
	if constexpr (isDebug) {
		printf("Clear caches\n");
	}

	iplKernelDcacheWritebackInvalidateAll();
	iplKernelIcacheInvalidateAll();

	sdkSync();

	if constexpr (isDebug) {
		printf("Boot\n");
	}

#ifdef RESET_EXPLOIT
	tmTempSetNoBootrom(true);
#else
	tmTempSetNoBootrom(false);
#endif

	jumpToPayload();

	__builtin_unreachable();
}
