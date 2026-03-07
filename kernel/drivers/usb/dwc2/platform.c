/*
 * platform.c - DesignWare HS OTG Controller platform driver
 *
 * Copyright (C) Matthijs Kooijman <matthijs@stdin.nl>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the above-listed copyright holders may not be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/of_device.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/phy/phy.h>
#include <linux/platform_data/s3c-hsotg.h>
#include <linux/reset.h>
//#include <linux/gpio/consumer.h>

#include <linux/gpio.h>

#include "core.h"
#include "hcd.h"
#include "debug.h"

// #if defined dev_dbg
// #undef dev_dbg
// #define dev_dbg dev_err
// #endif
static const char dwc2_driver_name[] = "dwc2";
typedef struct usb_phy_addr {
	unsigned int size;	
#if defined(CONFIG_ARCH_TS_TX5336)
	unsigned long virt_addr;
#else
	unsigned int virt_addr;
#endif
} vpu_phy_set_addr;

vpu_phy_set_addr usb_vir_addr;

#define USB_PHY_OSC_24M 24000000
#define USB_PHY_PLL_50M 50000000



/*
 * Check the dr_mode against the module configuration and hardware
 * capabilities.
 *
 * The hardware, module, and dr_mode, can each be set to host, device,
 * or otg. Check that all these values are compatible and adjust the
 * value of dr_mode if possible.
 *
 *                      actual
 *    HW  MOD dr_mode   dr_mode
 *  ------------------------------
 *   HST  HST  any    :  HST
 *   HST  DEV  any    :  ---
 *   HST  OTG  any    :  HST
 *
 *   DEV  HST  any    :  ---
 *   DEV  DEV  any    :  DEV
 *   DEV  OTG  any    :  DEV
 *
 *   OTG  HST  any    :  HST
 *   OTG  DEV  any    :  DEV
 *   OTG  OTG  any    :  dr_mode
 */
static int dwc2_get_dr_mode(struct dwc2_hsotg *hsotg)
{
	enum usb_dr_mode mode;

	hsotg->dr_mode = usb_get_dr_mode(hsotg->dev);
	//hsotg->dr_mode = USB_DR_MODE_PERIPHERAL;//usb_get_dr_mode(hsotg->dev);
	if (hsotg->dr_mode == USB_DR_MODE_UNKNOWN)
		hsotg->dr_mode = USB_DR_MODE_OTG;

	mode = hsotg->dr_mode;

	if (dwc2_hw_is_device(hsotg)) {
		if (IS_ENABLED(CONFIG_USB_DWC2_HOST)) {
			dev_err(hsotg->dev,
				"Controller does not support host mode.\n");
			return -EINVAL;
		}
		mode = USB_DR_MODE_PERIPHERAL;
	} else if (dwc2_hw_is_host(hsotg)) {
		if (IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL)) {
			dev_err(hsotg->dev,
				"Controller does not support device mode.\n");
			return -EINVAL;
		}
		mode = USB_DR_MODE_HOST;
	} else {
		if (IS_ENABLED(CONFIG_USB_DWC2_HOST))
			mode = USB_DR_MODE_HOST;
		else if (IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL))
			mode = USB_DR_MODE_PERIPHERAL;
	}

	if (mode != hsotg->dr_mode) {
		dev_warn(hsotg->dev,
			 "Configuration mismatch. dr_mode forced to %s\n",
			mode == USB_DR_MODE_HOST ? "host" : "device");

		hsotg->dr_mode = mode;
	}

	return 0;
}

static int __dwc2_lowlevel_hw_enable(struct dwc2_hsotg *hsotg)
{
	struct platform_device *pdev = to_platform_device(hsotg->dev);
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(hsotg->supplies),
				    hsotg->supplies);
	if (ret)
		return ret;

	if (hsotg->clk) {
		ret = clk_prepare_enable(hsotg->clk);
		if (ret)
			return ret;
	}

	if (hsotg->uphy) {
		ret = usb_phy_init(hsotg->uphy);
	} else if (hsotg->plat && hsotg->plat->phy_init) {
		ret = hsotg->plat->phy_init(pdev, hsotg->plat->phy_type);
	} else {
		ret = phy_power_on(hsotg->phy);
		if (ret == 0)
			ret = phy_init(hsotg->phy);
	}

	return ret;
}

/**
 * dwc2_lowlevel_hw_enable - enable platform lowlevel hw resources
 * @hsotg: The driver state
 *
 * A wrapper for platform code responsible for controlling
 * low-level USB platform resources (phy, clock, regulators)
 */
int dwc2_lowlevel_hw_enable(struct dwc2_hsotg *hsotg)
{


	int ret = __dwc2_lowlevel_hw_enable(hsotg);
	dev_dbg(hsotg->dev, "%s %d", __FUNCTION__,__LINE__);

	if (ret == 0)
		hsotg->ll_hw_enabled = true;
	return ret;
}

static int __dwc2_lowlevel_hw_disable(struct dwc2_hsotg *hsotg)
{
	struct platform_device *pdev = to_platform_device(hsotg->dev);
	int ret = 0;

	if (hsotg->uphy) {
		usb_phy_shutdown(hsotg->uphy);
	} else if (hsotg->plat && hsotg->plat->phy_exit) {
		ret = hsotg->plat->phy_exit(pdev, hsotg->plat->phy_type);
	} else {
		ret = phy_exit(hsotg->phy);
		if (ret == 0)
			ret = phy_power_off(hsotg->phy);
	}
	if (ret)
		return ret;

	if (hsotg->clk)
		clk_disable_unprepare(hsotg->clk);

	ret = regulator_bulk_disable(ARRAY_SIZE(hsotg->supplies),
				     hsotg->supplies);

	return ret;
}

/**
 * dwc2_lowlevel_hw_disable - disable platform lowlevel hw resources
 * @hsotg: The driver state
 *
 * A wrapper for platform code responsible for controlling
 * low-level USB platform resources (phy, clock, regulators)
 */
int dwc2_lowlevel_hw_disable(struct dwc2_hsotg *hsotg)
{


	int ret = __dwc2_lowlevel_hw_disable(hsotg);
	dev_dbg(hsotg->dev, "%s %d", __FUNCTION__,__LINE__);

	if (ret == 0)
		hsotg->ll_hw_enabled = false;
	return ret;
}



union ts_usb2phy_cfg0_un {
	u32 data;
	struct {
		u32 bypassdmdata:1; //[0]
		u32 bypassdmen:1;	//[1]
		u32 bypassdpdata:1;	//[2]
		u32 bypassdpen:1;	//[3]

		u32 bypassdpsel:1;	//[4]
		u32 commonn:1;		//[5]
		u32 compdistune:3;	//[8:6]

		u32 reserve:1;		//[9]
		u32 fsel:3;			//[12:10]

		u32 reserve2:1;		//[13]
		u32 loopback:1;		//[14]
		u32 otgdisable:1;	//[15]
		u32 otgtune:3;		//[18:16]
		u32 pllbtune:1;		//[19]
		u32 pllitune:2;		//[21:20]
		u32 pllptune:4;		//[25:22]

		u32 sleep:1;		//[26]
		u32 sqrxtune:3;		//[29:27]
		u32 testburnin:1;	//[30]
		u32 reserve3:1;		//[31]
	};
};

union ts_usb2c_cfg1_un {
	u32 data;
	struct {
		u32 txbitstuffen:1;			//[0]
		u32 txbitstuffenh:1;		//[1]
		u32 txfslstune:4;			//[5:2]
		u32 txhsvtune:2;			//[7:6]

		u32 txpreempamptune:2;		//[9:8]
		u32 txpreemppulsetune:1;	//[10]
		u32 txrestune:2;			//[12:11]

		u32 txrisetune:2;			//[14:13]
		u32 txvreftune:4;			//[18:15]
		u32 vbusvldext:1;			//[19]
		u32 vbusvldextsel:1;		//[20]

		u32 vdatreftune:2;			//[22:21]
		u32 reserve:1;				//[23]
		u32 analog_test_en:2;		//[25:24]
		u32 siddq:1;				//[26]
		u32 ss_scaledown_mode:2;	//[28:27]

		u32 reserve_2:3;			//[31:29]
	};
};
static int dwc2_lowlevel_hw_init(struct dwc2_hsotg *hsotg)
{
	int i, ret;
	unsigned int phy_cfg0 = 0;
	unsigned int phy_cfg1 = 0x003aa0cc;

	//unsigned int phy_cfg0 = 0x2c061e20;
	#if defined(CONFIG_ARCH_TS_TX5336)
	unsigned int phy_cfg0_addr = 0xf038f020;
	unsigned int phy_cfg1_addr = 0xf038f024;
	unsigned long phy_clk = 0;
	#else
	unsigned int phy_cfg0_addr = 0xF2440010;
	unsigned int phy_cfg1_addr = 0xF2440014;
	unsigned int phy_clk = 0;
	struct device_node *np = hsotg->dev->of_node;
	#endif
	unsigned int read_value = 0;
	//struct reset_control *ahb_reset;
	
	struct clk *hclk;


	struct gpio_desc *drvvbus;
			/* Clock */
	#if defined(CONFIG_ARCH_TS_TX5336)
		hclk = devm_clk_get(hsotg->dev, "usb_ahb_clk");
	#else
		hclk = devm_clk_get(hsotg->dev, "otg");
	#endif
		if (IS_ERR(hclk)) {
			hclk = NULL;
			dev_dbg(hsotg->dev, "cannot get otg clock\n");
		}
	/* Clock */
	#if defined(CONFIG_ARCH_TS_TX5336)
		hsotg->clk = devm_clk_get(hsotg->dev, "usb_ref_clk");
	#else
		hsotg->clk = devm_clk_get(hsotg->dev, "phy_clk");
	#endif
		if (IS_ERR(hsotg->clk)) {
			hsotg->clk = NULL;
			dev_dbg(hsotg->dev, "cannot get phy_clk\n");
		}

	#if defined(CONFIG_ARCH_TS_TX5336)
		phy_clk = clk_get_rate(hsotg->clk);
		dev_dbg(hsotg->dev, "%s phy_clk_sel = %ld\n", __FUNCTION__, phy_clk);
	#else

		ret = of_property_read_u32(np, "phy_clk_sel", &phy_clk);
		if(ret){
			dev_err(hsotg->dev, "get_clk_sel failed, %d\n", ret);
		}
		dev_dbg(hsotg->dev, "%s phy_clk_sel = %d\n", __FUNCTION__, phy_clk);
     #endif

	if(USB_PHY_OSC_24M == phy_clk)
	{
		phy_cfg0 = 0x2c0e0820;  //24M
	}
	else if(USB_PHY_PLL_50M == phy_clk)
	{
		phy_cfg0 = 0x2c061c20;  //50M
	}
	else
	{
		dev_err(hsotg->dev, "error, ##can not get phy_clk_sel, use 24MHz default cfg##\n");
		//phy_cfg0 = 0x2c061c20;  //50M
		phy_cfg0 = 0x2c0e0820;  //24M
	}
	
#if 0
	unsigned int phy_osc_addr = 0xf10040d4;  //dt56
	//unsigned int phy_osc_addr = 0xf100406c;  //dt57
	memset(&usb_vir_addr,0x0,sizeof(usb_vir_addr));
	usb_vir_addr.size = 4; 

	//cfg_clk_sel
	usb_vir_addr.virt_addr = (unsigned long)ioremap_nocache(phy_osc_addr, 4);
	if (!usb_vir_addr.virt_addr) {
		dev_err(hsotg->dev, "error error phy_cfg1 usb_vir_addr.virt_addr ==0\n");
		return -1;
	}
	*(volatile unsigned int *)(usb_vir_addr.virt_addr) = 0x10; //56:24M_clk_source
	//*(volatile unsigned int *)(usb_vir_addr.virt_addr) = 0x63; //57:24M_clk_source
	//*(volatile unsigned int *)(usb_vir_addr.virt_addr) = 0x07; //57:24M_clk_source


	iounmap((void *)usb_vir_addr.virt_addr);
	
#endif
	//ahb_reset = devm_reset_control_get_optional(hsotg->dev, "usbahb");
	//reset_control_assert(ahb_reset);
	//reset_control_deassert(ahb_reset);


	hsotg->reset = devm_reset_control_get_optional(hsotg->dev, "usbphy");

	if (IS_ERR(hsotg->reset)) {
		ret = PTR_ERR(hsotg->reset);
		dev_err(hsotg->dev, "error getting reset control %d\n", ret);
		return ret;
	}
	
	memset(&usb_vir_addr,0x0,sizeof(usb_vir_addr));
	usb_vir_addr.size = 4; 
    //reset
	reset_control_assert(hsotg->reset);
	//cfg_usbphy0
	usb_vir_addr.virt_addr = (unsigned long)ioremap_nocache(phy_cfg0_addr, 4);//sys_reg_phy0_cfg
	if (!usb_vir_addr.virt_addr) {
		dev_err(hsotg->dev, "error error phy_cfg1 usb_vir_addr.virt_addr ==0\n");
		return -1;
	}
	*(volatile unsigned int *)((uintptr_t)usb_vir_addr.virt_addr) = phy_cfg0;
	read_value = *(volatile unsigned int *)((uintptr_t)usb_vir_addr.virt_addr);
	//dev_dbg(hsotg->dev, "reg_phy_cfg0_0xF2440012= 0x%08x \n", read_value);
	iounmap((void *)(uintptr_t)usb_vir_addr.virt_addr);

	//cfg_usbphy1
	usb_vir_addr.virt_addr = (unsigned long)ioremap_nocache(phy_cfg1_addr, 4);//sys_reg_phy0_cfg
	if (!usb_vir_addr.virt_addr) {
		dev_err(hsotg->dev, "error error phy_cfg1 usb_vir_addr.virt_addr ==0\n");
		return -1;
	}
	*(volatile unsigned int *)((uintptr_t)usb_vir_addr.virt_addr) = phy_cfg1;
	read_value = *(volatile unsigned int *)((uintptr_t)usb_vir_addr.virt_addr);
	//dev_dbg(hsotg->dev, "reg_phy_cfg1_0xF2440014= 0x%08x \n", read_value);
	iounmap((void *)(uintptr_t)usb_vir_addr.virt_addr);


	//dereset
	reset_control_deassert(hsotg->reset);

	/* Set default UTMI width */
	hsotg->phyif = GUSBCFG_PHYIF16;

	/*
	 * Attempt to find a generic PHY, then look for an old style
	 * USB PHY and then fall back to pdata
	 */
	hsotg->phy = devm_phy_get(hsotg->dev, "usb2-phy");
	if (IS_ERR(hsotg->phy)) {
		ret = PTR_ERR(hsotg->phy);
		switch (ret) {
		case -ENODEV:
		case -ENOSYS:
			hsotg->phy = NULL;
			break;
		case -EPROBE_DEFER:
			return ret;
		default:
			dev_err(hsotg->dev, "error getting phy %d\n", ret);
			return ret;
		}
	}

	if (!hsotg->phy) {
		hsotg->uphy = devm_usb_get_phy(hsotg->dev, USB_PHY_TYPE_USB2);
		if (IS_ERR(hsotg->uphy)) {
			ret = PTR_ERR(hsotg->uphy);
			switch (ret) {
			case -ENODEV:
			case -ENXIO:
				hsotg->uphy = NULL;
				break;
			case -EPROBE_DEFER:
				return ret;
			default:
				dev_err(hsotg->dev, "error getting usb phy %d\n",
					ret);
				return ret;
			}
		}
	}

	hsotg->plat = dev_get_platdata(hsotg->dev);

	if (hsotg->phy) {
		/*
		 * If using the generic PHY framework, check if the PHY bus
		 * width is 8-bit and set the phyif appropriately.
		 */
		if (phy_get_bus_width(hsotg->phy) == 8)
			hsotg->phyif = GUSBCFG_PHYIF8;
	}
	hsotg->phyif = GUSBCFG_PHYIF16;//GUSBCFG_PHYIF16;//GUSBCFG_PHYIF8




	
	clk_prepare_enable(hclk);
	//clk_disable_unprepare(hclk);
	dev_dbg(hsotg->dev, "hclk enable\n");
	
	clk_prepare_enable(hsotg->clk);
	//clk_disable_unprepare(hsotg->clk);
	dev_dbg(hsotg->dev, "phy_clk enable\n");
	hsotg->dr_mode = usb_get_dr_mode(hsotg->dev);
	if (hsotg->dr_mode == USB_DR_MODE_HOST){
       //host_set_gpio_power_supply_for_device
		drvvbus = devm_gpiod_get(hsotg->dev,"drvvbus",GPIOD_OUT_HIGH);
		if (IS_ERR(drvvbus)) {
	       dev_dbg(hsotg->dev, "%s  host can not get vbus gpio,check power_directly???\r\n", __FUNCTION__);
	    }else{
			#if defined(CONFIG_ARCH_TS_TX5336)
			gpiod_direction_output(drvvbus, 1);
			#else
			gpiod_direction_output(drvvbus, 0);
			#endif
		   dev_dbg(hsotg->dev, "%s USB_VBUS gpiod_direction_output ==1\n", __FUNCTION__);
	    }

	}else{
		dev_dbg(hsotg->dev, "%s ###USB_VBUS does not need to be set in device mode###\n", __FUNCTION__);
	}




	/* Regulators */
	for (i = 0; i < ARRAY_SIZE(hsotg->supplies); i++)
		hsotg->supplies[i].supply = dwc2_hsotg_supply_names[i];

	ret = devm_regulator_bulk_get(hsotg->dev, ARRAY_SIZE(hsotg->supplies),
				      hsotg->supplies);
	if (ret) {
		dev_err(hsotg->dev, "failed to request supplies: %d\n", ret);
		return ret;
	}
	return 0;
}

/**
 * dwc2_driver_remove() - Called when the DWC_otg core is unregistered with the
 * DWC_otg driver
 *
 * @dev: Platform device
 *
 * This routine is called, for example, when the rmmod command is executed. The
 * device may or may not be electrically present. If it is present, the driver
 * stops device processing. Any resources used on behalf of this device are
 * freed.
 */
static int dwc2_driver_remove(struct platform_device *dev)
{
	struct dwc2_hsotg *hsotg = platform_get_drvdata(dev);

	dwc2_debugfs_exit(hsotg);
	if (hsotg->hcd_enabled)
		dwc2_hcd_remove(hsotg);
	if (hsotg->gadget_enabled)
		dwc2_hsotg_remove(hsotg);

	if (hsotg->ll_hw_enabled){
		//dwc2_lowlevel_hw_disable(hsotg);
		}
	reset_control_assert(hsotg->reset);

	return 0;
}

/**
 * dwc2_driver_shutdown() - Called on device shutdown
 *
 * @dev: Platform device
 *
 * In specific conditions (involving usb hubs) dwc2 devices can create a
 * lot of interrupts, even to the point of overwhelming devices running
 * at low frequencies. Some devices need to do special clock handling
 * at shutdown-time which may bring the system clock below the threshold
 * of being able to handle the dwc2 interrupts. Disabling dwc2-irqs
 * prevents reboots/poweroffs from getting stuck in such cases.
 */
static void dwc2_driver_shutdown(struct platform_device *dev)
{
	struct dwc2_hsotg *hsotg = platform_get_drvdata(dev);

	dwc2_disable_global_interrupts(hsotg);
	synchronize_irq(hsotg->irq);
}

/**
 * dwc2_driver_probe() - Called when the DWC_otg core is bound to the DWC_otg
 * driver
 *
 * @dev: Platform device
 *
 * This routine creates the driver components required to control the device
 * (core, HCD, and PCD) and initializes the device. The driver components are
 * stored in a dwc2_hsotg structure. A reference to the dwc2_hsotg is saved
 * in the device private data. This allows the driver to access the dwc2_hsotg
 * structure on subsequent calls to driver methods for this device.
 */
static int dwc2_driver_probe(struct platform_device *dev)
{
	struct dwc2_hsotg *hsotg;
	struct resource *res;
	int retval;

	hsotg = devm_kzalloc(&dev->dev, sizeof(*hsotg), GFP_KERNEL);
	if (!hsotg)
		return -ENOMEM;

	hsotg->dev = &dev->dev;

	/*
	 * Use reasonable defaults so platforms don't have to provide these.
	 */
	if (!dev->dev.dma_mask)
		dev->dev.dma_mask = &dev->dev.coherent_dma_mask;
	retval = dma_set_coherent_mask(&dev->dev, DMA_BIT_MASK(32));
	if (retval)
		return retval;

	res = platform_get_resource(dev, IORESOURCE_MEM, 0);
	hsotg->regs = devm_ioremap_resource(&dev->dev, res);
	if (IS_ERR(hsotg->regs))
		return PTR_ERR(hsotg->regs);

	dev_dbg(&dev->dev, "mapped PA %08lx to VA %p\n",
		(unsigned long)res->start, hsotg->regs);

	retval = dwc2_lowlevel_hw_init(hsotg);
	if (retval)
		return retval;

	spin_lock_init(&hsotg->lock);

	hsotg->irq = platform_get_irq(dev, 0);
	if (hsotg->irq < 0) {
		dev_err(&dev->dev, "missing IRQ resource\n");
		return hsotg->irq;
	}

	dev_dbg(hsotg->dev, "registering common handler for irq%d\n",hsotg->irq);
	retval = devm_request_irq(hsotg->dev, hsotg->irq,
				  dwc2_handle_common_intr, IRQF_SHARED,
				  dev_name(hsotg->dev), hsotg);
	if (retval)
		return retval;

	//retval = dwc2_lowlevel_hw_enable(hsotg);
	if (retval)
		return retval;

	retval = dwc2_get_dr_mode(hsotg);
	if (retval)
		goto error;

	/*
	 * Reset before dwc2_get_hwparams() then it could get power-on real
	 * reset value form registers.
	 */
	dwc2_core_reset_and_force_dr_mode(hsotg);

	/* Detect config values from hardware */
	retval = dwc2_get_hwparams(hsotg);
	dev_dbg(hsotg->dev, "dwc2_get_hwparams: return %d\n",retval);
	if (retval)
		goto error;

	dwc2_force_dr_mode(hsotg);

	retval = dwc2_init_params(hsotg);
	if (retval)
		goto error;

	if (hsotg->dr_mode != USB_DR_MODE_HOST) {
		retval = dwc2_gadget_init(hsotg, hsotg->irq);
		if (retval)
			goto error;
		hsotg->gadget_enabled = 1;
	}

	if (hsotg->dr_mode != USB_DR_MODE_PERIPHERAL) {
		retval = dwc2_hcd_init(hsotg);
		dev_dbg(hsotg->dev, "dwc2_hcd_init: return %d\n",retval);
		if (retval) {
			if (hsotg->gadget_enabled)
				dwc2_hsotg_remove(hsotg);
			goto error;
		}
		hsotg->hcd_enabled = 1;
	}

	platform_set_drvdata(dev, hsotg);

	dwc2_debugfs_init(hsotg);

	/* Gadget code manages lowlevel hw on its own */
	if (hsotg->dr_mode == USB_DR_MODE_PERIPHERAL){
		//dwc2_lowlevel_hw_disable(hsotg);
     }
#if IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL) || \
	IS_ENABLED(CONFIG_USB_DWC2_DUAL_ROLE)
	/* Postponed adding a new gadget to the udc class driver list */
	if (hsotg->gadget_enabled) {
		dev_dbg(hsotg->dev, "%s()\n", __func__);
		retval = usb_add_gadget_udc(hsotg->dev, &hsotg->gadget);
		if (retval) {
			hsotg->gadget.udc = NULL;
			dwc2_hsotg_remove(hsotg);
			goto error;
		}
	}
#endif /* CONFIG_USB_DWC2_PERIPHERAL || CONFIG_USB_DWC2_DUAL_ROLE */
	return 0;

error:
	if (hsotg->dr_mode != USB_DR_MODE_PERIPHERAL){
		//dwc2_lowlevel_hw_disable(hsotg);
	}
	return retval;
}

static int __maybe_unused dwc2_suspend(struct device *dev)
{
	struct dwc2_hsotg *dwc2 = dev_get_drvdata(dev);
	int ret = 0;

	if (dwc2_is_device_mode(dwc2))
		dwc2_hsotg_suspend(dwc2);

	if (dwc2->ll_hw_enabled)
		ret = __dwc2_lowlevel_hw_disable(dwc2);

	return ret;
}

static int __maybe_unused dwc2_resume(struct device *dev)
{
	struct dwc2_hsotg *dwc2 = dev_get_drvdata(dev);
	int ret = 0;

	if (dwc2->ll_hw_enabled) {
		ret = __dwc2_lowlevel_hw_enable(dwc2);
		if (ret)
			return ret;
	}

	if (dwc2_is_device_mode(dwc2))
		ret = dwc2_hsotg_resume(dwc2);

	return ret;
}

static const struct dev_pm_ops dwc2_dev_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(dwc2_suspend, dwc2_resume)
};

static struct platform_driver dwc2_platform_driver = {
	.driver = {
		.name = dwc2_driver_name,
		.of_match_table = dwc2_of_match_table,
		.pm = &dwc2_dev_pm_ops,
	},
	.probe = dwc2_driver_probe,
	.remove = dwc2_driver_remove,
	.shutdown = dwc2_driver_shutdown,
};

module_platform_driver(dwc2_platform_driver);

MODULE_DESCRIPTION("DESIGNWARE HS OTG Platform Glue");
MODULE_AUTHOR("Matthijs Kooijman <matthijs@stdin.nl>");
MODULE_LICENSE("Dual BSD/GPL");
