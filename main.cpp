#include <iostream>

#include "./CRC_Template.hpp"

int main() {

//  Create a pointer to the value you want the CRC of
    const char* x = "example";

/*  Create the CRC with parameters
        Size            Polynomial          Initial Value       RefIn  RefOut XorOut              Name    */
    CRC<uint64_t> crc64(0x42f0e1eba9ea3693, 0xFFFFFFFFFFFFFFFF, true,  true,  0xFFFFFFFFFFFFFFFF, "CRC64/XZ");
    CRC<uint32_t> crc32(0x04c11db7,         0xFFFFFFFF,         true,  true,  0xFFFFFFFF,         "CRC32");
    CRC<uint16_t> crc16(0x1021,             0xFFFF,             false, false, 0x0000,             "CRC-16/CCITT-FALSE");
    CRC<uint8_t>  crc8 (0x07,               0x00,               false, false, 0x00,               "CRC-8");

/*  Using std::hex to view hex value of the crc
    crc.calculate(value, length in bytes)   */
    std::cout << std::hex << crc64.calculate(x,7) << std::endl;
    std::cout << std::hex << crc32.table_calc(x,7) << std::endl;

    return 0;
}