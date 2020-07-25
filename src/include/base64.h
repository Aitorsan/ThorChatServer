#pragma once
#include <string>
#include <vector>
#include <array>
#include <cassert>
namespace
{
    constexpr unsigned short ENCODING_TABLE_SIZE = 64;
    
    constexpr std::array<char, 64> encoding_table =	    
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',	   
     'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
     'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',	    
     'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',	
     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',	
     'o', 'p', 'q', 'r', 's', 't', 'u', 'v',	
     'w', 'x', 'y', 'z', '0', '1', '2', '3',	
     '4', '5', '6', '7', '8', '9', '+', '/'};


    constexpr unsigned short MAX_USHORT = static_cast<unsigned short>(~0u);

    inline std::string base64(const std::string &data)
    {   //be carefull to initialize this to {0} because it counts as a character
        // if we append more letters this char \0 will be ignore but the size of the
        // strings to compare even though have the same letters the size is different
        // and therefore they will be not equal
        std::string base64data{};

        for (std::size_t i = 0; i < data.size(); i += 3)
        {
            std::vector<unsigned char> chars{};

            for (std::size_t j = i; j < i + 3; ++j)
            {
                if (j < data.size())
                    chars.push_back(data[j]);
            }

            int charsSize = chars.size();

            unsigned char first{}, second{}, third{}, fourth{};

            //check whether we have 3 chars
            if (charsSize == 3)
            {
                first = encoding_table[(chars[0] >> 2)];
                second = encoding_table[((chars[0] & 0x3u) << 4) | ((chars[1] & 0xF0u) >> 4)];
                third = encoding_table[((chars[1] & 0x0Fu) << 2) | (chars[2] & 0xC0u) >> 6];
                fourth = encoding_table[(chars[2] & 0x3Fu)];

                base64data += first;
                base64data += second;
                base64data += third;
                base64data += fourth;
            }
            else if (charsSize == 2) //check wether we have 2 chars
            {
                first = encoding_table[chars[0] >> 2];
                second = encoding_table[((chars[0] & 0x3u) << 4) | ((chars[1] & 0xF0) >> 4)];
                third = encoding_table[((chars[1] & 0x0Fu) << 2)]; //we pad zeroes
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

    std::string decodeBase64(const std::string &base64Data)
    {
        std::string originalData{};

        std::size_t dataLength = base64Data.length();

        for (std::size_t i = 0; i < dataLength; i += 4)
        {
            std::vector<unsigned char> chars{};

            for (std::size_t j = i; j < i + 4; ++j)
            {
                if (j < dataLength)
                    chars.push_back( base64Data[j]);
            }

            int charsSize = chars.size();

            assert(charsSize <= 4 && charsSize > 0);

            unsigned char first{}, second{}, third{};
            unsigned short index_first{MAX_USHORT};
            unsigned short index_second{MAX_USHORT};
            unsigned short index_third{MAX_USHORT};
            unsigned short index_fourth{MAX_USHORT};

            // find the chars in the table, note the magic number
            // we coud use a map and avoid O(n) but I want to be more
            // independent and not include std::map
            for (std::size_t k = 0; k < ENCODING_TABLE_SIZE; ++k)
            {
                if (chars[0] == encoding_table[k])
                    index_first = k;
                if (chars[1] == encoding_table[k])
                    index_second = k;
                if (chars[2] == encoding_table[k])
                    index_third = k;
                if (chars[3] == encoding_table[k])
                    index_fourth = k;
            }
            // the indexes are the actual values that we use to restore the original data
            assert(index_second != MAX_USHORT && index_first != MAX_USHORT);
            first = (index_first << 2) | (index_second >> 4);
            
            // we need to check the last 2 indexes because at maximum we had have to padd and add 2 '='
            // at the end of the encoding

            // if 3 char is MAX_USHORT it means we have the situation where 2 equal signs were added
            if (index_third == MAX_USHORT)
            {
                // In this case we just add the first character to the string
                originalData += first;
            }
            else if (index_fourth == MAX_USHORT) // then we had just one equal sign extra added
            {
                // in this case we rebuild second, and add the first and second char data
                second = ((index_second & 0x0Fu) << 4) | ((index_third) >> 2);

                originalData += first;
                originalData += second;
            }
            else
            {
                // in this case we rebuild second, and third and add all
                second = ((index_second & 0x0Fu) << 4) | ((index_third) >> 2);
                third = ((index_third & 0x03) << 6) | (index_fourth);

                originalData += first;
                originalData += second;
                originalData += third;
            }
        }
        return originalData;
    }

} // namespace