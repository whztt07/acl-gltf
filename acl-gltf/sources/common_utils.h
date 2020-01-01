#pragma once

////////////////////////////////////////////////////////////////////////////////
// The MIT License (MIT)
//
// Copyright (c) 2020 Nicholas Frechette
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include "tinygltf_impl.h"

#include <acl/compression/animation_clip.h>
#include <acl/compression/skeleton.h>
#include <acl/compression/track.h>
#include <acl/core/ansi_allocator.h>

#include <vector>

std::vector<uint16_t> build_node_parent_indices(const tinygltf::Model& model);

acl::RigidSkeleton build_skeleton(const tinygltf::Model& model, const std::vector<uint16_t>& node_parent_indices, acl::IAllocator& allocator);
acl::AnimationClip build_clip(const tinygltf::Model& model, const tinygltf::Animation& animation, const acl::RigidSkeleton& skeleton, acl::IAllocator& allocator);

uint32_t get_raw_animation_size(const tinygltf::Model& model, const tinygltf::Animation& animation);

template<typename track_type>
static track_type make_track_ref(const tinygltf::Model& model, const tinygltf::Accessor& sample_time_accessor)
{
	using sample_type = typename track_type::sample_type;

	const tinygltf::BufferView& buffer_view = model.bufferViews[sample_time_accessor.bufferView];
	const tinygltf::Buffer& buffer = model.buffers[buffer_view.buffer];

	const size_t start_offset = buffer_view.byteOffset + sample_time_accessor.byteOffset;
	const int sample_stride = sample_time_accessor.ByteStride(buffer_view);
	ACL_ASSERT(sample_stride >= static_cast<int>(sizeof(sample_type)), "Invalid sample stride");

	const acl::track_desc_scalarf dummy_desc = { 0, 0.0F, 0.0F };

	return  track_type::make_ref(dummy_desc, reinterpret_cast<const sample_type*>(buffer.data.data() + start_offset), static_cast<uint32_t>(sample_time_accessor.count), 0.0F, sample_stride);
}
