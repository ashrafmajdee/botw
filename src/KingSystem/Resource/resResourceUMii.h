#pragma once

#include "KingSystem/Resource/resResource.h"
#include "KingSystem/Utils/ParamIO.h"
#include "KingSystem/Utils/Types.h"

namespace ksys::res {

class UMii : public ParamIO, public Resource {
    SEAD_RTTI_OVERRIDE(UMii, Resource)
public:
    UMii();
    ~UMii() override;

    agl::utl::ResParameterArchiveData* getData() const { return mData; }

    bool needsParse() const override { return true; }
    bool m2_() override { return mData != nullptr; }
    void doCreate_(u8*, u32, sead::Heap*) override {}
    bool parse_(u8* data, size_t size, sead::Heap* heap) override;

private:
    agl::utl::ResParameterArchiveData* mData{};
};
KSYS_CHECK_SIZE_NX150(UMii, 0x2b8);

}  // namespace ksys::res
