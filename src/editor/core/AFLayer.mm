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
//  AFLayer.mm
//  arenaforge
//
//  Created by KK on 2025/9/7.
//

#import <arenaforge_editor/core/AFLayer.h>

#import <arenaforge_core/layers/BaseLayer.h>
#import <arenaforge_core/uuid/UUID.h>

@interface AFLayer ()
@property (nonatomic, copy) NSString *name;
@end

@implementation AFLayer {
    std::shared_ptr<arenaforge::BaseLayer> _layer;
}

#if DEBUG
- (void)dealloc {
    NSLog(@"[%@ dealloc]", NSStringFromClass(self.class));
}
#endif

+ (instancetype)createWithName:(NSString *)name {
    auto uuid = arenaforge::UUID::Instance();
    auto layerId = uuid();

    AFLayer *layer = [[AFLayer alloc] init];
    auto backLayer = arenaforge::BaseLayer::Make(layerId, arenaforge::ShapeType::Rectangle);
    backLayer->setName((name == nil ? "" : std::string(name.UTF8String)));
    layer->_layer = std::move(backLayer);
    return layer;
}
@end
