#include <iostream>
#include <boost\asio.hpp>
#include "client.hpp"
#include <iostream>
using boost::asio::ip::tcp;


int main()
{
    boost::asio::io_context io;

    Client client(std::move(io));
    while (client.isRunning()){}
    return 0;
}