/**
 * shared_types.h
 *
 * Coronado Bridge
 * Internal FSAL module SHARED TYPES
 * CBX INT SHARED TYPES
 */

#ifndef CBX_INT_SHARED_TYPES_H
#define CBX_INT_SHARED_TYPES_H
#include <shared/util.h>
/*Below declarations can be removed when the below data can be moved to someother .h file
  most of the declaration here are taken from shared/types.h in avenger code base*/
typedef uint64 uint64_t;
typedef uint32 uint32_t;
typedef uint16 uint16_t;
typedef uint8  uint8_t;
typedef int64 int64_t;
typedef int32 int32_t;
typedef int16 int16_t;
typedef int8  int8_t;
typedef uint32 cbx_port_t;
typedef uint16 cbx_vlan_t;
typedef uint32 cbx_portid_t;
typedef uint32 cbx_vlanid_t;
typedef uint32 cbx_stgid_t;
typedef uint32 cbx_meterid_t;
typedef uint32 cbx_cosqid_t;
typedef uint32 cbx_mcastid_t;
typedef uint32 cbx_trapid_t;
typedef uint32 cbx_mirrorid_t;
typedef uint8  eth_mac_t[6];
typedef int    cbx_encap_info_t;

typedef enum cbx_packet_type_e {
    /** IPv4 packets */
    cbxPacketIpv4,
    /** IPv6 packets */
    cbxPacketIpv6,
    /** other (non IPv4 or IPv6) packets */
    cbxPacketOther,
} cbx_packet_type_t;

typedef enum cbx_traffic_type_e {
    /** Broadcast packets */
    cbxTrafficTypeDLF,
    /** Unicast packets */
    cbxTrafficTypeUnicast,
    /** Multicast packets */
    cbxTrafficTypeMulticast,
    /** Broadcast packets */
    cbxTrafficTypeBroadcast,
} cbx_traffic_type_t;
/* For declaring re-usable API functions */
#define APIFUNC(_fn)    _fn
#define REENTRANT
#define USE_INTERNAL_RAM

/* Normal static attribute */
#ifndef STATIC
#define STATIC static
#define STATICFN        static
#define STATICCBK       static
/* For re-usable static function  */
#define APISTATIC
#endif
/* Add "const" declaration to web related generated files from sspgen.pl
 * so those will be put in .rodata section and reduce RAM usage.
 */
#define RES_CONST_DECL const

#define XDATA
#define CODE

/* Type of system ticks */
typedef uint32 tick_t;
#define _SHR_GPORT_MCAST_ID_SET(_gport, _id)                            \
        ((_gport) = (_SHR_GPORT_TYPE_MCAST << _SHR_GPORT_TYPE_SHIFT)  | \
         (((_id) & _SHR_GPORT_MCAST_MASK) << _SHR_GPORT_MCAST_SHIFT))

#define _SHR_GPORT_MCAST_ID_GET(_gport)   \
        (((_gport) >> _SHR_GPORT_MCAST_SHIFT) & _SHR_GPORT_MCAST_MASK)

#define PBMP_CPU(unit)                  0xC000 /* CPU ports:Ports 14, 15 */
#define PBMP_LOCAL_CPU(unit)            0x8000 /* CPU ports:Port 15 */
#define PBMP_EXTERNAL_CPU(unit)         0x4000 /* CPU ports:Port 14 */

#define CBX_PORTID_TYPE_LOCAL                _SHR_GPORT_TYPE_LOCAL
#define CBX_PORTID_IS_SET(_portid)           _SHR_GPORT_IS_SET(_portid)
#define CBX_PORTID_IS_LOCAL(_portid)         _SHR_GPORT_IS_LOCAL(_portid)
#define CBX_PORTID_IS_VLAN_PORT(_portid)     _SHR_GPORT_IS_VLAN_PORT(_portid)
#define CBX_PORTID_IS_TRUNK(_portid)         _SHR_GPORT_IS_TRUNK(_portid)
#define CBX_PORTID_IS_MCAST(_portid)         _SHR_GPORT_IS_MCAST(_portid)

#define IS_LOCAL_CPU_PORT(unit,port)          \
        (SOC_PBMP_MEMBER(PBMP_LOCAL_CPU(unit), port))

#define CBX_PORTID_LOCAL_SET(_portid, _port)  \
    _SHR_GPORT_LOCAL_SET(_portid, _port)

#define CBX_PORTID_LOCAL_GET(_portid)  \
    (!_SHR_GPORT_IS_LOCAL(_portid) ? -1 : \
    _SHR_GPORT_LOCAL_GET(_portid))

#define CBX_PORTID_VLAN_PORT_ID_SET(_portid, _id)    \
        _SHR_GPORT_VLAN_PORT_ID_SET(_portid, _id)

#define CBX_PORTID_VLAN_PORT_ID_GET(_portid)  \
    (!_SHR_GPORT_IS_VLAN_PORT(_portid) ? -1 :  \
    _SHR_GPORT_VLAN_PORT_ID_GET(_portid))

#define CBX_PORTID_TRUNK_SET(_portid, _port)  \
    _SHR_GPORT_TRUNK_SET(_portid, _port)

#define CBX_PORTID_TRUNK_GET(_portid)  \
    (!_SHR_GPORT_IS_TRUNK(_portid) ? -1 : \
    _SHR_GPORT_TRUNK_GET(_portid))

#define CBX_PORTID_MCAST_ID_SET(_portid, _id)  \
    _SHR_GPORT_MCAST_ID_SET(_portid, _id)

#define CBX_PORTID_MCAST_ID_GET(_portid)  \
    (!_SHR_GPORT_IS_MCAST(_portid) ? -1 : \
    _SHR_GPORT_MCAST_ID_GET(_portid))


#if defined(LE_HOST)
#define sal_htonl(_l)   _shr_swap32(_l)
#define sal_htons(_s)   _shr_swap16(_s)
#define sal_ntohl(_l)   _shr_swap32(_l)
#define sal_ntohs(_s)   _shr_swap16(_s)
#else /* BE_HOST */
#define sal_htonl(_l)   (_l)
#define sal_htons(_s)   (_s)
#define sal_ntohl(_l)   (_l)
#define sal_ntohs(_s)   (_s)
#endif /* BE_HOST */

#define SOC_AVENGER_PRIMARY    0
#define SOC_AVENGER_SECONDARY  1

/* following prototypes can be removed once phy and mac filesare checkedin */
extern int avng_phy_duplex_get(int unit, uint8_t lport, uint32_t *value);
extern int avng_phy_duplex_set(int unit, uint8_t lport, uint32_t value);
extern int avng_phy_speed_set(int unit, uint8_t lport, uint32_t value);
extern int avng_phy_speed_get(int unit, uint8_t lport, uint32_t *value);
extern int avng_phy_link_get(int unit, uint8_t lport, uint32_t *link_status);
extern int avng_phy_loopback_get(int unit, uint8_t lport, uint32_t *status);
extern int avng_phy_loopback_set(int unit, uint8_t lport, uint32_t enable);
extern int avng_phy_autoneg_set(int unit, uint8_t lport, uint32_t an);
extern int avng_phy_autoneg_get(int unit, uint8_t lport, uint32_t *an);
extern int avng_phy_reg_read(uint8_t unit, uint8_t lport, uint32_t reg_addr,
                              uint32_t *p_value);
extern int avng_phy_reg_write(uint8_t unit, uint8_t lport, uint32_t reg_addr,
                               uint32_t value);
extern int avng_phy_eee_set(int unit, uint8_t port, uint32_t mode);
extern int avng_phy_eee_get(int unit, uint8_t port, uint32_t *mode);
extern int soc_mac_init(void);
extern int mac_duplex_set(int unit, cbx_port_t port_out, int duplex);
extern int mac_duplex_get(int unit, cbx_port_t port_out, int *duplex);
extern int mac_speed_set(int unit, cbx_port_t port_out, int speed);
extern int mac_speed_get(int unit, cbx_port_t port_out, int *speed);
extern int mac_loopback_set(int unit, cbx_port_t port_out, int loopback);
extern int mac_loopback_get(int unit, cbx_port_t port_out, int *loopback);
extern int mac_max_frame_size_get(int unit, cbx_port_t port_out, uint32_t *size);
extern int mac_max_frame_size_set(int unit, cbx_port_t port_out, uint32_t size);


#endif  /* !_CBX_INT_SHARED_TYPES_H */
