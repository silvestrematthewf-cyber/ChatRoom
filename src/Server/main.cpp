#include <iostream>
#include <string>
#include "clientSession.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


class Server
{
    public:
        Server(boost::asio::io_context& io) : acceptor(io, tcp::endpoint(tcp::v4(), 8080))
        {
            acceptConnection();//start accepting connections
        }

        void acceptConnection()
        {
            acceptor.async_accept(
                [&](boost::system::error_code error, tcp::socket socket)
                {
                    if (!error)
                    {
                        //create session for the connection
                        session = std::make_shared<ClientSession>(std::move(socket));
                        session->start();
                    }
                    acceptConnection();
                }
            );
        }

    private:
        tcp::acceptor acceptor;
        int active_connections = 0;
        std::shared_ptr<ClientSession> session;
};


void main()
{
    try
    {
        //create the asio context
        boost::asio::io_context io;
        std::cout << "Server Listening on port 8080\n";
        Server s(io);
        io.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}