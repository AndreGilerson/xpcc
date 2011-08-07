// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_AT91__REGISTERS_H
#define XPCC_AT91__REGISTERS_H

#if defined __ARM_AT91SAM7S32__
#	include "device/at91sam7s32.h"
#elif defined __ARM_AT91SAM7S321__
#	include "device/at91sam7s321.h"
#elif defined __ARM_AT91SAM7S64__
#	include "device/at91sam7s64.h"
#elif defined __ARM_AT91SAM7S128__
#	include "device/at91sam7s128.h"
#elif defined __ARM_AT91SAM7S256__
#	include "device/at91sam7s256.h"
#elif defined __ARM_AT91SAM7S512__
#	include "device/at91sam7s512.h"
#endif

#ifndef __ASSEMBLER__
#	define __inline	static inline
#	include "device/at91lib.h"
#	undef __inline
#endif

#endif	// XPCC_AT91__REGISTERS_H
