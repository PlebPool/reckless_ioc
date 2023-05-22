#include "include/log/logger.h"
#include "include/factory/component_factory.h"
#include "include/ioc/ioc_container.h"
#include "include/application/ApplicationContext.h"

#include <iostream>
#ifdef __unix__
    #include <netinet/in.h>
#endif

class TestDep
{

};

class TestIdent
{
public:
    virtual void doThing() = 0;
    virtual ~TestIdent() = default;
};


class Test : public TestIdent
{
private:
    Logger<Test> _log;
    std::shared_ptr<TestDep> _dep;
public:
    void doThing() override
    {
        _log << "From overridden - TestIdent::doThing()";
        _log.log();
    }

    explicit Test(std::shared_ptr<TestDep> dep) : _dep(std::move(dep)) {}
};

#ifdef __unix__
class TcpConnection
{
public:
    explicit TcpConnection(const std::shared_ptr<Test>& test)
    {
        _sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
        if (_sock_fd < 0)
        {
            _logger << "Socket creation failed: " << strerror(errno);
            _logger.error();
            throw std::runtime_error("PLANNED ERROR...");
        }
    }
private:
    int _sock_fd;
    Logger<TcpConnection> _logger;
};

class TcpConfig : public Config
{
public:
    TcpConfig() = default;

    void configure(IoCContainer& container) override
    {
        container.registerConcreteComponent<TcpConnection, Test>();
    }
};
#endif

class InConstructorFetcher
{
public:
    InConstructorFetcher()
    {
        std::shared_ptr<TestIdent> ident = ApplicationContext::getComponent<TestIdent>("Getting from inside constructor");
        _logger << "INSIDE OF " << _logger.demangleName<InConstructorFetcher>();
        _logger.log();
        ident->doThing();
    }
private:
    Logger<InConstructorFetcher> _logger;
};

class TestConfig : public Config
{
public:
    void configure(IoCContainer& container) override
    {
        container.registerConcreteComponent<TestDep>();
        container.registerConcreteComponent<Test, TestDep>();
        container.registerInterfaceComponent<TestIdent, Test, TestDep>();

        container.registerConcreteComponent<InConstructorFetcher>();
    }
};

int main()
{
    NamedLogger log("Main");
    log << "Hey";
    log.log();

    // Registrerar konfigurationer.
    ApplicationContext::addConfig<TestConfig>(0);
#ifdef __unix__
    ApplicationContext::addConfig<TcpConfig>(1);
#endif

    // Exekverar konfigurationer.
    ApplicationContext::start();

    // Hämtar en instans av typ "Test" (konfigurerats i "TestConfig").
    std::shared_ptr<Test> test = ApplicationContext::getComponent<Test>("Concrete - by user");

    // Hämtar en instans av identiteten "TestIdent" (konfigurerats i "TestConfig").
    std::shared_ptr<TestIdent> test_ident = ApplicationContext::getComponent<TestIdent>("Interface - by user");

    // Använder instansen av identitet "TestIdent".
    test_ident->doThing();

    // Borde hämta en instans av "TestIdent" i klassens konstruktör.
    ApplicationContext::getComponent<InConstructorFetcher>("Concrete - by user");

#ifdef __unix__
    // Hämtar en instans av typ "TcpConnection" (konfigurerats i "TcpConfig").
    // Detta kommer kasta en error då konstruktör koden kommer misslyckas p.ga icke-kompatibla protokoll.
    std::shared_ptr<TcpConnection> tcpConnection = ApplicationContext::getComponent<TcpConnection>("Concrete - by user");
#endif
}
