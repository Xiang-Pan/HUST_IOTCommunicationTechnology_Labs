/* FileName:server_http_runner.cpp
 * Author:Hover
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description:server_http_runner
 */


#include "server_http.hpp"
#include "handler.hpp"

using namespace MyWeb;

int main() 
{
    //port: 12345 
    //thread num:4
    Server<HTTP> server(12345, 4);
    start_server<Server<HTTP>>(server);
    return 0;
}
