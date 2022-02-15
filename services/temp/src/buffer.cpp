/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "buffer.h"
#include "securec.h"
#include "face_auth_log_wrapper.h"
namespace OHOS {
namespace UserIAM {
namespace FaceAuth {
const int MAX_BUFFER_SIZE = 512000;

bool IsBufferValid(const Buffer *buffer)
{
    if ((buffer == nullptr) || (buffer->buf == nullptr) ||
        (buffer->maxSize == 0) || (buffer->maxSize > MAX_BUFFER_SIZE) ||
        (buffer->contentSize > buffer->maxSize)) {
        return false;
    }

    return true;
}

bool CheckBufferWithSize(const Buffer *buffer, const uint32_t size)
{
    if ((!IsBufferValid(buffer)) || (buffer->contentSize != size)) {
        return false;
    }

    return true;
}

Buffer *CreateBuffer(const uint32_t size)
{
    if ((size == 0) || (size > MAX_BUFFER_SIZE)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Bad param size:%zu", size);
        return nullptr;
    }

    Buffer *buffer = new Buffer;
    if (buffer == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Get buffer struct error");
        return nullptr;
    }

    buffer->buf = (uint8_t *)malloc(size);
    if (buffer->buf == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Get buffer error");
        delete buffer;
        return nullptr;
    }

    if (memset_s(buffer->buf, size, 0, size) != EOK) {
        delete buffer->buf;
        delete buffer;
        return nullptr;
    }
    buffer->maxSize = size;
    buffer->contentSize = 0;

    return buffer;
}

Buffer *CreateBufferByData(const uint8_t *data, const uint32_t dataSize)
{
    if ((data == nullptr) || (dataSize == 0) || (dataSize > MAX_BUFFER_SIZE)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Bad param size:%u", dataSize);
        return nullptr;
    }

    Buffer *buffer = new Buffer;
    if (buffer == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Get buffer struct error");
        return nullptr;
    }

    buffer->buf = new uint8_t;
    if (buffer->buf == nullptr) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Get buffer error");
        delete buffer;
        return nullptr;
    }

    if (memcpy_s(buffer->buf, dataSize, data, dataSize) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Cpy buffer error");
        DestoryBuffer(buffer);
        return nullptr;
    }
    buffer->maxSize = dataSize;
    buffer->contentSize = dataSize;

    return buffer;
}

ResultCode InitBuffer(Buffer *buffer, const uint8_t *buf, const uint32_t bufSize)
{
    if (!IsBufferValid(buffer) || (buf == nullptr) || (bufSize == 0)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Bad param");
        return RESULT_BAD_PARAM;
    }

    if (memcpy_s(buffer->buf, buffer->maxSize, buf, bufSize) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Copy buffer fail");
        return RESULT_BAD_COPY;
    }
    buffer->contentSize = bufSize;

    return RESULT_SUCCESS;
}

void DestoryBuffer(Buffer *buffer)
{
    if (buffer != nullptr) {
        if (buffer->buf != nullptr) {
            if (memset_s(buffer->buf, buffer->contentSize, 0, buffer->contentSize) != EOK) {
                FACEAUTH_HILOGE(MODULE_SERVICE, "DestoryBuffer memset fail!");
            }
            delete buffer->buf;
            buffer->buf = nullptr;
            buffer->contentSize = 0;
            buffer->maxSize = 0;
        }
        delete buffer;
    }
}

Buffer *CopyBuffer(const Buffer *buffer)
{
    if (!IsBufferValid(buffer)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Invalid buffer");
        return NULL;
    }

    Buffer *copyBuffer = CreateBuffer(buffer->maxSize);
    if (copyBuffer == NULL) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Invalid buffer");
        return NULL;
    }

    if (memcpy_s(copyBuffer->buf, copyBuffer->maxSize, buffer->buf, buffer->contentSize) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Copy buffer fail");
        DestoryBuffer(copyBuffer);
        return NULL;
    }
    copyBuffer->contentSize = buffer->contentSize;

    return copyBuffer;
}

bool CompareBuffer(const Buffer *buffer1, const Buffer *buffer2)
{
    if (!IsBufferValid(buffer1) || !IsBufferValid(buffer2) || (buffer1->contentSize != buffer2->contentSize)) {
        return false;
    }

    if (memcmp(buffer1->buf, buffer2->buf, buffer1->contentSize) == 0) {
        return true;
    }

    return false;
}

ResultCode GetBufferData(const Buffer *buffer, uint8_t *data, uint32_t *dataSize)
{
    if (!IsBufferValid(buffer) || (data == nullptr) || (dataSize == nullptr)) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Bad param");
        return RESULT_BAD_PARAM;
    }
    if (memcpy_s(data, *dataSize, buffer->buf, buffer->contentSize) != EOK) {
        FACEAUTH_HILOGE(MODULE_SERVICE, "Copy buffer fail");
        return RESULT_BAD_COPY;
    }
    *dataSize = buffer->contentSize;
    return RESULT_SUCCESS;
}
} // namespace FaceAuth
} // namespace UserIAM
} // namespace OHOS