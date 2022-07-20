#include <iostream>

//#include <boost/uuid/uuid.hpp>            // uuid class
//#include <boost/uuid/uuid_generators.hpp> // generators
//#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include "tools.h"
#include "SimplePocoHandler.h"

#include <random>
#include <sstream>
#include <chrono>

namespace uuid_cpp {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
}

int main(int argc, const char* argv[])
{
    //const std::string correlation(uuid());
    //boost::uuids::uuid uuid = boost::uuids::random_generator()();
    //const std::string correlation = boost::uuids::to_string(uuid);
    if (argc < 2) {
        std::cout << "Usage:\nrpc_client.exe number";
        return -1;
    }
    const std::string correlation(uuid_cpp::generate_uuid_v4());

    SimplePocoHandler handler("localhost", 5672);

    AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");

    // Record start time
    auto start = std::chrono::high_resolution_clock::now();

    AMQP::Channel channel(&connection);
    AMQP::QueueCallback callback = [&](const std::string &name,
            int msgcount,
            int consumercount)
    {
        const char* body = argv[1];
        std::string str_body(body);
        AMQP::Envelope env(str_body.c_str(), str_body.length());
        env.setCorrelationID(correlation);
        env.setReplyTo(name);
        channel.publish("","rpc_queue",env);
        std::cout<<" [x] Requesting fib(30)"<<std::endl;

    };
    channel.declareQueue(AMQP::exclusive).onSuccess(callback);

    auto receiveCallback = [&](const AMQP::Message &message,
            uint64_t deliveryTag,
            bool redelivered)
    {
        if(message.correlationID() != correlation)
            return;

        const char* data = message.body();
        int size = message.bodySize();
        std::string body(data, size);
        std::cout<<" [.] Got "<< body <<std::endl;
        // Record end time
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Elapsed time: " << 1000 * elapsed.count() << " ms\n";
        handler.quit();
    };

    channel.consume("", AMQP::noack).onReceived(receiveCallback);

    handler.loop();
    return 0;
}
