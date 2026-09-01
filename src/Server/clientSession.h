#include <boost\asio.hpp>
#include <iostream>
using boost::asio::ip::tcp;
class ClientSession
{
public:
    ClientSession(tcp::socket socket) : socket(std::move(socket))
    {
    }    

    void start()
    {
        readMessage();
    }

private:
    tcp::socket socket;
    boost::asio::streambuf buffer;

    void readMessage()
    {
        boost::asio::async_read_until(
            socket,
            buffer,
            '\n',
            [&](boost::system::error_code error, size_t bytes)
            {
                if (!error)
                {
                    processMessage();
                    readMessage();
                }
            }
        );
    }

    void processMessage()
    {
         std::istream input(& buffer);
        std::string message;
        std::getline(input, message);
        std::cout << "Client sent: " << message << '\n';
        std::string response = "Ack!";
        boost::asio::async_write(socket, boost::asio::buffer(response));
    }

};