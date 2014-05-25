#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/spinlock_types.h>

#define UINPUT_BASE 0xff200000
#define UINPUT_SIZE PAGE_SIZE
#define UINPUT_INT_NUM 72

void *fpga_uinput_mem;

static DEFINE_SEMAPHORE(interrupt_mutex);
static DECLARE_WAIT_QUEUE_HEAD(interrupt_wq);

static int interrupt_flag = 0;
static DEFINE_SPINLOCK(interrupt_flag_lock);
static uint8_t input_state;

static irqreturn_t fpga_uinput_interrupt(int irq, void *dev_id)
{
	if (irq != UINPUT_INT_NUM)
		return IRQ_NONE;

	spin_lock(&interrupt_flag_lock);
	interrupt_flag = 1;
	input_state = ioread8(fpga_uinput_mem);
	spin_unlock(&interrupt_flag_lock);

	wake_up_interruptible(&interrupt_wq);

	return IRQ_HANDLED;
}

static struct device_driver fpga_uinput_driver = {
	.name = "fpga_uinput",
	.bus = &platform_bus_type,
};

static ssize_t fpga_uinput_show(struct device_driver *drv, char *buf)
{
	int ret;

	if (down_trylock(&interrupt_mutex))
		return -EAGAIN;

	if (wait_event_interruptible(interrupt_wq, interrupt_flag != 0)) {
		ret = -ERESTART;
		goto release_and_exit;
	}

	spin_lock(&interrupt_flag_lock);
	interrupt_flag = 0;
	spin_unlock(&interrupt_flag_lock);

	buf[0] = input_state;
	ret = 1;

release_and_exit:
	up(&interrupt_mutex);
	return ret;
}

static ssize_t fpga_uinput_store(struct device_driver *drv,
		const char *buf, size_t count)
{
	return -EROFS;
}

static DRIVER_ATTR(fpga_uinput, S_IRUSR, fpga_uinput_show, fpga_uinput_store);

static int __init fpga_uinput_init(void)
{
	int ret;
	struct resource *res;

	ret = driver_register(&fpga_uinput_driver);
	if (ret < 0)
		goto fail_driver_register;

	ret = driver_create_file(&fpga_uinput_driver,
			&driver_attr_fpga_uinput);
	if (ret < 0)
		goto fail_create_file;

	res = request_mem_region(UINPUT_BASE, UINPUT_SIZE, "fpga_uinput");
	if (res == NULL) {
		ret = -EBUSY;
		goto fail_request_mem;
	}

	fpga_uinput_mem = ioremap(UINPUT_BASE, UINPUT_SIZE);
	if (fpga_uinput_mem == NULL) {
		ret = -EFAULT;
		goto fail_ioremap;
	}

	ret = request_irq(UINPUT_INT_NUM, fpga_uinput_interrupt,
			0, "fpga_uinput", NULL);
	if (ret < 0)
		goto fail_request_irq;

	return 0;

fail_request_irq:
	iounmap(fpga_uinput_mem);
fail_ioremap:
	release_mem_region(UINPUT_BASE, UINPUT_SIZE);
fail_request_mem:
	driver_remove_file(&fpga_uinput_driver, &driver_attr_fpga_uinput);
fail_create_file:
	driver_unregister(&fpga_uinput_driver);
fail_driver_register:
	return ret;
}

static void __exit fpga_uinput_exit(void)
{
	free_irq(UINPUT_INT_NUM, NULL);
	iounmap(fpga_uinput_mem);
	release_mem_region(UINPUT_BASE, UINPUT_SIZE);
	driver_remove_file(&fpga_uinput_driver, &driver_attr_fpga_uinput);
	driver_unregister(&fpga_uinput_driver);
}

MODULE_LICENSE("Dual BSD/GPL");

module_init(fpga_uinput_init);
module_exit(fpga_uinput_exit);
