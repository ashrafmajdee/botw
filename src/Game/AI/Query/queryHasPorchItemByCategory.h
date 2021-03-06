#pragma once

#include "KingSystem/ActorSystem/actAiQuery.h"

namespace uking::query {

class HasPorchItemByCategory : public ksys::act::ai::Query {
    SEAD_RTTI_OVERRIDE(HasPorchItemByCategory, Query)
public:
    explicit HasPorchItemByCategory(const InitArg& arg);
    ~HasPorchItemByCategory() override;
    int doQuery() override;

    void loadParams() override;
    void loadParams(const evfl::QueryArg& arg) override;

protected:
    int* mCategory{};
    int* mCount{};
};

}  // namespace uking::query
