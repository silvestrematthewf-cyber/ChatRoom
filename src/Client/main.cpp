#include <iostream>
#include <boost\asio.hpp>

int main()
{
    boost::asio::io_context io;

    boost::asio::ip::tcp::socket socket(io);

    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::make_address("127.0.0.1"), 
        8080
    );

    socket.connect(endpoint); //CONNECT to the server here

    std::cout << "Connected to server!\n";

    std::cout << ">... ";
    std::string message;

    std::getline(std::cin, message);
    
    message += '\n';

    boost::asio::write(socket, boost::asio::buffer(message));

    char buffer[1024];

    std::size_t length = socket.read_some(boost::asio::buffer(buffer));

    std::cout << "Server said: "
            << std::string(buffer, length)
            << '\n';
    return 0;
}