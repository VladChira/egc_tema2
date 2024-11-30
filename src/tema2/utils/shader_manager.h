#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

#include "core/gpu/shader.h"
#include "utils/text_utils.h"
#include "core/managers/resource_path.h"

namespace tema2
{
    using ShaderMap = std::unordered_map<std::string, std::unique_ptr<Shader>>;

    class ShaderManager
    {
    public:
        static void Initialize(const std::string &windowSelfDir)
        {
            ShaderManager::windowSelfDir = windowSelfDir;
        }

        static Shader *GetShaderByName(const std::string &name)
        {
            if (!isShaderLoaded(name))
                return nullptr;
            return loadedShaders.find(name)->second.get();
        }

        template <typename T, typename... Args>
        static std::unique_ptr<T> make_unique(Args &&...args)
        {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }

        static void LoadShader(const std::string &name, const std::string &vertexShaderName,
                               const std::string &fragmentShaderName)
        {
            std::unique_ptr<Shader> shader = make_unique<Shader>(name);
            shader->AddShader(PATH_JOIN(windowSelfDir, RESOURCE_PATH::SHADERS, vertexShaderName), GL_VERTEX_SHADER);
            shader->AddShader(PATH_JOIN(windowSelfDir, RESOURCE_PATH::SHADERS, fragmentShaderName), GL_FRAGMENT_SHADER);
            shader->CreateAndLink();
            loadedShaders[name] = std::move(shader);
        }

    private:
        static bool isShaderLoaded(const std::string &name)
        {
            return loadedShaders.find(name) != loadedShaders.end();
        }
        static ShaderMap loadedShaders;
        static std::string windowSelfDir;
    };
}