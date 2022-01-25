/*
 * Copyright (C) 2013, 2014 Mark Li (lixiaonan06@163.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */
#ifndef MEDIA_AV_PIPELINE_FACTORY_H
#define MEDIA_AV_PIPELINE_FACTORY_H
#include "media/av_pipeline.h"
#include <memory>
#include <string>

namespace media {

void SeekCB(media::PipelineStatus status);
void PipelineStatusCallBack(media::PipelineStatus status);
std::unique_ptr<AVPipeline> MakeAVPipeLine(const std::string& url, VideoRenderer::PaintCB paint_cb);
std::shared_ptr<media::Renderer> MakeRenderer();
} // namespace media
#endif