#include <dm.h>
#include <errno.h>
#include <log.h>
#include <malloc.h>
#include <memalign.h>
#include <miiphy.h>
#include <net.h>
#include <netdev.h>
#include <phy.h>
#include <asm/io.h>
#include <log.h>
#include <mapmem.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/delay.h>
#include <asm/gpio.h>
#include <dm/pinctrl.h>

#include "rtk_types.h"
#include "rtk_error.h"
#include "rtk_switch.h"
#include "vlan.h"
#include "stat.h"

#include "dal/rtl8367c/dal_rtl8367c_port.h"

extern int get_box_device_id(void);

#define VLANID_MNG				1
#define VLANID_INTER			1
#define VLANID_WAN				2
#define VLANID_LAN				3

#define PVID_COPU				1
#define PVID_MPU				1
#define PVID_WAN				2
#define PVID_LAN				3

#define BOX_PANEL_PORT 	UTP_PORT2 //UTP_PORT4
#define BOX_MPU_PORT 	UTP_PORT0
#define BOX_WAN_PORT 	UTP_PORT2
#define BOX_LAN_PORT 	UTP_PORT3

struct port_stats_simple {
	rtk_stat_port_type_t index;
	char *title;
	rtk_stat_counter_t val;
};

static struct mii_dev *switch_dev = NULL;

static bool init_chip_done = false;
static bool init_vlan_done = false;

int eth_switch_chip_init(void)
{
	if (init_chip_done) {
		return 0;
	}

	rtk_api_ret_t retVal;
	retVal = rtk_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("rtk_switch_init failed, ret %d\n", retVal);
		return retVal;
	}

	init_chip_done = true;
	return retVal;
}

int eth_switch_vlan_init(void)
{
	if (init_vlan_done) {
		return 0;
	}

	rtk_api_ret_t retVal;
	retVal = rtk_vlan_init();
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_init failed, ret %d\n", retVal);
		return retVal;
	}

	init_vlan_done = true;
	return retVal;
}

int eth_switch_init(void)
{
	rtk_api_ret_t retVal;
	retVal = eth_switch_chip_init();
	if (retVal)
		return retVal;
	
	retVal = eth_switch_vlan_init();
	if (retVal)
		return retVal;
	return retVal;
}



void eth_switch_get_phyid(rtk_port_t port)
{
    rtk_api_ret_t retVal;
	rtk_port_phy_data_t data1 = 0;
	rtk_port_phy_data_t data2 = 0;

	retVal = dal_rtl8367c_port_phyReg_get(port, PHY_REG_IDENTIFIER_1, &data1);
	if (retVal != RT_ERR_OK) {
		printf("dal_rtl8367c_port_phyReg_get(%d) id1 failed, ret %d\n", port, retVal);
		return;
	}

	retVal = dal_rtl8367c_port_phyReg_get(port, PHY_REG_IDENTIFIER_2, &data2);
	if (retVal != RT_ERR_OK) {
		printf("dal_rtl8367c_port_phyReg_get(%d) id2 failed, ret %d\n", port, retVal);
		return;
	}
	printf("PORT%d id: 0x%x 0x%x\n", port, data1, data2);
}

void eth_switch_get_phy_status(void)
{
	rtk_api_ret_t retVal;
	rtk_port_linkStatus_t linkStatus;
	rtk_port_speed_t speed;
	rtk_port_duplex_t duplex;

	//printf("[%s] enter\n", __func__);
	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return;
	}

	printf("-----Phy status-----\n");
	for (rtk_port_t port = UTP_PORT0; port < UTP_PORT5; port++) {
		retVal = rtk_port_phyStatus_get(port, &linkStatus, &speed, &duplex);
		if (retVal != RT_ERR_OK) {
			printf("rtk_port_phyStatus_get(%d) failed, ret %d\n", port, retVal);
			continue;
		}
		printf("PORT%d: link %d(%s), speed %d(%s), duplex %d(%s)\n",
			port, linkStatus, (linkStatus == 0) ? "down" : "up",
			speed, (speed == 0) ? "10M" : ((speed == 1) ? "100M" : "1000M"),
			duplex, (duplex == 0) ? "half" : "full");
	}
	//printf("[%s] exit\n", __func__);
}

void eth_switch_get_mac_status(void)
{
	rtk_api_ret_t retVal;

	//printf("[%s] enter\n", __func__);
	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return;
	}

	rtk_port_t port_list[] = {
		UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UTP_PORT4 };
		//EXT_PORT0, EXT_PORT1 };
	rtk_port_mac_ability_t portstatus;

	printf("-----Mac status-----\n");
	for (rtk_uint32 index = 0; index < ARRAY_SIZE(port_list); index++) {
		retVal = rtk_port_macStatus_get(port_list[index], &portstatus);
		if (retVal != RT_ERR_OK) {
			printf("rtk_port_macStatus_get(%d) failed, ret %d\n",
				port_list[index], retVal);
			continue;
		}
		printf("PORT%d: forcemode 0x%x, speed %d, duplex %d, link %d\n",
			port_list[index], portstatus.forcemode, portstatus.speed,
			portstatus.duplex, portstatus.link);
		printf("       nway %d, txpause %d, rxpause %d\n",
			portstatus.nway, portstatus.txpause, portstatus.rxpause);
	}
	//printf("[%s] exit\n", __func__);
}

rtk_api_ret_t eth_switch_get_mac_force_status(void)
{
	rtk_api_ret_t retVal;
	rtk_port_t port;
	rtk_mode_ext_t ext_mode;
	rtk_port_mac_ability_t mac_ablilty;

	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return retVal;
	}

	for (port = EXT_PORT0; port <= EXT_PORT1; port++) {
		retVal = rtk_port_macForceLinkExt_get(port, &ext_mode, &mac_ablilty);
		if (retVal != RT_ERR_OK) {
			printf("rtk_port_macForceLinkExt_get failed, ret %d\n", retVal);
			continue;
		}
		printf("EXT_PORT%d mode %d\n", port, ext_mode);
		printf("       forcemode 0x%x, speed %d, duplex %d, link %d\n",
			mac_ablilty.forcemode, mac_ablilty.speed,
			mac_ablilty.duplex, mac_ablilty.link);
		printf("       nway %d, txpause %d, rxpause %d\n",
			mac_ablilty.nway, mac_ablilty.txpause, mac_ablilty.rxpause);
	}
	return RT_ERR_OK;
}

rtk_api_ret_t eth_switch_set_mac_forcelink(rtk_port_t port, rtk_mode_ext_t mode)
{
	rtk_api_ret_t retVal;

	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return retVal;
	}

	rtk_mode_ext_t ext_mode;
	rtk_port_mac_ability_t mac_cfg;
	retVal = rtk_port_macForceLinkExt_get(port, &ext_mode, &mac_cfg);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_macForceLinkExt_get failed, ret %d\n", retVal);
		return retVal;
	}

	printf("EXT_PORT%d ori mode %d, new mode %d..\n", port, ext_mode, mode);
	mac_cfg.forcemode = PORT_MAC_FORCE;
	mac_cfg.speed = PORT_SPEED_100M;//PORT_SPEED_1000M;
	mac_cfg.duplex = PORT_FULL_DUPLEX;
	mac_cfg.link = PORT_LINKUP;
	mac_cfg.nway = ENABLED;
	mac_cfg.txpause = DISABLED;
	mac_cfg.rxpause = DISABLED;
	retVal = rtk_port_macForceLinkExt_set(port, mode, &mac_cfg);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_macForceLinkExt_set failed, ret %d\n", retVal);
		return retVal;
	}
	return RT_ERR_OK;
}

rtk_api_ret_t eth_switch_rgmii_delay_set(rtk_port_t port, rtk_data_t txdelay, rtk_data_t rxdelay)
{
	rtk_api_ret_t retVal;

	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return retVal;
	}

	rtk_data_t ori_TxDelay, ori_RxDelay;
	retVal = rtk_port_rgmiiDelayExt_get(port, &ori_TxDelay, &ori_RxDelay);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_rgmiiDelayExt_get failed, ret %d\n", retVal);
		return retVal;
	}

	printf("tx delay: %d -> %d\n", ori_TxDelay, txdelay);
	printf("rx delay: %d -> %d\n", ori_RxDelay, rxdelay);
	if ((ori_TxDelay != txdelay) || (ori_RxDelay != rxdelay)) {
		retVal = rtk_port_rgmiiDelayExt_set(port, txdelay, rxdelay);
		if (retVal != RT_ERR_OK) {
			printf("rtk_port_rgmiiDelayExt_set failed, ret %d\n", retVal);
			return retVal;
		}
	}
	return RT_ERR_OK;
}

rtk_api_ret_t eth_switch_rgmii_delay_get(rtk_port_t port)
{
	rtk_api_ret_t retVal;

	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return retVal;
	}

	rtk_data_t ori_TxDelay, ori_RxDelay;
	retVal = rtk_port_rgmiiDelayExt_get(port, &ori_TxDelay, &ori_RxDelay);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_rgmiiDelayExt_get failed, ret %d\n", retVal);
		return retVal;
	}

	printf("tx delay: %d\n", ori_TxDelay);
	printf("rx delay: %d\n", ori_RxDelay);
	return RT_ERR_OK;
}

void eth_switch_port_enable(rtk_port_t port, rtk_enable_t flag)
{
	printf("[%s] enter\n", __func__);

	rtk_api_ret_t retVal;
	rtk_enable_t enable = DISABLED;

	if (rtk_switch_isUtpPort(port) != RT_ERR_OK) {
		return;
	}

	retVal = rtk_port_phyEnable_get(port, &enable);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_phyEnable_get port%d failed, ret %d\n",
			port, retVal);
		return;
	}

	if (enable == flag) {
		return;
	}

	retVal = rtk_port_phyEnable_set(port, flag);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_phyEnable_set port%d failed, ret %d\n",
			port, retVal);
		return;
	}
	printf("[%s] exit\n", __func__);
}

void eth_switch_show_port_stats(rtk_port_t port, rtk_stat_port_cntr_t *port_cntrs)
{
	printf("\n-----port %d-----\n", port);
	printf("ifInOctets %llu\n", port_cntrs->ifInOctets);
	printf("dot3StatsFCSErrors %u\n", port_cntrs->dot3StatsFCSErrors);
	printf("dot3StatsSymbolErrors %u\n", port_cntrs->dot3StatsSymbolErrors);
	printf("dot3InPauseFrames %u\n", port_cntrs->dot3InPauseFrames);
	printf("dot3ControlInUnknownOpcodes %u\n", port_cntrs->dot3ControlInUnknownOpcodes);
	printf("etherStatsFragments %u\n", port_cntrs->etherStatsFragments);
	printf("etherStatsJabbers %u\n", port_cntrs->etherStatsJabbers);
	printf("ifInUcastPkts %u\n", port_cntrs->ifInUcastPkts);
	printf("etherStatsDropEvents %u\n", port_cntrs->etherStatsDropEvents);
	printf("etherStatsOctets %llu\n", port_cntrs->etherStatsOctets);
	printf("etherStatsUndersizePkts %u\n", port_cntrs->etherStatsUndersizePkts);
	printf("etherStatsOversizePkts %u\n", port_cntrs->etherStatsOversizePkts);
	printf("etherStatsPkts64Octets %u\n", port_cntrs->etherStatsPkts64Octets);
	printf("etherStatsPkts65to127Octets %u\n", port_cntrs->etherStatsPkts65to127Octets);
	printf("etherStatsPkts128to255Octets %u\n", port_cntrs->etherStatsPkts128to255Octets);
	printf("etherStatsPkts256to511Octets %u\n", port_cntrs->etherStatsPkts256to511Octets);
	printf("etherStatsPkts512to1023Octets %u\n", port_cntrs->etherStatsPkts512to1023Octets);
	printf("etherStatsPkts1024toMaxOctets %u\n", port_cntrs->etherStatsPkts1024toMaxOctets);
	printf("etherStatsMcastPkts %u\n", port_cntrs->etherStatsMcastPkts);
	printf("etherStatsBcastPkts %u\n", port_cntrs->etherStatsBcastPkts);
	printf("ifOutOctets %llu\n", port_cntrs->ifOutOctets);
	printf("dot3StatsSingleCollisionFrames %u\n", port_cntrs->dot3StatsSingleCollisionFrames);
	printf("dot3StatsMultipleCollisionFrames %u\n", port_cntrs->dot3StatsMultipleCollisionFrames);
	printf("dot3StatsDeferredTransmissions %u\n", port_cntrs->dot3StatsDeferredTransmissions);
	printf("dot3StatsLateCollisions %u\n", port_cntrs->dot3StatsLateCollisions);
	printf("etherStatsCollisions %u\n", port_cntrs->etherStatsCollisions);
	printf("dot3StatsExcessiveCollisions %u\n", port_cntrs->dot3StatsExcessiveCollisions);
	printf("dot3OutPauseFrames %u\n", port_cntrs->dot3OutPauseFrames);
	printf("dot1dBasePortDelayExceededDiscards %u\n", port_cntrs->dot1dBasePortDelayExceededDiscards);
	printf("dot1dTpPortInDiscards %u\n", port_cntrs->dot1dTpPortInDiscards);
	printf("ifOutUcastPkts %u\n", port_cntrs->ifOutUcastPkts);
	printf("ifOutMulticastPkts %u\n", port_cntrs->ifOutMulticastPkts);
	printf("ifOutBrocastPkts %u\n", port_cntrs->ifOutBrocastPkts);
	printf("outOampduPkts %u\n", port_cntrs->outOampduPkts);
	printf("inOampduPkts %u\n", port_cntrs->inOampduPkts);
	printf("pktgenPkts %u\n", port_cntrs->pktgenPkts);
	printf("inMldChecksumError %u\n", port_cntrs->inMldChecksumError);
	printf("inIgmpChecksumError %u\n", port_cntrs->inIgmpChecksumError);
	printf("inMldSpecificQuery %u\n", port_cntrs->inMldSpecificQuery);
	printf("inMldGeneralQuery %u\n", port_cntrs->inMldGeneralQuery);
	printf("inIgmpSpecificQuery %u\n", port_cntrs->inIgmpSpecificQuery);
	printf("inIgmpGeneralQuery %u\n", port_cntrs->inIgmpGeneralQuery);
	printf("inMldLeaves %u\n", port_cntrs->inMldLeaves);
	printf("inIgmpLeaves %u\n", port_cntrs->inIgmpLeaves);
	printf("inIgmpJoinsSuccess %u\n", port_cntrs->inIgmpJoinsSuccess);
	printf("inIgmpJoinsFail %u\n", port_cntrs->inIgmpJoinsFail);
	printf("inMldJoinsSuccess %u\n", port_cntrs->inMldJoinsSuccess);
	printf("inMldJoinsFail %u\n", port_cntrs->inMldJoinsFail);
	printf("inReportSuppressionDrop %u\n", port_cntrs->inReportSuppressionDrop);
	printf("inLeaveSuppressionDrop %u\n", port_cntrs->inLeaveSuppressionDrop);
	printf("outIgmpReports %u\n", port_cntrs->outIgmpReports);
	printf("outIgmpLeaves %u\n", port_cntrs->outIgmpLeaves);
	printf("outIgmpGeneralQuery %u\n", port_cntrs->outIgmpGeneralQuery);
	printf("outIgmpSpecificQuery %u\n", port_cntrs->outIgmpSpecificQuery);
	printf("outMldReports %u\n", port_cntrs->outMldReports);
	printf("outMldLeaves %u\n", port_cntrs->outMldLeaves);
	printf("outMldGeneralQuery %u\n", port_cntrs->outMldGeneralQuery);
	printf("outMldSpecificQuery %u\n", port_cntrs->outMldSpecificQuery);
	printf("inKnownMulticastPkts %u\n", port_cntrs->inKnownMulticastPkts);
	printf("ifInMulticastPkts %u\n", port_cntrs->ifInMulticastPkts);
	printf("ifInBroadcastPkts %u\n", port_cntrs->ifInBroadcastPkts);
	printf("ifOutDiscards %u\n", port_cntrs->ifOutDiscards);
	printf("----------------\n");
}

void eth_switch_get_port_stats_all(rtk_port_t port)
{
	rtk_api_ret_t retVal;
	rtk_stat_port_cntr_t port_cntrs = {0};

	printf("[%s] enter\n", __func__);
	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return;
	}

	retVal = rtk_stat_port_getAll(port, &port_cntrs);
	if (retVal != RT_ERR_OK) {
		printf("rtk_stat_port_getAll(%d) failed, ret %d\n", port, retVal);
		return;
	}

	eth_switch_show_port_stats(port, &port_cntrs);
	printf("[%s] exit\n", __func__);
}

void eth_switch_get_port_stats_simple(rtk_port_t port)
{
	rtk_api_ret_t retVal;

	printf("[%s] enter\n", __func__);
	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return;
	}

	struct port_stats_simple stats_arr[] = {
		{STAT_IfInUcastPkts, "IfInUcastPkts", 0}, 
		{STAT_IfInMulticastPkts, "IfInMulticastPkts", 0}, 
		{STAT_IfInBroadcastPkts, "IfInBroadcastPkts", 0}, 
		{STAT_IfOutDiscards, "IfOutDiscards", 0}, 
		{STAT_IfOutUcastPkts, "IfOutUcastPkts", 0}, 
		{STAT_IfOutMulticastPkts, "IfOutMulticastPkts", 0}, 
		{STAT_IfOutBroadcastPkts, "IfOutBroadcastPkts", 0}, 
		{STAT_EtherStatsPkts64Octets, "EtherStatsPkts64Octets", 0}, 
		{STAT_EtherStatsPkts65to127Octets, "EtherStatsPkts65to127Octets", 0}, 
		{STAT_EtherStatsPkts128to255Octets, "EtherStatsPkts128to255Octets", 0}, 
		{STAT_EtherStatsPkts256to511Octets, "EtherStatsPkts256to511Octets", 0}, 
		{STAT_EtherStatsPkts512to1023Octets, "EtherStatsPkts512to1023Octets", 0}, 
		{STAT_EtherStatsPkts1024to1518Octets, "EtherStatsPkts1024to1518Octets", 0}, 
	};
	printf("\n-----port %d---------------------------\n", port);
	for (int i = 0; i < ARRAY_SIZE(stats_arr); i++) {
		retVal = rtk_stat_port_get(port, stats_arr[i].index, &stats_arr[i].val);
		if (retVal != RT_ERR_OK) {
			printf("rtk_stat_port_get(%d) %s failed, ret %d\n",
				port, stats_arr[i].title, retVal);
			continue;
		}
		printf("%-32s: %llu\n", stats_arr[i].title, stats_arr[i].val);
	}
	printf("----------------------------------------\n");
}

void eth_switch_reset_port_stats(rtk_port_t port)
{
	printf("[%s] enter\n", __func__);

	rtk_api_ret_t retVal;
	retVal = rtk_stat_port_reset(port);
	if (retVal != RT_ERR_OK) {
		printf("rtk_stat_port_reset port%d failed, ret %d\n",
			port, retVal);
	}
	printf("[%s] exit\n", __func__);
}

void eth_switch_port_set_loop(rtk_port_t port, rtk_enable_t flag)
{
	printf("[%s] enter\n", __func__);

	rtk_api_ret_t retVal;
	rtk_enable_t enable = DISABLED;
	printf("set port%d loop %d\n", port, flag);
	
	retVal = rtk_port_macLocalLoopbackEnable_get(port, &enable);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_macLocalLoopbackEnable_get port%d failed, ret %d\n",
			port, retVal);
		return;
	}

	if (enable == flag) {
		return;
	}

	retVal = rtk_port_macLocalLoopbackEnable_set(port, flag);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_macLocalLoopbackEnable_set port%d failed, ret %d\n",
			port, retVal);
		return;
	}
	printf("[%s] exit\n", __func__);
}

rtk_port_t eth_switch_get_panel_port(void)
{
	return BOX_PANEL_PORT;
}

rtk_port_t eth_switch_get_mpu_port(void)
{
	return BOX_MPU_PORT;
}

rtk_port_t eth_switch_get_wan_port(void)
{
	return BOX_WAN_PORT;
}

rtk_port_t eth_switch_get_lan_port(void)
{
	return BOX_LAN_PORT;
}

/* tagged = flase, for uboot; tagged = true, for kernel */
int eth_switch_cfg_vlan_mng(rtk_port_t curr_port, u32 tagged)
{
	rtk_api_ret_t retVal;
	printf("[%s] port%d, tagged %d...\n", __func__, curr_port, tagged);

	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return retVal;
	}

	printf("rtk_vlan_reset\n");
	retVal = rtk_vlan_reset();
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_reset failed, ret %d\n", retVal);
		return retVal;
	}

	/* manage vlan member: panel_port, MPU or COPU port */
	rtk_vlan_cfg_t vlan_mng = {0}; 
	retVal = rtk_vlan_get(VLANID_MNG, &vlan_mng);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_set vlan%d failed, ret %d\n", VLANID_MNG, retVal);
		return retVal;
	}

	rtk_port_t panel_port = eth_switch_get_panel_port();
	printf("add port%d to manage vlan, panel port%d..\n", curr_port, panel_port);
	RTK_PORTMASK_PORT_SET(vlan_mng.mbr, panel_port);
	RTK_PORTMASK_PORT_SET(vlan_mng.mbr, curr_port);
	RTK_PORTMASK_PORT_SET(vlan_mng.untag, panel_port);
	if (!tagged) {
		RTK_PORTMASK_PORT_SET(vlan_mng.untag, curr_port);
	}
	retVal = rtk_vlan_set(VLANID_MNG, &vlan_mng);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_set vlan%d failed, ret %d\n", VLANID_MNG, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portIgrFilterEnable_set(panel_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portIgrFilterEnable_get port%d failed, ret %d\n",
			panel_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portAcceptFrameType_set(panel_port, ACCEPT_FRAME_TYPE_UNTAG_ONLY);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portAcceptFrameType_set port%d failed, ret %d\n",
			panel_port, retVal);
		return retVal;
	}

#if 1
	retVal = rtk_vlan_tagMode_set(panel_port, VLAN_TAG_MODE_ORIGINAL);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_tagMode_set port%d failed, ret %d\n",
			curr_port, retVal);
		return retVal;
	}
#endif

	retVal = rtk_vlan_portIgrFilterEnable_set(curr_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portIgrFilterEnable_get port%d failed, ret %d\n",
			curr_port, retVal);
		return retVal;
	}

	rtk_vlan_acceptFrameType_t frame_type;
	if (tagged) {
		frame_type = ACCEPT_FRAME_TYPE_ALL;
	} else {
		frame_type = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
	}
	retVal = rtk_vlan_portAcceptFrameType_set(curr_port, frame_type);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portAcceptFrameType_set port%d failed, ret %d\n",
			curr_port, retVal);
		return retVal;
	}

	if (tagged) {
		retVal = rtk_vlan_tagMode_set(curr_port, VLAN_TAG_MODE_ORIGINAL);
		if (retVal != RT_ERR_OK) {
			printf("rtk_vlan_tagMode_set port%d failed, ret %d\n",
				curr_port, retVal);
			return retVal;
		}
	}

	retVal = rtk_vlan_portPvid_set(panel_port, PVID_COPU, 0);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portPvid_set port%d vlan%d failed, ret %d\n",
			panel_port, PVID_COPU, retVal);
		return retVal;
	}

	rtk_vlan_t pvid;
	rtk_port_t mpu_port = eth_switch_get_mpu_port();
	if (mpu_port == curr_port) {
		pvid = PVID_MPU;
	} else {
		pvid = PVID_COPU;
	}
	printf("port%d, pvid %d\n", curr_port, pvid);
	retVal = rtk_vlan_portPvid_set(curr_port, pvid, 0);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portPvid_set port%d vlan%d failed, ret %d\n",
			curr_port, pvid, retVal);
		return retVal;
	}

	retVal = rtk_port_phyEnable_set(panel_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portAcceptFrameType_set port%d failed, ret %d\n",
			curr_port, retVal);
		return retVal;
	}

	retVal = rtk_port_phyEnable_set(curr_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portAcceptFrameType_set port%d failed, ret %d\n",
			curr_port, retVal);
		return retVal;
	}
	printf("[%s] exit\n", __func__);
	return RT_ERR_OK;
}

int eth_switch_cfg_vlan_inter(u32 mpu_tagged)
{
	rtk_api_ret_t retVal;
	printf("[%s] mpu_tagged %d\n", __func__, mpu_tagged);

	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return retVal;
	}

	retVal = rtk_vlan_reset();
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_reset failed, ret %d\n", retVal);
		return retVal;
	}

	/* manage vlan member: panel_port, MPU and COPU ports */
	rtk_vlan_cfg_t vlan_inter = {0}; 
	//rtk_port_t panel_port = eth_switch_get_panel_port();
	rtk_port_t mpu_port = eth_switch_get_mpu_port();
	//rtk_port_t port_list[] = {UTP_PORT0, UTP_PORT1, UTP_PORT4, EXT_PORT0, EXT_PORT1};
	rtk_port_t port_list[] = {UTP_PORT0, UTP_PORT1, EXT_PORT0, EXT_PORT1};
	u32 i;

	for (i = 0; i < ARRAY_SIZE(port_list); i++) {
		RTK_PORTMASK_PORT_SET(vlan_inter.mbr, port_list[i]);
#if 0
		if (panel_port == port_list[i]) {
			RTK_PORTMASK_PORT_SET(vlan_inter.untag, panel_port);
		}
#else
		if ((mpu_port == port_list[i]) && (mpu_tagged)) {
			continue;
		}

		RTK_PORTMASK_PORT_SET(vlan_inter.untag, port_list[i]);
#endif
		
	}

	retVal = rtk_vlan_set(VLANID_INTER, &vlan_inter);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_set vlan%d failed, ret %d\n", VLANID_INTER, retVal);
		return retVal;
	}


	for (i = 0; i < ARRAY_SIZE(port_list); i++) {
		retVal = rtk_vlan_portIgrFilterEnable_set(port_list[i], ENABLED);
		if (retVal != RT_ERR_OK) {
			printf("rtk_vlan_portIgrFilterEnable_get port%d failed, ret %d\n",
				port_list[i], retVal);
			return retVal;
		}

		rtk_vlan_acceptFrameType_t frame_type;
		//if (panel_port == port_list[i]) {
		if (mpu_port == port_list[i]) {
			frame_type = ACCEPT_FRAME_TYPE_ALL;
		} else {
			frame_type = ACCEPT_FRAME_TYPE_UNTAG_ONLY;
		}
		retVal = rtk_vlan_portAcceptFrameType_set(port_list[i], frame_type);
		if (retVal != RT_ERR_OK) {
			printf("rtk_vlan_portAcceptFrameType_set port%d failed, ret %d\n",
				port_list[i], retVal);
			return retVal;
		}

		if ((mpu_port == port_list[i]) && (mpu_tagged)) {
			retVal = rtk_vlan_tagMode_set(port_list[i], VLAN_TAG_MODE_ORIGINAL);
			if (retVal != RT_ERR_OK) {
				printf("rtk_vlan_tagMode_set port%d failed, ret %d\n",
					port_list[i], retVal);
				return retVal;
			}
		}

		rtk_vlan_t pvid;
		if (mpu_port == port_list[i]) {
			pvid = PVID_MPU;
		} else {
			pvid = PVID_COPU;
		}
		retVal = rtk_vlan_portPvid_set(port_list[i], pvid, 0);
		if (retVal != RT_ERR_OK) {
			printf("rtk_vlan_portPvid_set port%d vlan%d failed, ret %d\n",
				port_list[i], pvid, retVal);
			return retVal;
		}
	}

	for (i = 0; i < ARRAY_SIZE(port_list); i++) {
		retVal = rtk_port_phyEnable_set(port_list[i], ENABLED);
		if (retVal != RT_ERR_OK) {
			printf("rtk_port_phyEnable_set port%d failed, ret %d\n",
				port_list[i], retVal);
			return retVal;
		}
		
	}
	printf("[%s] exit\n", __func__);
	return RT_ERR_OK;
}

int eth_switch_cfg_vlan_wan(void)
{
	rtk_api_ret_t retVal;
	printf("[%s] enter\n", __func__);

	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return retVal;
	}

#if 0
	retVal = rtk_vlan_reset();
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_reset failed, ret %d\n", retVal);
		return retVal;
	}
#endif

	/* manage vlan member: panel_port, MPU and COPU ports */
	rtk_vlan_cfg_t vlan_wan = {0}; 
	rtk_port_t wan_port = eth_switch_get_wan_port();
	rtk_port_t mpu_port = eth_switch_get_mpu_port();

	RTK_PORTMASK_PORT_SET(vlan_wan.mbr, wan_port);
	RTK_PORTMASK_PORT_SET(vlan_wan.mbr, mpu_port);
	RTK_PORTMASK_PORT_SET(vlan_wan.untag, wan_port);

	retVal = rtk_vlan_set(VLANID_WAN, &vlan_wan);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_set vlan%d failed, ret %d\n", VLANID_WAN, retVal);
		return retVal;
	}

	/* MPU port already cfged, just cfg WAN port */
	retVal = rtk_vlan_portIgrFilterEnable_set(wan_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portIgrFilterEnable_get port%d failed, ret %d\n",
			wan_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portAcceptFrameType_set(wan_port, ACCEPT_FRAME_TYPE_UNTAG_ONLY);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portAcceptFrameType_set port%d failed, ret %d\n",
			wan_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_tagMode_set(wan_port, VLAN_TAG_MODE_ORIGINAL);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_tagMode_set port%d failed, ret %d\n",
			wan_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portPvid_set(wan_port, PVID_WAN, 0);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portPvid_set port%d vlan%d failed, ret %d\n",
			wan_port, PVID_WAN, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portIgrFilterEnable_set(mpu_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portIgrFilterEnable_get port%d failed, ret %d\n",
			mpu_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portAcceptFrameType_set(mpu_port, ACCEPT_FRAME_TYPE_ALL);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portAcceptFrameType_set port%d failed, ret %d\n",
			mpu_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_tagMode_set(mpu_port, VLAN_TAG_MODE_ORIGINAL);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_tagMode_set port%d failed, ret %d\n",
			mpu_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portPvid_set(mpu_port, PVID_MPU, 0);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portPvid_set port%d vlan%d failed, ret %d\n",
			mpu_port, PVID_WAN, retVal);
		return retVal;
	}

	retVal = rtk_port_phyEnable_set(wan_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_phyEnable_set port%d failed, ret %d\n",
			wan_port, retVal);
		return retVal;
	}

	retVal = rtk_port_phyEnable_set(mpu_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_phyEnable_set port%d failed, ret %d\n",
			mpu_port, retVal);
		return retVal;
	}
	printf("[%s] exit\n", __func__);
	return RT_ERR_OK;
}

int eth_switch_cfg_vlan_lan(void)
{
	rtk_api_ret_t retVal;
	printf("[%s] enter\n", __func__);

	retVal = eth_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("eth_switch_init failed, ret %d\n", retVal);
		return retVal;
	}

#if 0
	retVal = rtk_vlan_reset();
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_reset failed, ret %d\n", retVal);
		return retVal;
	}
#endif

	/* manage vlan member: panel_port, MPU and COPU ports */
	rtk_vlan_cfg_t vlan_lan = {0}; 
	rtk_port_t lan_port = eth_switch_get_lan_port();
	rtk_port_t mpu_port = eth_switch_get_mpu_port();

	RTK_PORTMASK_PORT_SET(vlan_lan.mbr, lan_port);
	RTK_PORTMASK_PORT_SET(vlan_lan.mbr, mpu_port);
	RTK_PORTMASK_PORT_SET(vlan_lan.untag, lan_port);

	retVal = rtk_vlan_set(VLANID_LAN, &vlan_lan);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_set vlan%d failed, ret %d\n", VLANID_LAN, retVal);
		return retVal;
	}

	/* MPU port already cfged, just cfg WAN port */
	retVal = rtk_vlan_portIgrFilterEnable_set(lan_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portIgrFilterEnable_get port%d failed, ret %d\n",
			lan_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portAcceptFrameType_set(lan_port, ACCEPT_FRAME_TYPE_UNTAG_ONLY);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portAcceptFrameType_set port%d failed, ret %d\n",
			lan_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_tagMode_set(lan_port, VLAN_TAG_MODE_ORIGINAL);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_tagMode_set port%d failed, ret %d\n",
			lan_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portPvid_set(lan_port, PVID_LAN, 0);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portPvid_set port%d vlan%d failed, ret %d\n",
			lan_port, PVID_LAN, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portIgrFilterEnable_set(mpu_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portIgrFilterEnable_get port%d failed, ret %d\n",
			mpu_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portAcceptFrameType_set(mpu_port, ACCEPT_FRAME_TYPE_ALL);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portAcceptFrameType_set port%d failed, ret %d\n",
			mpu_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_tagMode_set(mpu_port, VLAN_TAG_MODE_ORIGINAL);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_tagMode_set port%d failed, ret %d\n",
			mpu_port, retVal);
		return retVal;
	}

	retVal = rtk_vlan_portPvid_set(mpu_port, PVID_MPU, 0);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_portPvid_set port%d vlan%d failed, ret %d\n",
			mpu_port, PVID_WAN, retVal);
		return retVal;
	}

	retVal = rtk_port_phyEnable_set(lan_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_phyEnable_set port%d failed, ret %d\n",
			lan_port, retVal);
		return retVal;
	}

	retVal = rtk_port_phyEnable_set(mpu_port, ENABLED);
	if (retVal != RT_ERR_OK) {
		printf("rtk_port_phyEnable_set port%d failed, ret %d\n",
			mpu_port, retVal);
		return retVal;
	}

	printf("[%s] exit\n", __func__);
	return RT_ERR_OK;
}

int eth_switch_get_vlan_cfg(rtk_vlan_t vid)
{
	rtk_api_ret_t retVal;
	rtk_vlan_cfg_t vlanCfg = {0};

	retVal = rtk_vlan_get(vid, &vlanCfg);
	if (retVal != RT_ERR_OK) {
		printf("rtk_vlan_get failed, vid %d, ret %d\n", vid, retVal);
		return retVal;
	}

	printf("-----vlan %d config-----\n", vid);
	printf("mbr      : 0x%x\n", vlanCfg.mbr.bits[0]);
	printf("untag    : 0x%x\n", vlanCfg.untag.bits[0]);
	printf("ivl_en   : 0x%x\n", vlanCfg.ivl_en);
	printf("fid_msti : 0x%x\n", vlanCfg.fid_msti);
	printf("envlanpol: 0x%x\n", vlanCfg.envlanpol);
	printf("meteridx : 0x%x\n", vlanCfg.meteridx);
	printf("vbpen    : 0x%x\n", vlanCfg.vbpen);
	printf("vbpri    : 0x%x\n", vlanCfg.vbpri);
	printf("-----ports config-----\n");
	rtk_port_t port;
	rtk_enable_t igr_filter;
	rtk_vlan_acceptFrameType_t frame_type;
	rtk_vlan_tagMode_t tag_mode;
	rtk_vlan_t pvid;
	rtk_pri_t priority;
	u32 members = 0;
	for (port = UTP_PORT0; port < RTK_SWITCH_PORT_NUM; port++) {
		if (vlanCfg.mbr.bits[0] & BIT(port)) {
			members++;
			if (vlanCfg.untag.bits[0] & BIT(port)) {
				printf("port%d: untagged\n", port);
			} else {
				printf("port%d: tagged\n", port);
			}
			retVal = rtk_vlan_portIgrFilterEnable_get(port, &igr_filter);
			if (retVal != RT_ERR_OK) {
				printf("rtk_vlan_portIgrFilterEnable_get failed, ret %d\n", retVal);
				return retVal;
			}

			if (igr_filter) {
				printf("ingress filter: enabled\n");
			} else {
				printf("ingress filter: disabled\n");
			}

			if (igr_filter) {
				retVal = rtk_vlan_portAcceptFrameType_get(port, &frame_type);
				if (retVal != RT_ERR_OK) {
					printf("rtk_vlan_portAcceptFrameType_get failed, ret %d\n", retVal);
					return retVal;
				}
				if (frame_type == ACCEPT_FRAME_TYPE_ALL) {
					printf("accept frame type: all\n");
				} else if (frame_type == ACCEPT_FRAME_TYPE_TAG_ONLY) {
					printf("accept frame type: tag only\n");
				} else if (frame_type == ACCEPT_FRAME_TYPE_UNTAG_ONLY) {
					printf("accept frame type: untag only\n");
				}
			}

			retVal = rtk_vlan_tagMode_get(port, &tag_mode);
			if (retVal != RT_ERR_OK) {
				printf("rtk_vlan_tagMode_get failed, ret %d\n", retVal);
				return retVal;
			}
			if (tag_mode == VLAN_TAG_MODE_ORIGINAL) {
				printf("tag mode: original\n");
			} else if (tag_mode == VLAN_TAG_MODE_KEEP_FORMAT) {
				printf("tag mode: keep format\n");
			} else if (tag_mode == VLAN_TAG_MODE_PRI) {
				printf("tag mode: pri\n");
			} else if (tag_mode == VLAN_TAG_MODE_REAL_KEEP_FORMAT) {
				printf("tag mode: real keep format\n");
			}

			retVal = rtk_vlan_portPvid_get(port, &pvid, &priority);
			if (retVal != RT_ERR_OK) {
				printf("rtk_vlan_portPvid_get failed, ret %d\n", retVal);
				return retVal;
			}
			printf("pvid: %d, priority: %d\n", pvid, priority);
			printf("----------------------\n");
		}
	}

	printf("total members: %d\n", members);
	return RT_ERR_OK;
	
}

int switch_select_reset_pin(void)
{
	char *dev_name = "pinctrl@f0d95000";
	char *group = "PB15_grp";
	char *function = "PB";
	struct udevice *currdev;

	int ret = uclass_get_device_by_name(UCLASS_PINCTRL, dev_name, &currdev);
	if (ret) {
		printf("Can't get the pin-controller: %s!\n", dev_name);
		return -ENODEV;
	}

	struct pinctrl_ops *ops = pinctrl_get_ops(currdev);
	if (!ops->get_groups_count || !ops->get_group_name ||
		!ops->get_functions_count || !ops->get_function_name ||
		!ops->pinmux_group_set) {
		printf("pinctl ops missing\n");
		return -ENOSYS;
	}

	unsigned ngroups, grp_selector;
	ngroups = ops->get_groups_count(currdev);
	for (grp_selector = 0; grp_selector < ngroups; grp_selector++) {
		const char *gname = ops->get_group_name(currdev, grp_selector);
		if (!strcmp(group, gname)) {
			break;
		}
	}

	printf("grp_selector : %d\n", grp_selector);
	if (grp_selector == ngroups) {
		printf("group: %s not found\n", group);
		return -ENODEV;
	}

	unsigned nfuncs, func_selector;
	nfuncs = ops->get_functions_count(currdev);
	for (func_selector = 0; func_selector < nfuncs; func_selector++) {
		const char *fname = ops->get_function_name(currdev, func_selector);
		if (!strcmp(function, fname)) {
			break;
		}
	}

	printf("func_selector: %d\n", func_selector);
	if (func_selector == nfuncs) {
		printf("func: %s not found\n", function);
		return -ENODEV;
	}

	return ops->pinmux_group_set(currdev, grp_selector, func_selector);
}

void switch_reset(void)
{
	int ret;
	u32 gpio;
	char *reset_pin = "PB15";

	ret = switch_select_reset_pin();
	if (ret) {
		printf("select reset pin failed, ret %d\n", ret);
		return;
	}

#if defined(CONFIG_DM_GPIO)
	ret = gpio_lookup_name(reset_pin, NULL, NULL, &gpio);
	if (ret)
#else
	/* turn the gpio name into a gpio number */
	gpio = name_to_gpio(reset_pin);
	if (gpio < 0)
#endif
	{
		printf("GPIO: %s not found\n", reset_pin);
		return;
	}

	printf("gpio %d\n", gpio);
	ret = gpio_request(gpio, "cmd_gpio");
	if (ret && ret != -EBUSY) {
		printf("gpio: requesting pin %u failed\n", gpio);
		return;
	}

	printf("reset1\n");
	gpio_direction_output(gpio, 0);
	mdelay(200);
	printf("dereset1\n");
	gpio_direction_output(gpio, 1);
	mdelay(200);
	printf("reset2\n");
	gpio_direction_output(gpio, 0);
	mdelay(200);
	printf("dereset2\n");
	gpio_direction_output(gpio, 1);
	mdelay(200);
	
	printf("switch reset\n");
}

void switch_initialze(void)
{
	/* get dev type, only mpu init switch chip */
	u32 device_id = get_box_device_id();
	printf("device_id %d\n", device_id);
	if (device_id != 0) {
		return;
	}

	rtk_api_ret_t retVal;
	/* reset switch chip */
	for (int i = 0; i < 3; i++) {
		switch_reset();
		retVal = eth_switch_init();
		if (retVal == RT_ERR_OK) {
			break;
		}
	}

	/* init switch chip, cfg vlan */
#if 0
	rtk_port_t mpu_port = eth_switch_get_mpu_port();
	printf("mpu_port %d\n", mpu_port);
	int ret = eth_switch_cfg_vlan_mng(mpu_port, false);
	printf("eth_switch_cfg_vlan_mng() ret %d\n", ret);
#else
	eth_switch_cfg_vlan_inter(1);
	eth_switch_cfg_vlan_wan();
	eth_switch_cfg_vlan_lan();

	eth_switch_set_mac_forcelink(EXT_PORT0, MODE_EXT_RGMII);
	eth_switch_set_mac_forcelink(EXT_PORT1, MODE_EXT_RGMII);
	eth_switch_rgmii_delay_set(EXT_PORT0, 1, 4);
	eth_switch_rgmii_delay_set(EXT_PORT1, 1, 4);
#endif
}

int eth_switch_test_entry(int argc, char *const argv[])
{
	u32 port;
	u32 tagged;
	u32 linkmode;
	u32 txdelay;
	u32 rxdelay;
	if (argc < 2) {
		printf("argc %d err\n", argc);
		return -EINVAL;
	}

	if (!strncmp(argv[1], "init", 4)) {
		switch_initialze();
	} else if (!strncmp(argv[1], "vlanmng", 7)) {
		port = simple_strtoul(argv[2], NULL, 16);
		tagged = simple_strtoul(argv[3], NULL, 16);
		eth_switch_cfg_vlan_mng(port, tagged);
	} else if (!strncmp(argv[1], "vlanint", 7)) {
		tagged = simple_strtoul(argv[2], NULL, 16);
		eth_switch_cfg_vlan_inter(tagged);
	} else if (!strncmp(argv[1], "vlanwan", 7)) {
		eth_switch_cfg_vlan_wan();
	} else if (!strncmp(argv[1], "vlanlan", 7)) {
		eth_switch_cfg_vlan_lan();
	} else if (!strncmp(argv[1], "vlanget", 7)) {
		u32 vid = simple_strtoul(argv[2], NULL, 16);
		eth_switch_get_vlan_cfg(vid);
	} else if (!strncmp(argv[1], "porten", 6)) {
		port = simple_strtoul(argv[2], NULL, 16);
		eth_switch_port_enable(port, ENABLED);
	} else if (!strncmp(argv[1], "portdis", 7)) {
		port = simple_strtoul(argv[2], NULL, 16);
		eth_switch_port_enable(port, DISABLED);
	} else if (!strncmp(argv[1], "portloop", 8)) {
		port = simple_strtoul(argv[2], NULL, 16);
		eth_switch_port_set_loop(port, ENABLED);
	} else if (!strncmp(argv[1], "portunloop", 10)) {
		port = simple_strtoul(argv[2], NULL, 16);
		eth_switch_port_set_loop(port, DISABLED);
	} else if (!strncmp(argv[1], "link", 4)) {
		eth_switch_get_phy_status();
		eth_switch_get_mac_status();
		eth_switch_get_mac_force_status();
	} else if (!strncmp(argv[1], "statall", 7)) {
		port = simple_strtoul(argv[2], NULL, 16);
		eth_switch_get_port_stats_all(port);
	} else if (!strncmp(argv[1], "statsim", 7)) {
		port = simple_strtoul(argv[2], NULL, 16);
		eth_switch_get_port_stats_simple(port);
	} else if (!strncmp(argv[1], "statrst", 7)) {
		port = simple_strtoul(argv[2], NULL, 16);
		eth_switch_reset_port_stats(port);
	} else if (!strncmp(argv[1], "frcstate", 8)) {
		eth_switch_get_mac_force_status();
	} else if (!strncmp(argv[1], "frclink", 7)) {
		port = strtoul(argv[2], NULL, 16);
		linkmode = strtoul(argv[3], NULL, 16);
		eth_switch_set_mac_forcelink(port, linkmode);
	} else if (!strncmp(argv[1], "dlyset", 6)) {
		port = strtoul(argv[2], NULL, 16);
		txdelay = strtoul(argv[3], NULL, 16);
		rxdelay = strtoul(argv[4], NULL, 16);
		eth_switch_rgmii_delay_set(port, txdelay, rxdelay);
	} else if (!strncmp(argv[1], "dlyget", 6)) {
		port = strtoul(argv[2], NULL, 16);
		eth_switch_rgmii_delay_get(port);
	} else {
		printf("unsupport cmd: %s\n", argv[1]);
	}

	return 0;
}


void eth_switch_set_mdio_dev(struct mii_dev *dev)
{
	printf("\nmii_dev 0x%p\nread 0x%p\nwrite 0x%p\n",
		dev, dev->read, dev->write);
	switch_dev = dev;
}

int eth_switch_mdio_read(int mdio_addr, int mdio_reg)
{
	if ((switch_dev != NULL) && (switch_dev->read != NULL)) {
		return switch_dev->read(switch_dev, mdio_addr, 0, mdio_reg);
	} else {
		printf("mdio read failed, switch_dev 0x%p not initialized\n",
			switch_dev);
		return -ENOENT;
	}
}

int eth_switch_mdio_write(int mdio_addr, int mdio_reg, u16 mdio_val)
{
	if ((switch_dev != NULL) && (switch_dev->write != NULL)) {
		return switch_dev->write(switch_dev, mdio_addr, 0,
				mdio_reg, mdio_val);
	} else {
		printf("mdio write failed, switch_dev 0x%p not initialized\n",
			switch_dev);
		return -ENOENT;
	}
}
