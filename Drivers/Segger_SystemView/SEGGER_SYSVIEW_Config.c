/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co. KG                 *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 2015  SEGGER Microcontroller GmbH & Co. KG               *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* * This software may in its unmodified form be freely redistributed *
*   in source form.                                                  *
* * The source code may be modified, provided the source code        *
*   retains the above copyright notice, this list of conditions and  *
*   the following disclaimer.                                        *
* * Modified versions of this software in source or linkable form    *
*   may not be distributed without prior consent of SEGGER.          *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND     *
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  *
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A        *
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL               *
* SEGGER Microcontroller BE LIABLE FOR ANY DIRECT, INDIRECT,         *
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES           *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS    *
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS            *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,       *
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING          *
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS *
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.       *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: V2.26                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File        : SEGGER_SYSVIEW_Config.c
Purpose     : Setup configuration of SystemView.
*/
#include <stddef.h> /* for NULL */
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_SYSVIEW_Conf.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
%if (%CPUDB_prph_has_feature(CPU,SDK_SUPPORT) = 'yes') | (defined(KinetisSDKenabled) & %KinetisSDKenabled='yes') %- TRUE/FALSE not defined for SDK projects
#define SYSVIEW_USING_KINETIS_SDK                                    %>50 1 /* 1: project is a Kinetis SDK Processor Expert project; 0: No Kinetis Processor Expert project */
%else
#define SYSVIEW_USING_KINETIS_SDK                                    %>50 0 /* 1: project is a Kinetis SDK Processor Expert project; 0: No Kinetis Processor Expert project */
%endif
%if defined(OperatingSystemId) & %OperatingSystemId='FreeRTOS'
#define SYSVIEW_USING_FREERTOS                                       %>50 1 /* 1: using FreeRTOS; 0: Bare metal */
%else
#define SYSVIEW_USING_FREERTOS                                       %>50 0 /* 1: using FreeRTOS; 0: Bare metal */
%endif

#if !SYSVIEW_USING_KINETIS_SDK
  #include "%ProcessorModule.h"
#endif
#if SYSVIEW_USING_FREERTOS
  #include "FreeRTOS.h"
#endif

// The application name to be displayed in SystemViewer
#define SYSVIEW_APP_NAME        %SysViewAppName

// The operating system, if any
#if SYSVIEW_USING_FREERTOS
  extern const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI;
  #define SYSVIEW_OS_NAME         "FreeRTOS"
  #define SYSVIEW_OS_API          &SYSVIEW_X_OS_TraceAPI
#else
  #define SYSVIEW_OS_NAME         "Bare-metal"
  #define SYSVIEW_OS_API          NULL
#endif

// The target device name
#define SYSVIEW_DEVICE_NAME     %SysViewDeviceName

// System Frequency. SystemcoreClock is used in most CMSIS compatible projects.
#if SYSVIEW_USING_KINETIS_SDK
  /* The SDK variable SystemCoreClock contains the current clock speed */
  extern unsigned int SystemCoreClock;
  #define SYSVIEW_CPU_FREQ                                       %>50 (SystemCoreClock) /* CPU clock frequency */
#elif SYSVIEW_USING_FREERTOS
  #define SYSVIEW_CPU_FREQ                                       %>50 configCPU_CLOCK_HZ
#else
  #define SYSVIEW_CPU_FREQ                                       %>50 CPU_CORE_CLK_HZ /* CPU core clock defined in %ProcessorModule.h */
#endif /* SYSVIEW_USING_KINETIS_SDK */

// Frequency of the timestamp. Must match SEGGER_SYSVIEW_Conf.h
#define SYSVIEW_TIMESTAMP_FREQ  (SYSVIEW_CPU_FREQ >> 4)

// The lowest RAM address used for IDs (pointers)
#define SYSVIEW_RAM_BASE        (0x%#l%SysViewRamBase)

#if 1 /* << EST */
#define portNVIC_SYSTICK_LOAD_REG           (*((volatile unsigned long *)0xe000e014)) /* SYST_RVR, SysTick reload value register */
#define portNVIC_SYSTICK_CURRENT_VALUE_REG  (*((volatile unsigned long *)0xe000e018)) /* SYST_CVR, SysTick current value register */

#define TICK_NOF_BITS               24
#define COUNTS_UP                   0 /* SysTick is counting down to zero */
#define SET_TICK_DURATION(val)      portNVIC_SYSTICK_LOAD_REG = val
#define GET_TICK_DURATION()         portNVIC_SYSTICK_LOAD_REG
#define GET_TICK_CURRENT_VAL(addr)  *(addr)=portNVIC_SYSTICK_CURRENT_VALUE_REG


uint32_t SEGGER_uxGetTickCounterValue(void) {
  uint32_t val;

  GET_TICK_CURRENT_VAL(&val);
  return val;
}
#endif

#if SEGGER_SYSVIEW_CORE == SEGGER_SYSVIEW_CORE_CM0 /* << EST */
//
// SEGGER_SYSVIEW_TickCnt has to be defined in the module which
// handles the SysTick and must be incremented in the SysTick
// handler before any SYSVIEW event is generated.
//
// Example in embOS RTOSInit.c:
//
// unsigned int SEGGER_SYSVIEW_TickCnt; // <<-- Define SEGGER_SYSVIEW_TickCnt.
// void SysTick_Handler(void) {
// #if OS_PROFILE
//   SYSVIEW_TickCnt++;                 // <<-- Increment SEGGER_SYSVIEW_TickCnt before calling OS_EnterNestableInterrupt.
// #endif
//   OS_EnterNestableInterrupt();
//   OS_TICK_Handle();
//   OS_LeaveNestableInterrupt();
// }
//
extern unsigned int SEGGER_SYSVIEW_TickCnt;

#ifndef SCB_ICSR
#define SCB_ICSR  (*(volatile U32*) (0xE000ED04uL)) // Interrupt Control State Register
#endif

#ifndef SCB_ICSR_PENDSTSET_MASK
#define SCB_ICSR_PENDSTSET_MASK     (1UL << 26)     // SysTick pending bit
#endif

#ifndef SYST_RVR
#define SYST_RVR  (*(volatile U32*) (0xE000E014uL)) // SysTick Reload Value Register
#endif

#ifndef SYST_CVR
#define SYST_CVR  (*(volatile U32*) (0xE000E018uL)) // SysTick Current Value Register
#endif

/*********************************************************************
*
*       SEGGER_SYSVIEW_X_GetTimestamp()
*
* Function description
*   Returns the current timestamp in ticks using the system tick
*   count and the SysTick counter.
*   All parameters of the SysTick have to be known and are set via
*   configuration defines on top of the file.
*
* Return value
*   The current timestamp.
*
* Additional information
*   SEGGER_SYSVIEW_X_GetTimestamp is always called when interrupts are
*   disabled. Therefore locking here is not required.
*/
U32 SEGGER_SYSVIEW_X_GetTimestamp(void) {
  U32 TickCount;
  U32 Cycles;
  U32 CyclesPerTick;
  //
  // Get the cycles of the current system tick.
  // SysTick is down-counting, subtract the current value from the number of cycles per tick.
  //
  CyclesPerTick = SYST_RVR + 1;
  Cycles = (CyclesPerTick - SYST_CVR);
  //
  // Get the system tick count.
  //
  TickCount = SEGGER_SYSVIEW_TickCnt;
  //
  // If a SysTick interrupt is pending increment the TickCount
  //
  if ((SCB_ICSR & SCB_ICSR_PENDSTSET_MASK) != 0) {
    TickCount++;
  }
  Cycles += TickCount * CyclesPerTick;

  return Cycles;
}
#endif /* << EST */

/********************************************************************* 
*
*       _cbSendSystemDesc()
*
*  Function description
*    Sends SystemView description strings.
*/
static void _cbSendSystemDesc(void) {
  SEGGER_SYSVIEW_SendSysDesc("N="SYSVIEW_APP_NAME",O="SYSVIEW_OS_NAME",D="SYSVIEW_DEVICE_NAME);
  SEGGER_SYSVIEW_SendSysDesc("I#15=SysTick");
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
void SEGGER_SYSVIEW_Conf(void) {
#if SYSVIEW_USING_FREERTOS
  #if configUSE_TRACE_HOOKS /* using Percepio Trace */
    #error "Percepio Trace is enabled, this might conflict with Segger System View."
  #endif
#endif
  SEGGER_SYSVIEW_Init(SYSVIEW_TIMESTAMP_FREQ, SYSVIEW_CPU_FREQ, 
      SYSVIEW_OS_API, _cbSendSystemDesc);
  SEGGER_SYSVIEW_SetRAMBase(SYSVIEW_RAM_BASE);
}

/*************************** End of file ****************************/