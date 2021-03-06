/*
 * Copyright (c) 2011-2014 Graham Edgecombe <graham@grahamedgecombe.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <arc/proc/proc.h>
#include <arc/cpu/cr.h>
#include <arc/smp/cpu.h>
#include <arc/mm/pmm.h>
#include <arc/mm/vmm.h>
#include <arc/lock/intr.h>
#include <stdlib.h>

proc_t *proc_create(void)
{
  proc_t *proc = malloc(sizeof(*proc));
  if (!proc)
    return 0;

  proc->pml4_table = pmm_alloc();
  if (!proc->pml4_table)
  {
    free(proc);
    return 0;
  }

  if (!vmm_init_pml4(proc->pml4_table))
  {
    pmm_free(proc->pml4_table);
    free(proc);
    return 0;
  }

  proc->vmm_lock = SPIN_UNLOCKED;

  if (!seg_init(&proc->segments))
  {
    pmm_free(proc->pml4_table);
    free(proc);
    return 0;
  }

  proc->state = PROC_RUNNING;
  list_init(&proc->thread_list);
  return proc;
}

proc_t *proc_get(void)
{
  cpu_t *cpu = cpu_get();
  return cpu->proc;
}

void proc_switch(proc_t *proc)
{
  cpu_t *cpu = cpu_get();
  cpu->proc = proc;
  cr3_write(proc->pml4_table);
}

void proc_thread_add(proc_t *proc, thread_t *thread)
{
  list_add_tail(&proc->thread_list, &thread->proc_node);
}

void proc_thread_remove(proc_t *proc, thread_t *thread)
{
  list_remove(&proc->thread_list, &thread->proc_node);
}

void proc_destroy(proc_t *proc)
{
  // TODO: destroy threads within the process and make sure they aren't queued

  /* lock interrupts so we can temporarily switch address spaces */
  intr_lock();

  /* record the old pml4 table and switch to the new one */
  uintptr_t old_pml4_table = cr3_read();
  cr3_write(proc->pml4_table);

  /* destroy the user memory segments */
  seg_destroy();

  /* switch back to the old address space and unlock interrupts */
  cr3_write(old_pml4_table);
  intr_unlock();

  // TODO: if cpu->proc == proc, change it to 0?

  /* free the pml4 table and process struct */
  pmm_free(proc->pml4_table);
  free(proc);
}
