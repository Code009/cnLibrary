#include "DataHash.h"

using namespace cnLibrary;
using namespace cnRTL;

//---------------------------------------------------------------------------
uInt8 cnRTL::CRC8_LT(const void *Buffer,uIntn Size,const uInt8 *Table,uInt8 Accum)noexcept(true)
{ 
	if(Size==0)
		return 0;

	cnLib_ASSERT(Buffer!=nullptr);
	auto Data=static_cast<const uInt8*>(Buffer);
	for(uIntn i=0;i<Size;i++){
		Accum = Table[Accum^Data[i] ];
	}
	return Accum;
}
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnRTL{
//---------------------------------------------------------------------------
static const uInt8 CRC8_07_Table[256]={
	0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,
	0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,
	0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,
	0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,
	0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,
	0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,
	0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,
	0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,
	0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,
	0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,
	0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,
	0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,
	0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,
	0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,
	0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,
	0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3,
};
//---------------------------------------------------------------------------
static const uInt8 CRC8_1D_Table[256]={
	0x00,0x1D,0x3A,0x27,0x74,0x69,0x4E,0x53,0xE8,0xF5,0xD2,0xCF,0x9C,0x81,0xA6,0xBB,
	0xCD,0xD0,0xF7,0xEA,0xB9,0xA4,0x83,0x9E,0x25,0x38,0x1F,0x02,0x51,0x4C,0x6B,0x76,
	0x87,0x9A,0xBD,0xA0,0xF3,0xEE,0xC9,0xD4,0x6F,0x72,0x55,0x48,0x1B,0x06,0x21,0x3C,
	0x4A,0x57,0x70,0x6D,0x3E,0x23,0x04,0x19,0xA2,0xBF,0x98,0x85,0xD6,0xCB,0xEC,0xF1,
	0x13,0x0E,0x29,0x34,0x67,0x7A,0x5D,0x40,0xFB,0xE6,0xC1,0xDC,0x8F,0x92,0xB5,0xA8,
	0xDE,0xC3,0xE4,0xF9,0xAA,0xB7,0x90,0x8D,0x36,0x2B,0x0C,0x11,0x42,0x5F,0x78,0x65,
	0x94,0x89,0xAE,0xB3,0xE0,0xFD,0xDA,0xC7,0x7C,0x61,0x46,0x5B,0x08,0x15,0x32,0x2F,
	0x59,0x44,0x63,0x7E,0x2D,0x30,0x17,0x0A,0xB1,0xAC,0x8B,0x96,0xC5,0xD8,0xFF,0xE2,
	0x26,0x3B,0x1C,0x01,0x52,0x4F,0x68,0x75,0xCE,0xD3,0xF4,0xE9,0xBA,0xA7,0x80,0x9D,
	0xEB,0xF6,0xD1,0xCC,0x9F,0x82,0xA5,0xB8,0x03,0x1E,0x39,0x24,0x77,0x6A,0x4D,0x50,
	0xA1,0xBC,0x9B,0x86,0xD5,0xC8,0xEF,0xF2,0x49,0x54,0x73,0x6E,0x3D,0x20,0x07,0x1A,
	0x6C,0x71,0x56,0x4B,0x18,0x05,0x22,0x3F,0x84,0x99,0xBE,0xA3,0xF0,0xED,0xCA,0xD7,
	0x35,0x28,0x0F,0x12,0x41,0x5C,0x7B,0x66,0xDD,0xC0,0xE7,0xFA,0xA9,0xB4,0x93,0x8E,
	0xF8,0xE5,0xC2,0xDF,0x8C,0x91,0xB6,0xAB,0x10,0x0D,0x2A,0x37,0x64,0x79,0x5E,0x43,
	0xB2,0xAF,0x88,0x95,0xC6,0xDB,0xFC,0xE1,0x5A,0x47,0x60,0x7D,0x2E,0x33,0x14,0x09,
	0x7F,0x62,0x45,0x58,0x0B,0x16,0x31,0x2C,0x97,0x8A,0xAD,0xB0,0xE3,0xFE,0xD9,0xC4,
};
//---------------------------------------------------------------------------
static const uInt8 CRC8R_1D_Table[256]={
	0x00,0x64,0xC8,0xAC,0xE1,0x85,0x29,0x4D,0xB3,0xD7,0x7B,0x1F,0x52,0x36,0x9A,0xFE,
	0x17,0x73,0xDF,0xBB,0xF6,0x92,0x3E,0x5A,0xA4,0xC0,0x6C,0x08,0x45,0x21,0x8D,0xE9,
	0x2E,0x4A,0xE6,0x82,0xCF,0xAB,0x07,0x63,0x9D,0xF9,0x55,0x31,0x7C,0x18,0xB4,0xD0,
	0x39,0x5D,0xF1,0x95,0xD8,0xBC,0x10,0x74,0x8A,0xEE,0x42,0x26,0x6B,0x0F,0xA3,0xC7,
	0x5C,0x38,0x94,0xF0,0xBD,0xD9,0x75,0x11,0xEF,0x8B,0x27,0x43,0x0E,0x6A,0xC6,0xA2,
	0x4B,0x2F,0x83,0xE7,0xAA,0xCE,0x62,0x06,0xF8,0x9C,0x30,0x54,0x19,0x7D,0xD1,0xB5,
	0x72,0x16,0xBA,0xDE,0x93,0xF7,0x5B,0x3F,0xC1,0xA5,0x09,0x6D,0x20,0x44,0xE8,0x8C,
	0x65,0x01,0xAD,0xC9,0x84,0xE0,0x4C,0x28,0xD6,0xB2,0x1E,0x7A,0x37,0x53,0xFF,0x9B,
	0xB8,0xDC,0x70,0x14,0x59,0x3D,0x91,0xF5,0x0B,0x6F,0xC3,0xA7,0xEA,0x8E,0x22,0x46,
	0xAF,0xCB,0x67,0x03,0x4E,0x2A,0x86,0xE2,0x1C,0x78,0xD4,0xB0,0xFD,0x99,0x35,0x51,
	0x96,0xF2,0x5E,0x3A,0x77,0x13,0xBF,0xDB,0x25,0x41,0xED,0x89,0xC4,0xA0,0x0C,0x68,
	0x81,0xE5,0x49,0x2D,0x60,0x04,0xA8,0xCC,0x32,0x56,0xFA,0x9E,0xD3,0xB7,0x1B,0x7F,
	0xE4,0x80,0x2C,0x48,0x05,0x61,0xCD,0xA9,0x57,0x33,0x9F,0xFB,0xB6,0xD2,0x7E,0x1A,
	0xF3,0x97,0x3B,0x5F,0x12,0x76,0xDA,0xBE,0x40,0x24,0x88,0xEC,0xA1,0xC5,0x69,0x0D,
	0xCA,0xAE,0x02,0x66,0x2B,0x4F,0xE3,0x87,0x79,0x1D,0xB1,0xD5,0x98,0xFC,0x50,0x34,
	0xDD,0xB9,0x15,0x71,0x3C,0x58,0xF4,0x90,0x6E,0x0A,0xA6,0xC2,0x8F,0xEB,0x47,0x23,
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
}	// namespace cnLibrary
//---------------------------------------------------------------------------
uInt8 cnRTL::CRC8_07(const void *Buffer,uIntn Size)noexcept(true)
{
	return CRC_LT(Buffer,Size,CRC8_07_Table,0);
}
//---------------------------------------------------------------------------
uInt8 cnRTL::CRC8_ICODE(const void *Buffer,uIntn Size)noexcept(true)
{
	return CRC_LT(Buffer,Size,CRC8_1D_Table,0xFD);
}
//---------------------------------------------------------------------------
uInt8 cnRTL::CRC8_EBU(const void *Buffer,uIntn Size)noexcept(true)
{
	return CRCReverse_LT(Buffer,Size,CRC8R_1D_Table,0xFF);
}
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnRTL{
//---------------------------------------------------------------------------
static const uInt16 CRC16_8005_Table[256]={
	0x0000,0x8005,0x800F,0x000A,0x801B,0x001E,0x0014,0x8011,0x8033,0x0036,0x003C,0x8039,0x0028,0x802D,0x8027,0x0022,
	0x8063,0x0066,0x006C,0x8069,0x0078,0x807D,0x8077,0x0072,0x0050,0x8055,0x805F,0x005A,0x804B,0x004E,0x0044,0x8041,
	0x80C3,0x00C6,0x00CC,0x80C9,0x00D8,0x80DD,0x80D7,0x00D2,0x00F0,0x80F5,0x80FF,0x00FA,0x80EB,0x00EE,0x00E4,0x80E1,
	0x00A0,0x80A5,0x80AF,0x00AA,0x80BB,0x00BE,0x00B4,0x80B1,0x8093,0x0096,0x009C,0x8099,0x0088,0x808D,0x8087,0x0082,
	0x8183,0x0186,0x018C,0x8189,0x0198,0x819D,0x8197,0x0192,0x01B0,0x81B5,0x81BF,0x01BA,0x81AB,0x01AE,0x01A4,0x81A1,
	0x01E0,0x81E5,0x81EF,0x01EA,0x81FB,0x01FE,0x01F4,0x81F1,0x81D3,0x01D6,0x01DC,0x81D9,0x01C8,0x81CD,0x81C7,0x01C2,
	0x0140,0x8145,0x814F,0x014A,0x815B,0x015E,0x0154,0x8151,0x8173,0x0176,0x017C,0x8179,0x0168,0x816D,0x8167,0x0162,
	0x8123,0x0126,0x012C,0x8129,0x0138,0x813D,0x8137,0x0132,0x0110,0x8115,0x811F,0x011A,0x810B,0x010E,0x0104,0x8101,
	0x8303,0x0306,0x030C,0x8309,0x0318,0x831D,0x8317,0x0312,0x0330,0x8335,0x833F,0x033A,0x832B,0x032E,0x0324,0x8321,
	0x0360,0x8365,0x836F,0x036A,0x837B,0x037E,0x0374,0x8371,0x8353,0x0356,0x035C,0x8359,0x0348,0x834D,0x8347,0x0342,
	0x03C0,0x83C5,0x83CF,0x03CA,0x83DB,0x03DE,0x03D4,0x83D1,0x83F3,0x03F6,0x03FC,0x83F9,0x03E8,0x83ED,0x83E7,0x03E2,
	0x83A3,0x03A6,0x03AC,0x83A9,0x03B8,0x83BD,0x83B7,0x03B2,0x0390,0x8395,0x839F,0x039A,0x838B,0x038E,0x0384,0x8381,
	0x0280,0x8285,0x828F,0x028A,0x829B,0x029E,0x0294,0x8291,0x82B3,0x02B6,0x02BC,0x82B9,0x02A8,0x82AD,0x82A7,0x02A2,
	0x82E3,0x02E6,0x02EC,0x82E9,0x02F8,0x82FD,0x82F7,0x02F2,0x02D0,0x82D5,0x82DF,0x02DA,0x82CB,0x02CE,0x02C4,0x82C1,
	0x8243,0x0246,0x024C,0x8249,0x0258,0x825D,0x8257,0x0252,0x0270,0x8275,0x827F,0x027A,0x826B,0x026E,0x0264,0x8261,
	0x0220,0x8225,0x822F,0x022A,0x823B,0x023E,0x0234,0x8231,0x8213,0x0216,0x021C,0x8219,0x0208,0x820D,0x8207,0x0202,
};
//---------------------------------------------------------------------------
static const uInt16 CRC16R_8005_Table[256]={
	0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
	0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
	0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
	0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
	0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
	0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
	0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
	0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
	0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
	0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
	0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
	0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
	0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
	0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
	0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
	0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040,
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
}	// namespace cnLibrary
//---------------------------------------------------------------------------
uInt16 cnRTL::CRC16_ARC(const void *Buffer,uIntn Size)noexcept(true)
{
	return CRCReverse_LT(Buffer,Size,CRC16R_8005_Table,0);
}
//---------------------------------------------------------------------------
uInt16 cnRTL::CRC16_BUYPASS(const void *Buffer,uIntn Size)noexcept(true)
{ 
	return CRC_LT(Buffer,Size,CRC16_8005_Table,0);
}
//---------------------------------------------------------------------------
namespace cnLibrary{
namespace cnRTL{
//---------------------------------------------------------------------------
static const uInt32 CRC32_04C11DB7_Table[256]={
	0x00000000,0x04C11DB7,0x09823B6E,0x0D4326D9,0x130476DC,0x17C56B6B,0x1A864DB2,0x1E475005,
	0x2608EDB8,0x22C9F00F,0x2F8AD6D6,0x2B4BCB61,0x350C9B64,0x31CD86D3,0x3C8EA00A,0x384FBDBD,
	0x4C11DB70,0x48D0C6C7,0x4593E01E,0x4152FDA9,0x5F15ADAC,0x5BD4B01B,0x569796C2,0x52568B75,
	0x6A1936C8,0x6ED82B7F,0x639B0DA6,0x675A1011,0x791D4014,0x7DDC5DA3,0x709F7B7A,0x745E66CD,
	0x9823B6E0,0x9CE2AB57,0x91A18D8E,0x95609039,0x8B27C03C,0x8FE6DD8B,0x82A5FB52,0x8664E6E5,
	0xBE2B5B58,0xBAEA46EF,0xB7A96036,0xB3687D81,0xAD2F2D84,0xA9EE3033,0xA4AD16EA,0xA06C0B5D,
	0xD4326D90,0xD0F37027,0xDDB056FE,0xD9714B49,0xC7361B4C,0xC3F706FB,0xCEB42022,0xCA753D95,
	0xF23A8028,0xF6FB9D9F,0xFBB8BB46,0xFF79A6F1,0xE13EF6F4,0xE5FFEB43,0xE8BCCD9A,0xEC7DD02D,
	0x34867077,0x30476DC0,0x3D044B19,0x39C556AE,0x278206AB,0x23431B1C,0x2E003DC5,0x2AC12072,
	0x128E9DCF,0x164F8078,0x1B0CA6A1,0x1FCDBB16,0x018AEB13,0x054BF6A4,0x0808D07D,0x0CC9CDCA,
	0x7897AB07,0x7C56B6B0,0x71159069,0x75D48DDE,0x6B93DDDB,0x6F52C06C,0x6211E6B5,0x66D0FB02,
	0x5E9F46BF,0x5A5E5B08,0x571D7DD1,0x53DC6066,0x4D9B3063,0x495A2DD4,0x44190B0D,0x40D816BA,
	0xACA5C697,0xA864DB20,0xA527FDF9,0xA1E6E04E,0xBFA1B04B,0xBB60ADFC,0xB6238B25,0xB2E29692,
	0x8AAD2B2F,0x8E6C3698,0x832F1041,0x87EE0DF6,0x99A95DF3,0x9D684044,0x902B669D,0x94EA7B2A,
	0xE0B41DE7,0xE4750050,0xE9362689,0xEDF73B3E,0xF3B06B3B,0xF771768C,0xFA325055,0xFEF34DE2,
	0xC6BCF05F,0xC27DEDE8,0xCF3ECB31,0xCBFFD686,0xD5B88683,0xD1799B34,0xDC3ABDED,0xD8FBA05A,
	0x690CE0EE,0x6DCDFD59,0x608EDB80,0x644FC637,0x7A089632,0x7EC98B85,0x738AAD5C,0x774BB0EB,
	0x4F040D56,0x4BC510E1,0x46863638,0x42472B8F,0x5C007B8A,0x58C1663D,0x558240E4,0x51435D53,
	0x251D3B9E,0x21DC2629,0x2C9F00F0,0x285E1D47,0x36194D42,0x32D850F5,0x3F9B762C,0x3B5A6B9B,
	0x0315D626,0x07D4CB91,0x0A97ED48,0x0E56F0FF,0x1011A0FA,0x14D0BD4D,0x19939B94,0x1D528623,
	0xF12F560E,0xF5EE4BB9,0xF8AD6D60,0xFC6C70D7,0xE22B20D2,0xE6EA3D65,0xEBA91BBC,0xEF68060B,
	0xD727BBB6,0xD3E6A601,0xDEA580D8,0xDA649D6F,0xC423CD6A,0xC0E2D0DD,0xCDA1F604,0xC960EBB3,
	0xBD3E8D7E,0xB9FF90C9,0xB4BCB610,0xB07DABA7,0xAE3AFBA2,0xAAFBE615,0xA7B8C0CC,0xA379DD7B,
	0x9B3660C6,0x9FF77D71,0x92B45BA8,0x9675461F,0x8832161A,0x8CF30BAD,0x81B02D74,0x857130C3,
	0x5D8A9099,0x594B8D2E,0x5408ABF7,0x50C9B640,0x4E8EE645,0x4A4FFBF2,0x470CDD2B,0x43CDC09C,
	0x7B827D21,0x7F436096,0x7200464F,0x76C15BF8,0x68860BFD,0x6C47164A,0x61043093,0x65C52D24,
	0x119B4BE9,0x155A565E,0x18197087,0x1CD86D30,0x029F3D35,0x065E2082,0x0B1D065B,0x0FDC1BEC,
	0x3793A651,0x3352BBE6,0x3E119D3F,0x3AD08088,0x2497D08D,0x2056CD3A,0x2D15EBE3,0x29D4F654,
	0xC5A92679,0xC1683BCE,0xCC2B1D17,0xC8EA00A0,0xD6AD50A5,0xD26C4D12,0xDF2F6BCB,0xDBEE767C,
	0xE3A1CBC1,0xE760D676,0xEA23F0AF,0xEEE2ED18,0xF0A5BD1D,0xF464A0AA,0xF9278673,0xFDE69BC4,
	0x89B8FD09,0x8D79E0BE,0x803AC667,0x84FBDBD0,0x9ABC8BD5,0x9E7D9662,0x933EB0BB,0x97FFAD0C,
	0xAFB010B1,0xAB710D06,0xA6322BDF,0xA2F33668,0xBCB4666D,0xB8757BDA,0xB5365D03,0xB1F740B4,
};
//---------------------------------------------------------------------------
static const uInt32 CRC32R_04C11DB7_Table[256]={
	0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,
	0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
	0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,
	0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
	0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,
	0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
	0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,
	0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
	0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,
	0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
	0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,
	0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
	0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,
	0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
	0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,
	0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
	0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,
	0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
	0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,
	0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
	0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,
	0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
	0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,
	0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
	0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,
	0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
	0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,
	0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
	0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,
	0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
	0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,
	0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D,
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
}	// namespace cnLibrary
//---------------------------------------------------------------------------
uInt32 cnRTL::CRC32_04C11DB7(const void *Buffer,uIntn Size)noexcept(true)
{
	uInt32 c=CRCReverse_LT(Buffer,Size,CRC32R_04C11DB7_Table,0xFFFFFFFF);
	return c^0xFFFFFFFF;
}
//---------------------------------------------------------------------------
uInt32 cnRTL::CRC32_POSIX(const void *Buffer,uIntn Size)noexcept(true)
{
	uInt32 c=CRC_LT(Buffer,Size,CRC32_04C11DB7_Table,0);
	return c^0xFFFFFFFF;
}
//---------------------------------------------------------------------------
