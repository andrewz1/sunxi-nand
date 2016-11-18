obj-m := sunxi_nand.o

sunxi_nand-y := nand_clock.o nand_gpio.o nand_dma.o nand_ctl.o main.o hynix_data.o
