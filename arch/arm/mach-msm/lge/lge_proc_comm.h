#ifndef __LGE_PROC_COMM_H
#define __LGE_PROC_COMM_H

/* 2011-01-26 by baborobo@lge.com
 * match with
 * /modem/AMSS/products/7x30/modem/rfa/pmic/common/app/src/pmapp_pcil.c
 */
enum {
	CUSTOMER_CMD1_SET_SPECIAL_CLOCK0,
	CUSTOMER_CMD1_GET_BOOT_ON_TYPE,
	CUSTOMER_CMD1_TEST_FRST_SYNC,
	CUSTOMER_CMD1_GET_QEM_ENABLED,
	CUSTOMER_CMD1_GET_FRST_STATUS, /* Get Frst status via proc comm */
	CUSTOMER_CMD1_SET_FRST_STATUS, /* Set Frst status via proc comm */
	CUSTOMER_CMD1_RESERVED3,
	CUSTOMER_CMD1_RESERVED4,
	CUSTOMER_CMD1_RESERVED5,
	CUSTOMER_CMD1_RESERVED6,
	CUSTOMER_CMD1_RESERVED7
};

#endif