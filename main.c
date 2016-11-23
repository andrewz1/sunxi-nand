#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include "main.h"

MODULE_AUTHOR("andrewz1");
MODULE_LICENSE("GPL");

int get_nand_param(char *param)
{
	int val;

	if (script_parser_fetch("nand_para", param, &val, 1) < 0)
		return -1;
	return val;
}

static int __devinit nand_probe(struct platform_device *pdev)
{
	int err;
	struct nand_info *info;
	u64 tmp;

	err = nand_controller_init();
	if (err < 0)
		return err;
	__nand_chip_reset();
	info = kzalloc(sizeof(struct nand_info), GFP_KERNEL);
	if (!info) {
		nand_err("nand info alloc fail\n");
		err = -ENOMEM;
		goto nand_exit;
	}
	info->mtd.priv = &info->chip;
	info->mtd.name = dev_name(&pdev->dev);
	info->mtd.owner = THIS_MODULE;

	info->chip.read_byte = chip_read_byte;
	info->chip.read_word = chip_read_word;
	info->chip.read_buf = chip_read_buf;
	info->chip.write_buf = chip_write_buf;
	info->chip.select_chip = chip_select_chip;

	info->chip.cmd_ctrl = chip_cmd_ctrl; /* unused */
	info->chip.init_size = chip_init_size;
	info->chip.dev_ready = chip_dev_ready;
	info->chip.cmdfunc = chip_cmdfunc;
	info->chip.waitfunc = chip_waitfunc;
	info->chip.erase_cmd = chip_erase_cmd; /* reinit after scan */
	info->chip.write_page = chip_write_page;

	info->chip.ecc.hwctl = chip_ecc_hwctl; /* empty need for scan */
	info->chip.ecc.calculate = chip_ecc_calculate; /* empty need for scan */
	info->chip.ecc.correct = chip_ecc_correct; /* empty need for scan */

	info->chip.ecc.read_page = chip_ecc_read_page; /* nand_read_page_hwecc */
	info->chip.ecc.read_page_raw = chip_ecc_read_page_raw; /* nand_read_page_raw */
	// info->chip.ecc.read_subpage = chip_ecc_read_subpage; /* used only for soft ecc - not used */
	info->chip.ecc.read_oob = chip_ecc_read_oob; /* nand_read_oob_std */
	info->chip.ecc.read_oob_raw = chip_ecc_read_oob; /* nand_read_oob_std */

	info->chip.ecc.write_page = chip_ecc_write_page; /* nand_write_page_hwecc */
	info->chip.ecc.write_page_raw = chip_ecc_write_page; /* nand_write_page_raw */
	info->chip.ecc.write_oob = chip_ecc_write_oob; /* nand_write_oob_std */
	info->chip.ecc.write_oob_raw = chip_ecc_write_oob; /* nand_write_oob_std */

	err = nand_scan(&info->mtd, 1);
	if (err < 0) {
		nand_err("nand scan fail\n");
		goto free_info;
	}
	info->chip.erase_cmd = chip_erase_cmd;

	tmp = info->mtd.writesize * info->pages1k;
	info->parts[0].name = "SPL";
	info->parts[0].size = tmp;
	info->parts[0].offset = 0;
//	info->parts[0].mask_flags = MTD_WRITEABLE; /* for ro */
	info->parts[0].mask_flags = 0;
	info->parts[0].ecclayout = &info->el;

	info->parts[1].name = "UBI";
	info->parts[1].offset = tmp;
//	info->parts[1].mask_flags = MTD_WRITEABLE; /* for ro */
	info->parts[1].mask_flags = 0;
	info->parts[1].ecclayout = &info->el;

	tmp = info->mtd.size - tmp;
	tmp -= info->mtd.erasesize * 4; /* reserve 4 blocks for bbt */
	info->parts[1].size = tmp;

	err = mtd_device_parse_register(&info->mtd, NULL, NULL, info->parts, MAX_PARTS);
	if (err < 0) {
		nand_err("register mtd device fail\n");
		goto release_nand;
	}

	platform_set_drvdata(pdev, info);
	return 0;

release_nand:
	nand_release(&info->mtd);
free_info:
	kfree(info);
nand_exit:
	nand_controller_exit();
	return err;
}

static int __devexit nand_remove(struct platform_device *pdev)
{
	struct nand_info *info = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);
	nand_release(&info->mtd);
	kfree(info);
	nand_controller_exit();
	return 0;
}

static struct platform_driver nand_driver = {
	.probe  = nand_probe,
	.remove = __devexit_p(nand_remove),
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};

static void nand_noop(struct device *dev)
{
	return;
}

static struct platform_device nand_device = {
	.name = DRIVER_NAME,
	.id = -1,
	.dev = {
		.release = nand_noop,
	},
};

static int __init nand_init(void)
{
	int err;

	if (get_nand_param("nand_used") <= 0)
		return -ENODEV;
	err = platform_driver_register(&nand_driver);
	if (err < 0) {
		nand_err("platform_driver_register fail\n");
		return err;
	}
	err = platform_device_register(&nand_device);
	if (err < 0) {
		nand_err("platform_device_register fail\n");
		goto free_driver;
	}
	return 0;
free_driver:
	platform_driver_unregister(&nand_driver);
	return err;
}

static void __exit nand_exit(void)
{
	if (get_nand_param("nand_used") <= 0)
		return;
	platform_device_unregister(&nand_device);
	platform_driver_unregister(&nand_driver);
}

module_init(nand_init);
module_exit(nand_exit);
