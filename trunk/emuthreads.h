/*
   Source for x86 emulator IdaPro plugin
   File: emuthreads.h
   Copyright (c) 2006, Chris Eagle
   
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your option) 
   any later version.
   
   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
   more details.
   
   You should have received a copy of the GNU General Public License along with 
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
   Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __EMU_THREADS_H
#define __EMU_THREADS_H

#include "cpu.h"
#include "buffer.h"

#define THREAD_MAGIC 0xDEADBEEF
#define THREAD_ID_BASE 0x500
#define THREAD_HANDLE_BASE 0x700

class ThreadNode {
public:
   ThreadNode();
   ThreadNode(dword threadFunc, dword threadArg);
   ThreadNode(Buffer &b, dword currentActive);
   
   void save(Buffer &b, bool saveStack);

   dword handle;
   dword id;
   dword hasStarted;
   dword threadArg;
   Registers regs;
   ThreadNode *next;
};

extern ThreadNode *threadList;
extern ThreadNode *activeThread;

/*
 * return thread handle for new thread
 */
ThreadNode *emu_create_thread(dword threadFunc, dword threadArg);

/*
 * destroy the thread indicated by threadId
 */
ThreadNode *emu_destroy_thread(dword threadId);

/*
 * switch threads
 */
void emu_switch_threads(ThreadNode *new_thread);

/*
 * locate the thread with the given handle
 */
ThreadNode *findThread(dword handle);

#define TEB_SEH_FRAME 0
#define TEB_STACK_TOP 4
#define TEB_STACK_BOTTOM 8
#define TEB_FIBER_DATA 16
#define TEB_LINEAR_ADDR 24
#define TEB_ENV_PTR 28
#define TEB_PROCESS_ID 32
#define TEB_THREAD_ID 36
#define TEB_PEB_PTR 48
#define TEB_LAST_ERROR 52
#define TEB_TLS_ARRAY 0xE10
#define TEB_TLS_EXPANSION 0xf94

#define PEB_DEBUG_FLAG 3
#define PEB_IMAGE_BASE 8
#define PEB_LDR_DATA 12
#define PEB_PROCESS_PARMS 16
#define PEB_PROCESS_HEAP 0x18
#define PEB_FASTPEBLOCK 0x1C
#define PEB_FASTPEBLOCK_FUNC 0x20
#define PEB_FASTPEBUNLOCK_FUNC 0x24
#define PEB_TLS_BITMAP 0x40
#define PEB_TLS_BITMAP_BITS 0x44
#define PEB_NUM_PROCESSORS 0x64
#define PEB_NUM_HEAPS 0x88
#define PEB_MAX_HEAPS 0x8C
#define PEB_OS_MAJOR 0xA4
#define PEB_OS_MINOR 0xA8
#define PEB_OS_BUILD 0xAC
#define PEB_OS_PLATFORM_ID 0xB0
#define PEB_TLS_EXP_BITMAP 0x150
#define PEB_TLS_EXP_BITMAP_BITS 0x154

#define SIZEOF_PEB 0x1E8

#endif
