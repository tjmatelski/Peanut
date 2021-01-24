#pragma once

#include "Application.h"
#include "Input.h"
#include "KeyCodes.h"
#include "Log.h"
#include "Math.h"
#include "Window.h"
#include "Util.h"

// Renderer
#include "Renderer/BufferLayout.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/OrthoCamera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/TextureLibrary.h"
#include "Renderer/VertexArray.h"
#include "Renderer/VertexBuffer.h"

// Events
#include "Events/Event.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvents.h"

// Scene
#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"

// Dear ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>