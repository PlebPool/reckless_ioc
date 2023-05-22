cmake_minimum_required(VERSION 3.23.2)
project(reckless_ioc)

set(CMAKE_CXX_STANDARD 23)

add_library(reckless_ioc library.cpp include/log/logger.h include/factory/factory_base.h include/factory/component_factory.h include/ioc/ioc_container.h include/ioc/type_id_assigner.h include/application/ApplicationContext.h)

add_executable(exec library.cpp include/application/config/config.h)
target_link_libraries(exec reckless_ioc)