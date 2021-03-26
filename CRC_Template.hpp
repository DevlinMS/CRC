#pragma once

#include <iostream>

template <typename CRC_Size>
struct CRC {
    CRC_Size      POLY;
    CRC_Size      INIT;
    bool          RefIn;
    bool          RefOut;
    CRC_Size      XorOut;
    const char*   Name;
    CRC_Size      Table[256];
    const int size_self = sizeof(CRC_Size) * 8;
    
    CRC(CRC_Size poly, CRC_Size init, bool reflect_in, bool reflect_out, CRC_Size xor_out, const char* name = "crc8_generic") {
        POLY = poly;
        INIT = init;
        RefIn = reflect_in;
        RefOut = reflect_out;
        XorOut = xor_out;
        Name = name;
        generate_table();
        
    }

    void generate_table() {
    // Generataes the values for the CRC value table
        for(int i = 0x00; i <= 0xff; i++) {
            CRC_Size crc = i;
            for (int bit = size_self; bit > 0; bit--) {
                crc = (crc & 0x1UL << size_self - 1) ? (crc << 1) ^ POLY : crc << 1;
            }
            Table[i] = crc;
        }
    }

    CRC_Size table_calc(const void* data, int len, const unsigned char seed = 0) {
    // Uses the CRC table calculate instead of doing all the math
        const unsigned char* buffer = (unsigned char*) data;

    /*  if this is is only a part of a larger data stream,
        the crc of the last iteration is used as the seed for the next iteration
        instead of the INIT value */ 
        CRC_Size crc = (seed == 0) ? INIT : seed;

    /*  Table only goes up to 256 entries
        So we xor the current buffer value with the first two bytes of the current crc
        that is used to get the table value, which is than xored with the last two bytes of the crc
        resulting in the 16bit crc value
        crc = xx00 ^ Table[buffer ^ 00xx] */
        do {
            unsigned char bfr = RefIn ? reflect_in(*buffer++) : *buffer++;
            crc = (crc << 8) ^ Table[bfr ^ (crc >> size_self - 8)];
        } while (--len);

        xor_output(crc);
        crc = RefOut ? reflect_out(crc) : crc;
        return crc;
    }

    void print_table() {
        for(int i = 1; i <= 256; i++) {
            std::cout << std::hex << Table[i-1] << " ";
            if( (i % 8 == 0) ) {std::cout<<std::endl;}
        }
        std::cout<<std::endl;
    }

    CRC_Size calculate(const void* data, int len, const unsigned char seed = 0) {

        const unsigned char* buffer = (unsigned char*) data;
        /* if this is is only a part of a larger data stream,
           the crc of the last iteration is used as the seed for the next iteration
           instead of the INIT value */
        CRC_Size crc = (seed == 0) ? INIT : seed;

        while(len--) {
            // XOR the current crc with the buffer, then move to the next byte
            //std::cout << std::bitset<16>(reflect_in(*buffer) << 8) << std::endl;
            crc ^= (RefIn) ? (reflect_in(*buffer++) << size_self - 8) : (*buffer++ << size_self - 8);
            // For every bit in the current crc register, if the MSB is a one, XOR the next 7 bits with the poly
            // Always shift the bits to push out the MSB
            for (int bit = 8; bit > 0; bit--) {
                crc = (crc & 0x1UL << size_self - 1) ? (crc << 1) ^ POLY : crc << 1;
            }
        }
        
        xor_output(crc);
        crc = RefOut ? reflect_out(crc) : crc;

        return crc;
    }

    CRC_Size reflect_in(unsigned char in) {
//  Reflects the bits of a char (8 bits)
        unsigned char reversed = 0x00;
        for(int i = 0; i < 8; i++) {
            reversed |= ((in>>i) & 1) << (7-i);
        }
        return reversed;
    }

    CRC_Size reflect_out(CRC_Size in) {
/*  Reflects the bits of the CRC
    Will only work with data that has the same size as the CRC
    ie: 1000 0110 --> 0110 0001     */
        CRC_Size reversed = 0x00;
        for(int i = 0; i < size_self; i++) {
            reversed |= ((in>>i) & 1) << ((size_self - 1)-i);
        }
        return reversed;
    }

    void xor_output(CRC_Size& in) {
//  Xor the input value with the XorOut parameter
        in = in ^ XorOut;
    }

    void print_info() {
//  Prints the parameters of the CRC
        std::cout << Name << " has the following prameters:\n";
        std::cout << "Polymonial Value: 0x"    << std::hex << (int)POLY << '\n';
        std::cout << "Initial Value:    0x" << std::hex << (int)INIT << '\n';
        std::cout << "Relfect In: "  << std::boolalpha << RefIn << '\n';
        std::cout << "Reflect Out: " << std::boolalpha << RefOut << '\n';
        std::cout << "XOR Output by: 0x" << std::hex << (int)XorOut << '\n';
        std::cout << "CRC size is: " << size_self << '\n';
        std::cout << std::endl;
    }

    ~CRC() {};
};
