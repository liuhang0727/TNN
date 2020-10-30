// Tencent is pleased to support the open source community by making TNN available.
//
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "test/unit_test/layer_test/layer_test.h"
#include "test/unit_test/unit_test_common.h"
#include "test/unit_test/utils/network_helpers.h"
#include "tnn/utils/dims_vector_utils.h"

namespace TNN_NS {

class PReluLayerTest : public LayerTest, public ::testing::WithParamInterface<std::tuple<int, int, int, bool>> {};

INSTANTIATE_TEST_SUITE_P(LayerTest, PReluLayerTest,
                         ::testing::Combine(BASIC_BATCH_CHANNEL_SIZE,
                                            // share channel
                                            testing::Values(false, true)));

TEST_P(PReluLayerTest, PReluLayer) {
    // get param
    int batch          = std::get<0>(GetParam());
    int channel        = std::get<1>(GetParam());
    int input_size     = std::get<2>(GetParam());
    bool share_channel = std::get<3>(GetParam());

    DeviceType dev = ConvertDeviceType(FLAGS_dt);

    // blob desc
    auto inputs_desc  = CreateInputBlobsDesc(batch, channel, input_size, 1, DATA_TYPE_FLOAT);
    auto outputs_desc = CreateOutputBlobsDesc(1, DATA_TYPE_FLOAT);

    // param
    PReluLayerParam param;
    param.name           = "PRelu";
    param.channel_shared = share_channel ? 1 : 0;

    // resource
    PReluLayerResource resource;
    int scope_count = share_channel ? 1 : channel;
    RawBuffer scope(scope_count * sizeof(float));
    float* scope_data = scope.force_to<float*>();
    InitRandom(scope_data, scope_count, 1.0f);
    resource.slope_handle = scope;

    Run(LAYER_PRELU, &param, &resource, inputs_desc, outputs_desc);
}

TEST_P(PReluLayerTest, PReluLayerWithProto) {
    // get param
    int batch          = std::get<0>(GetParam());
    int channel        = std::get<1>(GetParam());
    int input_size     = std::get<2>(GetParam());
    bool share_channel = std::get<3>(GetParam());

    DeviceType dev = ConvertDeviceType(FLAGS_dt);

    // param
    PReluLayerParam param;
    param.name           = "PRelu";
    param.channel_shared = share_channel ? 1 : 0;

    // generate proto string
    std::string head = GenerateHeadProto({batch, channel, input_size, input_size});
    std::ostringstream ostr;
    ostr << "\""
         << "PReLU layer_name 1 1 input output " << param.channel_shared << " " << param.has_filler << ",\"";

    std::string proto = head + ostr.str();
    RunWithProto(proto);
}

}  // namespace TNN_NS
