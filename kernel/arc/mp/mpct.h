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

#ifndef ARC_MP_MPCT_H
#define ARC_MP_MPCT_H

#include <arc/pack.h>
#include <stdint.h>
#include <stdbool.h>

#define MPCT_SIGNATURE 0x504D4350 /* 'PCMP' */

#define MPCT_TYPE_PROC       0
#define MPCT_TYPE_BUS        1
#define MPCT_TYPE_IO_APIC    2
#define MPCT_TYPE_IO_INTR    3
#define MPCT_TYPE_LOCAL_INTR 4

/* extended types */
#define MPCT_TYPE_ADDR_MAPPING     128
#define MPCT_TYPE_BUS_HIERARCHY    129
#define MPCT_TYPE_BUS_ADDR_MAPPING 130

typedef PACK(struct
{
  uint8_t type;
  union
  {
    PACK(struct
    {
      uint8_t id;
      uint8_t ver;
      uint8_t flags;
      uint32_t signature;
      uint32_t features;
      uint64_t reserved;
    }) proc;

    PACK(struct
    {
      uint8_t id;
      uint8_t type[6];
    }) bus;

    PACK(struct
    {
      uint8_t id;
      uint8_t ver;
      uint8_t flags;
      uint32_t addr;
    }) io_apic;

    PACK(struct
    {
      uint8_t type;
      uint16_t flags;
      uint8_t bus;    /* source bus id */
      uint8_t irq;    /* bus irq number */
      uint8_t ioapic; /* dest ioapic id */
      uint8_t intr;   /* ioapic interrupt number */
    }) io_intr;

    PACK(struct
    {
      uint8_t type;
      uint16_t flags;
      uint8_t bus;    /* source bus id */
      uint8_t irq;    /* bus irq number */
      uint8_t lapic;  /* lapic interrupt number */
      uint8_t lint;   /* lapic LINT number */
    }) local_intr;
  };
}) mpct_entry_t;

typedef PACK(struct
{
  uint8_t type;
  uint8_t length;
  union
  {
    PACK(struct
    {
      uint8_t busd;
      uint8_t type;
      uint64_t base;
      uint64_t len;
    }) addr_mapping;

    PACK(struct
    {
      uint8_t bus;
      uint8_t info;
      uint8_t parent_bus;
      uint8_t reserved[3];
    }) bus_hierarchy;

    PACK(struct
    {
      uint8_t bus;
      uint8_t mod;
      uint32_t range;
    }) bus_addr_mapping;
  };
}) mpct_ext_entry_t;

typedef PACK(struct
{
  uint32_t signature;
  uint16_t len;
  uint8_t spec_rev;
  uint8_t checksum;
  uint8_t oem_str[8];
  uint8_t product_str[12];
  uint32_t oem_table_phy_addr;
  uint16_t oem_table_size;
  uint16_t entry_count;
  uint32_t lapic_phy_addr;
  uint16_t ext_len;
  uint8_t ext_checksum;
  uint8_t reserved;
  mpct_entry_t entries[1];
}) mpct_t;

bool mpct_valid(mpct_t *mpct);
void mpct_scan(mpct_t *mpct);

#endif
