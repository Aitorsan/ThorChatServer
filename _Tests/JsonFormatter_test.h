#include "test_common.h"
#include "JsonFormatter.h"




TEST(JASON_MSG_FORMATTER,format_login_server_response_msg)
{
    //we create one per test because it is not an expensive thing and is easier to udnerstand and test
    JsonFormatter jsonFormatter;


     const std::string data ="this is the message";
     const std::string server = "server";


    std::string jsonResult = jsonFormatter.formatData(data,server,MsgType::LOGIN_RESPONSE);
    std::cout << jsonResult;

}

