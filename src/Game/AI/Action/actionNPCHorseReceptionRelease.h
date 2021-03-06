#pragma once

#include "KingSystem/ActorSystem/actAiAction.h"

namespace uking::action {

class NPCHorseReceptionRelease : public ksys::act::ai::Action {
    SEAD_RTTI_OVERRIDE(NPCHorseReceptionRelease, ksys::act::ai::Action)
public:
    explicit NPCHorseReceptionRelease(const InitArg& arg);
    ~NPCHorseReceptionRelease() override;

    void enter_(ksys::act::ai::InlineParamPack* params) override;

protected:
    void calc_() override;
};

}  // namespace uking::action
