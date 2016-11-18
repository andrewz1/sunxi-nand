/*
 * defs.h
 *
 * Copyright (C) 2013 Qiang Yu <yuq825@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SUNXI_NAND_DEFS_H
#define _SUNXI_NAND_DEFS_H

#include <linux/kernel.h>

#define DRIVER_NAME "sunxi-nand"
#define PREFIX "[" DRIVER_NAME "]: "

#define nand_info(fmt, ...) printk(KERN_INFO PREFIX fmt, ##__VA_ARGS__)
#define nand_err(fmt, ...) printk(KERN_ERR PREFIX fmt, ##__VA_ARGS__)

#endif
