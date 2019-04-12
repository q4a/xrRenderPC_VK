/*!
 * \file    model_pool.h
 * \brief   Mesh manager interface
 */

#ifndef FRONTEND_MODEL_POOL_H_
#define FRONTEND_MODEL_POOL_H_

#include "frontend/render_visual.h"

class ModelPool
{
public:
    ModelPool() = default;
    ~ModelPool() = default;

    /*!
     * \brief   Creates model object
     */
    RenderVisual *Create( const std::string &name ///< model asset name
                        , IReader           *rstream ///< stream to read from
                        );
};

#endif // FRONTEND_MODEL_POOL_H_
