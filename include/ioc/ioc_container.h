//
// Created by wegblad on 4/5/23.
//

#ifndef GYARB_IOC_CONTAINER_H
#define GYARB_IOC_CONTAINER_H

#include <map>
#include <memory>
#include <iostream>
#include "type_id_assigner.h"
#include "../factory/factory_base.h"
#include "../factory/component_factory.h"
#include "../log/logger.h"

class IoCContainer
{
private:
    TypeIdAssigner _type_id_assigner;
    std::map<component_id_t, std::shared_ptr<FactoryBase>> _factories;
    Logger<IoCContainer> _logger;
public:
    template<typename T, typename ...T_DEPS>
    void registerConcreteComponent()
    {
        registerInterfaceComponent<T, T, T_DEPS...>();
    }

    template<typename T_IDENT, typename T_CONCRETE, typename ...T_DEPS>
    void registerInterfaceComponent()
    {
        static_assert(std::is_base_of<T_IDENT, T_CONCRETE>::value || std::is_same<T_IDENT, T_CONCRETE>::value);
        component_id_t type_id = _type_id_assigner.getTypeId<T_IDENT>();
        auto constructor = [this]()
        {
            // Perfect forwarding to constructor via variadic argument constructor.
            return std::make_shared<T_CONCRETE>(std::forward<std::shared_ptr<T_DEPS>>(
                    this->getComponent<T_DEPS>("Dependency"))...
            );
            // Errors are thrown by this->getComponent.
        };
        _factories[type_id] = std::make_shared<ComponentFactory<T_IDENT>>(constructor);
        _logger << "Registered component: " << _logger.demangleName<T_IDENT>();
        _logger.log();
    }

    template<class T_REQUESTED>
    std::shared_ptr<T_REQUESTED> getComponent()
    {
        return this->getComponent<T_REQUESTED>(nullptr);
    }

    template<class T_REQUESTED>
    std::shared_ptr<T_REQUESTED> getComponent(const char* reason)
    {
        _logger << "Requested: " << _logger.template demangleName<T_REQUESTED>();
        if (reason != nullptr)
        {
            _logger << " (" << reason << ")";
        }
        _logger.log();
        component_id_t type_id = _type_id_assigner.getTypeId<T_REQUESTED>();
        if (_factories.count(type_id) == 0)
        {
            _logger << "Requested component \"" << _logger.demangleName<T_REQUESTED>() << "\" is not registered.";
            _logger.log();
            throw std::runtime_error("FATAL...");
        }
        std::shared_ptr<FactoryBase> base = _factories.at(type_id);
        std::shared_ptr<ComponentFactory<T_REQUESTED>> c_factory =
                std::static_pointer_cast<ComponentFactory<T_REQUESTED>>(base);
        std::shared_ptr<T_REQUESTED> requested = c_factory->getInstance();
        _logger << "Responding with: " << _logger.template demangleName<T_REQUESTED>();
        _logger.log();
        return requested;
    }
};

#endif //GYARB_IOC_CONTAINER_H
