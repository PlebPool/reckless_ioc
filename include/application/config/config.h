//
// Created by wegblad on 5/21/23.
//

#ifndef GYARB_CONFIG_H
#define GYARB_CONFIG_H

#include "../../ioc/ioc_container.h"

class Config
{
public:
    virtual void configure(IoCContainer& container) = 0;
    virtual ~Config() = default;
private:
};

#endif //GYARB_CONFIG_H
