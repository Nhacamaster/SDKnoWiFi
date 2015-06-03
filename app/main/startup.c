/******************************************************************************
 * FileName: app_main.c
 * Description: Alternate SDK (libmain.a)
 * Author: PV`
 * (c) PV` 2015
*******************************************************************************/
#include "user_config.h"
#include "bios.h"
#include "startup.h"
#include "timer_us.h"
#include "wdt.h"
#include "user_interface.h"
//=============================================================================
// Data
//-----------------------------------------------------------------------------
extern uint8_t _bss_start, _bss_end;
//=============================================================================
// Init data (flash)
//-----------------------------------------------------------------------------
//=============================================================================
// extern funcs
//-----------------------------------------------------------------------------
extern void user_init(void);
extern void call_user_start(void);
//=============================================================================
// IRAM code
//-----------------------------------------------------------------------------
//=============================================================================
// FLASH code (ICACHE_FLASH_ATTR)
//-----------------------------------------------------------------------------
//=============================================================================
// startup()
//-----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR startup(void)
{
	ets_set_user_start(call_user_start);
	// ������� �������� bss //	mem_clr_bss();
	uint8 * ptr = &_bss_start;
	while(ptr < &_bss_end) *ptr++ = 0;
	//
	if(rom_i2c_readReg(103,4,1) == 8) { // 8: 40MHz, 136: 26MHz
			// set 80MHz PLL CPU
			rom_i2c_writeReg(103,4,1,136);
			rom_i2c_writeReg(103,4,2,145);
			UART0_CLKDIV = (UART0_CLKDIV * 394) >> 8;
			UART1_CLKDIV = (UART1_CLKDIV * 394) >> 8;
			ets_delay_us(150);
//			ets_update_cpu_frequency(80); // set clk cpu (rom-bios set default 80)
	}
	//
	_xtos_set_exception_handler(9, default_exception_handler);
	_xtos_set_exception_handler(0, default_exception_handler);
	_xtos_set_exception_handler(2, default_exception_handler);
	_xtos_set_exception_handler(3, default_exception_handler);
	_xtos_set_exception_handler(28, default_exception_handler);
	_xtos_set_exception_handler(29, default_exception_handler);
	_xtos_set_exception_handler(8, default_exception_handler);
	//
	ets_timer_init();
#ifdef USE_US_TIMER
	system_timer_init_us();
#endif
	// print sdk version
#ifdef DEBUG_UART
	os_printf("\nOpenLoaderSDK v1.3\n");
	//
	os_print_reset_error(); // print fatal errors/restart ����� ��������� ������, ��������� �������. ��. � ������ wdt
#endif
	//
	wdt_init();
	//
	vPortFree(pvPortMalloc(8)); // init mem_manager ����� �� �������� xPortGetFreeHeapSize()
	//
	user_init();
	//
}


