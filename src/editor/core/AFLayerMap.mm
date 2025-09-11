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
//  AFLayerMap.m
//  arenaforge_editor
//
//  Created by KK on 2025/9/11.
//

#import "AFLayerMap.h"

#import "AFLayer+Private.h"

@interface AFLayerMap ()
@property (nonatomic, strong) NSMutableDictionary<NSString *, AFLayer *> *layerMap;
@end
@implementation AFLayerMap

- (instancetype)init {
    if (self == [super init]) {
        _layerMap = [NSMutableDictionary<NSString *, AFLayer *> dictionary];
    }
    return self;
}

- (void)addLayer:(AFLayer *)layer {
    if (layer) {
        _layerMap[layer.layerId] = layer;
    }
}

- (void)removeLayer:(AFLayer *)layer {
    if (layer) {
        _layerMap[layer.layerId] = nil;
    }
}

- (void)removeLayerById:(NSString *)layerId {
    if (layerId.length > 0) {
        _layerMap[layerId] = nil;
    }
}

- (AFLayer *_Nullable)getLayerById:(NSString *)layerId {
    if (layerId.length == 0) {
        return nil;
    }
    AFLayer *layer = _layerMap[layerId];
    return layer;
}
@end
