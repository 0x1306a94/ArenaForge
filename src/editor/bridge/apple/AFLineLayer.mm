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
//  AFLineLayer.mm
//  arenaforge_editor
//
//  Created by king on 2025/9/28.
//

#import <arenaforge_editor/bridge/apple/AFLineLayer.h>

#import "AFLayer+Private.h"

#import <arenaforge_core/layers/ShapeLayer.h>

@implementation AFLineLayer

- (void)updateStartPoint:(CGPoint)startPoint endPoint:(CGPoint)endPoint {

    CGFloat x = fmin(startPoint.x, endPoint.x);
    CGFloat y = fmin(startPoint.y, endPoint.y);
    CGFloat w = fabs(endPoint.x - startPoint.x);
    CGFloat h = fabs(endPoint.y - startPoint.y);
    if (w < FLT_EPSILON) {
        w = 1;
    }
    if (h < FLT_EPSILON) {
        h = 1;
    }

    [self disableNotifyPropertyChanged];
    CGRect frame = CGRectMake(x, y, w, h);
    self.frame = frame;
    [self enableNotifyPropertyChanged];

    CGFloat startX = (startPoint.x - x) / w;
    CGFloat startY = (startPoint.y - y) / h;
    CGFloat endX = (endPoint.x - x) / w;
    CGFloat endY = (endPoint.y - y) / h;

    auto layer = std::static_pointer_cast<arenaforge::ShapeLayer>([self cppObject]);

    std::vector<arenaforge::PathCommand> commands;
    commands.push_back(arenaforge::PathCommand::MakeMoveTo({static_cast<float>(startX), static_cast<float>(startY)}));
    commands.push_back(arenaforge::PathCommand::MakeLineTo({static_cast<float>(endX), static_cast<float>(endY)}));
    layer->setPathCommands(std::move(commands));
}
@end
