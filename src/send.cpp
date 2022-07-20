#include <iostream>

#include "SimplePocoHandler.h"

int main(void)
{
    SimplePocoHandler handler("localhost", 5672);

    AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");
    AMQP::Channel channel(&connection);

    channel.onReady([&]()
    {
        if(handler.connected())//TODO fix connecting issue
        {
            channel.publish("", "hello", "Hello World!");
            std::cout << " [x] Sent 'Hello World!'" << std::endl;
            handler.quit();
        }
        else {
            std::cout << "not connected!\n";
        }
    });

    handler.loop();
    return 0;
}
