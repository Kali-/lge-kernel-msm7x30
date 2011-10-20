/* drivers/video/msm/logo.c
 *
 * Show Logo in RLE 565 format
 *
 * Copyright (C) 2008 Google Incorporated
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <linux/vt_kern.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>

#include <linux/irq.h>
#include <asm/system.h>

#ifdef CONFIG_FB_MSM_DEFAULT_DEPTH_RGB565
#define fb_width(fb)	((fb)->var.xres)
#define fb_height(fb)	((fb)->var.yres)
#define fb_size(fb)	((fb)->var.xres * (fb)->var.yres * 2)

static void memset16(void *_ptr, unsigned short val, unsigned count)
{
	unsigned short *ptr = _ptr;
	count >>= 1;
	while (count--)
		*ptr++ = val;
}
#elif defined (CONFIG_FB_MSM_DEFAULT_DEPTH_RGBA8888) \
    | defined (CONFIG_FB_MSM_DEFAULT_DEPTH_ARGB8888)
#define fb_width(fb)	((fb)->var.xres)
#define fb_height(fb)	((fb)->var.yres)
#define fb_size(fb)	((fb)->var.xres * (fb)->var.yres * 4)

static void memset32(void *_ptr, unsigned val, unsigned count)
{
	unsigned *ptr = _ptr;
	count >>= 1;
	while (count--)
		*ptr++ = val;
}
#endif

/* 565RLE image format: [count(2 bytes), rle(2 bytes)] */
/* RGBA888RLE/ARGB888RLE image format: [count(4 bytes), rle(4 bytes)] */
int load_rle_image(char *filename)
{
	struct fb_info *info;
	int fd, count, err = 0;
	unsigned max;
#ifdef CONFIG_FB_MSM_DEFAULT_DEPTH_RGB565
	unsigned short *data, *bits, *ptr;
#elif defined (CONFIG_FB_MSM_DEFAULT_DEPTH_RGBA8888) \
    | defined (CONFIG_FB_MSM_DEFAULT_DEPTH_ARGB8888)
	unsigned *data, *bits, *ptr;
#endif

	info = registered_fb[0];
	if (!info) {
		printk(KERN_WARNING "%s: Can not access framebuffer\n",
			__func__);
		return -ENODEV;
	}

	fd = sys_open(filename, O_RDONLY, 0);
	if (fd < 0) {
		printk(KERN_WARNING "%s: Can not open %s\n",
			__func__, filename);
		return -ENOENT;
	}
	count = sys_lseek(fd, (off_t)0, 2);
	if (count <= 0) {
		err = -EIO;
		goto err_logo_close_file;
	}
	sys_lseek(fd, (off_t)0, 0);
	data = kmalloc(count, GFP_KERNEL);
	if (!data) {
		printk(KERN_WARNING "%s: Can not alloc data\n", __func__);
		err = -ENOMEM;
		goto err_logo_close_file;
	}
	if (sys_read(fd, (char *)data, count) != count) {
		err = -EIO;
		goto err_logo_free_data;
	}

	max = fb_width(info) * fb_height(info);
	ptr = data;
#ifdef CONFIG_FB_MSM_DEFAULT_DEPTH_RGB565
	bits = (unsigned short *)(info->screen_base);
#elif defined (CONFIG_FB_MSM_DEFAULT_DEPTH_RGBA8888) \
    | defined (CONFIG_FB_MSM_DEFAULT_DEPTH_ARGB8888)
	bits = (unsigned *)(info->screen_base);
#endif

	while (count > 3) {
		unsigned n = ptr[0];
		if (n > max)
			break;

#ifdef CONFIG_FB_MSM_DEFAULT_DEPTH_RGB565
		memset16(bits, ptr[1], n << 1);
#elif defined (CONFIG_FB_MSM_DEFAULT_DEPTH_RGBA8888) \
    | defined (CONFIG_FB_MSM_DEFAULT_DEPTH_ARGB8888)
		memset32(bits, ptr[1], n << 1);
#endif
		bits += n;
		max -= n;
		ptr += 2;
		count -= 4;
	}

err_logo_free_data:
	kfree(data);
err_logo_close_file:
	sys_close(fd);
	return err;
}
EXPORT_SYMBOL(load_rle_image);
