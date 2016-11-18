#include <linux/kernel.h>
#include <linux/module.h>
#include <plat/sys_config.h>

#include "main.h"

static u32 gpio_handle;

int nand_request_gpio(void)
{
	gpio_handle = gpio_request_ex("nand_para", NULL);
	if (!gpio_handle)
		return -ENODEV;
	return 0;
}

void nand_release_gpio(void)
{
	if (gpio_handle)
		gpio_release(gpio_handle, 1);
}
