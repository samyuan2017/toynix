#include <kernel/e1000.h>
#include <kernel/pmap.h>
#include <stdio.h>

#define NTXDESCS	64

#define E1000_REG_ADDR(base, offset) (((uintptr_t)base) + (offset))

volatile uint32_t *e1000;

struct tx_desc {
	uint64_t addr;
	uint16_t length;
	uint8_t	cso;
	uint8_t cmd;
	uint8_t	status;
	uint8_t	css;
	uint16_t special;
};

static struct tx_desc tx_desc_table[NTXDESCS];
//static struct rx_desc rx_desc_table[NTXDESCS];

int
pci_e1000_attach(struct pci_func *pcif)
{
	pci_func_enable(pcif);

	e1000 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);

	/* initialize tx_desc_table */
	int i;
	struct tx_desc tx_dec = {
		.addr = 0,
		.length = 0,
		.cso = 0,
		.cmd = 0,
		.status = E1000_TXD_STAT_DD,
		.css = 0,
		.special = 0,
	};

	for (i = 0; i < NTXDESCS; i++)
		tx_desc_table[i] = tx_dec;

	uintptr_t tdbal = E1000_REG_ADDR(e1000, E1000_TDBAL);
	*(uint32_t *)tdbal = PADDR(tx_desc_table);
	uintptr_t tdbah = E1000_REG_ADDR(e1000, E1000_TDBAH);
	*(uint32_t *)tdbah = 0;

	uintptr_t tdlen = E1000_REG_ADDR(e1000, E1000_TDLEN);
	*(uint32_t *)tdbah = sizeof(tx_desc_table);

	uintptr_t tdh = E1000_REG_ADDR(e1000, E1000_TDH);
	*(uint32_t *)tdh = 0;
	uintptr_t tdt = E1000_REG_ADDR(e1000, E1000_TDT);
	*(uint32_t *)tdt = 0;

	uint32_t tflag = 0;
	uintptr_t tctl = E1000_REG_ADDR(e1000, E1000_TCTL);

	tflag |= E1000_TCTL_EN;
	tflag |= E1000_TCTL_PSP;
	tflag |= E1000_TCTL_COLD(0x40);
	*(uint32_t *)tctl = tflag;

	uint32_t tipg = E1000_REG_ADDR(e1000, E1000_TIPG);
	*(uint32_t *)tipg = 10;
	/* IPGR1 and IPGR2 are not needed in full duplex */

	return 0;
}

