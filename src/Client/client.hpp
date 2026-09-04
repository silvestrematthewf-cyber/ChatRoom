#include <boost/asio.hpp>
#include <chrono>
#include <deque>
#include <cstdlib>
#include <thread>
#include <algorithm>
using boost::asio::ip::tcp;
class Client
{
    public:
        Client(boost::asio::io_context& io) : endpoint(boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 8080)), socket(io)
        {
            connect();
            start();
        }

        ~Client() //clean up operations
        {
            running.store(false); //set the atomic flag
            messageSenderThread.join();
            messageRecieverThread.join();
            messageUpdaterThread.join();
        }

        void start() //start up the sender and reciever threads
        {
            messageSenderThread = std::thread(&Client::messageSender, this);
            messageRecieverThread = std::thread(&Client::messageReciever, this);
            messageUpdaterThread = std::thread(&Client::messageUpdater, this);
        }

        void connect()
        {
            try 
            {
                socket.connect(endpoint);
                std::cout << "Connected!\n";
            }
            catch (const std::exception& e)
            {
                std::cout << "Failed to Connect Trying again after 2 seconds..." << std::endl;
                std::this_thread ::sleep_for(std::chrono::seconds(2));
                connect();
            }
        }

        void messageSender()
        {
            while (running)
            {
                createMessage();
                sendMessage();
            }
        }

        void messageReciever()
        {
            while (running)
                recieveMessage();
        };

        void createMessage()
        {
            std::string newMessage;
            std::cout << "> ";
            std::getline(std::cin, newMessage);
            message = newMessage;

            if (newMessage == "quit")
            {
                running.store(false);
            }
        };

        void sendMessage()
        {
            message += '\n';
            boost::asio::write(socket, boost::asio::buffer(message));
            messagesRecieved.push_back(message);
            messageRecieved.store(true);
        }

        void recieveMessage()
        {
            char buffer[1024];
            std::size_t length = socket.read_some(boost::asio::buffer(buffer));
            std::string incMessage(buffer, length);
            //messagesRecieved.push_back(incMessage);
            //messageRecieved.store(true);
        }

        void messageUpdater()
        {
            //will update the screen when a message is sent or recieved
            while(running)
            {    
                if (messageRecieved.load())
                {    
                    std::system("cls");
                    //we only want to display the last 6 messages
                    size_t endIndex = std::min(maxMessages, messagesRecieved.size());
                    
                    auto it = messagesRecieved.end() - endIndex;
                    for (int i = 0; i < endIndex; i++)
                    {
                        std::cout << *it;
                        ++it;
                    }
                    std::cout << "> ";
                    messageRecieved.store(false);
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }
            }
        } 

        bool isRunning()
        {
            return running.load();
        }

    private:
        boost::asio::ip::tcp::endpoint endpoint;
        tcp::socket socket;
        std::string message = "";
        char delimiter = '/n';

        std::thread messageSenderThread;
        std::thread messageRecieverThread;
        std::thread messageUpdaterThread;
        std::deque<std::string> messagesRecieved;

        std::atomic<bool> running = true;
        std::atomic<bool> messageRecieved = false;

        const size_t maxMessages = 6;
};