#include <stdio.h>
#include <libusb.h>

static struct libusb_context *ctx = NULL;

int linux_usb_init(void)
{
	int r;

	r = libusb_init(&ctx);
	if (r != 0)
		return r;
	libusb_set_debug(ctx,0xFF);
}

void linux_usb_fini(void)
{
	libusb_exit(ctx);
	ctx = NULL;
}

int linux_usb_open(unsigned vendor, unsigned device, void **result)
{
	struct libusb_device_handle *dev;
	int r;

	dev = libusb_open_device_with_vid_pid(ctx, vendor, device);
	if (dev) {
		(void)libusb_detach_kernel_driver(dev, 0);
		r = libusb_set_configuration(dev, 1);
		if (r != 0)
			return r;
		r = libusb_claim_interface(dev, 0);
		if (r != 0)
			return r;
	}
	*result = dev;
	return 0;
}

void linux_usb_close(void *h)
{
	if (h) {
		libusb_release_interface((struct libusb_device_handle*)h, 0);
		libusb_close((struct libusb_device_handle*)h);
	}
}

int linux_usb_write(void *h, void *data, int len)
{
	int actual;

	return libusb_bulk_transfer((struct libusb_device_handle*)h, 
		0x01, data, len, &actual, 5000);
}

int linux_usb_read(void *h, void *data, int len)
{
	int actual;

	return libusb_bulk_transfer((struct libusb_device_handle*)h, 
		0x81, data, len, &actual, 5000);
}


