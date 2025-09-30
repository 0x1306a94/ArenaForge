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
//  LayerTreeAdapter.cpp
//  arenaforge_editor
//
//  Created by king on 2025/9/18.
//

#include "LayerTreeAdapter.h"

#include "PathBuilder.h"
#include "RenderLayerBuilder.h"

#include <tgfx/layers/Layer.h>
#include <tgfx/layers/ShapeLayer.h>
#include <tgfx/layers/SolidColor.h>

#include <arenaforge_core/layers/ShapeLayer.h>
#include <arenaforge_editor/core/defines.h>

namespace arenaforge::editor {

LayerTreeAdapter::LayerTreeAdapter(std::shared_ptr<Layer> rootDataLayer, std::shared_ptr<tgfx::Layer> rootRenderLayer)
    : _rootDataLayer(std::move(rootDataLayer))
    , _rootRenderLayer(std::move(rootRenderLayer)) {

    syncTree(_rootDataLayer, _rootRenderLayer);

    setupSynchronization();
}

// 手动强制同步（用于复杂场景）
void LayerTreeAdapter::forceResync() {
    clearChildren(_rootRenderLayer);
    _renderLayerCache.clear();
    _dataLayerCache.clear();
    syncTree(_rootDataLayer, _rootRenderLayer);
}

void LayerTreeAdapter::setSelectedLayers(const std::vector<std::shared_ptr<arenaforge::Layer>> &targets) {
    UNUSED_PARAM(targets);
    if (targets.empty()) {
        _selectedLayers.clear();
        return;
    }

    _selectedLayers = std::move(targets);
}

void LayerTreeAdapter::updateSelectionDisplay() {
    if (_selectedLayers.empty()) {
        return;
    }

    tgfx::Point min{FLT_MAX, FLT_MAX}, max{FLT_MIN, FLT_MIN};
    for (const auto &data : _selectedLayers) {
        auto renderLayer = findRenderLayer(data->layerId());
        if (!renderLayer) {
            continue;
        }

        auto frame = data->frame();
        auto topLeft = renderLayer->localToGlobal({0.0, 0.0});
        auto bottomRight = renderLayer->localToGlobal({frame.width(), frame.height()});
        min.x = std::min(min.x, topLeft.x);
        min.y = std::min(min.y, topLeft.y);

        max.x = std::max(max.x, bottomRight.x);
        max.y = std::max(max.y, bottomRight.y);
    }

    if (!_selectedBoundingBoxLayer) {
        _selectedBoundingBoxLayer = tgfx::ShapeLayer::Make();
        _selectedBoundingBoxLayer->setStrokeStyle(tgfx::SolidColor::Make(tgfx::Color::FromRGBA(0x0c, 0x8c, 0xe9)));
        //    _selectedBoundingBoxLayer->setPosition(position);
        _selectedBoundingBoxLayer->setLineWidth(2);
        _selectedBoundingBoxLayer->setStrokeAlign(tgfx::StrokeAlign::Outside);

        _rootRenderLayer->root()->addChild(_selectedBoundingBoxLayer);
    }

    tgfx::Path boundingBoxPath;
    boundingBoxPath.addRect(min.x, min.y, max.x, max.y);
    _selectedBoundingBoxLayer->setPath(std::move(boundingBoxPath));
}

void LayerTreeAdapter::clearSelectionDisplay() {
    if (_selectedBoundingBoxLayer) {
        _selectedBoundingBoxLayer->removeFromParent();
        _selectedBoundingBoxLayer = nullptr;
    }
}

bool LayerTreeAdapter::hitTestPointInSelectedBoundingBox(float x, float y) const {
    if (!_selectedBoundingBoxLayer) {
        return false;
    }
    auto hit = _selectedBoundingBoxLayer->hitTestPoint(x, y);
    return hit;
}

void LayerTreeAdapter::setupSynchronization() {
    subscribe(_rootDataLayer);
}

void LayerTreeAdapter::subscribe(const std::shared_ptr<Layer> &layer) {
    layer->setOnPropertyChanged([this](Layer *layer) {
        this->handlePropertyChanged(layer);
    });

    layer->setOnStructureChanged([this](Layer *parent, Layer *child, Layer::StructureChangeType type, int index) {
        this->handleStructureChanged(parent, child, type, index);
    });

    subscribeChildren(_rootDataLayer);
}

void LayerTreeAdapter::unsubscribe(const std::shared_ptr<Layer> &layer) {
    layer->setOnPropertyChanged(nullptr);
    layer->setOnStructureChanged(nullptr);

    unsubscribeChildren(layer);
}

void LayerTreeAdapter::subscribeChildren(const std::shared_ptr<Layer> &parent) {
    for (const auto &child : parent->children()) {
        child->setOnPropertyChanged([this](Layer *layer) {
            this->handlePropertyChanged(layer);
        });

        child->setOnStructureChanged([this](Layer *p, Layer *c, Layer::StructureChangeType t, int i) {
            this->handleStructureChanged(p, c, t, i);
        });

        subscribeChildren(child);
    }
}

void LayerTreeAdapter::unsubscribeChildren(const std::shared_ptr<Layer> &parent) {
    for (const auto &child : parent->children()) {
        child->setOnPropertyChanged(nullptr);
        child->setOnStructureChanged(nullptr);
    }
}

// 完整树同步（初始构建或强制更新）
void LayerTreeAdapter::syncTree(const std::shared_ptr<Layer> &dataLayer, std::shared_ptr<tgfx::Layer> &renderLayer) {
    if (!dataLayer || !renderLayer) {
        return;
    }

    // 1. 同步当前节点属性
    syncNodeProperties(dataLayer.get(), renderLayer.get());

    // 2. 清空现有子节点
    clearChildren(renderLayer);

    // 3. 递归构建子树
    for (const auto &childData : dataLayer->children()) {
        // 创建子节点的TGfx层
        auto childRender = RenderLayerBuilder::BuildFromData(childData.get());
        if (!childRender) {
            continue;
        }

        // 缓存管理
        _renderLayerCache[childData->layerId()] = childRender;
        _dataLayerCache[childData->layerId()] = childData;

        // 添加到父节点
        renderLayer->addChild(childRender);

        // 递归同步子树（关键！）
        syncTree(childData, childRender);
    }
}

// 单节点属性同步
void LayerTreeAdapter::syncNodeProperties(const Layer *dataLayer, tgfx::Layer *renderLayer) {
    auto frame = dataLayer->frame();
    renderLayer->setName(dataLayer->layerId());
    renderLayer->setVisible(dataLayer->visible());
    renderLayer->setPosition(tgfx::Point{frame.x(), frame.y()});

    auto type = dataLayer->type();
    switch (type) {
        case LayerType::Group: {
            auto shapeLayer = dynamic_cast<tgfx::ShapeLayer *>(renderLayer);
            if (shapeLayer) {
                syncGroupNodeProperties(dataLayer, shapeLayer);
            }
            break;
        }
        case LayerType::Shape: {
            auto shapeLayer = dynamic_cast<tgfx::ShapeLayer *>(renderLayer);
            if (shapeLayer) {
                const auto shapeData = dynamic_cast<const ShapeLayer *>(dataLayer);
                syncShapeNodeProperties(shapeData, shapeLayer);
            }
            break;
        }
        default:
            break;
    }
}

void LayerTreeAdapter::syncGroupNodeProperties(const Layer *dataLayer, tgfx::ShapeLayer *renderLayer) {
    if (dataLayer == nullptr || renderLayer == nullptr) {
        return;
    }

    auto frame = dataLayer->frame();

    tgfx::Path path;
    path.addRect(tgfx::Rect::MakeWH(frame.width(), frame.height()));
    renderLayer->setPath(std::move(path));
}

void LayerTreeAdapter::syncShapeNodeProperties(const ShapeLayer *dataLayer, tgfx::ShapeLayer *renderLayer) {
    if (dataLayer == nullptr || renderLayer == nullptr) {
        return;
    }

    auto frame = dataLayer->frame();

    const auto &commands = dataLayer->pathCommands();
    tgfx::Path path = PathBuilder::BuildPath(commands, frame.size());
    //    if (path.isLine()) {
    //        path.reset();
    //        path.addRect(tgfx::Rect::MakeWH(frame.width(), frame.height()));
    //    }
    renderLayer->setPath(std::move(path));

    auto fill = dataLayer->fill();
    if (fill) {
        auto color = fill.value();
        renderLayer->setFillStyle(tgfx::SolidColor::Make(tgfx::Color{color.red, color.green, color.blue, color.alpha}));
    } else {
        renderLayer->setFillStyle(nullptr);
    }

    auto stroke = dataLayer->stroke();
    if (stroke) {
        auto color = stroke.value();
        renderLayer->setStrokeStyle(tgfx::SolidColor::Make(tgfx::Color{color.red, color.green, color.blue, color.alpha}));
        renderLayer->setLineWidth(dataLayer->lineWidth());
        renderLayer->setLineCap(static_cast<tgfx::LineCap>(dataLayer->lineCap()));
        renderLayer->setLineJoin(static_cast<tgfx::LineJoin>(dataLayer->lineJoin()));
        renderLayer->setStrokeAlign(static_cast<tgfx::StrokeAlign>(dataLayer->strokeAlign()));
    } else {
        renderLayer->setStrokeStyle(nullptr);
    }
}

// 结构变更处理
void LayerTreeAdapter::handleStructureChanged(Layer *parentData, Layer *childData, Layer::StructureChangeType type, int index) {
    auto parentRender = findRenderLayer(parentData->layerId());
    if (!parentRender) {
        return;
    }

    switch (type) {
        case Layer::StructureChangeType::ChildAdded:
            handleChildAdded(parentRender, childData, index);
            break;

        case Layer::StructureChangeType::ChildRemoved:
            handleChildRemoved(parentRender, childData, index);
            break;

        case Layer::StructureChangeType::ChildReordered:
            handleChildReordered(parentRender, childData, index);
            break;
    }
}

void LayerTreeAdapter::handleChildAdded(std::shared_ptr<tgfx::Layer> &parentRender, Layer *childDataLayer, int index) {
    UNUSED_PARAM(parentRender);
    UNUSED_PARAM(childDataLayer);
    UNUSED_PARAM(index);
    if (childDataLayer == nullptr) {
        return;
    }
    // 从数据层创建渲染子节点
    _dataLayerCache[childDataLayer->layerId()] = childDataLayer->weak_from_this();
    auto childRender = RenderLayerBuilder::BuildFromData(childDataLayer);
    if (childRender) {
        _renderLayerCache[childDataLayer->layerId()] = childRender;
        syncNodeProperties(childDataLayer, childRender.get());
        parentRender->addChildAt(childRender, index);
        subscribe(childDataLayer->shared_from_this());
    }
}

void LayerTreeAdapter::handleChildRemoved(std::shared_ptr<tgfx::Layer> &parentRender, Layer *childDataLayer, int index) {
    UNUSED_PARAM(index);
    if (childDataLayer == nullptr) {
        return;
    }
    // 找到对应的渲染子节点并移除
    auto child = parentRender->getChildByName(childDataLayer->layerId());
    if (child) {

        _renderLayerCache.erase(childDataLayer->layerId());
        child->removeFromParent();
    }

    _dataLayerCache.erase(childDataLayer->layerId());
    unsubscribe(childDataLayer->shared_from_this());
}

void LayerTreeAdapter::handleChildReordered(std::shared_ptr<tgfx::Layer> &parentRender, Layer *childDataLayer, int newIndex) {
    // 找到要移动的子节点
    auto child = parentRender->getChildByName(childDataLayer->layerId());
    if (child) {
        parentRender->setChildIndex(child, newIndex);
    }
}

// 属性变更处理
void LayerTreeAdapter::handlePropertyChanged(Layer *changedLayer) {
    auto renderLayer = findRenderLayer(changedLayer->layerId());
    if (!renderLayer) {
        return;
    }

    // 增量同步属性
    syncNodeProperties(changedLayer, renderLayer.get());
}

std::shared_ptr<Layer> LayerTreeAdapter::findDataLayer(const std::string &layerId) {
    if (layerId.empty()) {
        return nullptr;
    }

    // 检查缓存
    auto cacheIt = _dataLayerCache.find(layerId);
    if (cacheIt != _dataLayerCache.end()) {
        auto cached = cacheIt->second.lock();
        if (cached) {
            return cached;
        }
    }

    // DFS查找并缓存
    auto found = findDataLayerRecursive(_rootDataLayer, layerId);
    if (found) {
        _dataLayerCache[layerId] = found;
    }
    return found;
}

std::shared_ptr<Layer> LayerTreeAdapter::findDataLayerRecursive(const std::shared_ptr<Layer> &node, const std::string &layerId) {
    if (node == nullptr) {
        return nullptr;
    }

    if (node->layerId() == layerId) {
        return node;
    }

    for (const auto &child : node->children()) {
        auto found = findDataLayerRecursive(child, layerId);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

// 快速查找渲染层（缓存优化）
std::shared_ptr<tgfx::Layer> LayerTreeAdapter::findRenderLayer(const std::string &layerId) {
    if (layerId.empty()) {
        return nullptr;
    }

    // 检查缓存
    auto cacheIt = _renderLayerCache.find(layerId);
    if (cacheIt != _renderLayerCache.end()) {
        auto cached = cacheIt->second.lock();
        if (cached) {
            return cached;
        }
    }

    // DFS查找并缓存
    auto found = findRenderLayerRecursive(_rootRenderLayer, layerId);
    if (found) {
        _renderLayerCache[layerId] = found;
    }
    return found;
}

std::shared_ptr<tgfx::Layer> LayerTreeAdapter::findRenderLayerRecursive(const std::shared_ptr<tgfx::Layer> &node, const std::string &layerId) {
    if (node == nullptr) {
        return nullptr;
    }

    if (node->name() == layerId) {
        return node;
    }

    for (const auto &child : node->children()) {
        auto found = findRenderLayerRecursive(child, layerId);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

void LayerTreeAdapter::clearChildren(std::shared_ptr<tgfx::Layer> &renderLayer) {
    if (renderLayer == nullptr) {
        return;
    }
    // 移除所有子节点
    auto children = renderLayer->children();
    for (auto &it : children) {
        it->removeFromParent();
        auto layerId = it->name();
        _renderLayerCache.erase(layerId);
        _dataLayerCache.erase(layerId);
    }
}
};  // namespace arenaforge::editor
