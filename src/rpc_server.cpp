#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

#include "SimplePocoHandler.h"

int fib(int n)
{
    switch (n)
    {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return fib(n - 1) + fib(n - 2);
    }
}

int main(void)
{
    SimplePocoHandler handler("localhost", 5672);

    AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");

    AMQP::Channel channel(&connection);
    channel.setQos(1);

    channel.declareQueue("rpc_queue");
    channel.consume("").onReceived([&channel](const AMQP::Message &message,
            uint64_t deliveryTag,
            bool redelivered)
    {
        //const auto body = message.message();
        const char* data = message.body();
        int size = message.bodySize();
        std::string body(data, size);
        //std::cout << "data:" << data << "\n";
        //std::cout << "size:" << size << "\n";

        std::cout<<" [.] fib("<< body<<")"<<std::endl;

        // Record start time
        auto start = std::chrono::high_resolution_clock::now();

        std::string msg = std::to_string(fib(std::stoi(body)));

        // Record end time
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Elapsed time: " << 1000 * elapsed.count() << " ms\n";

        AMQP::Envelope env(msg.c_str(), msg.length());
        env.setCorrelationID(message.correlationID());

        channel.publish("", message.replyTo(), env);
        channel.ack(deliveryTag);
    });

    std::cout << " [x] Awaiting RPC requests" << std::endl;
    handler.loop();
    return 0;
}
