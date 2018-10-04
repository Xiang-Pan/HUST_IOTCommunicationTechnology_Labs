/* FileName:server_http.hpp
 * Author:Hover
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description:server_http
 */

#ifndef SERVER_HTTP_HPP
#define	SERVER_HTTP_HPP

#include "server_base.hpp"

namespace MyWeb 
{
    typedef boost::asio::ip::tcp::socket HTTP;
    template<>
    class Server<HTTP> : public ServerBase<HTTP> 
    {
    public:
        //with conf file for init
        Server(unsigned short port, size_t num_threads=1) :
            ServerBase<HTTP>::ServerBase(port, num_threads) {};
    private:
        // the accept implemantation
        void accept() 
        {
            // new socket
            auto socket = std::make_shared<HTTP>(m_io_service);
            acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& ec) 
            {
                accept();
                if(!ec) //error
                {
                    process_request_and_respond(socket);
                }
            });
        }
    };
}
#endif	/* SERVER_HTTP_HPP */
