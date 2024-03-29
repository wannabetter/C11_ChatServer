#ifndef CHATSERVER_REDIS_H
#define CHATSERVER_REDIS_H

#include <string>
#include <functional>
#include "hiredis/hiredis.h"

class Redis {
public:
    Redis();

    ~Redis();

    bool connect();

    bool publish(int channel, std::string message);

    bool subscribe(int channel);

    bool unsubscribe(int channel);

    void observer_channel_message();

    void init_notify_handler(std::function<void(int, std::string)> fn);

private:
    redisContext *_publish_context;

    redisContext *_subcribe_context;

    std::function<void(int, std::string)> _notify_message_handler;
};

#endif //CHATSERVER_REDIS_H
