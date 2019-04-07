/*!
 * \file    main_menu.h
 * \brief   Main menu render interface
 */

#ifndef FRONTEND_MAIN_MENU_H_
#define FRONTEND_MAIN_MENU_H_

#include <memory>

#include "resources/shader.h"

class MenuRender
{
    std::shared_ptr<Shader> shader_;
public:
    MenuRender()  = default;
    ~MenuRender() = default;

    void Create();
    void Destroy();
    void Render();
};

#endif // FRONTEND_MAIN_MENU_H_
