/*
 * Copyright (c) 2011-2012 Graham Edgecombe <graham@grahamedgecombe.com>
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

#ifndef ARC_LOCK_RWLOCK_H
#define ARC_LOCK_RWLOCK_H

#include <arc/lock/spinlock.h>
#include <stdbool.h>

typedef struct
{
  int read_permits;
  bool writing; /* false = 0, so this is ok */
  spinlock_t lock; /* SPIN_UNLOCKED = 0, so this is ok */
} rwlock_t;

void rw_rlock(rwlock_t *lock);
void rw_runlock(rwlock_t *lock);
void rw_wlock(rwlock_t *lock);
void rw_wunlock(rwlock_t *lock);

#endif

