#include <common.h>
#include <errno.h>
#include <i2c.h>
#if 0
#define GAUGE_DEVICE_ADDRESS 0xAA

#define SET_CFGUPDATE 	0x0013
#define EXIT_RESIM 	0x0044
#define SOFT_RESET 	0x0042
#define RESET		0x0041
#define SET_HIBERNATE	0x0011

#define CMD_DATA_CLASS 	0x3E
#define CMD_DATA_BLOCK 	0x3F
#define CMD_BLOCK_DATA	0x40
#define CMD_CHECK_SUM 	0x60
#define CMD_FLAGS 	0x06

#define DC_STATE	0x52
#define DC_STATE_LENGTH	38

#define CFGUPD 0x0010
#else

#define GAUGE_DEVICE_ADDRESS 0x55
#define SET_CFGUPDATE 	0x0090

#define EXIT_RESIM 	0x0091
#define SOFT_RESET 	0x0042
#define RESET		0x0041
#define SET_HIBERNATE	0x0011

#define CMD_DATA_CLASS 	0x3E
#define CMD_DATA_BLOCK 	0x3F
#define CMD_BLOCK_DATA	0x40
#define CMD_CHECK_SUM 	0x60
#define CMD_FLAGS 	0x3b

#define DC_STATE	0x52
#define DC_STATE_LENGTH	38

#define CFGUPD 0x0002
#endif

// nI2C = handle to your I2C driver (if required)
// nRegister = gauge register
// pData = pointer to data block which will hold the data from the gauge
// nLength = length
int gauge_i2c_read(int nI2C, unsigned char nRegister, unsigned char *pData, unsigned char nLength)
{
	int ret;
	 nI2C = GAUGE_DEVICE_ADDRESS;
	// implement your I2C read function here. Make sure you follow the timing requirements from the datasheet
	ret = i2c_read(nI2C, nRegister, 1, pData, nLength);
	udelay(800);

	return ret;
}

// nI2C = handle to your I2C driver (if required)
// nRegister = gauge register
// pData = pointer to data block which holds the data for the gauge
// nLength = length
int gauge_i2c_write(int nI2C, unsigned char nRegister, unsigned char *pData, unsigned char nLength)
{
	int ret;
	nI2C = GAUGE_DEVICE_ADDRESS;
	// implement your I2C write function here. Make sure you follow the timing requirements from the datasheet 
	ret = i2c_write(nI2C, nRegister, 1, pData, nLength);
	udelay(800);

	return ret;
}

// issue a control command (nSubCmd = sub command) to the gauge
unsigned int gauge_control(int nI2C, unsigned int nSubCmd)
{
	int ret;
	unsigned int nResult = 0;

	char pData[2];

	pData[0] = nSubCmd & 0xFF;
	pData[1] = (nSubCmd >> 8) & 0xFF;
	
	ret=gauge_i2c_write(nI2C, 0x00, pData, 2); // issue control and sub command

	ret|=gauge_i2c_read(nI2C, 0x00, pData, 2); // read data
	if(ret<0)
		return -1;

	nResult = (pData[1] << 8) | pData[0];

	return nResult; 
}

// read 2 bytes from the gauge (nCmd = command)
unsigned int gauge_cmd_read(int nI2C, unsigned char nCmd)
{
	int ret;
	unsigned char pData[2];

	ret = gauge_i2c_read(nI2C, nCmd, pData, 2);
	if(ret < 0)
		return -1;

	return (pData[1] << 8) | pData[0];
} 

// write 2 bytes to the gauge (nCmd = command, nData = 16 bit parameter)
unsigned int gauge_cmd_write(int nI2C, unsigned char nCmd, unsigned int nData)
{
	int ret;
	unsigned char pData[2];

	pData[0] = nData & 0xFF;
	pData[1] = (nData >> 8) & 0xFF;

	ret=gauge_i2c_write(nI2C, nCmd, pData, 2);
	if(ret < 0)
		return -1;

	return (pData[1] << 8) | pData[0];
} 

#define MAX_ATTEMPTS 5
// exit CFG_UPDATE mode with one of the supported exit commands (e.g. EXIT_RESIM)
int gauge_exit(int nI2C, unsigned int nCmd)
{
	unsigned int nFlags;
	int nAttempts = 0;
	gauge_control(nI2C, nCmd);

	do
	{
		nFlags = gauge_cmd_read(nI2C, CMD_FLAGS);
		if (nFlags & CFGUPD) udelay(500000);
	} while ((nFlags & CFGUPD) && (nAttempts++ < MAX_ATTEMPTS));

	return (nAttempts < MAX_ATTEMPTS);
}

// enter CFG_UPDATE mode
int gauge_cfg_update(int nI2C)
{
	unsigned int nFlags;
	int nAttempts = 0;
	gauge_control(nI2C, SET_CFGUPDATE);

	do
	{
		nFlags = gauge_cmd_read(nI2C, CMD_FLAGS);
		if (!(nFlags & CFGUPD)) udelay(500000);
	} while (!(nFlags & CFGUPD) && (nAttempts++ < MAX_ATTEMPTS));

	return (nAttempts < MAX_ATTEMPTS);
}

// enter UNSET and full access mode
int gauge_unset_fullaccess(int nI2C)
{
	gauge_i2c_write(nI2C, 0x00,0x1404, 2); 
	gauge_i2c_write(nI2C, 0x00,0x7236, 2); 
	
	gauge_i2c_write(nI2C, 0x00,0xFFFF, 2); 
	gauge_i2c_write(nI2C, 0x00,0xFFFF, 2);
	
	return 0;
}


// read a data class.
// nDataClass = data class number
// pData = raw data (for the whole data class)
// nLength = length of the whole data class
int gauge_read_data_class(int nI2C, unsigned char nDataClass, unsigned char *pData, unsigned char nLength)
{
	unsigned char nRemainder = nLength;
	unsigned int nOffset = 0;
	unsigned char nDataBlock = 0x00;
	unsigned int nData;

	if (nLength < 1) return 0;

	do
	{

		nLength = nRemainder;
		if (nLength > 32)
		{
			nRemainder = nLength - 32;
			nLength = 32;
		}
		else nRemainder = 0;

		nData = (nDataBlock << 8) | nDataClass;
		gauge_cmd_write(nI2C, CMD_DATA_CLASS, nData);

		gauge_i2c_read(nI2C, CMD_BLOCK_DATA, pData, nLength);

		pData += nLength;
		nDataBlock++;
	} while (nRemainder > 0);

	return nLength;
}

unsigned char check_sum(unsigned char *pData, unsigned char nLength)
{
	unsigned char nSum = 0x00;
	unsigned char n;

	for (n = 0; n < nLength; n++)
		nSum += pData[n];

	nSum = 0xFF - nSum;

	return nSum;
}

// write a data class.
// nDataClass = data class number
// pData = raw data (for the whole data class)
// nLength = length of the whole data class
int gauge_write_data_class(int nI2C, unsigned char nDataClass, unsigned char *pData, unsigned char nLength)
{
	unsigned char nRemainder = nLength;
	unsigned int nOffset = 0;
	unsigned char pCheckSum[2] = {0x00, 0x00};
	unsigned int nData;
	unsigned char nDataBlock = 0x00;

	if (nLength < 1) return 0;

	do
	{
		nLength = nRemainder;
		if (nLength > 32)
		{
			nRemainder = nLength - 32;
			nLength = 32;
		}
		else nRemainder = 0;

		nData = (nDataBlock << 8) | nDataClass;
		gauge_cmd_write(nI2C, CMD_DATA_CLASS, nData);

		gauge_i2c_write(nI2C, CMD_BLOCK_DATA, pData, nLength);
		pCheckSum[0] = check_sum(pData, nLength);
		gauge_i2c_write(nI2C, CMD_CHECK_SUM, pCheckSum, 1);

		udelay(10000);

		gauge_cmd_write(nI2C, CMD_DATA_CLASS, nData);
		gauge_i2c_read(nI2C, CMD_CHECK_SUM, pCheckSum + 1, 1);
		if (pCheckSum[0] != pCheckSum[1])
			printf("gauge_write_data_class(): CheckSum mismatch 0x%02X vs. 0x%02X\n\r", pCheckSum[0], pCheckSum[1]);

		pData += nLength;
		nDataBlock++;
	} while (nRemainder > 0);

	return nLength;
}

static void print_data(unsigned char *pData, unsigned int nLength)
{
	unsigned int n;

	for (n = 0; n < nLength; n++)
	{
		printf("%02X ", pData[n]);
		if (!((n + 1) % 16)) printf("\n\r");
	}

	printf("\n\r");
}

#if 0
//Examples:
int gauge_test(void)
{
	int nResult;
	int nI2C=0;
	unsigned char pData[20];
//read flags:
	//nResult = gauge_cmd_read(nI2C, CMD_FLAGS);
	nResult = gauge_cmd_read(nI2C, 0x2c);

	printf("SOC = %d\n\r", nResult);
	
#if 1
//read data class DC_STATE:
	gauge_unset_fullaccess(nI2C);

//	gauge_cfg_update(nI2C);
        //gauge_exit(nI2C, EXIT_RESIM);

//	gauge_control(nI2C, RESET);

	//gauge_read_data_class(nI2C, DC_STATE, pData, DC_STATE_LENGTH);
	printf("Data Class 'State' (0x52):\n\r");
	//print_data(pData, DC_STATE_LENGTH); 
#else
//read data class DC_STATE:
	gauge_read_data_class(nI2C, 0x2c, pData, 1);
	printf("Data Class 'State' (0x52):\n\r");
	print_data(pData, DC_STATE_LENGTH); 

#endif	
#if 1
//write data class DC_STATE:
	unsigned char pClass52Data[]={0x09,0xc4};

	gauge_unset_fullaccess(nI2C);
	gauge_cfg_update(nI2C);
//	gauge_write_data_class(nI2C, DC_STATE, pClass52Data, sizeof(pClass52Data));
	gauge_write_data_class(nI2C, 0x929f, 2650, sizeof(pClass52Data));

//	gauge_exit(nI2C, EXIT_RESIM);
#endif
}
#endif
