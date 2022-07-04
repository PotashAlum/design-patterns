//
//  main.cpp
//  observer-pattern
//
//

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

class Subscriber {
public:
    virtual void notify(const std::string& publisherName, const std::string& message) = 0;
    virtual std::string getName() = 0;
};

class Publisher {
public:
    virtual void subscribe(Subscriber *subscriber) = 0;
    virtual void unsubscribe(Subscriber *subscriber) = 0;
    virtual void publish(const std::string& message) = 0;
};

class ChatGroup : public Publisher {
std:: string groupName;
std::vector<Subscriber*> subscribers;
public:
    ChatGroup(const std::string& name) : groupName(name) {}
    void subscribe(Subscriber* subscriber) override {
        subscribers.push_back(subscriber);
    }
    
    void unsubscribe(Subscriber* subscriber) override {
        subscribers.erase(std::remove_if(subscribers.begin(), subscribers.end(), [subscriber](Subscriber* s) {return s->getName() == subscriber->getName(); }));
    }

    void publish(const std::string& message) override {
        for (const auto& subscriber : subscribers) {
            subscriber->notify(groupName, message);
        }
    }
};

class ChatUser : public Subscriber {
    std::string userName;
public:
    ChatUser(const std::string& userName) : userName(userName) {}
    void notify(const std::string& publisherName, const std::string& message) {
        std::cout << userName << " received a new message from " << publisherName << ": " << message << "\n";
    }

    std::string getName() override { return userName; };
};

class MessageCommand {
public:
    virtual ~MessageCommand() {}
    virtual void execute() = 0;
    virtual const std::string& getMessage() = 0;
};

class SendMessageCommand : public MessageCommand {
    ChatGroup* chatGroup;
    std::string message;
public:
    SendMessageCommand(ChatGroup* chatGroup, const std::string& message) : chatGroup(chatGroup), message(message) {}

    const std::string& getMessage() 
    {
        return message;
    }

    void execute() override {
        chatGroup->publish(message);
    }
};


class Handler {
public:
    virtual ~Handler() {};
    virtual Handler* setNext(Handler* nextValidator) = 0;
    virtual std::string handle(MessageCommand* messageCommand) = 0;
};

class BaseHandler : public Handler {
protected:
    Handler* next = nullptr;
public:
    virtual ~BaseHandler() { delete next; };
    Handler* setNext(Handler* nextValidator) override {
        next = nextValidator;
        return nextValidator;
    }
    virtual std::string handle(MessageCommand* messageCommand) override {
        if (next) {
            return next->handle(messageCommand);
        }
        return "Success";
    }
};

class NotEmptyValidator : public BaseHandler {
public:
    NotEmptyValidator() {};
    std::string handle(MessageCommand* messageCommand) override {
        puts("Checking if empty...");

        if (messageCommand->getMessage().empty()) {
            return "Please enter a value";
        }

        return BaseHandler::handle(messageCommand);
    }
};

class LengthValidator : public BaseHandler {
    int minLength;
public:
    LengthValidator(int minLength) : minLength(minLength) {};
    std::string handle(MessageCommand* messageCommand) override {
        puts("Checking string length...");

        if (messageCommand->getMessage().length() < minLength) {
            return "Please enter a value longer than " + std::to_string(minLength);
        }

        return BaseHandler::handle(messageCommand);
    }
};

class PostMessageHandler : public BaseHandler {
public:
    std::string handle(ChatGroup* group, const std::string& message) {
        group->publish(message);
        return "Message Sent!";
    }

};

int main(int argc, const char * argv[]) {


    ChatUser* user1 = new ChatUser("Jim");
    ChatUser* user2 = new ChatUser("Barb");
    ChatUser* user3 = new ChatUser("Hannah");
    
    ChatGroup* group1 = new ChatGroup("Gardening group");
    ChatGroup* group2 = new ChatGroup("Dog lovers group");

    group1->subscribe(user1);
    group1->subscribe(user2);

    group2->subscribe(user2);
    group2->subscribe(user3);
    
    Handler* sendMessageChain = new BaseHandler;
    sendMessageChain
        ->setNext(new NotEmptyValidator)
        ->setNext(new LengthValidator(2))
        ->setNext(new PostMessageHandler);

    
    SendMessageCommand msg1(group1, "");
    SendMessageCommand msg2(group1, "H");
    SendMessageCommand msg3(group1, "Hello everyone in group 1!");
    SendMessageCommand msg4(group2, "Hello everyone in group 2!");
    std::cout << "Sending empty message:\n" << sendMessageChain->handle(&msg1) << "\n\n";
    std::cout << "Sending short message:\n" << sendMessageChain->handle(&msg2) << "\n\n";
    std::cout << "Sending message to group 1:\n" << sendMessageChain->handle(&msg3) << "\n\n";
    std::cout << "Sending message to group 2:\n" << sendMessageChain->handle(&msg4) << "\n\n";

    delete user1;
    delete user2;
    delete user3;

    delete group1;
    delete group2;

    delete sendMessageChain;

    return 0;
}
