//
// Created by wegblad on 4/13/23.
//

#ifndef GYARB_APPLICATIONCONTEXT_H
#define GYARB_APPLICATIONCONTEXT_H

#include <utility>
#include <vector>
#include <memory>
#include <list>
#include "config/config.h"

class OrderedConfig
{
public:
    OrderedConfig(int orderInt, std::shared_ptr<Config> &&config, std::string name) : order_int(orderInt), config(config), name(std::move(name)) {}
    int order_int;
    std::shared_ptr<Config> config;
    std::string name;
    virtual ~OrderedConfig() = default;
};

class ApplicationContext
{
public:
    static void start()
    {
        _logger << "In start";
        _logger.log();
        for (auto & _configuration : _configurations)
        {
            _logger << "Running: " << _configuration.name.c_str() << " configuration";
            _logger.log();
            _configuration.config->configure(_container);
        }
    }

    template<typename T>
    static void addConfig(int order)
    {
        addConfig<T>(std::forward<int>(order), _logger.demangleName<T>());
    }

    template<typename T>
    static void addConfig(int order, const std::string& name)
    {
        addConfig(std::make_unique<T>(), order, name);
    }

    static void addConfig(std::unique_ptr<Config> config, int order, const std::string& name)
    {
        OrderedConfig oc(order, std::forward<std::unique_ptr<Config>>(config), name);
        _logger << "Registered: " << name.c_str() << " configuration";
        _logger.log();
        bool inserted = false;
        for (auto it = _configurations.begin(); it > _configurations.end(); it++)
        {
            if (it->order_int < order) continue;
            else
            {
                _configurations.insert(it, oc);
                inserted = true;
            }
        }
        if (_configurations.empty() || !inserted) _configurations.push_back(oc);
    }

    template<typename T_REQUESTED>
    static std::shared_ptr<T_REQUESTED> getComponent()
    {
        return _container.getComponent<T_REQUESTED>();
    }

    template<class T_REQUESTED>
    static std::shared_ptr<T_REQUESTED> getComponent(const char* reason)
    {
        return _container.getComponent<T_REQUESTED>(reason);
    }

private:
    static std::vector<OrderedConfig> _configurations;
    static Logger<ApplicationContext> _logger;
    static IoCContainer _container;
};

IoCContainer ApplicationContext::_container;
std::vector<OrderedConfig> ApplicationContext::_configurations;
Logger<ApplicationContext> ApplicationContext::_logger;

#endif //GYARB_APPLICATIONCONTEXT_H
