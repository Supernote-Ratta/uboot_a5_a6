0. 更新方法
 a.先运行时用到的splash.bmp 和epdc_splash.bin  和zImage dtb 放在板子的同一目录下
 b.将u-boot.imx 放入板子里
 c.启动板子,进入uboot.imx所在的目录,在板子里运行以下三条命令(注意不要在pc机上运行,否则可能会损坏电脑)
    echo 0 > /sys/block/mmcblk1boot0/force_ro
    dd if=u-boot.imx of=/dev/mmcblk1boot0 bs=512 seek=2
    echo 1 > /sys/block/mmcblk1boot0/force_ro
   d.如果不要uboot开机画面,可以将u-boot-no-epdc.imx 重命名为u-boot.imx,重复以上步骤

* history:
 	1. u-boot-201603-imx6-ratta20180107-poweroffok.tar.gz
	2. u-boot-201603-imx6-20180205.tar.gz
	3. u-boot-201603-imx6-180315a.tar.gz
	4. u-boot-201603-imx6-20180403a.tar.gz
	5. u-boot-201603-imx6-20180416b.tar.gz
		a.add epdc display
		b.build make ratta_defconfig  && make
		c.运行时要将ratta_resource(在源文件的根目录下)里的splash.bmp 和epdc_splash.bin  和zImage dtb 放在板子的同一目录下
	
	6. 0516  更新内容: 修复 开机时u-boot加载画面前出现的拉丝特效

