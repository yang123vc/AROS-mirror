/* Our IRQs are UNIX signals */
#define IRQ_COUNT 32

/* We have no interrupt controller */
#define ictl_enable_irq(irq)
#define ictl_disable_irq(irq)
