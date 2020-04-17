/*
 * $Id: etcgmac.h,v 1.4 Broadcom SDK $
 * 
 * This license is set out in https://github.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-xsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Broadcom Gigabit Ethernet MAC defines.
 */
#ifndef _etcgmac_h_
#define _etcgmac_h_

/* chip interrupt bit error summary */
#define	I_ERRORS		(INTMASK_PCIDESCERROR_MASK | INTMASK_PCIDATAERROR_MASK | \
     INTMASK_DESCERROR_MASK  | INTMASK_XMTFIFOUNDERFLOW_MASK)
#define	DEF_INTMASK		(INTMASK_XMT0INTERRUPT_MASK | INTMASK_XMT1INTERRUPT_MASK \
    | INTMASK_XMT2INTERRUPT_MASK | INTMASK_XMT3INTERRUPT_MASK | INTMASK_RCVINTERRUPT_0_MASK | \
    INTMASK_RCVINTERRUPT_1_MASK | INTMASK_RCVINTERRUPT_2_MASK | INTMASK_RCVINTERRUPT_3_MASK | \
    I_ERRORS)

#define GMAC_RESET_DELAY 	2

#define GMAC_MIN_FRAMESIZE	17	/* gmac can only send frames of
	                                 * size above 17 octetes.
	                                 */

#define LOOPBACK_MODE_DMA	0	/* loopback the packet at the DMA engine */
#define LOOPBACK_MODE_MAC	1	/* loopback the packet at MAC */
#define LOOPBACK_MODE_NONE	2	/* no Loopback */

#define RC_MAC_DATA_PERIOD	9

#define DMAREG(ch, dir, qnum)	((dir == DMA_TX) ? \
	                         (void *)(uint*)(&(ch->regs->d64xmt0control) + (0x10 * qnum)) : \
	                         (void *)(uint*)(&(ch->regs->d64rcv0control) + (0x10 * qnum)))


/*
 * Add multicast address to the list. Multicast address are maintained as
 * hash table with chaining.
 */
typedef struct mclist {
	struct ether_addr mc_addr;	/* multicast address to allow */
	struct mclist *next;		/* next entry */
} mflist_t;

#define GMAC_HASHT_SIZE		16	/* hash table size */
#define GMAC_MCADDR_HASH(m)	((((uint8 *)(m))[3] + ((uint8 *)(m))[4] + \
	                         ((uint8 *)(m))[5]) & (GMAC_HASHT_SIZE - 1))

#define ETHER_MCADDR_CMP(x, y) ((((uint16 *)(x))[0] ^ ((uint16 *)(y))[0]) | \
				(((uint16 *)(x))[1] ^ ((uint16 *)(y))[1]) | \
				(((uint16 *)(x))[2] ^ ((uint16 *)(y))[2]))

#define SUCCESS			0
#define FAILURE			-1
#define TIMEOUT                 -2

typedef struct mcfilter {
					/* hash table for multicast filtering */
	mflist_t *bucket[GMAC_HASHT_SIZE];
} mcfilter_t;

extern uint32 find_priq(uint32 pri_map);


typedef enum cfp_field_e {
    CFP_FIELD_IPV4_VALID,
    CFP_FIELD_IPV4_SLICE_ID,
    CFP_FIELD_IPV4_UDF0,
    CFP_FIELD_IPV4_UDF1,
    CFP_FIELD_IPV4_UDF2,
    CFP_FIELD_IPV4_UDF3,
    CFP_FIELD_IPV4_UDF4,
    CFP_FIELD_IPV4_UDF5,
    CFP_FIELD_IPV4_UDF6,
    CFP_FIELD_IPV4_UDF7,
    CFP_FIELD_IPV4_UDF8,
    CFP_FIELD_IPV4_CTAG,
    CFP_FIELD_IPV4_STAG,
    CFP_FIELD_IPV4_UDF0_VLD,
    CFP_FIELD_IPV4_UDF1_VLD,
    CFP_FIELD_IPV4_UDF2_VLD,
    CFP_FIELD_IPV4_UDF3_VLD,
    CFP_FIELD_IPV4_UDF4_VLD,
    CFP_FIELD_IPV4_UDF5_VLD,
    CFP_FIELD_IPV4_UDF6_VLD,
    CFP_FIELD_IPV4_UDF7_VLD,
    CFP_FIELD_IPV4_UDF8_VLD,
    CFP_FIELD_IPV4_TTL_RANGE,
    CFP_FIELD_IPV4_IP_AUTH,
    CFP_FIELD_IPV4_NON_FIRST_FRAG,
    CFP_FIELD_IPV4_IP_FRAG,
    CFP_FIELD_IPV4_IP_PROTO,
    CFP_FIELD_IPV4_IP_TOS,
    CFP_FIELD_IPV4_L3_FRAMING,
    CFP_FIELD_IPV4_L2_FRAMING,
    CFP_FIELD_IPV4_C_TAGGED,
    CFP_FIELD_IPV4_S_TAGGED,
    CFP_FIELD_IPV4_SRC_PMAP,
    CFP_FIELD_IPV6_VALID,
    CFP_FIELD_IPV6_SLICE_ID,
    CFP_FIELD_IPV6_UDF0,
    CFP_FIELD_IPV6_UDF1,
    CFP_FIELD_IPV6_UDF2,
    CFP_FIELD_IPV6_UDF3,
    CFP_FIELD_IPV6_UDF4,
    CFP_FIELD_IPV6_UDF5,
    CFP_FIELD_IPV6_UDF6,
    CFP_FIELD_IPV6_UDF7,
    CFP_FIELD_IPV6_UDF8,
    CFP_FIELD_IPV6_CTAG,
    CFP_FIELD_IPV6_STAG,
    CFP_FIELD_IPV6_UDF0_VLD,
    CFP_FIELD_IPV6_UDF1_VLD,
    CFP_FIELD_IPV6_UDF2_VLD,
    CFP_FIELD_IPV6_UDF3_VLD,
    CFP_FIELD_IPV6_UDF4_VLD,
    CFP_FIELD_IPV6_UDF5_VLD,
    CFP_FIELD_IPV6_UDF6_VLD,
    CFP_FIELD_IPV6_UDF7_VLD,
    CFP_FIELD_IPV6_UDF8_VLD,
    CFP_FIELD_IPV6_HOPLIMIT_RANGE,
    CFP_FIELD_IPV6_IP_AUTH,
    CFP_FIELD_IPV6_NON_FIRST_FRAG,
    CFP_FIELD_IPV6_IP_FRAG,
    CFP_FIELD_IPV6_IP_PROTO,
    CFP_FIELD_IPV6_IP_TOS,
    CFP_FIELD_IPV6_L3_FRAMING,
    CFP_FIELD_IPV6_L2_FRAMING,
    CFP_FIELD_IPV6_C_TAGGED,
    CFP_FIELD_IPV6_S_TAGGED,
    CFP_FIELD_IPV6_SRC_PMAP,
    CFP_FIELD_NONIP_VALID,
    CFP_FIELD_NONIP_SLICE_ID,
    CFP_FIELD_NONIP_UDF0,
    CFP_FIELD_NONIP_UDF1,
    CFP_FIELD_NONIP_UDF2,
    CFP_FIELD_NONIP_UDF3,
    CFP_FIELD_NONIP_UDF4,
    CFP_FIELD_NONIP_UDF5,
    CFP_FIELD_NONIP_UDF6,
    CFP_FIELD_NONIP_UDF7,
    CFP_FIELD_NONIP_UDF8,
    CFP_FIELD_NONIP_CTAG,
    CFP_FIELD_NONIP_STAG,
    CFP_FIELD_NONIP_UDF0_VLD,
    CFP_FIELD_NONIP_UDF1_VLD,
    CFP_FIELD_NONIP_UDF2_VLD,
    CFP_FIELD_NONIP_UDF3_VLD,
    CFP_FIELD_NONIP_UDF4_VLD,
    CFP_FIELD_NONIP_UDF5_VLD,
    CFP_FIELD_NONIP_UDF6_VLD,
    CFP_FIELD_NONIP_UDF7_VLD,
    CFP_FIELD_NONIP_UDF8_VLD,
    CFP_FIELD_NONIP_ETHERTYPE_SAP,
    CFP_FIELD_NONIP_L3_FRAMING,
    CFP_FIELD_NONIP_L2_FRAMING,
    CFP_FIELD_NONIP_C_TAGGED,
    CFP_FIELD_NONIP_S_TAGGED,
    CFP_FIELD_NONIP_SRC_PMAP,
    CFP_FIELD_CHAIN_VALID,
    CFP_FIELD_CHAIN_SLICE_ID,
    CFP_FIELD_CHAIN_UDF0,
    CFP_FIELD_CHAIN_UDF1,
    CFP_FIELD_CHAIN_UDF2,
    CFP_FIELD_CHAIN_UDF3,
    CFP_FIELD_CHAIN_UDF4,
    CFP_FIELD_CHAIN_UDF5,
    CFP_FIELD_CHAIN_UDF6,
    CFP_FIELD_CHAIN_UDF7,
    CFP_FIELD_CHAIN_UDF8,
    CFP_FIELD_CHAIN_UDF9,
    CFP_FIELD_CHAIN_UDF10,
    CFP_FIELD_CHAIN_UDF11,
    CFP_FIELD_CHAIN_UDF0_VLD,
    CFP_FIELD_CHAIN_UDF1_VLD,
    CFP_FIELD_CHAIN_UDF2_VLD,
    CFP_FIELD_CHAIN_UDF3_VLD,
    CFP_FIELD_CHAIN_UDF4_VLD,
    CFP_FIELD_CHAIN_UDF5_VLD,
    CFP_FIELD_CHAIN_UDF6_VLD,
    CFP_FIELD_CHAIN_UDF7_VLD,
    CFP_FIELD_CHAIN_UDF8_VLD,
    CFP_FIELD_CHAIN_UDF9_VLD,
    CFP_FIELD_CHAIN_UDF10_VLD,
    CFP_FIELD_CHAIN_UDF11_VLD,
    CFP_FIELD_CHAIN_CHAIN_ID,
    CFP_FIELD_ACT_CHAIN_ID,
    CFP_FIELD_ACT_CLASSFICATION_ID = CFP_FIELD_ACT_CHAIN_ID,
    CFP_FIELD_ACT_RX_CHANNEL_ID,
    CFP_FIELD_ACT_DROP,
    CFP_FIELD_COUNT
} cfp_field_t;

#define CFP_FIELD_STRINGS \
{ \
    "CFP_FIELD_IPV4_VALID", \
    "CFP_FIELD_IPV4_SLICE_ID", \
    "CFP_FIELD_IPV4_UDF0", \
    "CFP_FIELD_IPV4_UDF1", \
    "CFP_FIELD_IPV4_UDF2", \
    "CFP_FIELD_IPV4_UDF3", \
    "CFP_FIELD_IPV4_UDF4", \
    "CFP_FIELD_IPV4_UDF5", \
    "CFP_FIELD_IPV4_UDF6", \
    "CFP_FIELD_IPV4_UDF7", \
    "CFP_FIELD_IPV4_UDF8", \
    "CFP_FIELD_IPV4_CTAG", \
    "CFP_FIELD_IPV4_STAG", \
    "CFP_FIELD_IPV4_UDF0_VLD", \
    "CFP_FIELD_IPV4_UDF1_VLD", \
    "CFP_FIELD_IPV4_UDF2_VLD", \
    "CFP_FIELD_IPV4_UDF3_VLD", \
    "CFP_FIELD_IPV4_UDF4_VLD", \
    "CFP_FIELD_IPV4_UDF5_VLD", \
    "CFP_FIELD_IPV4_UDF6_VLD", \
    "CFP_FIELD_IPV4_UDF7_VLD", \
    "CFP_FIELD_IPV4_UDF8_VLD", \
    "CFP_FIELD_IPV4_TTL_RANGE", \
    "CFP_FIELD_IPV4_IP_AUTH", \
    "CFP_FIELD_IPV4_NON_FIRST_FRAG", \
    "CFP_FIELD_IPV4_IP_FRAG", \
    "CFP_FIELD_IPV4_IP_PROTO", \
    "CFP_FIELD_IPV4_IP_TOS", \
    "CFP_FIELD_IPV4_L3_FRAMING", \
    "CFP_FIELD_IPV4_L2_FRAMING", \
    "CFP_FIELD_IPV4_C_TAGGED", \
    "CFP_FIELD_IPV4_S_TAGGED", \
    "CFP_FIELD_IPV4_SRC_PMAP", \
    "CFP_FIELD_IPV6_VALID", \
    "CFP_FIELD_IPV6_SLICE_ID", \
    "CFP_FIELD_IPV6_UDF0", \
    "CFP_FIELD_IPV6_UDF1", \
    "CFP_FIELD_IPV6_UDF2", \
    "CFP_FIELD_IPV6_UDF3", \
    "CFP_FIELD_IPV6_UDF4", \
    "CFP_FIELD_IPV6_UDF5", \
    "CFP_FIELD_IPV6_UDF6", \
    "CFP_FIELD_IPV6_UDF7", \
    "CFP_FIELD_IPV6_UDF8", \
    "CFP_FIELD_IPV6_CTAG", \
    "CFP_FIELD_IPV6_STAG", \
    "CFP_FIELD_IPV6_UDF0_VLD", \
    "CFP_FIELD_IPV6_UDF1_VLD", \
    "CFP_FIELD_IPV6_UDF2_VLD", \
    "CFP_FIELD_IPV6_UDF3_VLD", \
    "CFP_FIELD_IPV6_UDF4_VLD", \
    "CFP_FIELD_IPV6_UDF5_VLD", \
    "CFP_FIELD_IPV6_UDF6_VLD", \
    "CFP_FIELD_IPV6_UDF7_VLD", \
    "CFP_FIELD_IPV6_UDF8_VLD", \
    "CFP_FIELD_IPV6_HOPLIMIT_RANGE", \
    "CFP_FIELD_IPV6_IP_AUTH", \
    "CFP_FIELD_IPV6_NON_FIRST_FRAG", \
    "CFP_FIELD_IPV6_IP_FRAG", \
    "CFP_FIELD_IPV6_IP_PROTO", \
    "CFP_FIELD_IPV6_IP_TOS", \
    "CFP_FIELD_IPV6_L3_FRAMING", \
    "CFP_FIELD_IPV6_L2_FRAMING", \
    "CFP_FIELD_IPV6_C_TAGGED", \
    "CFP_FIELD_IPV6_S_TAGGED", \
    "CFP_FIELD_IPV6_SRC_PMAP", \
    "CFP_FIELD_NONIP_VALID", \
    "CFP_FIELD_NONIP_SLICE_ID", \
    "CFP_FIELD_NONIP_UDF0", \
    "CFP_FIELD_NONIP_UDF1", \
    "CFP_FIELD_NONIP_UDF2", \
    "CFP_FIELD_NONIP_UDF3", \
    "CFP_FIELD_NONIP_UDF4", \
    "CFP_FIELD_NONIP_UDF5", \
    "CFP_FIELD_NONIP_UDF6", \
    "CFP_FIELD_NONIP_UDF7", \
    "CFP_FIELD_NONIP_UDF8", \
    "CFP_FIELD_NONIP_CTAG", \
    "CFP_FIELD_NONIP_STAG", \
    "CFP_FIELD_NONIP_UDF0_VLD", \
    "CFP_FIELD_NONIP_UDF1_VLD", \
    "CFP_FIELD_NONIP_UDF2_VLD", \
    "CFP_FIELD_NONIP_UDF3_VLD", \
    "CFP_FIELD_NONIP_UDF4_VLD", \
    "CFP_FIELD_NONIP_UDF5_VLD", \
    "CFP_FIELD_NONIP_UDF6_VLD", \
    "CFP_FIELD_NONIP_UDF7_VLD", \
    "CFP_FIELD_NONIP_UDF8_VLD", \
    "CFP_FIELD_NONIP_ETHERTYPE_SAP", \
    "CFP_FIELD_NONIP_L3_FRAMING", \
    "CFP_FIELD_NONIP_L2_FRAMING", \
    "CFP_FIELD_NONIP_C_TAGGED", \
    "CFP_FIELD_NONIP_S_TAGGED", \
    "CFP_FIELD_NONIP_SRC_PMAP", \
    "CFP_FIELD_CHAIN_VALID", \
    "CFP_FIELD_CHAIN_SLICE_ID", \
    "CFP_FIELD_CHAIN_UDF0", \
    "CFP_FIELD_CHAIN_UDF1", \
    "CFP_FIELD_CHAIN_UDF2", \
    "CFP_FIELD_CHAIN_UDF3", \
    "CFP_FIELD_CHAIN_UDF4", \
    "CFP_FIELD_CHAIN_UDF5", \
    "CFP_FIELD_CHAIN_UDF6", \
    "CFP_FIELD_CHAIN_UDF7", \
    "CFP_FIELD_CHAIN_UDF8", \
    "CFP_FIELD_CHAIN_UDF9", \
    "CFP_FIELD_CHAIN_UDF10", \
    "CFP_FIELD_CHAIN_UDF11", \
    "CFP_FIELD_CHAIN_UDF0_VLD", \
    "CFP_FIELD_CHAIN_UDF1_VLD", \
    "CFP_FIELD_CHAIN_UDF2_VLD", \
    "CFP_FIELD_CHAIN_UDF3_VLD", \
    "CFP_FIELD_CHAIN_UDF4_VLD", \
    "CFP_FIELD_CHAIN_UDF5_VLD", \
    "CFP_FIELD_CHAIN_UDF6_VLD", \
    "CFP_FIELD_CHAIN_UDF7_VLD", \
    "CFP_FIELD_CHAIN_UDF8_VLD", \
    "CFP_FIELD_CHAIN_UDF9_VLD", \
    "CFP_FIELD_CHAIN_UDF10_VLD", \
    "CFP_FIELD_CHAIN_UDF11_VLD", \
    "CFP_FIELD_CHAIN_CHAIN_ID", \
    "CFP_FIELD_ACT_CHAIN_ID", \
    "CFP_FIELD_ACT_RX_CHANNEL_ID", \
    "CFP_FIELD_ACT_DROP", \
    "CFP_FIELD_COUNT" \
}

typedef struct cfp_field_info_s {
    uint32  shift;
    uint32  len;
} cfp_field_info_t;


/* Field Index */
/* IPV4 SLICE */
#define CFP_FIELD_IPV4_VALID_LEN  2
#define CFP_FIELD_IPV4_VALID_SHIFT  0
#define CFP_FIELD_IPV4_SLICE_ID_LEN  2
#define CFP_FIELD_IPV4_SLICE_ID_SHIFT  2
#define CFP_FIELD_IPV4_UDF0_LEN  16
#define CFP_FIELD_IPV4_UDF0_SHIFT  8
#define CFP_FIELD_IPV4_UDF1_LEN  16
#define CFP_FIELD_IPV4_UDF1_SHIFT  24
#define CFP_FIELD_IPV4_UDF2_LEN  16
#define CFP_FIELD_IPV4_UDF2_SHIFT  40
#define CFP_FIELD_IPV4_UDF3_LEN  16
#define CFP_FIELD_IPV4_UDF3_SHIFT  56
#define CFP_FIELD_IPV4_UDF4_LEN  16
#define CFP_FIELD_IPV4_UDF4_SHIFT  72
#define CFP_FIELD_IPV4_UDF5_LEN  16
#define CFP_FIELD_IPV4_UDF5_SHIFT  88
#define CFP_FIELD_IPV4_UDF6_LEN  16
#define CFP_FIELD_IPV4_UDF6_SHIFT  104
#define CFP_FIELD_IPV4_UDF7_LEN  16
#define CFP_FIELD_IPV4_UDF7_SHIFT  120
#define CFP_FIELD_IPV4_UDF8_LEN  16
#define CFP_FIELD_IPV4_UDF8_SHIFT  136
#define CFP_FIELD_IPV4_CTAG_LEN  16
#define CFP_FIELD_IPV4_CTAG_SHIFT  152
#define CFP_FIELD_IPV4_STAG_LEN  16
#define CFP_FIELD_IPV4_STAG_SHIFT  168
#define CFP_FIELD_IPV4_UDF0_VLD_LEN  1
#define CFP_FIELD_IPV4_UDF0_VLD_SHIFT  184
#define CFP_FIELD_IPV4_UDF1_VLD_LEN  1
#define CFP_FIELD_IPV4_UDF1_VLD_SHIFT  185
#define CFP_FIELD_IPV4_UDF2_VLD_LEN  1
#define CFP_FIELD_IPV4_UDF2_VLD_SHIFT  186
#define CFP_FIELD_IPV4_UDF3_VLD_LEN  1
#define CFP_FIELD_IPV4_UDF3_VLD_SHIFT  187
#define CFP_FIELD_IPV4_UDF4_VLD_LEN  1
#define CFP_FIELD_IPV4_UDF4_VLD_SHIFT  188
#define CFP_FIELD_IPV4_UDF5_VLD_LEN  1
#define CFP_FIELD_IPV4_UDF5_VLD_SHIFT  189
#define CFP_FIELD_IPV4_UDF6_VLD_LEN  1
#define CFP_FIELD_IPV4_UDF6_VLD_SHIFT  190
#define CFP_FIELD_IPV4_UDF7_VLD_LEN  1
#define CFP_FIELD_IPV4_UDF7_VLD_SHIFT  191
#define CFP_FIELD_IPV4_UDF8_VLD_LEN  1
#define CFP_FIELD_IPV4_UDF8_VLD_SHIFT  192
#define CFP_FIELD_IPV4_TTL_RANGE_LEN  2
#define CFP_FIELD_IPV4_TTL_RANGE_SHIFT  195
#define CFP_FIELD_IPV4_IP_AUTH_LEN  1
#define CFP_FIELD_IPV4_IP_AUTH_SHIFT  197
#define CFP_FIELD_IPV4_NON_FIRST_FRAG_LEN  1
#define CFP_FIELD_IPV4_NON_FIRST_FRAG_SHIFT  198
#define CFP_FIELD_IPV4_IP_FRAG_LEN  1
#define CFP_FIELD_IPV4_IP_FRAG_SHIFT  199
#define CFP_FIELD_IPV4_IP_PROTO_LEN  8
#define CFP_FIELD_IPV4_IP_PROTO_SHIFT  200
#define CFP_FIELD_IPV4_IP_TOS_LEN  8
#define CFP_FIELD_IPV4_IP_TOS_SHIFT  208
#define CFP_FIELD_IPV4_L3_FRAMING_LEN  2
#define CFP_FIELD_IPV4_L3_FRAMING_SHIFT  216
#define CFP_FIELD_IPV4_L2_FRAMING_LEN  2
#define CFP_FIELD_IPV4_L2_FRAMING_SHIFT  218
#define CFP_FIELD_IPV4_C_TAGGED_LEN  2
#define CFP_FIELD_IPV4_C_TAGGED_SHIFT  220
#define CFP_FIELD_IPV4_S_TAGGED_LEN  2
#define CFP_FIELD_IPV4_S_TAGGED_SHIFT  222
#define CFP_FIELD_IPV4_SRC_PMAP_LEN  8
#define CFP_FIELD_IPV4_SRC_PMAP_SHIFT  224


/* IPV6 SLICE */
#define CFP_FIELD_IPV6_VALID_LEN  2
#define CFP_FIELD_IPV6_VALID_SHIFT  0
#define CFP_FIELD_IPV6_SLICE_ID_LEN  2
#define CFP_FIELD_IPV6_SLICE_ID_SHIFT  2
#define CFP_FIELD_IPV6_UDF0_LEN  16
#define CFP_FIELD_IPV6_UDF0_SHIFT  8
#define CFP_FIELD_IPV6_UDF1_LEN  16
#define CFP_FIELD_IPV6_UDF1_SHIFT  24
#define CFP_FIELD_IPV6_UDF2_LEN  16
#define CFP_FIELD_IPV6_UDF2_SHIFT  40
#define CFP_FIELD_IPV6_UDF3_LEN  16
#define CFP_FIELD_IPV6_UDF3_SHIFT  56
#define CFP_FIELD_IPV6_UDF4_LEN  16
#define CFP_FIELD_IPV6_UDF4_SHIFT  72
#define CFP_FIELD_IPV6_UDF5_LEN  16
#define CFP_FIELD_IPV6_UDF5_SHIFT  88
#define CFP_FIELD_IPV6_UDF6_LEN  16
#define CFP_FIELD_IPV6_UDF6_SHIFT  104
#define CFP_FIELD_IPV6_UDF7_LEN  16
#define CFP_FIELD_IPV6_UDF7_SHIFT  120
#define CFP_FIELD_IPV6_UDF8_LEN  16
#define CFP_FIELD_IPV6_UDF8_SHIFT  136
#define CFP_FIELD_IPV6_CTAG_LEN  16
#define CFP_FIELD_IPV6_CTAG_SHIFT  152
#define CFP_FIELD_IPV6_STAG_LEN  16
#define CFP_FIELD_IPV6_STAG_SHIFT  168
#define CFP_FIELD_IPV6_UDF0_VLD_LEN  1
#define CFP_FIELD_IPV6_UDF0_VLD_SHIFT  184
#define CFP_FIELD_IPV6_UDF1_VLD_LEN  1
#define CFP_FIELD_IPV6_UDF1_VLD_SHIFT  185
#define CFP_FIELD_IPV6_UDF2_VLD_LEN  1
#define CFP_FIELD_IPV6_UDF2_VLD_SHIFT  186
#define CFP_FIELD_IPV6_UDF3_VLD_LEN  1
#define CFP_FIELD_IPV6_UDF3_VLD_SHIFT  187
#define CFP_FIELD_IPV6_UDF4_VLD_LEN  1
#define CFP_FIELD_IPV6_UDF4_VLD_SHIFT  188
#define CFP_FIELD_IPV6_UDF5_VLD_LEN  1
#define CFP_FIELD_IPV6_UDF5_VLD_SHIFT  189
#define CFP_FIELD_IPV6_UDF6_VLD_LEN  1
#define CFP_FIELD_IPV6_UDF6_VLD_SHIFT  190
#define CFP_FIELD_IPV6_UDF7_VLD_LEN  1
#define CFP_FIELD_IPV6_UDF7_VLD_SHIFT  191
#define CFP_FIELD_IPV6_UDF8_VLD_LEN  1
#define CFP_FIELD_IPV6_UDF8_VLD_SHIFT  192
#define CFP_FIELD_IPV6_HOPLIMIT_RANGE_LEN  2
#define CFP_FIELD_IPV6_HOPLIMIT_RANGE_SHIFT  195
#define CFP_FIELD_IPV6_IP_AUTH_LEN  1
#define CFP_FIELD_IPV6_IP_AUTH_SHIFT  197
#define CFP_FIELD_IPV6_NON_FIRST_FRAG_LEN  1
#define CFP_FIELD_IPV6_NON_FIRST_FRAG_SHIFT  198
#define CFP_FIELD_IPV6_IP_FRAG_LEN  1
#define CFP_FIELD_IPV6_IP_FRAG_SHIFT  199
#define CFP_FIELD_IPV6_IP_PROTO_LEN  8
#define CFP_FIELD_IPV6_IP_PROTO_SHIFT  200
#define CFP_FIELD_IPV6_IP_TOS_LEN  8
#define CFP_FIELD_IPV6_IP_TOS_SHIFT  208
#define CFP_FIELD_IPV6_L3_FRAMING_LEN  2
#define CFP_FIELD_IPV6_L3_FRAMING_SHIFT  216
#define CFP_FIELD_IPV6_L2_FRAMING_LEN  2
#define CFP_FIELD_IPV6_L2_FRAMING_SHIFT  218
#define CFP_FIELD_IPV6_C_TAGGED_LEN  2
#define CFP_FIELD_IPV6_C_TAGGED_SHIFT  220
#define CFP_FIELD_IPV6_S_TAGGED_LEN  2
#define CFP_FIELD_IPV6_S_TAGGED_SHIFT  222
#define CFP_FIELD_IPV6_SRC_PMAP_LEN  8
#define CFP_FIELD_IPV6_SRC_PMAP_SHIFT  224


/* NONIP SLICE */
#define CFP_FIELD_NONIP_VALID_LEN  2
#define CFP_FIELD_NONIP_VALID_SHIFT  0
#define CFP_FIELD_NONIP_SLICE_ID_LEN  2
#define CFP_FIELD_NONIP_SLICE_ID_SHIFT  2
#define CFP_FIELD_NONIP_UDF0_LEN  16
#define CFP_FIELD_NONIP_UDF0_SHIFT  8
#define CFP_FIELD_NONIP_UDF1_LEN  16
#define CFP_FIELD_NONIP_UDF1_SHIFT  24
#define CFP_FIELD_NONIP_UDF2_LEN  16
#define CFP_FIELD_NONIP_UDF2_SHIFT  40
#define CFP_FIELD_NONIP_UDF3_LEN  16
#define CFP_FIELD_NONIP_UDF3_SHIFT  56
#define CFP_FIELD_NONIP_UDF4_LEN  16
#define CFP_FIELD_NONIP_UDF4_SHIFT  72
#define CFP_FIELD_NONIP_UDF5_LEN  16
#define CFP_FIELD_NONIP_UDF5_SHIFT  88
#define CFP_FIELD_NONIP_UDF6_LEN  16
#define CFP_FIELD_NONIP_UDF6_SHIFT  104
#define CFP_FIELD_NONIP_UDF7_LEN  16
#define CFP_FIELD_NONIP_UDF7_SHIFT  120
#define CFP_FIELD_NONIP_UDF8_LEN  16
#define CFP_FIELD_NONIP_UDF8_SHIFT  136
#define CFP_FIELD_NONIP_CTAG_LEN  16
#define CFP_FIELD_NONIP_CTAG_SHIFT  152
#define CFP_FIELD_NONIP_STAG_LEN  16
#define CFP_FIELD_NONIP_STAG_SHIFT  168
#define CFP_FIELD_NONIP_UDF0_VLD_LEN  1
#define CFP_FIELD_NONIP_UDF0_VLD_SHIFT  184
#define CFP_FIELD_NONIP_UDF1_VLD_LEN  1
#define CFP_FIELD_NONIP_UDF1_VLD_SHIFT  185
#define CFP_FIELD_NONIP_UDF2_VLD_LEN  1
#define CFP_FIELD_NONIP_UDF2_VLD_SHIFT  186
#define CFP_FIELD_NONIP_UDF3_VLD_LEN  1
#define CFP_FIELD_NONIP_UDF3_VLD_SHIFT  187
#define CFP_FIELD_NONIP_UDF4_VLD_LEN  1
#define CFP_FIELD_NONIP_UDF4_VLD_SHIFT  188
#define CFP_FIELD_NONIP_UDF5_VLD_LEN  1
#define CFP_FIELD_NONIP_UDF5_VLD_SHIFT  189
#define CFP_FIELD_NONIP_UDF6_VLD_LEN  1
#define CFP_FIELD_NONIP_UDF6_VLD_SHIFT  190
#define CFP_FIELD_NONIP_UDF7_VLD_LEN  1
#define CFP_FIELD_NONIP_UDF7_VLD_SHIFT  191
#define CFP_FIELD_NONIP_UDF8_VLD_LEN  1
#define CFP_FIELD_NONIP_UDF8_VLD_SHIFT  192
#define CFP_FIELD_NONIP_ETHERTYPE_SAP_LEN  16
#define CFP_FIELD_NONIP_ETHERTYPE_SAP_SHIFT  200
#define CFP_FIELD_NONIP_L3_FRAMING_LEN  2
#define CFP_FIELD_NONIP_L3_FRAMING_SHIFT  216
#define CFP_FIELD_NONIP_L2_FRAMING_LEN  2
#define CFP_FIELD_NONIP_L2_FRAMING_SHIFT  218
#define CFP_FIELD_NONIP_C_TAGGED_LEN  2
#define CFP_FIELD_NONIP_C_TAGGED_SHIFT  220
#define CFP_FIELD_NONIP_S_TAGGED_LEN  2
#define CFP_FIELD_NONIP_S_TAGGED_SHIFT  222
#define CFP_FIELD_NONIP_SRC_PMAP_LEN  8
#define CFP_FIELD_NONIP_SRC_PMAP_SHIFT  224

/* CHAIN SLICE */
#define CFP_FIELD_CHAIN_VALID_LEN  2
#define CFP_FIELD_CHAIN_VALID_SHIFT  0
#define CFP_FIELD_CHAIN_SLICE_ID_LEN  2
#define CFP_FIELD_CHAIN_SLICE_ID_SHIFT  2
#define CFP_FIELD_CHAIN_UDF0_LEN  16
#define CFP_FIELD_CHAIN_UDF0_SHIFT  8
#define CFP_FIELD_CHAIN_UDF1_LEN  16
#define CFP_FIELD_CHAIN_UDF1_SHIFT  24
#define CFP_FIELD_CHAIN_UDF2_LEN  16
#define CFP_FIELD_CHAIN_UDF2_SHIFT  40
#define CFP_FIELD_CHAIN_UDF3_LEN  16
#define CFP_FIELD_CHAIN_UDF3_SHIFT  56
#define CFP_FIELD_CHAIN_UDF4_LEN  16
#define CFP_FIELD_CHAIN_UDF4_SHIFT  72
#define CFP_FIELD_CHAIN_UDF5_LEN  16
#define CFP_FIELD_CHAIN_UDF5_SHIFT  88
#define CFP_FIELD_CHAIN_UDF6_LEN  16
#define CFP_FIELD_CHAIN_UDF6_SHIFT  104
#define CFP_FIELD_CHAIN_UDF7_LEN  16
#define CFP_FIELD_CHAIN_UDF7_SHIFT  120
#define CFP_FIELD_CHAIN_UDF8_LEN  16
#define CFP_FIELD_CHAIN_UDF8_SHIFT  136
#define CFP_FIELD_CHAIN_UDF9_LEN  16
#define CFP_FIELD_CHAIN_UDF9_SHIFT  152
#define CFP_FIELD_CHAIN_UDF10_LEN  16
#define CFP_FIELD_CHAIN_UDF10_SHIFT  168
#define CFP_FIELD_CHAIN_UDF11_LEN  16
#define CFP_FIELD_CHAIN_UDF11_SHIFT  184
#define CFP_FIELD_CHAIN_UDF0_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF0_VLD_SHIFT  200
#define CFP_FIELD_CHAIN_UDF1_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF1_VLD_SHIFT  201
#define CFP_FIELD_CHAIN_UDF2_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF2_VLD_SHIFT  202
#define CFP_FIELD_CHAIN_UDF3_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF3_VLD_SHIFT  203
#define CFP_FIELD_CHAIN_UDF4_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF4_VLD_SHIFT  204
#define CFP_FIELD_CHAIN_UDF5_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF5_VLD_SHIFT  205
#define CFP_FIELD_CHAIN_UDF6_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF6_VLD_SHIFT  206
#define CFP_FIELD_CHAIN_UDF7_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF7_VLD_SHIFT  207
#define CFP_FIELD_CHAIN_UDF8_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF8_VLD_SHIFT  208
#define CFP_FIELD_CHAIN_UDF9_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF9_VLD_SHIFT  209
#define CFP_FIELD_CHAIN_UDF10_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF10_VLD_SHIFT  210
#define CFP_FIELD_CHAIN_UDF11_VLD_LEN  1
#define CFP_FIELD_CHAIN_UDF11_VLD_SHIFT  211
#define CFP_FIELD_CHAIN_CHAIN_ID_LEN  8
#define CFP_FIELD_CHAIN_CHAIN_ID_SHIFT  216


/* Action Ram */
#define CFP_FIELD_ACTION_CHAIN_ID_LEN  8
#define CFP_FIELD_ACTION_CHAIN_ID_SHIFT  0
#define CFP_FIELD_ACTION_RX_CHANNEL_ID_LEN  4
#define CFP_FIELD_ACTION_RX_CHANNEL_ID_SHIFT  8
#define CFP_FIELD_ACTION_DROP_LEN  1
#define CFP_FIELD_ACTION_DROP_SHIFT  12

#define CFP_FIELD_INFO \
{ \
    {CFP_FIELD_IPV4_VALID_SHIFT, CFP_FIELD_IPV4_VALID_LEN}, \
    {CFP_FIELD_IPV4_SLICE_ID_SHIFT, CFP_FIELD_IPV4_SLICE_ID_LEN}, \
    {CFP_FIELD_IPV4_UDF0_SHIFT, CFP_FIELD_IPV4_UDF0_LEN}, \
    {CFP_FIELD_IPV4_UDF1_SHIFT, CFP_FIELD_IPV4_UDF1_LEN}, \
    {CFP_FIELD_IPV4_UDF2_SHIFT, CFP_FIELD_IPV4_UDF2_LEN}, \
    {CFP_FIELD_IPV4_UDF3_SHIFT, CFP_FIELD_IPV4_UDF3_LEN}, \
    {CFP_FIELD_IPV4_UDF4_SHIFT, CFP_FIELD_IPV4_UDF4_LEN}, \
    {CFP_FIELD_IPV4_UDF5_SHIFT, CFP_FIELD_IPV4_UDF5_LEN}, \
    {CFP_FIELD_IPV4_UDF6_SHIFT, CFP_FIELD_IPV4_UDF6_LEN}, \
    {CFP_FIELD_IPV4_UDF7_SHIFT, CFP_FIELD_IPV4_UDF7_LEN}, \
    {CFP_FIELD_IPV4_UDF8_SHIFT, CFP_FIELD_IPV4_UDF8_LEN}, \
    {CFP_FIELD_IPV4_CTAG_SHIFT, CFP_FIELD_IPV4_CTAG_LEN}, \
    {CFP_FIELD_IPV4_STAG_SHIFT, CFP_FIELD_IPV4_STAG_LEN}, \
    {CFP_FIELD_IPV4_UDF0_VLD_SHIFT, CFP_FIELD_IPV4_UDF0_VLD_LEN}, \
    {CFP_FIELD_IPV4_UDF1_VLD_SHIFT, CFP_FIELD_IPV4_UDF1_VLD_LEN}, \
    {CFP_FIELD_IPV4_UDF2_VLD_SHIFT, CFP_FIELD_IPV4_UDF2_VLD_LEN}, \
    {CFP_FIELD_IPV4_UDF3_VLD_SHIFT, CFP_FIELD_IPV4_UDF3_VLD_LEN}, \
    {CFP_FIELD_IPV4_UDF4_VLD_SHIFT, CFP_FIELD_IPV4_UDF4_VLD_LEN}, \
    {CFP_FIELD_IPV4_UDF5_VLD_SHIFT, CFP_FIELD_IPV4_UDF5_VLD_LEN}, \
    {CFP_FIELD_IPV4_UDF6_VLD_SHIFT, CFP_FIELD_IPV4_UDF6_VLD_LEN}, \
    {CFP_FIELD_IPV4_UDF7_VLD_SHIFT, CFP_FIELD_IPV4_UDF7_VLD_LEN}, \
    {CFP_FIELD_IPV4_UDF8_VLD_SHIFT, CFP_FIELD_IPV4_UDF8_VLD_LEN}, \
    {CFP_FIELD_IPV4_TTL_RANGE_SHIFT, CFP_FIELD_IPV4_TTL_RANGE_LEN}, \
    {CFP_FIELD_IPV4_IP_AUTH_SHIFT, CFP_FIELD_IPV4_IP_AUTH_LEN}, \
    {CFP_FIELD_IPV4_NON_FIRST_FRAG_SHIFT, CFP_FIELD_IPV4_NON_FIRST_FRAG_LEN}, \
    {CFP_FIELD_IPV4_IP_FRAG_SHIFT, CFP_FIELD_IPV4_IP_FRAG_LEN}, \
    {CFP_FIELD_IPV4_IP_PROTO_SHIFT, CFP_FIELD_IPV4_IP_PROTO_LEN}, \
    {CFP_FIELD_IPV4_IP_TOS_SHIFT, CFP_FIELD_IPV4_IP_TOS_LEN}, \
    {CFP_FIELD_IPV4_L3_FRAMING_SHIFT, CFP_FIELD_IPV4_L3_FRAMING_LEN}, \
    {CFP_FIELD_IPV4_L2_FRAMING_SHIFT, CFP_FIELD_IPV4_L2_FRAMING_LEN}, \
    {CFP_FIELD_IPV4_C_TAGGED_SHIFT, CFP_FIELD_IPV4_C_TAGGED_LEN}, \
    {CFP_FIELD_IPV4_S_TAGGED_SHIFT, CFP_FIELD_IPV4_S_TAGGED_LEN}, \
    {CFP_FIELD_IPV4_SRC_PMAP_SHIFT, CFP_FIELD_IPV4_SRC_PMAP_LEN}, \
    {CFP_FIELD_IPV6_VALID_SHIFT, CFP_FIELD_IPV6_VALID_LEN}, \
    {CFP_FIELD_IPV6_SLICE_ID_SHIFT, CFP_FIELD_IPV6_SLICE_ID_LEN}, \
    {CFP_FIELD_IPV6_UDF0_SHIFT, CFP_FIELD_IPV6_UDF0_LEN}, \
    {CFP_FIELD_IPV6_UDF1_SHIFT, CFP_FIELD_IPV6_UDF1_LEN}, \
    {CFP_FIELD_IPV6_UDF2_SHIFT, CFP_FIELD_IPV6_UDF2_LEN}, \
    {CFP_FIELD_IPV6_UDF3_SHIFT, CFP_FIELD_IPV6_UDF3_LEN}, \
    {CFP_FIELD_IPV6_UDF4_SHIFT, CFP_FIELD_IPV6_UDF4_LEN}, \
    {CFP_FIELD_IPV6_UDF5_SHIFT, CFP_FIELD_IPV6_UDF5_LEN}, \
    {CFP_FIELD_IPV6_UDF6_SHIFT, CFP_FIELD_IPV6_UDF6_LEN}, \
    {CFP_FIELD_IPV6_UDF7_SHIFT, CFP_FIELD_IPV6_UDF7_LEN}, \
    {CFP_FIELD_IPV6_UDF8_SHIFT, CFP_FIELD_IPV6_UDF8_LEN}, \
    {CFP_FIELD_IPV6_CTAG_SHIFT, CFP_FIELD_IPV6_CTAG_LEN}, \
    {CFP_FIELD_IPV6_STAG_SHIFT, CFP_FIELD_IPV6_STAG_LEN}, \
    {CFP_FIELD_IPV6_UDF0_VLD_SHIFT, CFP_FIELD_IPV6_UDF0_VLD_LEN}, \
    {CFP_FIELD_IPV6_UDF1_VLD_SHIFT, CFP_FIELD_IPV6_UDF1_VLD_LEN}, \
    {CFP_FIELD_IPV6_UDF2_VLD_SHIFT, CFP_FIELD_IPV6_UDF2_VLD_LEN}, \
    {CFP_FIELD_IPV6_UDF3_VLD_SHIFT, CFP_FIELD_IPV6_UDF3_VLD_LEN}, \
    {CFP_FIELD_IPV6_UDF4_VLD_SHIFT, CFP_FIELD_IPV6_UDF4_VLD_LEN}, \
    {CFP_FIELD_IPV6_UDF5_VLD_SHIFT, CFP_FIELD_IPV6_UDF5_VLD_LEN}, \
    {CFP_FIELD_IPV6_UDF6_VLD_SHIFT, CFP_FIELD_IPV6_UDF6_VLD_LEN}, \
    {CFP_FIELD_IPV6_UDF7_VLD_SHIFT, CFP_FIELD_IPV6_UDF7_VLD_LEN}, \
    {CFP_FIELD_IPV6_UDF8_VLD_SHIFT, CFP_FIELD_IPV6_UDF8_VLD_LEN}, \
    {CFP_FIELD_IPV6_HOPLIMIT_RANGE_SHIFT, CFP_FIELD_IPV6_HOPLIMIT_RANGE_LEN}, \
    {CFP_FIELD_IPV6_IP_AUTH_SHIFT, CFP_FIELD_IPV6_IP_AUTH_LEN}, \
    {CFP_FIELD_IPV6_NON_FIRST_FRAG_SHIFT, CFP_FIELD_IPV6_NON_FIRST_FRAG_LEN}, \
    {CFP_FIELD_IPV6_IP_FRAG_SHIFT, CFP_FIELD_IPV6_IP_FRAG_LEN}, \
    {CFP_FIELD_IPV6_IP_PROTO_SHIFT, CFP_FIELD_IPV6_IP_PROTO_LEN}, \
    {CFP_FIELD_IPV6_IP_TOS_SHIFT, CFP_FIELD_IPV6_IP_TOS_LEN}, \
    {CFP_FIELD_IPV6_L3_FRAMING_SHIFT, CFP_FIELD_IPV6_L3_FRAMING_LEN}, \
    {CFP_FIELD_IPV6_L2_FRAMING_SHIFT, CFP_FIELD_IPV6_L2_FRAMING_LEN}, \
    {CFP_FIELD_IPV6_C_TAGGED_SHIFT, CFP_FIELD_IPV6_C_TAGGED_LEN}, \
    {CFP_FIELD_IPV6_S_TAGGED_SHIFT, CFP_FIELD_IPV6_S_TAGGED_LEN}, \
    {CFP_FIELD_IPV6_SRC_PMAP_SHIFT, CFP_FIELD_IPV6_SRC_PMAP_LEN}, \
    {CFP_FIELD_NONIP_VALID_SHIFT, CFP_FIELD_NONIP_VALID_LEN}, \
    {CFP_FIELD_NONIP_SLICE_ID_SHIFT, CFP_FIELD_NONIP_SLICE_ID_LEN}, \
    {CFP_FIELD_NONIP_UDF0_SHIFT, CFP_FIELD_NONIP_UDF0_LEN}, \
    {CFP_FIELD_NONIP_UDF1_SHIFT, CFP_FIELD_NONIP_UDF1_LEN}, \
    {CFP_FIELD_NONIP_UDF2_SHIFT, CFP_FIELD_NONIP_UDF2_LEN}, \
    {CFP_FIELD_NONIP_UDF3_SHIFT, CFP_FIELD_NONIP_UDF3_LEN}, \
    {CFP_FIELD_NONIP_UDF4_SHIFT, CFP_FIELD_NONIP_UDF4_LEN}, \
    {CFP_FIELD_NONIP_UDF5_SHIFT, CFP_FIELD_NONIP_UDF5_LEN}, \
    {CFP_FIELD_NONIP_UDF6_SHIFT, CFP_FIELD_NONIP_UDF6_LEN}, \
    {CFP_FIELD_NONIP_UDF7_SHIFT, CFP_FIELD_NONIP_UDF7_LEN}, \
    {CFP_FIELD_NONIP_UDF8_SHIFT, CFP_FIELD_NONIP_UDF8_LEN}, \
    {CFP_FIELD_NONIP_CTAG_SHIFT, CFP_FIELD_NONIP_CTAG_LEN}, \
    {CFP_FIELD_NONIP_STAG_SHIFT, CFP_FIELD_NONIP_STAG_LEN}, \
    {CFP_FIELD_NONIP_UDF0_VLD_SHIFT, CFP_FIELD_NONIP_UDF0_VLD_LEN}, \
    {CFP_FIELD_NONIP_UDF1_VLD_SHIFT, CFP_FIELD_NONIP_UDF1_VLD_LEN}, \
    {CFP_FIELD_NONIP_UDF2_VLD_SHIFT, CFP_FIELD_NONIP_UDF2_VLD_LEN}, \
    {CFP_FIELD_NONIP_UDF3_VLD_SHIFT, CFP_FIELD_NONIP_UDF3_VLD_LEN}, \
    {CFP_FIELD_NONIP_UDF4_VLD_SHIFT, CFP_FIELD_NONIP_UDF4_VLD_LEN}, \
    {CFP_FIELD_NONIP_UDF5_VLD_SHIFT, CFP_FIELD_NONIP_UDF5_VLD_LEN}, \
    {CFP_FIELD_NONIP_UDF6_VLD_SHIFT, CFP_FIELD_NONIP_UDF6_VLD_LEN}, \
    {CFP_FIELD_NONIP_UDF7_VLD_SHIFT, CFP_FIELD_NONIP_UDF7_VLD_LEN}, \
    {CFP_FIELD_NONIP_UDF8_VLD_SHIFT, CFP_FIELD_NONIP_UDF8_VLD_LEN}, \
    {CFP_FIELD_NONIP_ETHERTYPE_SAP_SHIFT, CFP_FIELD_NONIP_ETHERTYPE_SAP_LEN}, \
    {CFP_FIELD_NONIP_L3_FRAMING_SHIFT, CFP_FIELD_NONIP_L3_FRAMING_LEN}, \
    {CFP_FIELD_NONIP_L2_FRAMING_SHIFT, CFP_FIELD_NONIP_L2_FRAMING_LEN}, \
    {CFP_FIELD_NONIP_C_TAGGED_SHIFT, CFP_FIELD_NONIP_C_TAGGED_LEN}, \
    {CFP_FIELD_NONIP_S_TAGGED_SHIFT, CFP_FIELD_NONIP_S_TAGGED_LEN}, \
    {CFP_FIELD_NONIP_SRC_PMAP_SHIFT, CFP_FIELD_NONIP_SRC_PMAP_LEN}, \
    {CFP_FIELD_CHAIN_VALID_SHIFT, CFP_FIELD_CHAIN_VALID_LEN}, \
    {CFP_FIELD_CHAIN_SLICE_ID_SHIFT, CFP_FIELD_CHAIN_SLICE_ID_LEN}, \
    {CFP_FIELD_CHAIN_UDF0_SHIFT, CFP_FIELD_CHAIN_UDF0_LEN}, \
    {CFP_FIELD_CHAIN_UDF1_SHIFT, CFP_FIELD_CHAIN_UDF1_LEN}, \
    {CFP_FIELD_CHAIN_UDF2_SHIFT, CFP_FIELD_CHAIN_UDF2_LEN}, \
    {CFP_FIELD_CHAIN_UDF3_SHIFT, CFP_FIELD_CHAIN_UDF3_LEN}, \
    {CFP_FIELD_CHAIN_UDF4_SHIFT, CFP_FIELD_CHAIN_UDF4_LEN}, \
    {CFP_FIELD_CHAIN_UDF5_SHIFT, CFP_FIELD_CHAIN_UDF5_LEN}, \
    {CFP_FIELD_CHAIN_UDF6_SHIFT, CFP_FIELD_CHAIN_UDF6_LEN}, \
    {CFP_FIELD_CHAIN_UDF7_SHIFT, CFP_FIELD_CHAIN_UDF7_LEN}, \
    {CFP_FIELD_CHAIN_UDF8_SHIFT, CFP_FIELD_CHAIN_UDF8_LEN}, \
    {CFP_FIELD_CHAIN_UDF9_SHIFT, CFP_FIELD_CHAIN_UDF9_LEN}, \
    {CFP_FIELD_CHAIN_UDF10_SHIFT, CFP_FIELD_CHAIN_UDF10_LEN}, \
    {CFP_FIELD_CHAIN_UDF11_SHIFT, CFP_FIELD_CHAIN_UDF11_LEN}, \
    {CFP_FIELD_CHAIN_UDF0_VLD_SHIFT, CFP_FIELD_CHAIN_UDF0_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF1_VLD_SHIFT, CFP_FIELD_CHAIN_UDF1_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF2_VLD_SHIFT, CFP_FIELD_CHAIN_UDF2_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF3_VLD_SHIFT, CFP_FIELD_CHAIN_UDF3_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF4_VLD_SHIFT, CFP_FIELD_CHAIN_UDF4_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF5_VLD_SHIFT, CFP_FIELD_CHAIN_UDF5_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF6_VLD_SHIFT, CFP_FIELD_CHAIN_UDF6_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF7_VLD_SHIFT, CFP_FIELD_CHAIN_UDF7_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF8_VLD_SHIFT, CFP_FIELD_CHAIN_UDF8_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF9_VLD_SHIFT, CFP_FIELD_CHAIN_UDF9_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF10_VLD_SHIFT, CFP_FIELD_CHAIN_UDF10_VLD_LEN}, \
    {CFP_FIELD_CHAIN_UDF11_VLD_SHIFT, CFP_FIELD_CHAIN_UDF11_VLD_LEN}, \
    {CFP_FIELD_CHAIN_CHAIN_ID_SHIFT, CFP_FIELD_CHAIN_CHAIN_ID_LEN}, \
    {CFP_FIELD_ACTION_CHAIN_ID_SHIFT, CFP_FIELD_ACTION_CHAIN_ID_LEN}, \
    {CFP_FIELD_ACTION_RX_CHANNEL_ID_SHIFT, CFP_FIELD_ACTION_RX_CHANNEL_ID_LEN}, \
    {CFP_FIELD_ACTION_DROP_SHIFT, CFP_FIELD_ACTION_DROP_LEN}, \
}

/* Definition for CFP for GMAC */

/* operation type */
#define CFP_TCAM_OP_READ    0x1 
#define CFP_TCAM_OP_WRITE    0x2 

/* RAM Type */
#define CFP_RAM_TYPE_TCAM    0x1
#define CFP_RAM_TYPE_TCAM_MASK    0x2
#define CFP_RAM_TYPE_ACTION    0x3

/* HW Ram Type value */
#define CFP_RAM_HW_TCAM     0x1
#define CFP_RAM_HW_ACTION   0x2

/* L3 FRAMING */
#define CFP_L3_FRAMING_IPV4    0x0
#define CFP_L3_FRAMING_IPV6    0x1
#define CFP_L3_FRAMING_NONIP    0x3

#define CFP_IPV4_MAX_SLICE_ID        0x2
#define CFP_IPV6_MAX_SLICE_ID        0x3
#define CFP_NONIP_MAX_SLICE_ID        0x2

/* UDF Offset Base flags*/
#define CFP_UDF_OFFSET_BASE_STARTOFFRAME    0x0
#define CFP_UDF_OFFSET_BASE_ENDOFL2    0x40
#define CFP_UDF_OFFSET_BASE_ENDOFL3    0x60

/* bit 0~4 is the UDF offset base value */
#define CFP_UDF_OFFSET_BASE_VALUE_MASK    0x1f
#define CFP_UDF_OFFSET_BASE_VALUE_SHIFT    0
/* bit 5~7 is the UDF offset base flags value */
#define CFP_UDF_OFFSET_BASE_FLAGS_MASK    0x7
#define CFP_UDF_OFFSET_BASE_FLAGS_SHIFT    5

#define CFP_TCAM_ENTRY_LEN    230
#define CFP_TCAM_ENTRY_WORD     (CFP_TCAM_ENTRY_LEN + 31) / 32

typedef struct cfp_hw_entry {
    uint32  tcam[CFP_TCAM_ENTRY_WORD];
    uint32  tcam_mask[CFP_TCAM_ENTRY_WORD];
    uint32  action;
} cfp_hw_entry_t;

typedef struct cfp_ioctl_buf {
    uint32  entry_idx; /* CFP entry index */
    uint32  ram_type; /* RAM TYPE */
    uint32  l3_framing; /* L3 FRAMING TYPE */
    uint32  slice_id; /* SLICE ID number */
    uint32  field_idx; /* CFP Field index */
    uint32  field_value; /* CFP Field value */
    uint32  flags; /* UDF Offset flags */
    cfp_hw_entry_t cfp_entry; /* CFP raw data */
} cfp_ioctl_buf_t;


#define CFP_TCAM_NUM 128
#define CFP_RETRY_TIME  100

#define RX_RATE_MAX_MAP_VALUE   63
#define RX_RATE_VALUE_MAPPING \
{ 384, 512, 640, 768, 1024, 1280, 1536, 1792, 2304, 2816, \
   3328, 3840, 4864, 5888, 6912, 7936, 9984, 12032, 14080, 16128, \
   20224, 24320, 28416, 32512, 40704, 48896, 57088, 65280, 81664, 98048, \
   114432, 130816, 147200, 163584, 179968, 196352, 212736, 229120, 245504, \
   261888, 278272, 294656, 311040, 327424, 343808, 360192, 376576, 392960, \
   409344, 442112, 474880, 507648, 540416, 573184, 605952, 638720, \
   671488, 704256, 835328, 966400, 1097472, 1228544, 1359616, 1490688 }

    

#define TX_RATE_VALUE_BASE   64000
#define TX_RATE_BURST_BASE    64


#define MAX_PRIVATE_TAG_VALUE   7


/* Voyager device reversion ID */
#define GMAC_VOYAGER_REVISION_ID            2


#endif /* _etcgmac_h_ */
