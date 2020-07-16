#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <bitset>
#include <array>

constexpr std::array<unsigned char, 64> encoding_table =
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
     'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
     'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
     'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
     'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
     'w', 'x', 'y', 'z', '0', '1', '2', '3',
     '4', '5', '6', '7', '8', '9', '+', '/'};

std::string base64(std::string &data)
{

    std::string base64data{0};

    for (int i = 0; i < data.size(); i += 3)
    {
        std::vector<unsigned char> chars{};

        for (int j = i; j < i + 3; ++j)
        {
            if (j < data.size())
                chars.push_back(data[j]);
        }

        int charsSize = chars.size();

        char first{}, second{}, third{}, fourth{};

        //check whether we have 3 chars
        if (charsSize == 3)
        {
            first = encoding_table[(chars[0] >> 2)];
            second = encoding_table[((chars[0] & 0x3) << 4) | ((chars[1] & 0xF0) >> 4)];
            third = encoding_table[((chars[1] & 0x0F) << 2) | (chars[2] & 0xC0) >> 6];
            fourth = encoding_table[(chars[2] & 0x3F)];

            base64data += first;
            base64data += second;
            base64data += third;
            base64data += fourth;
        }
        else if (charsSize == 2) //check wether we have 2 chars
        {
            first = encoding_table[chars[0] >> 2];
            second = encoding_table[((chars[0] & 0x3) << 4) | ((chars[1] & 0xF0) >> 4)];
            third = encoding_table[((chars[1] & 0x0F) << 2)]; //we pad zeroes
            // Because we need one more char we put the wilcard character = sing to complete the encoding
            fourth = '=';

            base64data += first;
            base64data += second;
            base64data += third;
            base64data += fourth;
        }
        else if (charsSize == 1) // check whether we have 1 char left
        {
            first = encoding_table[chars[0] >> 2];
            second = encoding_table[((chars[0] & 0x3) << 4)]; //we pad zeroes
            // we ned to add = to the rest of the chars
            third = '=';
            fourth = '=';
            base64data += first;
            base64data += second;
            base64data += third;
            base64data += fourth;
        }
    }

    return base64data;
}