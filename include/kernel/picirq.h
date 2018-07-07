#ifndef KERN_PICIRQ_H
#define KERN_PICIRQ_H
#ifndef TOYNIX_KERNEL
# error "This is a Toynix kernel header; user programs should not #include it"
#endif

#define MAX_IRQS	16		// Number of IRQs

// I/O Addresses of the two 8259A programmable interrupt controllers
#define IO_PIC1		0x20	// Master (IRQs 0-7)
#define IO_PIC2		0xA0	// Slave (IRQs 8-15)

#define IRQ_SLAVE	2	// IRQ at which slave connects to master

#ifndef __ASSEMBLER__
extern uint16_t irq_mask_8259A;

void pic_init(void);
void irq_setmask_8259A(uint16_t mask);
void irq_eoi(void);
#endif // !__ASSEMBLER__

#endif // KERN_PICIRQ_H
