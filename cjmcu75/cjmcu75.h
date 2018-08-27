/*
 * file name  : cjmcu75.h
 * description: CJMCU-75 LM75A IC Breakout Board
 * 			Temperature range : -55 ~ 125 DegC
 * 			Resolution        : 9-bit ADC, 0.125
 */
#pragma once 
 
#include "mbed.h"
 
#define	CJMCU75_DEV_ID		0xA1		//-- device id, "0xA1"

//-- assert level one -----------------------------------------------
#define assertOne(x)	{								\
		if(x) { 										\
			return 0; 									\
		} else { 										\
			printf("assertOne: assertion failed at %d\r\n", __LINE__);\
			abort();									\
		}												\
	}

//-- assert level two -----------------------------------------------
#define assertTwo(x)	{								\
		if(x) { 										\
			return 0; 									\
		} else { 										\
			printf("assertTwo: assertion failed at %d\r\n", __LINE__);\
			return -1;									\
		}												\
	}

//-- CJMCU75 Register Address Map  -----------------------------------
#define CJMCU75_REG_TEMP	0x00
#define CJMCU75_REG_CFG		0x01
#define CJMCU75_REG_T_HYST	0x02	//-- default Thyst = 75 DegC
#define CJMCU75_REG_T_OS	0x03	//-- default Tos   = 80 DegC
#define CJMCU75_REG_ID		0x07

typedef struct {
	unsigned :3;			//-- reserved [7:5]
	unsigned FAULT_QUEUE:2;	//-- FAULT_QUEUE[2]
	unsigned OSPOLARITY:1;	//-- alter polarity (1==active high, 0 ==active low)
	unsigned MODE:1;		//-- mode (Cmp/Int) (1==interrupt mode, 0 == Comparator)	
	unsigned SHUTDOWN:1;	//-- shutdown -> low power shutdown mode
} 	LM75AConfigBits_t;

typedef union {
	LM75AConfigBits_t 	bit;
	uint8_t				all;
} LM75AConfigReg_t;

const float LM75A_DEGREES_RESOLUTION = 0.125;

/* Library for the CJMCU75 temperature sensor. */

class CJMCU75 {        // Creates an instance of the class

    public:
           /** Create an CJMCU75 object connected to the specified I2C object and using the specified deviceAddress
            *
            * @param sda The I2C port data
            * @param scl The I2C port clock
            * @param addr The address of the MMA7660FC
            */
      CJMCU75(PinName sda, PinName scl, int addr);
    
            /** Destroys an CJMCU75 object
            *
            */
      ~CJMCU75();
    
            /** Reads the current temperature
            *
            * @param returns Return the Temperature value 
            */
      float get_temp();
      
            /** Reads from specified CJMCU75 register
            *
            * @param addr Pointer register
            * @param returns Return the data from the register 
            */
      char read_reg(char regaddr);
      
            /** Writes to specified CJMCU75 register
            *
            * @param addr Pointer register
            * @param data Data to write
            */ 
      void write_reg(char regaddr, char data);

	  		/** get device id and check, 
			 *  this is just sanity check but often time not working
			*/ 
	  char get_id(void);

	  void init(void);
	  bool verify(void);

	  LM75AConfigReg_t	get_config(void);
	  LM75AConfigReg_t	set_config(void);
	  void				set_config(uint8_t newconfig);

	  void				get_T_hyst();
	  void				set_T_hyst(int8_t hyst_setpoint);
	  void				get_T_os();
	  void				set_T_os(int8_t os_setpoint);
      
    private:
      I2C 		m_i2c;
      int 		m_addr;   

	  int8_t	_T_os;
	  int8_t	_T_hyst;

	  LM75AConfigReg_t	devConfig;

      char 		reg;	//-- register address map
	  char		id;		//-- device id
	  char		buf[4];	//-- common character buffer
};
