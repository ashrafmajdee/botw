#pragma once

#include "Game/AI/Action/actionAreaTagAction.h"
#include "KingSystem/ActorSystem/actAiAction.h"

namespace uking::action {

class ForbidComeback : public AreaTagAction {
    SEAD_RTTI_OVERRIDE(ForbidComeback, AreaTagAction)
public:
    explicit ForbidComeback(const InitArg& arg);
    ~ForbidComeback() override;

    bool init_(sead::Heap* heap) override;
    void enter_(ksys::act::ai::InlineParamPack* params) override;
    void leave_() override;
    void loadParams_() override;

protected:
    void calc_() override;
};

}  // namespace uking::action
