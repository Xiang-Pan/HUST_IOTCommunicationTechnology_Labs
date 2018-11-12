/* FileName:server_https_runner.cpp
 * Author:Hover
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description:server_https_runner
 */


#include "server_https.hpp"
#include "handler.hpp"
using namespace MyWeb;

int main() 
{
    //port: 12345 
    //thread num:4
    //certificate file: server.crt
    //private_key file: server.key
    Server<HTTPS> server(12345, 4, "server.crt", "server.key");
    start_server<Server<HTTPS>>(server);
    return 0;
}
