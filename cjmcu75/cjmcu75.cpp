/*
 * Filename   : cjmcu75.cpp
 * Description: CJMCU-75 LM75A Temperature Sensor Board
 */
#include "cjmcu75.h"

CJMCU75::CJMCU75(PinName sda, PinName scl, int addr) : m_i2c(sda, scl), m_addr(addr)
{
	id 	= CJMCU75_DEV_ID;
    reg = CJMCU75_REG_TEMP;
	devConfig.all = 0x00;				//-- reset state and configuration
}

CJMCU75::~CJMCU75()
{

}

void CJMCU75::init()
{
	set_config(devConfig.all);
	get_T_os();
	get_T_hyst();
	printf("T_hyst : %d\r\n", _T_hyst);
	printf("T_os   : %d\r\n", _T_os);
}

bool CJMCU75::verify()
{
	char id=CJMCU75::get_id();
	return (id == 0xA1);
}

float CJMCU75::get_temp()
{
	uint16_t val;
	float realval;

	memset(buf, 0x00, 4);				//-- common 4 bytes buffer

	reg = CJMCU75_REG_TEMP;

	m_i2c.write(m_addr, &reg, 1);		//-- Pointer to the temperature register
	m_i2c.read(m_addr, buf, 2);			//-- read temperature register
  
	val = ((buf[0] << 8) | buf[1]);	
	val >>= 5;							//-- only 12-bits are used

	//-- process sign (+/-), 2's complement
	if(val & 0x0100) {
		val |= 0xF800;
		val = ~val+1;
		realval = (float) val * (-1) * 0.125;
	} else {
		realval = (float) val * 0.125;
	}
	return realval;
}

char CJMCU75::read_reg(char addr)
{

    char regaddr = addr;

	memset(buf, 0x00, 4);

    m_i2c.write(m_addr, &regaddr, 1);           
    m_i2c.read(m_addr, buf, 1);              // Read register content
    
    return buf[0];

}

void CJMCU75::write_reg(char addr, char data)
{
	memset(buf, 0x00, 4);
    
    buf[0] = addr;
    buf[1] = data;
    
    m_i2c.write(m_addr, buf, 2);             
}

char CJMCU75::get_id(void)
{
	//-- clear the char buffer
	memset(buf, 0x00, 4);

	reg = CJMCU75_REG_ID;	//-- 0x07

	m_i2c.start();
		m_i2c.write(m_addr, &reg, 1);
		m_i2c.read(m_addr, buf, 1);
	m_i2c.stop();

	printf("id val = 0x%02x\r\n", buf[0]);

	//-- assertOne(idval == id); //-- abort if assertion failed
	assertTwo(buf[0] == id);

	return buf[0];
}

LM75AConfigReg_t CJMCU75::get_config()
{
	char cfg = CJMCU75::read_reg(CJMCU75_REG_CFG);
	devConfig.all = cfg;
	return devConfig;
}

void CJMCU75::set_config(uint8_t value)
{
	CJMCU75::write_reg((char)CJMCU75_REG_CFG, value); 
}

void CJMCU75::get_T_hyst()
{
	char val = CJMCU75::read_reg(CJMCU75_REG_T_HYST);
	if(val & 0x80) {
		_T_hyst = (~val +1) * (-1);
	} else {
		_T_hyst = (int8_t) val;
	}
}

void CJMCU75::set_T_hyst(int8_t setpoint)
{
	//-- it should not go beyond the Tmax=125 DegC and Tmin=-55 DegC
	//-- default setpoint for Thyst = 80 DegC
	//-- default setpoint for Tos = 80 DegC
	//-- O.S. active low
	CJMCU75::write_reg((char)CJMCU75_REG_T_HYST, setpoint);
}

void CJMCU75::get_T_os()
{
	char val = CJMCU75::read_reg(CJMCU75_REG_T_OS);
	if(val & 0x80) {
		_T_os = (~val + 1) *(-1);
	} else {
		_T_os = (int8_t) val;
	}
}

void CJMCU75::set_T_os(int8_t setpoint)
{
	//-- it should not go beyond the Tmax=125 DegC and Tmin=-55 DegC
	//-- default setpoint for Thyst = 80 DegC
	//-- default setpoint for Tos = 80 DegC
	CJMCU75::write_reg(CJMCU75_REG_T_OS, setpoint);	
}
//-- end of program -------------------------------------------------
