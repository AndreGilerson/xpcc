// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_ATOMIC__LOCK_HPP
#define	XPCC_ATOMIC__LOCK_HPP

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>

namespace xpcc
{
	namespace atomic
	{
		/**
		 * \ingroup	atomic
		 * \brief		Critical section
		 *  
		 * Typical usage:
		 * \code
		 * function()
		 * {
		 *     // some code with interrupts enabled
		 *     
		 *     {
		 *         atomic::Lock lock;
		 *         
		 *         // code which should be executed with disabled interrupts.
		 *         
		 *         // with the end of this block the lock object is destroyed
		 *         // and the interrupts are reenabled.
		 *     }
		 *     
		 *     // other code with interrupts enabled
		 * }
		 * \endcode
		 * 
		 * \warning	Interrupts should be disabled the shortest possible time!
		 */
		class Lock
		{
		public:
			/**
			 * \brief	Constructor
			 * 
			 * Disables interrupts.
			 */
			xpcc_always_inline
			Lock();
			
			/**
			 * \brief	Destructor
			 * 
			 * Restore the interrupt settings.
			 */
			xpcc_always_inline
			~Lock();
		
		private:
#if defined(XPCC__CPU_AVR)
			uint8_t sreg;
#elif defined(XPCC__CPU_ARM)
			uint32_t cpsr;
#endif
		};
		
		/**
		 * \ingroup	atomic
		 * \brief	Opposite to atomic::Lock
		 * 
		 * Use this class to create a block of code with interrupts enabled
		 * inside a locked block.
		 * 
		 * Most of the time you won't need this class. But on some rare
		 * times it is useful. The xpcc::Scheduler is an example for that.
		 */
		class Unlock
		{
		public:
			/**
			 * \brief	Constructor
			 * 
			 * Enable interrupts
			 */
			xpcc_always_inline
			Unlock();
			
			/**
			 * \brief	Destructor
			 * 
			 * Restore the interrupt settings.
			 */
			xpcc_always_inline
			~Unlock();
		
		private:
#if defined(XPCC__CPU_AVR)
			uint8_t sreg;
#elif defined(XPCC__CPU_ARM)
			uint32_t cpsr;
#endif
		};
	}
}

#ifdef XPCC__CPU_AVR

	#include <avr/interrupt.h>

	xpcc::atomic::Lock::Lock() :
		sreg(SREG)
	{
		cli();
	}

	xpcc::atomic::Lock::~Lock()
	{
		SREG = sreg;
		__asm__ volatile ("" ::: "memory");
	}

	// ------------------------------------------------------------------------

	xpcc::atomic::Unlock::Unlock() :
		sreg(SREG)
	{
		sei();
	}

	xpcc::atomic::Unlock::~Unlock()
	{
		SREG = sreg;
		__asm__ volatile ("" ::: "memory");
	}

#elif defined(XPCC__CPU_CORTEX_M0) || defined(XPCC__CPU_CORTEX_M3) || defined(XPCC__CPU_CORTEX_M4) || defined(XPCC__CPU_CORTEX_M7)

	xpcc_always_inline
	xpcc::atomic::Lock::Lock()
	{
		// cpsr = __get_PRIMASK();
		// __disable_irq();
		// disable interrupts -> PRIMASK=1
		// enable interrupts -> PRIMASK=0
		asm volatile (
				"mrs %0, PRIMASK"	"\n\t"
				"cpsid i"
				: "=&r" (cpsr)
				:: "memory");
	}

	xpcc_always_inline
	xpcc::atomic::Lock::~Lock()
	{
		// __set_PRIMASK(cpsr);
		asm volatile ("msr PRIMASK, %0" : : "r" (cpsr) : "memory" );
	}

	xpcc_always_inline
	xpcc::atomic::Unlock::Unlock()
	{
		// cpsr = __get_PRIMASK();
		// __disable_irq();
		// disable interrupts -> PRIMASK=1
		// enable interrupts -> PRIMASK=0
		asm volatile (
				"mrs %0, PRIMASK"	"\n\t"
				"cpsie i"
				: "=&r" (cpsr)
				:: "memory");
	}

	xpcc_always_inline
	xpcc::atomic::Unlock::~Unlock()
	{
		// __set_PRIMASK(cpsr);
		asm volatile ("msr PRIMASK, %0" : : "r" (cpsr) : "memory" );
	}

#elif defined(XPCC__OS_HOSTED)

	xpcc::atomic::Lock::Lock()
	{
	}

	xpcc::atomic::Lock::~Lock()
	{
	}

	xpcc::atomic::Unlock::Unlock()
	{
	}

	xpcc::atomic::Unlock::~Unlock()
	{
	}
#else
#	error	"Please provide an atomic lock implementation for this target!"
#endif

#endif	// XPCC_ATOMIC__LOCK_HPP
