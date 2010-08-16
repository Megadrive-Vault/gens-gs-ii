/***************************************************************************
 * libgens: Gens Emulation Library.                                        *
 * EEPRom.cpp: Serial EEPROM handler.                                      *
 *                                                                         *
 * Copyright (C) 2007, 2008, 2009  Eke-Eke (Genesis Plus GCN/Wii port)     *
 * Copyright (c) 2010 by David Korth.                                      *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the Free Software Foundation, Inc., *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           *
 ***************************************************************************/

/**
 * Based on cart_hw/eeprom.c from Genesis Plus GX.
 */

#include "EEPRom.hpp"

namespace LibGens
{

/**
 * ms_Database[]: EEPROM information database.
 */
const EEPRom::GameEEPRomInfo EEPRom::ms_Database[] =
{
	/** ACCLAIM mappers **/
	
	/* 24C02 (old mapper) */
	{"T-081326"   , 0,      {8,  0xFF,   0xFF,   0x200001, 0x200001, 0x200001, 0, 1, 1}},   /* NBA Jam (UE) */
	{"T-81033"    , 0,      {8,  0xFF,   0xFF,   0x200001, 0x200001, 0x200001, 0, 1, 1}},   /* NBA Jam (J) */
	
	/* 24C02 */
	{"T-81406"    , 0,      {8,  0xFF,   0xFF,   0x200001, 0x200001, 0x200000, 0, 0, 0}},   /* NBA Jam TE */
	{"T-081276"   , 0,      {8,  0xFF,   0xFF,   0x200001, 0x200001, 0x200000, 0, 0, 0}},   /* NFL Quarterback Club */
	
	/* 24C16 */
	{"T-081586"   , 0,      {8,  0x7FF,  0x7FF,  0x200001, 0x200001, 0x200000, 0, 0, 0}},   /* NFL Quarterback Club '96 */
	
	/* 24C65 */
	{"T-81576"    , 0,      {16, 0x1FFF, 0x1FFF, 0x200001, 0x200001, 0x200000, 0, 0, 0}},   /* College Slam */
	{"T-81476"    , 0,      {16, 0x1FFF, 0x1FFF, 0x200001, 0x200001, 0x200000, 0, 0, 0}},   /* Frank Thomas Big Hurt Baseball */
	
	/** EA mapper (24C01 only) **/
	{"T-50176"    , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 7, 7, 6}},   /* Rings of Power */
	{"T-50396"    , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 7, 7, 6}},   /* NHLPA Hockey 93 */
	{"T-50446"    , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 7, 7, 6}},   /* John Madden Football 93 */
	{"T-50516"    , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 7, 7, 6}},   /* John Madden Football 93 (Championship Ed.) */
	{"T-50606"    , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 7, 7, 6}},   /* Bill Walsh College Football */
	
	/** SEGA mapper (24C01 only) **/
	{"T-12046"    , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 0, 0, 1}},   /* Megaman - The Wily Wars */
	{"T-12053"    , 0xEA80, {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 0, 0, 1}},   /* Rockman Mega World (J) [A] */
	{"MK-1215"    , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 0, 0, 1}},   /* Evander 'Real Deal' Holyfield's Boxing */
	{"MK-1228"    , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 0, 0, 1}},   /* Greatest Heavyweights of the Ring (U) */
	{"G-5538"     , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 0, 0, 1}},   /* Greatest Heavyweights of the Ring (J) */
	{"PR-1993"    , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 0, 0, 1}},   /* Greatest Heavyweights of the Ring (E) */
	{"G-4060"     , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 0, 0, 1}},   /* Wonderboy in Monster World */
	{"00001211-00", 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 0, 0, 1}},   /* Sports Talk Baseball */
	{"00004076-00", 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 0, 0, 1}},   /* Honoo no Toukyuuji Dodge Danpei */
	{"G-4524"     , 0,      {7,  0x7F,   0x7F,   0x200001, 0x200001, 0x200001, 0, 0, 1}},   /* Ninja Burai Densetsu */
	
	/** CODEMASTERS mapper **/
	
	/* 24C01 */
	{"T-120106",    0,      {7,  0x7F,   0x7F,   0x300000, 0x380001, 0x300000, 0, 7, 1}},   /* Brian Lara Cricket */
	
	/* 24C08 */
	{"T-120096"   , 0,      {8,  0x3FF,  0x3FF,  0x300000, 0x380001, 0x300000, 0, 7, 1}},   /* Micro Machines 2 - Turbo Tournament (E) */
	{"00000000-00", 0x168B, {8,  0x3FF,  0x3FF,  0x300000, 0x380001, 0x300000, 0, 7, 1}},   /* Micro Machines Military */
	{"00000000-00", 0xCEE0, {8,  0x3FF,  0x3FF,  0x300000, 0x380001, 0x300000, 0, 7, 1}},   /* Micro Machines Military (Bad)*/
	
	/* 24C16 */
	{"00000000-00", 0x165E, {8,  0x7FF,  0x7FF,  0x300000, 0x380001, 0x300000, 0, 7, 1}},   /* Micro Machines Turbo Tournament 96 */
	{"00000000-00", 0x2C41, {8,  0x7FF,  0x7FF,  0x300000, 0x380001, 0x300000, 0, 7, 1}},   /* Micro Machines Turbo Tournament 96 (Bad)*/
	
	/* 24C65 */
	{"T-120146-50", 0,      {16, 0x1FFF, 0x1FFF, 0x300000, 0x380001, 0x300000, 0, 7, 1}},   /* Brian Lara Cricket 96, Shane Warne Cricket */
};


/**
 * EEPRom(): Initialize the EEPRom chip.
 */
EEPRom::EEPRom()
{
	// Clear the EEPRom type.
	memset(&m_eprType, 0x00, sizeof(m_eprType));
	
	// Reset the EEPRom on startup.
	reset();
}


/**
 * reset(): Clear EEPRom and initialize settings.
 * This function does NOT reset the EEPRom type!
 */
void EEPRom::reset(void)
{
	/**
	 * EEProm is initialized with 0xFF.
	 */
	
	memset(m_eeprom, 0xFF, sizeof(m_eeprom));
	
	// Initial EEPRom state.
	m_state = EEP_STANDBY;
	m_dirty = false;
	
	// Reset the clock and data lines
	m_scl = false;
	m_old_scl = false;
	m_sda = false;
	m_old_sda = false;
	
	// Clear the counter and addresses.
	m_counter = 0;
	m_rw = false;
	m_slave_mask = 0;
	m_word_address = 0;
}


/**
 * DetectEEPRomType(): Detect the EEPRom type used by the specified ROM.
 * @param serial Serial number. (NOTE: This does NOT include the "GM " prefix!)
 * @param serial_len Length of the serial number string.
 * @param checksum Checksum.
 * @return EEPRom type, or -1 if this ROM isn't known.
 */
int EEPRom::DetectEEPRomType(const char *serial, size_t serial_len, uint16_t checksum)
{
	// Scan the database for potential matches.
	for (int i = 0; i < (sizeof(ms_Database)/sizeof(ms_Database[0])); i++)
	{
		// TODO: Figure out how to get rid of the strlen().
		int dbSerial_len = strlen(ms_Database[i].game_id);
		if (dbSerial_len > serial_len)
		{
			// Serial number in the database is longer than
			// the given serial number data.
			continue;
		}
		
		if (!memcmp(serial, ms_Database[i].game_id, dbSerial_len))
		{
			// Serial number matches.
			if (ms_Database[i].checksum == 0)
			{
				// No checksum verification required.
				return i;
			}
			
			// Checksum verification is required.
			if (checksum == ms_Database[i].checksum)
			{
				// Checksum matches.
				return i;
			}
			
			// Checksum doesn't match. Wrong ROM.
		}
	}
	
	// The ROM wasn't found in the database.
	return -1;
}


/**
 * setEEPRomType(): Set the EEPRom type.
 * @param type EEPRom type. (Specify a negative number to clear)
 * @return 0 on success; non-zero on error.
 */
int EEPRom::setEEPRomType(int type)
{
	if (type < 0)
	{
		// Negative type ID. Clear the EEPRom type.
		memset(&m_eprType, 0x00, sizeof(m_eprType));
		return 0;
	}
	else if (type >= ((sizeof(ms_Database)/sizeof(ms_Database[0]))))
	{
		// Type ID is out of range.
		return 1;
	}
	
	// Set the EEPRom type.
	memcpy(&m_eprType, &ms_Database[type], sizeof(m_eprType));
	return 0;
}


/**
 * readByte(): Read the specified port. (byte-wide)
 * @param address Address.
 * @return Port value.
 */
uint8_t EEPRom::readByte(uint32_t address)
{
	if (address != m_eprType.type.sda_out_adr)
	{
		// Wrong address.
		return 0xFF;
	}
	
	uint8_t sda_out = m_sda;
	switch (m_state)
	{
		case EEP_READ_DATA:
			// Read data.
			if (m_counter < 9)
			{
				// Return DATA bits. (Maximum of 8 KB)
				uint16_t eeprom_address = ((m_slave_mask | m_word_address) & EEPROM_ADDRESS_MASK);
				sda_out = ((m_eeprom[eeprom_address] >> (8 - m_counter)) & 1);
				
				if (m_counter == 8)
				{
					// Word Address is incremented.
					// NOTE: Word address wraps around at array size.
					m_word_address++;
					m_word_address &= m_eprType.type.size_mask;
				}
			}
			break;
		
		case EEP_GET_WORD_ADR_7BITS:
		case EEP_GET_SLAVE_ADR:
		case EEP_GET_WORD_ADR_HIGH:
		case EEP_GET_WORD_ADR_LOW:
		case EEP_WRITE_DATA:
			if (m_counter == 9)
				sda_out = 0;
			break;
		
		default:
			break;
	}
	
	// Return the data.
	return (sda_out << m_eprType.type.sda_out_bit);
}


/**
 * readWord(): Read the specified port. (word-wide)
 * @param address Address.
 * @return Port value.
 */
uint16_t EEPRom::readWord(uint32_t address)
{
	// TODO: Verify that the address is valid.
	
	uint8_t sda_out = m_sda;
	switch (m_state)
	{
		case EEP_READ_DATA:
			// Read data.
			if (m_counter < 9)
			{
				// Return DATA bits. (Maximum of 8 KB)
				uint16_t eeprom_address = ((m_slave_mask | m_word_address) & EEPROM_ADDRESS_MASK);
				sda_out = ((m_eeprom[eeprom_address] >> (8 - m_counter)) & 1);
				
				if (m_counter == 8)
				{
					// Word Address is incremented.
					// NOTE: Word address wraps around at array size.
					m_word_address++;
					m_word_address &= m_eprType.type.size_mask;
				}
			}
			break;
		
		case EEP_GET_WORD_ADR_7BITS:
		case EEP_GET_SLAVE_ADR:
		case EEP_GET_WORD_ADR_HIGH:
		case EEP_GET_WORD_ADR_LOW:
		case EEP_WRITE_DATA:
			if (m_counter == 9)
				sda_out = 0;
			break;
		
		default:
			break;
	}
	
	// Return the data.
	if (m_eprType.type.sda_out_adr & 1)
		return (sda_out << m_eprType.type.sda_out_bit); /* LSB */
	else
		return (sda_out << (m_eprType.type.sda_out_bit + 8)); /* MSB */
}


/**
 * writeByte(): Write to the specified port. (byte-wide)
 * @param address Address.
 * @param data Data.
 */
void EEPRom::writeByte(uint32_t address, uint8_t data)
{
	if (address != m_eprType.type.scl_adr &&
	    address != m_eprType.type.sda_in_adr)
	{
		// Invalid address.
		return;
	}
	
	// Check if this is the clock line. (/SCL)
	if (address == m_eprType.type.scl_adr)
		m_scl = !!(data & (1 << m_eprType.type.scl_bit));
	else
		m_scl = m_old_scl;
	
	// Check if this is the data line. (/SDA)
	if (address == m_eprType.type.sda_in_adr)
		m_sda = !!(data & (1 << m_eprType.type.sda_in_bit));
	else
		m_sda = m_old_sda;
	
	// Process the write command.
	processWriteCmd();
}


/**
 * writeWord(): Write to the specified port. (word-wide)
 * @param address Address.
 * @param data Data.
 */
void EEPRom::writeWord(uint32_t address, uint16_t data)
{
	// TODO: Verify that the address is valid.
	
	// Mask off the address LSB.
	address &= ~1;
	
	// Check if this is the clock line. (/SCL)
	if (address == m_eprType.type.scl_adr)
		m_scl = !!(data & (1 << (m_eprType.type.scl_bit + 8)));
	else if ((address | 1) == m_eprType.type.scl_adr)
		m_scl = !!(data & (1 << m_eprType.type.scl_bit));
	else
		m_scl = m_old_scl;
	
	// Check if this is the data line. (/SDA)
	if (address == m_eprType.type.sda_in_adr)
		m_sda = !!(data & (1 << (m_eprType.type.sda_in_bit + 8)));
	else if ((address | 1) == m_eprType.type.sda_in_adr)
		m_sda = !!(data & (1 << m_eprType.type.sda_in_bit));
	else
		m_sda = m_old_sda;
	
	// Process the write command.
	processWriteCmd();
}


/**
 * processWriteCmd(): Process a port write command.
 */
void EEPRom::processWriteCmd(void)
{
	switch (m_state)
	{
		case EEP_STANDBY:
			// Standby. Wait for START or STOP.
			checkStart();
			checkStop();
			break;
		
		case EEP_WAIT_STOP:
			// Suspended due to missing ACK.
			// Wait for STOP.
			checkStop();
			break;
		
		case EEP_GET_WORD_ADR_7BITS:
			/**
			 * EEP_GET_WORD_ADR_7BITS: Get the 7-bit WORD ADDRESS.
			 * MODE 1: 24C01 only.
			 * Contains 7-bit address and R/W bit.
			 */
			checkStart();
			checkStop();
			
			// Wait for /SCL rising edge before starting the counter.
			if (!m_old_scl && m_scl)
			{
				if (m_counter == 0)
					m_counter++;
			}
			
			// Wait for /SCL falling edge before reading the data bit.
			if (m_old_scl && !m_scl)
			{
				// Data bit.
				// Mode 1 has the following format: (MSB first)
				// [A6 A5 A4 A3 A2 A1 A0 RW]
				if (m_counter < 8)
				{
					m_word_address |= (m_old_sda << (7 - m_counter));
				}
				else if (m_counter == 8)
				{
					m_rw = m_old_sda;
				}
				else
				{
					// ACK cycle.
					m_counter = 0;
					m_word_address &= m_eprType.type.size_mask;
					m_state = (m_rw ? EEP_READ_DATA : EEP_WRITE_DATA);
				}
				
				// Increment the counter.
				m_counter++;
			}
			
			break;
		
		case EEP_GET_SLAVE_ADR:
			/**
			 * EEP_GET_SLAVE_ADR: Get the slave device address or word address MSB.
			 * MODE2 and MODE3 only.
			 * 24C01-24C512: 0-3 bit device address, based on chip size.
			 * 24C04-24C16 (MODE2): 0-3 bit word address MSB, based on chip size.
			 */
			checkStart();
			checkStop();
			
			// Wait for /SCL rising edge before starting the counter.
			if (!m_old_scl && m_scl)
			{
				if (m_counter == 0)
					m_counter++;
			}
			
			// Wait for /SCL falling edge before reading the data bit.
			if (m_old_scl && !m_scl)
			{
				// Data bit.
				// MODE2 has the following format: (MSB first)
				// Word 1: [x x x x DEV2 DEV1 DEV0 RW]
				// Word 2: [A7 A6 A5 A4 A3 A2 A1 A0]
				// (A7 is ignored for 24C01 in MODE2.)
				// NOTE: Only Word 1 is sent for READ operationsin Modes 2 and 3.
				if (m_counter >= 5 && m_counter < 8)
				{
					if ((m_eprType.type.address_bits == 16) ||
					    (m_eprType.type.size_mask < (1 << (15 - m_counter))))
					{
						// SLAVE ADDRESS bit.
						m_slave_mask |= (m_old_sda << (7 - m_counter));
					}
					else
					{
						// WORD ADDRESS high bit.
						if (m_old_sda)
							m_word_address |= (1 << (15 - m_counter));
						else
							m_word_address &= ~(1 << (15 - m_counter));
					}
				}
				else if (m_counter == 8)
				{
					// R/W bit.
					m_rw = m_old_sda;
				}
				else if (m_counter > 8)
				{
					// ACK cycle.
					m_counter = 0;
					
					if (m_eprType.type.address_bits == 16)
					{
						// MODE3: Two ADDRESS bytes.
						m_state = (m_rw ? EEP_READ_DATA : EEP_GET_WORD_ADR_HIGH);
						m_slave_mask <<= 16;
					}
					else
					{
						// MODE2: One ADDRESS byte.
						m_state = (m_rw ? EEP_READ_DATA : EEP_GET_WORD_ADR_LOW);
						m_slave_mask <<= 8;
					}
				}
				
				// Increment the counter.
				m_counter++;
			}
			
			break;
		
		case EEP_GET_WORD_ADR_HIGH:
			/**
			 * EEP_GET_WORD_ADR_HIGH: Get the word address high byte.
			 * MODE3 only. (24C32 - 24C512)
			 */
			checkStart();
			checkStop();
			
			// Wait for /SCL falling edge before reading the data bit.
			if (m_old_scl && !m_scl)
			{
				// Data bit.
				if (m_counter < 9)
				{
					if ((m_eprType.type.size_mask + 1) < (1 << (17 - m_counter)))
					{
						// Ignored bit: slave mask should be right-shifted by one.
						m_slave_mask >>= 1;
					}
					else
					{
						// WORD ADDRESS high bit.
						if (m_old_sda)
							m_word_address |= (1 << (16 - m_counter));
						else
							m_word_address &= ~(1 << (16 - m_counter));
					}
					
					// Increment the counter.
					m_counter++;
				}
				else
				{
					// ACK cycle.
					m_counter = 1;
					m_state = EEP_GET_WORD_ADR_LOW;
				}
			}
			
			break;
		
		case EEP_GET_WORD_ADR_LOW:
			/**
			 * EEP_GET_WORD_ADR_LOW: Get the word address low byte.
			 * 24C01: MODE1: 7-bit address word.
			 * 24C01-24C512: MODE2/MODE3: Word address low byte.
			 */
			checkStart();
			checkStop();
			
			// Wait for /SCL falling edge before reading the data bit.
			if (m_old_scl && !m_scl)
			{
				// Data bit.
				if (m_counter < 9)
				{
					if ((m_eprType.type.size_mask + 1) < (1 << (9 - m_counter)))
					{
						// Ignored bit (X24C01): slave mask should be right-shifted by one.
						m_slave_mask >>= 1;
					}
					else
					{
						// WORD ADDRESS high bit.
						if (m_old_sda)
							m_word_address |= (1 << (8 - m_counter));
						else
							m_word_address &= ~(1 << (8 - m_counter));
					}
					
					// Increment the counter.
					m_counter++;
				}
				else
				{
					// ACK cycle.
					m_counter = 1;
					m_word_address &= m_eprType.type.size_mask;
					m_state = EEP_WRITE_DATA;
				}
			}
			
			break;
		
		case EEP_READ_DATA:
			// Read cycle.
			checkStart();
			checkStop();

			// Wait for /SCL falling edge.
			if (m_old_scl && !m_scl)
			{
				if (m_counter < 9)
					m_counter++;
				else
				{
					m_counter = 1;
					
					if (m_old_sda)
					{
						// ACK not received!
						// Wait for a STOP condition.
						m_state = EEP_WAIT_STOP;
					}
				}
			}
			break;
		
		case EEP_WRITE_DATA:
			// Write cycle.
			checkStart();
			checkStop();
			
			// Wait for /SCL falling edge.
			if (m_old_scl && !m_scl)
			{
				if (m_counter < 9)
				{
					// Write DATA bits. (Maximum of 8 KB)
					uint16_t eeprom_address = ((m_slave_mask | m_word_address) & EEPROM_ADDRESS_MASK);
					if (m_old_sda)
						m_eeprom[eeprom_address] |= (1 << (8 - m_counter));
					else
						m_eeprom[eeprom_address] &= ~(1 << (8 - m_counter));
					
					// Set the dirty flag.
					// TODO: Only if the bit was actually modified?
					m_dirty = true;
					
					if (m_counter == 8)
					{
						// Word Address is incremented.
						// NOTE: Word address wraps around at pagesize.
						m_word_address = (m_word_address & (0xFFFF - m_eprType.type.pagewrite_mask)) |
								 ((m_word_address + 1) & m_eprType.type.pagewrite_mask);
					}
					
					// Increment the counter.
					m_counter++;
				}
				else
				{
					// ACK cycle.
					m_counter = 1;
				}
			}
			
			break;
	}
	
	// Save the clock and data line variables.
	m_old_scl = m_scl;
	m_old_sda = m_sda;
}


/**
 * checkStart(): Check for a START condition. (/SCL = 1, /SDA = rising edge)
 */
void EEPRom::checkStart(void)
{
	if (!m_old_scl || !m_scl)
	{
		// Clock is either low or has made a transition.
		return;
	}
	
	if (m_old_sda && !m_sda)
	{
		// Data In line has a falling edge transition.
		m_counter = 0;
		m_slave_mask = 0;
		
		if (m_eprType.type.address_bits == 7)
		{
			// MODE1. (7-bit)
			m_word_address = 0;
			m_state = EEP_GET_WORD_ADR_7BITS;
		}
		else
		{
			// MODE2 or MODE3.
			m_state = EEP_GET_SLAVE_ADR;
		}
	}
}


/**
 * checkStop(): Check for a STOP condition. (/SCL = 1, /SDA = falling edge)
 */
void EEPRom::checkStop(void)
{
	if (!m_old_scl || !m_scl)
	{
		// Clock is either low or has made a transition.
		return;
	}
	
	if (!m_old_sda && m_sda)
	{
		// Data In line has a rising edge transition.
		m_state = EEP_STANDBY;
	}
}

}
