//
// Created by wegblad on 4/5/23.
//

#ifndef GYARB_COMPONENT_FACTORY_H
#define GYARB_COMPONENT_FACTORY_H

#include <functional>
#include <memory>
#include "factory_base.h"

template<typename T_COMPONENT>
class ComponentFactory : public FactoryBase
{
private:
    std::function<std::shared_ptr<T_COMPONENT>()> _constructor;
public:
    explicit ComponentFactory(const std::function<std::shared_ptr<T_COMPONENT>()> &constructor)
    : _constructor(constructor) {}

    ComponentFactory(ComponentFactory&& fac) noexcept
    {
        _constructor = fac._constructor;
        fac._constructor = nullptr;
    }

    std::shared_ptr<T_COMPONENT> getInstance()
    {
        return _constructor();
    }
};

#endif //GYARB_COMPONENT_FACTORY_H
