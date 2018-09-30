/* FileName:server_https.hpp
 * Author:Hover
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description:server_https
 */

#ifndef SERVER_HTTPS_HPP
#define	SERVER_HTTPS_HPP

#include "server_http.hpp"
#include <boost/asio/ssl.hpp>

namespace MyWeb 
{
    // HTTPS type
    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> HTTPS;

    // HTTPS templeate 
    template<>
    class Server<HTTPS> : public ServerBase<HTTPS>
    {
    public:
        // certificate file
        // private_key file
        Server(unsigned short port, 
               size_t num_threads,
               const std::string& cert_file, 
               const std::string& private_key_file) :
        ServerBase<HTTPS>::ServerBase(port, num_threads),
        context(boost::asio::ssl::context::sslv23) 
        {
            context.use_certificate_chain_file(cert_file);
            // file type
            context.use_private_key_file(private_key_file, boost::asio::ssl::context::pem);
        }

    private:
        //ssl context
        boost::asio::ssl::context context;

        // IO stream (ssl context) encryption
        void accept() 
        {
            auto socket = std::make_shared<HTTPS>(m_io_service, context);

            acceptor.async_accept
            (
                (*socket).lowest_layer(),
                [this, socket](const boost::system::error_code& ec)
                {
                    accept();
                    if(!ec) 
                    {
                        (*socket).async_handshake(boost::asio::ssl::stream_base::server,
                            [this, socket](const boost::system::error_code& ec) {
                            if(!ec) process_request_and_respond(socket);
                        });
                    }
            });
        }
    };
}

#endif	/* SERVER_HTTPS_HPP */
