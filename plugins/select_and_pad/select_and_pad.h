/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>

#include "NvInfer.h"
#include <cuda_fp16.h>

#include "common.h"

namespace nvinfer1 {
namespace plugin {

class SelectAndPadPlugin: public IPluginV2DynamicExt {
public:
    SelectAndPadPlugin() noexcept {};
    SelectAndPadPlugin(PluginFieldCollection const& fc) noexcept;
    SelectAndPadPlugin(const std::string name, const void* serialData, size_t serialLength); 

    ~SelectAndPadPlugin() override = default;

    int getNbOutputs() const noexcept override;
    DimsExprs getOutputDimensions(
        int32_t outputIndex, DimsExprs const* inputs, int32_t nbInputs, IExprBuilder& exprBuilder) noexcept override;

    int initialize() noexcept override;
    void terminate() noexcept override;
    
    size_t getWorkspaceSize(
        PluginTensorDesc const* inputs, int32_t nbInputs, 
        PluginTensorDesc const* outputs,
        int32_t nbOutputs) const noexcept override;

    int32_t enqueue(
        PluginTensorDesc const* inputDesc, PluginTensorDesc const* outputDesc,
        void const* const* inputs, void* const* outputs, void* workspace, cudaStream_t stream) noexcept override;

    size_t getSerializationSize() const noexcept override;
    void serialize(void* buffer) const noexcept override;

    bool supportsFormatCombination(
        int32_t pos, PluginTensorDesc const* inOut, 
        int32_t nbInputs, int32_t nbOutputs) noexcept override;

    const char* getPluginType() const noexcept override;
    const char* getPluginVersion() const noexcept override;
    void destroy() noexcept override;
    IPluginV2DynamicExt* clone() const noexcept override;
    void setPluginNamespace(const char* pluginNamespace) noexcept override;
    const char* getPluginNamespace() const noexcept override;

    DataType getOutputDataType(
        int index, 
        const DataType* inputTypes, 
        int nbInputs) const noexcept override;
    void configurePlugin(
        DynamicPluginTensorDesc const* in, int32_t nbInputs,
        DynamicPluginTensorDesc const* out, int32_t nbOutputs) noexcept override;

    template <typename T>
    void write(char*& buffer, const T& val) const noexcept {
        *reinterpret_cast<T*>(buffer) = val;
        buffer += sizeof(T);
    }

    template <typename T>
    T read(const char*& buffer) const noexcept {
        T val = *reinterpret_cast<const T*>(buffer);
        buffer += sizeof(T);
        return val;
    }

    size_t decideTemp();

    const char* mPluginNamespace = "custom_op";
    const std::string mName = "SelectAndPadPlugin"; 
    PluginTensorDesc mInputTensor;
    PluginTensorDesc mOutputTensor;
    int P, Q;
    size_t tmp_bytes;
}; // class SelectAndPadPlugin


class SelectAndPadPluginCreator : public nvinfer1::IPluginCreator {
public:
    SelectAndPadPluginCreator();
    ~SelectAndPadPluginCreator() override = default;
    const char* getPluginName() const noexcept override;
    const char* getPluginVersion() const noexcept override;
    const PluginFieldCollection* getFieldNames() noexcept override;

    IPluginV2* createPlugin(const char* name, const PluginFieldCollection* fc) noexcept override;
    IPluginV2* deserializePlugin(const char* name, const void* serialData, size_t serialLength) noexcept override;

    void setPluginNamespace(const char* libNamespace) noexcept override;
    const char* getPluginNamespace() const noexcept override;
    void setupPluginAttributes(std::vector<PluginField>& attributes);

private:
    static PluginFieldCollection mFC;
    static std::vector<PluginField> mPluginAttributes;
    std::string mNamespace;
}; // class SelectAndPadPluginCreator

REGISTER_TENSORRT_PLUGIN(SelectAndPadPluginCreator);

} // namespace plugin
} // namespace nvinfer1
