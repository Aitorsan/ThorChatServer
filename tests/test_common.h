#include <iostream>
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>


#define CONCAT_STR (A,B) A ## B

#define ASSERT_STD_STR(str1,str2,message) \
   if( str1 != str2 ){\
       assert(!message);}
      

