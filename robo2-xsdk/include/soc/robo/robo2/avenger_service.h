/*
 * $Id: avenger_service.h $
 *
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _AVENGER_SERVICE_H
#define _AVENGER_SERVICE_H

int drv_avgr_port_init(int unit);
int drv_avgr_port_set(int unit, soc_pbmp_t bmp,
                uint32 prop_type, uint32 prop_val);
int drv_avgr_port_get(int unit, int port,
                uint32 prop_type, uint32 *prop_val);

int drv_avgr_port_status_get(int unit, uint32 port, uint32 status_type, uint32 *val);

int drv_avgr_port_pri_mapop_get(int unit, int port, int op_type, uint32 pkt_pri,
                       uint32 cfi, uint32 *internal_pri, uint32 *color);
int drv_avgr_port_pri_mapop_set(int unit, int port, int op_type, uint32 pkt_pri,
                       uint32 cfi, uint32 internal_pri, uint32 color);

int drv_avgr_counter_thread(int unit,uint32 thread_op,uint32 flags,
                           int interval,soc_pbmp_t bmp);
int drv_avgr_counter_set(int unit,soc_pbmp_t bmp,uint32 counter_type,
                        uint64 val);
int drv_avgr_counter_get(int unit, uint32 port, uint32 counter_type,
                        int sync_hw, uint64 *val);
int drv_avgr_counter_reset(int unit);

/* Trunk driver APIs */
int drv_avgr_trunk_init(int unit);
int drv_avgr_trunk_create(int unit,uint32 flags,int *tid);
int drv_avgr_trunk_destroy(int unit,int tid);
int drv_avgr_trunk_modify(int unit,int tid,int gport,int op);
int drv_avgr_trunk_get(int unit, int tid, soc_pbmp_t *bmp,
                   uint32 flag, uint32 *hash_op);
int drv_avgr_trunk_psc_get(int unit,int tid,int *psc);
int drv_avgr_trunk_psc_set(int unit,int tid,int psc);

extern drv_if_t drv_avenger_services;
int drv_avgr_vlan_init(int unit);
int drv_avgr_vlan_create(int unit, uint32 vlanid);
int drv_avgr_vlan_destroy(int unit, uint32 vid);
int drv_avgr_vlan_destroy_all(int unit);
int drv_avgr_vlan_port_add(int unit, uint32 vid, int port, int flags);
int drv_avgr_vlan_port_remove(int unit, uint32 vid, int port);
int drv_avgr_vlan_gport_get_all(int unit, uint32 vlan, int array_max,
                           int *gport_array, int *flags_array,
                           int* array_size);
int drv_avgr_port_vlan_pvid_set(int unit,uint32 port,uint32 outer_tag,uint32 inner_tag);
int drv_avgr_port_vlan_pvid_get(int unit,uint32 port,uint32 *outer_tag,uint32 *inner_tag);

int drv_avgr_vlan_stg_get(int unit, uint32 vid, int *stg_state);
int drv_avgr_vlan_stg_set(int unit, uint32 vid, uint32 stg_state);
int drv_avgr_vlan_control_vlan_get(int unit, uint32 vid, uint32 *vlan_index, uint32 *flags,
                                   uint32 *learnmode, uint32 *mcastid, uint32 *ucastid);
int drv_avgr_vlan_control_vlan_set(int unit, uint32 vid, uint32 vlan_index, uint32 flags,
                                   uint32 learnmode, uint32 mcastid, uint32 ucastid);


int drv_avgr_mirror_init(int unit);
int drv_avgr_mirror_dest_create(int unit,drv_mirror_dest_t *mirror_dest);
int drv_avgr_mirror_dest_destroy(int unit,int destid);
int drv_avgr_mirror_port_add(int unit,uint32 port,uint32 flags,int destid);
int drv_avgr_mirror_port_delete(int unit,uint32 port,uint32 flags,int destid);
int drv_avgr_mirror_vlan_add(int unit,uint32 vlan,uint32 flags,int destid);
int drv_avgr_mirror_vlan_delete(int unit,uint32 vlan,uint32 flags,int destid);
int drvi_avgr_mirror_get_mirror_id(int unit, int destid, uint32 *mirror_id);

int drvi_avgr_lookup_meter_id_from_policer_id(int unit, bcm_policer_t policer_id, uint32 *meterid);

/* Linkscan APIs */
int drv_avgr_linkscan_init(int unit);
int drv_avgr_linkscan_detach(int unit);
int drv_avgr_linkscan_register(int unit,drv_link_scan_callback_t cb);
int drv_avgr_linkscan_unregister(int unit,drv_link_scan_callback_t cb);
int drv_avgr_linkscan_enable_get(int unit,int *us);
int drv_avgr_linkscan_enable_set(int unit,int us);
int drv_avgr_linkscan_dump(int unit);

int drv_avgr_fsal_init(int unit);
int drv_avgr_l2_init(int unit);
int drv_avgr_l2_add(int unit, bcm_l2_addr_t *l2addr);
int drv_avgr_l2_remove(int unit, bcm_l2_addr_t *l2addr);
int drv_avgr_l2_lookup(int unit, bcm_l2_addr_t *l2addr);
int drv_avgr_l2_lookup_next(int unit, bcm_l2_addr_t *l2addr,
                       bcm_l2_addr_t *l2addr_next);
int drv_avgr_l2_age_timer_get(int unit, uint32 *age_seconds);
int drv_avgr_l2_age_timer_set(int unit, uint32 age_seconds);
int drv_avgr_l2_flush_by_vlan(int unit, uint32 vlan_id);
int drv_avgr_l2_flush_by_port(int unit, uint32 port_id);
int drv_avgr_l2_reseed(int unit);
int drv_avgr_mcast_init(int unit);
int drv_avgr_mcast_create(int unit, bcm_mcast_addr_t *mcaddr);
int drv_avgr_mcast_destroy(int unit, bcm_mcast_addr_t *mcaddr);
int drv_avgr_mcast_add(int unit, bcm_mcast_addr_t *mcaddr, uint32 portid);
int drv_avgr_mcast_remove(int unit, bcm_mcast_addr_t *mcaddr, uint32 portid);
int drv_avgr_mcast_info_get(int unit, bcm_mcast_addr_t *mcaddr);

int drv_avgr_pkt_rx_register(int unit, drv_packet_io_callback_t cb);
int drv_avgr_pkt_tx(int unit, bcm_pkt_t **pkt, int count);

uint32 drvi_avgr_mcast_get_mcastid_by_mac_vid(bcm_mac_t mac, uint32 vid);

int drv_avgr_eav_enable_set(int unit, uint32 port, uint32 enable);
int drv_avgr_eav_enable_get(int unit, uint32 port, uint32 *enable);
int drv_avgr_eav_egress_timestamp_get(int unit, uint32 port, uint32 *timestamp);
int drv_avgr_eav_link_status_set(int unit, uint32 port, uint32 link);
int drv_avgr_eav_link_status_get(int unit, uint32 port, uint32 *link);
int drv_avgr_eav_timesync_mac_set(int unit, uint8 *mac, uint16 ether_type);
int drv_avgr_eav_timesync_mac_get(int unit, uint8 *mac, uint16 *ether_type);
int drv_avgr_eav_srp_mac_ethertype_set(int unit, uint8 *mac, uint16 ether_type);
int drv_avgr_eav_srp_mac_ethertype_get(int unit, uint8 *mac, uint16 *ether_type);
int drv_avgr_eav_pcp_mapping_set(int unit, uint32 type, int pcp,
                                 int remapped_pcp);
int drv_avgr_eav_pcp_mapping_get(int unit, uint32 type, int *pcp,
                                 int *remapped_pcp);
int drv_avgr_eav_bandwidth_set(int unit, uint32 port, uint32 type,
                               uint32 bytes_sec, uint32 bytes_burst);
int drv_avgr_eav_bandwidth_get(int unit, uint32 port, uint32 type,
                               uint32 *bytes_sec, uint32 *bytes_burst);

/* Custom field apis */
int drv_avgr_custom_field_action_t_init(int unit, bcm_custom_field_action_t *action);
int drv_avgr_custom_field_init(int unit, bcm_custom_field_params_t *field_params, bcm_custom_field_id_t *field_id);
int drv_avgr_custom_field_detach(int unit, bcm_custom_field_id_t field_id);
int drv_avgr_custom_field_rule_t_init(int unit, bcm_custom_field_l3_l4_pkt_type_t pkt_type, bcm_custom_field_rule_t *rule);
int drv_avgr_custom_field_rule_set(int unit, bcm_custom_field_id_t *field_id, bcm_custom_field_rule_t *rule, uint32 index);
int drv_avgr_custom_field_rule_clear(int unit, bcm_custom_field_id_t *field_id, uint32 index);
int drv_avgr_custom_field_action_set(int unit, bcm_custom_field_id_t *field_id, bcm_custom_field_action_t *action, uint32 index);
int drv_avgr_custom_field_action_clear(int unit, bcm_custom_field_id_t *field_id, uint32 index);
int drv_avgr_custom_field_rule_dump(int unit, bcm_custom_field_id_t *field_id, uint32 index);
int drv_avgr_custom_field_action_dump(int unit, bcm_custom_field_id_t *field_id, uint32 index);
int drv_avgr_custom_field_stat_multi_get(int unit,
                                         bcm_custom_field_id_t *field_id,
                                         uint32 index,
                                         int nstat,
                                         bcm_field_stat_t *stat_arr,
                                         uint64 *value_arr);
int drv_avgr_custom_field_stat_multi_set(int unit,
                                         bcm_custom_field_id_t *field_id,
                                         uint32 index,
                                         int nstat,
                                         bcm_field_stat_t *stat_arr,
                                         uint64 *value_arr);
int drv_avgr_stg_init(int unit);
int drv_avgr_stg_create(int unit, int *stgid);
int drv_avgr_stg_destroy(int unit, int stgid);
int drv_avgr_stg_vlan_add(int unit, int stgid, uint32 vlanid);
int drv_avgr_stg_vlan_remove(int unit, int stgid, uint32 vlanid);
int drv_avgr_stg_vlan_remove_all(int unit, int stgid);
int drv_avgr_stg_stp_get(int unit, int stgid, int port, int *stp_state);
int drv_avgr_stg_stp_set(int unit, int stgid, int port, int stp_state);
/* Custom meter apis */
extern int drv_avgr_custom_meter_init(int unit);

extern int
drv_avgr_custom_meter_profile_params_t_init(int unit,
                                            bcm_custom_meter_profile_params_t *params);
extern int
drv_avgr_custom_meter_profile_create(int unit,
                                     bcm_custom_meter_profile_params_t *profile_params,
                                     bcm_custom_meter_profile_t *profile_id);
extern int
drv_avgr_custom_meter_profile_destroy(int unit,
                                      bcm_custom_meter_profile_t profile_id);
extern int
drv_avgr_custom_meter_profile_get(int unit,
                                  bcm_custom_meter_profile_t profile_id,
                                  bcm_custom_meter_profile_params_t *profile_params);
extern int
drv_avgr_custom_meter_profile_set(int unit,
                                  bcm_custom_meter_profile_t profile_id,
                                  bcm_custom_meter_profile_params_t *profile_params);
extern int
drv_avgr_custom_meter_params_t_init(int unit,
                                    bcm_custom_meter_params_t *pm_params);
extern int
drv_avgr_custom_meter_create(int unit, bcm_custom_meter_profile_t profile_id,
                             bcm_custom_meter_params_t *meter_params,
                             bcm_custom_meterid_t *meter_id);
extern int
drv_avgr_custom_meter_destroy(int unit, bcm_custom_meterid_t meter_id);

extern int
drv_avgr_custom_meter_pm_params_t_init(int unit,
                                       bcm_custom_meter_pm_params_t *pm_params);

extern int
drv_avgr_custom_meter_pm_enable(int unit, bcm_custom_meterid_t meter_id);

extern int
drv_avgr_custom_meter_pm_disable(int unit, bcm_custom_meterid_t meter_id);

extern int
drv_avgr_custom_meter_pm_get(int unit, bcm_custom_meterid_t meter_id,
                             bcm_custom_meter_pm_params_t *pm_params);

/* Custom CosQ apis */
extern int drv_avgr_custom_cosq_init(int unit);

/* Initialize COSQ flowcontrol object */
extern int drv_avgr_custom_flowcontrol_params_t_init(
    int unit,
    bcm_gport_t portid,
    bcm_custom_flowcontrol_params_t *flowcontrol_params);

/* Initialize COSQ shaper object */
extern int drv_avgr_custom_cosq_shaper_params_t_init(
    int unit,
    bcm_gport_t portid,
    bcm_custom_cosq_shaper_params_t *shaper_params);

/* Initialize TSN parameters object */
extern int drv_avgr_custom_cosq_tsn_params_t_init(
    int unit,
    bcm_custom_cosq_tsn_params_t *tsn_params);

/*
 * This routine is used to set the default ingress traffic class on a
 * port
 */
extern int drv_avgr_custom_cosq_port_default_tc_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_tc_t tc);

/*
 * This routine is used to get the default ingress traffic class on a
 * port
 */
extern int drv_avgr_custom_cosq_port_default_tc_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_tc_t *tc);

/* This routine is used to set the default egress {pcp,dei} for port. */
extern int drv_avgr_custom_cosq_port_default_pcp_set(
    int unit,
    bcm_gport_t portid,
    int pcp,
    int dei);

/* This routine is used to get the default egress {pcp,dei} for port. */
extern int drv_avgr_custom_cosq_port_default_pcp_get(
    int unit,
    bcm_gport_t portid,
    int *pcp,
    int *dei);

/*
 * This routine is used to set the ingress mapping of {pcp,dei} to
 * traffic class
 */
extern int drv_avgr_custom_cosq_port_pcp2tc_set(
    int unit,
    bcm_gport_t portid,
    int pcp,
    int dei,
    bcm_custom_tc_t tc);

/*
 * This routine is used to get the ingress mapping of {pcp,dei} to
 * traffic class
 */
extern int drv_avgr_custom_cosq_port_pcp2tc_get(
    int unit,
    bcm_gport_t portid,
    int pcp,
    int dei,
    bcm_custom_tc_t *tc);

/*
 * This routine is used to set the egress mapping of traffic class to
 * {pcpi,dei}
 */
extern int drv_avgr_custom_cosq_port_tc2pcp_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_tc_t tc,
    int pcp,
    int dei);

/*
 * This routine is used to get the egress mapping of traffic class to
 * {pcpi,dei}
 */
extern int drv_avgr_custom_cosq_port_tc2pcp_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_tc_t tc,
    int *pcp,
    int *dei);

/*
 * This routine is used to set the ingress mapping of dscp to traffic
 * class
 */
extern int drv_avgr_custom_cosq_dscp2tc_set(
    int unit,
    bcm_custom_dscp_t dscp,
    bcm_custom_tc_t tc);

/*
 * This routine is used to get the ingress mapping of dscp to traffic
 * class
 */
extern int drv_avgr_custom_cosq_dscp2tc_get(
    int unit,
    bcm_custom_dscp_t dscp,
    bcm_custom_tc_t *tc);

/*
 * This routine is used to set the egress mapping of traffic class to
 * dscp
 */
extern int drv_avgr_custom_cosq_tc2dscp_set(
    int unit,
    bcm_custom_tc_t tc,
    bcm_custom_dscp_t dscp);

/*
 * This routine is used to get the egress mapping of traffic class to
 * dscp
 */
extern int drv_avgr_custom_cosq_tc2dscp_get(
    int unit,
    bcm_custom_tc_t tc,
    bcm_custom_dscp_t *dscp);

/* This routine is used to get the COSQ scheduling of a port */
extern int drv_avgr_custom_cosq_port_sched_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_schedule_mode_t *sched_mode,
    int *weights);

/* This routine is used to set the COSQ scheduling of a port */
extern int drv_avgr_custom_cosq_port_sched_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_schedule_mode_t sched_mode,
    int *weights);

/* This routine is used to get the global flowcontrol configuration */
extern int drv_avgr_custom_cosq_flowcontrol_get(
    int unit,
    bcm_custom_flowcontrol_t *flowcontrol_mode);

/* This routine is used to set the global flowcontrol configuration */
extern int drv_avgr_custom_cosq_flowcontrol_set(
    int unit,
    bcm_custom_flowcontrol_t flowcontrol_mode);

/* This routine is used to get the port flowcontrol configuration */
extern int drv_avgr_custom_cosq_port_flowcontrol_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_flowcontrol_params_t *fc_params);

/* This routine is used to set the port flowcontrol configuration */
extern int drv_avgr_custom_cosq_port_flowcontrol_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_flowcontrol_params_t fc_params);

/* This routine is used to get the PFC flowcontrol configuration */
extern int drv_avgr_custom_cosq_pfc_flowcontrol_get(
    int unit,
    int int_pri,
    bcm_custom_flowcontrol_params_t *fc_params);

/* This routine is used to set the PFC flowcontrol configuration */
extern int drv_avgr_custom_cosq_pfc_flowcontrol_set(
    int unit,
    int int_pri,
    bcm_custom_flowcontrol_params_t fc_params);

/* This routine is used to get an egress shaper on a port. */
extern int drv_avgr_custom_cosq_port_shaper_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_cosq_shaper_params_t *shaper_params);

/* This routine is used to set an egress shaper on a port. */
extern int drv_avgr_custom_cosq_port_shaper_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_cosq_shaper_params_t *shaper_params);

/* This routine is used to get an egress shaper on a qos queue. */
extern int drv_avgr_custom_cosq_queue_shaper_get(
    int unit,
    bcm_gport_t portid,
    int int_pri,
    bcm_custom_cosq_shaper_params_t *shaper_params);

/* This routine is used to set an egress shaper on a qos queue. */
extern int drv_avgr_custom_cosq_queue_shaper_set(
    int unit,
    bcm_gport_t portid,
    int int_pri,
    bcm_custom_cosq_shaper_params_t *shaper_params);

/* This routine is used to get TSN parameters on a qos queue. */
extern int drv_avgr_custom_cosq_queue_tsn_get(
    int unit,
    bcm_gport_t portid,
    int int_pri,
    bcm_custom_cosq_tsn_params_t *tsn_params);

/* This routine is used to set TSN parameters on a qos queue. */
extern int drv_avgr_custom_cosq_queue_tsn_set(
    int unit,
    bcm_gport_t portid,
    int int_pri,
    bcm_custom_cosq_tsn_params_t *tsn_params);

/*  Initialize Auth parameters. */
extern int drv_avgr_custom_auth_params_t_init(
    int unit,
    bcm_custom_auth_params_t *auth_params);

/*  Initialize Port auth mode parameters. */
extern int drv_avgr_custom_auth_port_mode_t_init(
    int unit,
    bcm_custom_auth_port_mode_t *auth_port_mode);


/*  Initialize Auth module */
extern int drv_avgr_custom_auth_init(
    int unit,
    bcm_custom_auth_params_t *auth_params);

/*  Detach Auth module */
extern int drv_avgr_custom_auth_detach(
    int unit,
    bcm_custom_auth_params_t *auth_params);

/*  This routine is used to get the global authentication mode  */
extern int drv_avgr_custom_auth_mode_get(
    int unit,
    bcm_custom_auth_params_t *auth_params);

/*  This routine is used to set the global authentication mode  */
extern int drv_avgr_custom_auth_mode_set(
    int unit,
    bcm_custom_auth_params_t *auth_params);

/*  This routine is used to get the auth parameters on a port  */
extern int drv_avgr_custom_auth_port_get(
    int unit,
    bcm_gport_t portid,
    bcm_custom_auth_port_mode_t *port_mode);

/*  This routine is used to set the auth parameters on a port  */
extern int drv_avgr_custom_auth_port_set(
    int unit,
    bcm_gport_t portid,
    bcm_custom_auth_port_mode_t *port_mode);

/* This routine is used to initialize Time Sync module */
extern int drv_avgr_custom_ts_init(int unit);

/* This routine is used to set Time Sync generator configuration */
extern int drv_avgr_custom_ts_gen_config_set(
    int unit,
    int generator_id,
    bcm_custom_ts_gen_config_t *config);

/* This routine is used to configure exeternal Time Sync events */
extern int drv_avgr_custom_ts_event_config_set(
    int unit,
    int event_id,
    int event_source,
    bcm_custom_ts_event_config_t *config);

/* This routine is used to get current TOD and time stamp */
extern int drv_avgr_custom_ts_tod_get(
    int unit,
    bcm_custom_ts_time_t *time,
    uint64 *timestamp);

/* This routine is used to set current TOD and time stamp */
extern int drv_avgr_custom_ts_tod_set(
    int unit,
    bcm_custom_ts_time_t time,
    uint64 timestamp);

/* This routine enables packet flow into the port */
extern int drv_avgr_port_pkt_flow_enable(int unit);

#endif
