/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ArenaForge - An open source venue editing and rendering framework
//
//  Copyright (c) 2025 0x1306a94 (https://github.com/0x1306a94)
//  All rights reserved.
//
//  Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
//  in compliance with the License. You may obtain a copy of the License at:
//
//      https://opensource.org/licenses/BSD-3-Clause
//
//  Unless required by applicable law or agreed to in writing, software distributed under the
//  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
//  either express or implied. See the License for the specific language governing permissions
//  and limitations under the License.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//
//  LayerTreeAdapter.h
//  arenaforge_editor
//
//  Created by king on 2025/9/18.
//

#ifndef LayerTreeAdapter_h_ArenaForge
#define LayerTreeAdapter_h_ArenaForge

#include <arenaforge_core/layers/Layer.h>
#include <memory>

namespace tgfx {
class Layer;
class ShapeLayer;
};  // namespace tgfx

namespace arenaforge::editor {
class Editor;
};

namespace arenaforge {
class Layer;
class ShapeLayer;
class LayerTreeAdapter {
  public:
    LayerTreeAdapter(std::shared_ptr<Layer> rootDataLayer, std::shared_ptr<tgfx::Layer> rootRenderLayer);

    std::shared_ptr<tgfx::Layer> getRootRenderLayer() const {
        return _rootRenderLayer;
    }

    void forceResync();

  private:
    void setupSynchronization();

    void subscribe(const std::shared_ptr<Layer> &layer);
    void unsubscribe(const std::shared_ptr<Layer> &layer);

    void subscribeChildren(const std::shared_ptr<Layer> &parent);
    void unsubscribeChildren(const std::shared_ptr<Layer> &parent);

    // 完整树同步（初始构建或强制更新）
    void syncTree(const std::shared_ptr<Layer> &dataLayer, std::shared_ptr<tgfx::Layer> &renderLayer);

    // 单节点属性同步
    void syncNodeProperties(const Layer *dataLayer, tgfx::Layer *renderLayer);
    void syncGroupNodeProperties(const Layer *dataLayer, tgfx::ShapeLayer *renderLayer);
    void syncShapeNodeProperties(const ShapeLayer *dataLayer, tgfx::ShapeLayer *renderLayer);

    void handleStructureChanged(Layer *parentData, Layer *childData, Layer::StructureChangeType type, int index);

    void handleChildAdded(std::shared_ptr<tgfx::Layer> &parentRender, Layer *childDataLayer, int index);

    void handleChildRemoved(std::shared_ptr<tgfx::Layer> &parentRender, Layer *childDataLayer, int index);

    void handleChildReordered(std::shared_ptr<tgfx::Layer> &parentRender, Layer *childDataLayer, int newIndex);
    void handlePropertyChanged(Layer *changedLayer);

    std::shared_ptr<Layer> findDataLayer(const std::string &layerId);

    std::shared_ptr<Layer> findDataLayerRecursive(const std::shared_ptr<Layer> &node, const std::string &layerId);

    std::shared_ptr<tgfx::Layer> findRenderLayer(const std::string &layerId);

    std::shared_ptr<tgfx::Layer> findRenderLayerRecursive(const std::shared_ptr<tgfx::Layer> &node, const std::string &layerId);

    void clearChildren(std::shared_ptr<tgfx::Layer> &renderLayer);

  private:
    std::shared_ptr<Layer> _rootDataLayer{nullptr};
    std::shared_ptr<tgfx::Layer> _rootRenderLayer{nullptr};
    std::unordered_map<std::string, std::weak_ptr<Layer>> _dataLayerCache{};
    std::unordered_map<std::string, std::weak_ptr<tgfx::Layer>> _renderLayerCache{};

    friend class arenaforge::editor::Editor;
};
};  // namespace arenaforge

#endif /* LayerTreeAdapter_h_ArenaForge */
