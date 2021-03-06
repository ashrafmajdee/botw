#pragma once

#include "KingSystem/ActorSystem/actAiAi.h"

namespace uking::ai {

class SwarmEscapeDie : public ksys::act::ai::Ai {
    SEAD_RTTI_OVERRIDE(SwarmEscapeDie, ksys::act::ai::Ai)
public:
    explicit SwarmEscapeDie(const InitArg& arg);
    ~SwarmEscapeDie() override;

    bool init_(sead::Heap* heap) override;
    void enter_(ksys::act::ai::InlineParamPack* params) override;
    void leave_() override;
    void loadParams_() override;

protected:
    // static_param at offset 0x38
    const int* mTime_s{};
    // static_param at offset 0x40
    const float* mRiseHeight_s{};
    // static_param at offset 0x48
    const float* mRiseDist_s{};
    // static_param at offset 0x50
    const float* mEndDist_s{};
};

}  // namespace uking::ai
