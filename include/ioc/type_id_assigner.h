//
// Created by wegblad on 4/5/23.
//

#ifndef GYARB_TYPE_ID_ASSIGNER_H
#define GYARB_TYPE_ID_ASSIGNER_H

typedef unsigned int component_id_t;

class TypeIdAssigner
{
private:
    static component_id_t s_current;
public:
    template<typename T>
    static component_id_t getTypeId()
    {
        static component_id_t type_id = s_current++;
        return type_id;
    }
};

component_id_t TypeIdAssigner::s_current = 0;

#endif //GYARB_TYPE_ID_ASSIGNER_H
