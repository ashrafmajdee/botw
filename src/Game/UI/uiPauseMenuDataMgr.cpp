#include "Game/UI/uiPauseMenuDataMgr.h"
#include <container/seadBuffer.h>
#include <limits>
#include <prim/seadScopedLock.h>
#include "Game/Actor/actWeapon.h"
#include "Game/DLC/aocManager.h"
#include "Game/UI/uiUtils.h"
#include "KingSystem/ActorSystem/Profiles/actPlayerBase.h"
#include "KingSystem/ActorSystem/actActorUtil.h"
#include "KingSystem/ActorSystem/actInfoData.h"
#include "KingSystem/ActorSystem/actPlayerInfo.h"
#include "KingSystem/GameData/gdtCommonFlagsUtils.h"
#include "KingSystem/System/PlayReportMgr.h"
#include "KingSystem/Utils/Byaml/Byaml.h"
#include "KingSystem/Utils/InitTimeInfo.h"

namespace uking::ui {

SEAD_SINGLETON_DISPOSER_IMPL(PauseMenuDataMgr)

sead::Vector2f sDummyCookEffect0{-1, 0};

namespace {

sead::SafeArray<CookTagInfo, 11> sCookItemOrder_{{
    {1, "CookFruit", ksys::act::tags::CookFruit},
    {1, "CookMushroom", ksys::act::tags::CookMushroom},
    {1, "CookPlant", ksys::act::tags::CookPlant},
    {1, "CookMeat", ksys::act::tags::CookMeat},
    {1, "CookSpice", ksys::act::tags::CookSpice},
    {1, "CookFish", ksys::act::tags::CookFish},
    {0, "Animal_Insect_F", 0},
    {1, "CookInsect", ksys::act::tags::CookInsect},
    {1, "CookOre", ksys::act::tags::CookOre},
    {1, "CookEnemy", ksys::act::tags::CookEnemy},
    {0, "Obj_FireWoodBundle", 0},
}};

struct PouchStaticData {
    ksys::util::InitTimeInfoEx info;

    u32 last_added_weapon_add_type{};
    u32 last_added_weapon_add_value{};
    f32 _20 = std::numeric_limits<f32>::infinity();
    f32 _24 = std::numeric_limits<f32>::infinity();
    f32 _28 = std::numeric_limits<f32>::infinity();
    u32 _2c{};
    u32 _30{};
    sead::SafeArray<f32, 70> _34{{
        0.0, 1.0, 0.0, 0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, 0.5, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.5, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 0.5, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.5, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.5,
        1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.5, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.5,
    }};
    u32 _14c{};
    u32 _150{};
    u32 _154{};
    u32 _158{};

    sead::SafeString WeaponSmallSword = "WeaponSmallSword";
    sead::SafeString WeaponLargeSword = "WeaponLargeSword";
    sead::SafeString WeaponSpear = "WeaponSpear";
    sead::SafeString WeaponBow = "WeaponBow";
    sead::SafeString WeaponShield = "WeaponShield";
    sead::SafeString ArmorHead = "ArmorHead";
    sead::SafeString ArmorUpper = "ArmorUpper";
    sead::SafeString ArmorLower = "ArmorLower";
    sead::SafeString Item = "Item";
    sead::SafeString PlayerItem = "PlayerItem";
    sead::SafeString HorseReins = "HorseReins";

    sead::SafeString None = "None";

    sead::SafeString Obj_KorokNuts = "Obj_KorokNuts";
    sead::SafeString Obj_DungeonClearSeal = "Obj_DungeonClearSeal";
    sead::SafeString Animal_Insect_F = "Animal_Insect_F";

    sead::SafeString Obj_HeroSoul_Zora = "Obj_HeroSoul_Zora";
    sead::SafeString Obj_HeroSoul_Rito = "Obj_HeroSoul_Rito";
    sead::SafeString Obj_HeroSoul_Goron = "Obj_HeroSoul_Goron";
    sead::SafeString Obj_HeroSoul_Gerudo = "Obj_HeroSoul_Gerudo";

    sead::SafeString Obj_DLC_HeroSoul_Zora = "Obj_DLC_HeroSoul_Zora";
    sead::SafeString Obj_DLC_HeroSoul_Rito = "Obj_DLC_HeroSoul_Rito";
    sead::SafeString Obj_DLC_HeroSoul_Goron = "Obj_DLC_HeroSoul_Goron";
    sead::SafeString Obj_DLC_HeroSoul_Gerudo = "Obj_DLC_HeroSoul_Gerudo";
    sead::SafeString Obj_WarpDLC = "Obj_WarpDLC";

    sead::SafeString Armor_116_Upper = "Armor_116_Upper";
    sead::SafeString Armor_148_Upper = "Armor_148_Upper";
    sead::SafeString Armor_149_Upper = "Armor_149_Upper";
    sead::SafeString Armor_150_Upper = "Armor_150_Upper";
    sead::SafeString Armor_151_Upper = "Armor_151_Upper";

    sead::Buffer<CookTagInfo> cook_item_order{sCookItemOrder_.size(),
                                              sCookItemOrder_.getBufferPtr()};

    sead::SafeArray<sead::SafeString, 7> default_equipment_names{{
        "Weapon_Default_Right",
        "Weapon_Default_Bow",
        "Weapon_Default_Arrow",
        "Weapon_Default_Left",
        "Armor_Default_Head",
        "Armor_Default_Upper",
        "Armor_Default_Lower",
    }};

    sead::SafeArray<sead::SafeString, 5 * 4> divine_helms{{
        "Armor_168_Head", "Armor_169_Head", "Armor_181_Head", "Armor_182_Head", "Armor_183_Head",
        "Armor_184_Head", "Armor_186_Head", "Armor_187_Head", "Armor_188_Head", "Armor_189_Head",
        "Armor_190_Head", "Armor_191_Head", "Armor_192_Head", "Armor_193_Head", "Armor_194_Head",
        "Armor_195_Head", "Armor_196_Head", "Armor_197_Head", "Armor_198_Head", "Armor_199_Head",
    }};
};

PouchStaticData sValues;

void getSameGroupActorName(sead::SafeString* group, const sead::SafeString& item,
                           al::ByamlIter* iter = nullptr) {
    if (iter) {
        ksys::act::getSameGroupActorName(group, item, iter);
    } else {
        ksys::act::getSameGroupActorName(group, item);
    }
}

}  // namespace

int pouchItemSortPredicate(const PouchItem* lhs, const PouchItem* rhs);
int pouchItemSortPredicateForArrow(const PouchItem* lhs, const PouchItem* rhs);

PauseMenuDataMgr::PauseMenuDataMgr() {
    mListHeads.fill(nullptr);
    for (s32 i = 0; i < NumPouch50; ++i) {
        mArray1[i] = nullptr;
        mArray2[i] = PouchItemType::Invalid;
    }
    for (auto& x : mArray3)
        x = {};
    _447e0 = {};
    _447e8 = {};
    _447f0 = {};
    _447f8 = {};
}

PauseMenuDataMgr::~PauseMenuDataMgr() = default;

PouchItem::PouchItem() {
    mData.cook.mCookEffect0 = sDummyCookEffect0;
    for (s32 i = 0; i < NumIngredientsMax; ++i)
        mIngredients.emplaceBack();
}

void PauseMenuDataMgr::resetItem() {
    mNewlyAddedItem.mType = PouchItemType::Invalid;
    mNewlyAddedItem._1c = -1;
    mNewlyAddedItem.mValue = 0;
    mNewlyAddedItem.mValid = false;
    mNewlyAddedItem._25 = 0;
    mNewlyAddedItem.mName.clear();
    mNewlyAddedItem.mData.cook = {};
    mNewlyAddedItem.mData.cook.mCookEffect0 = sDummyCookEffect0;
}

void PauseMenuDataMgr::setItemModifier(PouchItem& item, const act::WeaponModifierInfo* modifier) {
    if (modifier && !modifier->flags.isZero()) {
        item.setWeaponAddType(modifier->flags.getDirect());
        item.setWeaponAddValue(static_cast<u32>(modifier->value));
    } else {
        item.setWeaponAddType(0);
    }
}

void PauseMenuDataMgr::init(sead::Heap* heap) {}

void PauseMenuDataMgr::initForNewSave() {
    const auto lock = sead::makeScopedLock(mCritSection);

    for (auto* item = getItems().popFront(); item; item = getItems().popFront()) {
        item->~PouchItem();
        new (item) PouchItem;
        mItemLists.list2.pushFront(item);
    }

    mListHeads.fill(nullptr);
    for (s32 i = 0; i < NumPouch50; ++i) {
        mArray1[i] = nullptr;
        mArray2[i] = PouchItemType::Invalid;
    }

    _44498 = {};
    ksys::gdt::setFlag_KorokNutsNum(0);
    ksys::gdt::setFlag_DungeonClearSealNum(0);
    ksys::gdt::setFlag_FairyCountCheck(0);
    _444fc = {};
    mItem_44488 = {};
    mItem_444f0 = {};
    _444f8 = -1;
    resetItem();
    mIsPouchForQuest = false;
    for (auto& x : mArray3)
        x = {};
    _44504 = {};
    _44508 = {};
    _4450c = {};
    _44510 = {};
    _44514 = {};
    mRitoSoulItem = {};
    mGoronSoulItem = {};
    mZoraSoulItem = {};
    mGerudoSoulItem = {};
    _44538 = false;
    _447e0 = {};
    _447e8 = {};
    _447f0 = {};
    _447f8 = {};

    auto* player = ksys::act::PlayerInfo::instance()->getPlayer();
    if (player) {
        player->switchEquipment(getDefaultEquipment(EquipmentSlot::WeaponRight), 1);
        player->switchEquipment(getDefaultEquipment(EquipmentSlot::WeaponLeft), 1);
        player->switchEquipment(getDefaultEquipment(EquipmentSlot::WeaponBow), 1);
        player->switchEquipment(getDefaultEquipment(EquipmentSlot::ArmorHead), 30);
        player->switchEquipment(getDefaultEquipment(EquipmentSlot::ArmorUpper), 30);
        player->switchEquipment(getDefaultEquipment(EquipmentSlot::ArmorLower), 30);
    }
}

void PauseMenuDataMgr::loadFromGameData() {
    doLoadFromGameData();

    for (auto& x : mArray3)
        x = {};
    mItem_44488 = {};
    mItem_444f0 = {};
    _444f8 = -1;
    resetItem();
    _444fc = 0;
    mIsPouchForQuest = false;
    _447e0 = {};
    _447e8 = {};
    _447f0 = {};
    _447f8 = {};

    const auto lock = sead::makeScopedLock(mCritSection);
    updateInventoryInfo(getItems());
}

bool PauseMenuDataMgr::cannotGetItem(const sead::SafeString& name, int n) const {
    namespace act = ksys::act;

    al::ByamlIter iter;
    if (!act::InfoData::instance()->getActorIter(&iter, name.cstr()))
        return true;

    if (!act::InfoData::instance()->hasTag(iter, act::tags::CanGetPouch))
        return true;

    if (act::InfoData::instance()->getSortKey(name.cstr()) == 99999)
        return true;

    const auto lock = sead::makeScopedLock(mCritSection);
    const auto type = getType(name);

    if (act::InfoData::instance()->hasTag(iter, act::tags::CanStack))
        return !hasFreeSpaceForItem(mItemLists, name, n);

    if (type <= PouchItemType::Shield) {
        switch (type) {
        case PouchItemType::Weapon: {
            const auto limit = ksys::gdt::getFlag_WeaponPorchStockNum();
            return isList2Empty() || limit < countItems(PouchItemType::Weapon) + n;
        }
        case PouchItemType::Shield: {
            const auto limit = ksys::gdt::getFlag_ShieldPorchStockNum();
            return isList2Empty() || limit < countItems(PouchItemType::Shield) + n;
        }
        case PouchItemType::Bow: {
            const auto limit = ksys::gdt::getFlag_BowPorchStockNum();
            return isList2Empty() || limit < countItems(PouchItemType::Bow) + n;
        }
        default: {
            const auto limit = ksys::gdt::getFlag_WeaponPorchStockNum();
            return isList2Empty() || limit < countItems(PouchItemType::Weapon, true) + n;
        }
        }
    }

    if (type == PouchItemType::Food)
        return isList2Empty() || countItems(PouchItemType::Food, true) + n > NumFoodMax;

    if (type == PouchItemType::Material)
        return isList2Empty() || countItems(PouchItemType::Material) + n > NumMaterialsMax;

    if (type == PouchItemType::KeyItem)
        return isList2Empty() || countItems(PouchItemType::KeyItem) + n > NumKeyItemsMax;

    return isList2Empty() || countItems(PouchItemType::ArmorHead) + n > NumArmorsMax;
}

PouchItemType PauseMenuDataMgr::getType(const sead::SafeString& item, al::ByamlIter* iter) {
    sead::SafeString group;
    getSameGroupActorName(&group, item, iter);

    al::ByamlIter actor_iter;
    if (iter && iter->isValid() && group == item) {
        actor_iter = *iter;
    } else {
        if (!ksys::act::InfoData::instance()->getActorIter(&actor_iter, group.cstr()))
            return PouchItemType::Invalid;
    }

    if (ksys::act::InfoData::instance()->hasTag(actor_iter, ksys::act::tags::Arrow))
        return PouchItemType::Arrow;

    const char* profile_c;
    if (!ksys::act::InfoData::instance()->getActorProfile(&profile_c, actor_iter))
        return PouchItemType::Invalid;

    const sead::SafeString profile = profile_c;

    if (profile == sValues.WeaponSmallSword)
        return PouchItemType::Weapon;
    if (profile == sValues.WeaponLargeSword)
        return PouchItemType::Weapon;
    if (profile == sValues.WeaponSpear)
        return PouchItemType::Weapon;

    if (profile == sValues.WeaponBow)
        return PouchItemType::Bow;

    if (profile == sValues.WeaponShield)
        return PouchItemType::Shield;

    if (profile == sValues.ArmorHead)
        return PouchItemType::ArmorHead;
    if (profile == sValues.ArmorUpper)
        return PouchItemType::ArmorUpper;
    if (profile == sValues.ArmorLower)
        return PouchItemType::ArmorLower;

    if (profile == sValues.HorseReins)
        return PouchItemType::KeyItem;

    if (ksys::act::InfoData::instance()->hasTag(actor_iter, ksys::act::tags::CookResult))
        return PouchItemType::Food;
    if (ksys::act::InfoData::instance()->hasTag(actor_iter, ksys::act::tags::RoastItem))
        return PouchItemType::Food;

    if (ksys::act::InfoData::instance()->hasTag(actor_iter, ksys::act::tags::Important))
        return PouchItemType::KeyItem;

    return PouchItemType::Material;
}

bool PauseMenuDataMgr::hasFreeSpaceForItem(const PauseMenuDataMgr::Lists& lists,
                                           const sead::SafeString& name, int n) const {
    sead::SafeString group_name;
    getSameGroupActorName(&group_name, name);

    const int count = getItemCount(group_name);
    const bool is_arrow =
        ksys::act::InfoData::instance()->hasTag(name.cstr(), ksys::act::tags::Arrow);

    if (count == 0 && is_arrow) {
        for (const auto& item : lists.list1) {
            if (item.getType() == PouchItemType::Arrow && group_name == item.getName())
                return true;
        }

        return countItems(PouchItemType::Arrow) < NumArrowItemsMax;
    }

    if (count == 0) {
        const auto type = getType(name);

        if (type == PouchItemType::Food)
            return !lists.list2.isEmpty() && countItems(PouchItemType::Food, true) < NumFoodMax;

        if (type == PouchItemType::Material)
            return !lists.list2.isEmpty() && countItems(PouchItemType::Material) < NumMaterialsMax;

        if (type == PouchItemType::KeyItem)
            return !lists.list2.isEmpty() && countItems(PouchItemType::KeyItem) < NumKeyItemsMax;

        return false;
    }

    return count + n <= ItemStackSizeMax;
}

int PauseMenuDataMgr::countItems(PouchItemType type, bool count_any_weapon) const {
    if (isPouchItemInvalid(type) || getItems().isEmpty())
        return 0;

    const auto& list = getItems();

    if (count_any_weapon) {
        PouchItem** head = nullptr;

        if (type <= PouchItemType::Shield) {
            int count = 0;
            for (auto* item = list.nth(0); item && item->getType() <= PouchItemType::Shield;
                 item = list.next(item)) {
                count += item->get25();
            }
            return count;

        } else if (type <= PouchItemType::ArmorLower) {
            int count = 0;
            for (auto* item = getItemHead(PouchCategory::Armor);
                 item && item->getType() <= PouchItemType::ArmorLower; item = list.next(item)) {
                count += item->get25();
            }
            return count;

        } else if (type == PouchItemType::Material) {
            head = getItemHeadp(PouchCategory::Material);
            if (!head)
                return 0;

        } else if (type == PouchItemType::Food) {
            head = getItemHeadp(PouchCategory::Food);
            if (!head)
                return 0;

        } else if (type == PouchItemType::KeyItem) {
            head = getItemHeadp(PouchCategory::KeyItem);
            if (!head)
                return 0;

        } else {
            return 0;
        }

        if (!head)
            return 0;

        int count = 0;
        for (auto* item = *head; item && item->getType() == type; item = list.next(item)) {
            count += item->get25();
        }
        return count;
    }

    PouchItem* first = nullptr;

    if (type == PouchItemType::Weapon) {
        first = getItemHead(PouchCategory::Weapon);

    } else if (type == PouchItemType::Bow) {
        first = getItemHead(PouchCategory::Bow);

    } else if (type == PouchItemType::Arrow) {
        for (auto* item = getItemHead(PouchCategory::Bow);
             item && item->getType() <= PouchItemType::Arrow; item = list.next(item)) {
            if (item->getType() == PouchItemType::Arrow) {
                first = item;
                break;
            }
        }

    } else if (type == PouchItemType::Shield) {
        first = getItemHead(PouchCategory::Shield);

    } else if (type <= PouchItemType::ArmorLower) {
        int count = 0;
        for (auto* item = getItemHead(PouchCategory::Armor);
             item && item->getType() <= PouchItemType::ArmorLower; item = list.next(item)) {
            count += item->get25();
        }
        return count;

    } else if (type == PouchItemType::Material) {
        first = getItemHead(PouchCategory::Material);

    } else if (type == PouchItemType::Food) {
        first = getItemHead(PouchCategory::Food);

    } else if (type == PouchItemType::KeyItem) {
        first = getItemHead(PouchCategory::KeyItem);

    } else {
        return 0;
    }

    int count = 0;
    for (auto* item = first; item && item->getType() == type; item = list.next(item)) {
        count += item->get25();
    }
    return count;
}

bool PauseMenuDataMgr::isWeaponSectionFull(const sead::SafeString& weapon_type) const {
    const auto lock = sead::makeScopedLock(mCritSection);

    const auto check = [this](auto get_flag, PouchCategory category, PouchItemType type) {
        const s32 num_slots = get_flag(false);
        if (mItemLists.list2.isEmpty())
            return true;

        s32 num = 0;
        if (!getItems().isEmpty()) {
            for (auto item = getItemHead(category); item; item = nextItem(item)) {
                if (item->getType() != type)
                    break;
                num += item->_25;
            }
        }

        return num_slots <= num;
    };

    if (weapon_type == sValues.WeaponSmallSword || weapon_type == sValues.WeaponLargeSword ||
        weapon_type == sValues.WeaponSpear) {
        return check(ksys::gdt::getFlag_WeaponPorchStockNum, PouchCategory::Weapon,
                     PouchItemType::Weapon);
    }

    if (weapon_type == sValues.WeaponBow) {
        return check(ksys::gdt::getFlag_BowPorchStockNum, PouchCategory::Bow, PouchItemType::Bow);
    }

    if (weapon_type == sValues.WeaponShield) {
        return check(ksys::gdt::getFlag_ShieldPorchStockNum, PouchCategory::Shield,
                     PouchItemType::Shield);
    }

    return false;
}

void PauseMenuDataMgr::itemGet(const sead::SafeString& name, int value,
                               const act::WeaponModifierInfo* modifier) {
    if (name.include("Default") || name.include("Extra"))
        return;

    const auto type = getType(name);

    mNewlyAddedItem.mType = type;
    mNewlyAddedItem.mValue = value;
    mNewlyAddedItem._25 = 1;
    mNewlyAddedItem.mName = name;
    mNewlyAddedItem.mValid = false;

    if (modifier) {
        setItemModifier(mNewlyAddedItem, modifier);
        const auto add_type = modifier->flags.getDirect();
        const auto add_value = mNewlyAddedItem.getWeaponAddValue();
        sValues.last_added_weapon_add_type = add_type;
        sValues.last_added_weapon_add_value = add_value;
    }

    const auto lock = sead::makeScopedLock(mCritSection);
    auto& items = getItems();
    ksys::PlayReportMgr::instance()->reportDebug("PouchGet", name);
    addToPouch(name, type, items, value, false, modifier);
    saveToGameData(items);
}

void PauseMenuDataMgr::updateAfterAddingItem(bool only_sort) {
    const auto lock = sead::makeScopedLock(mCritSection);

    if (getItems().isEmpty())
        return;

    _44800 = PouchCategory::Invalid;
    auto& items = getItems();
    items.sort(pouchItemSortPredicateForArrow);
    if (!only_sort) {
        updateInventoryInfo(items);
        updateListHeads();
        saveToGameData(items);
    }
}

void PauseMenuDataMgr::updateListHeads() {
    mListHeads.fill(nullptr);
    for (s32 i = 0; i < NumPouch50; ++i) {
        auto& ptr = mArray1[i];
        switch (mArray2[i]) {
        case PouchItemType::Weapon:
            if (!mListHeads[s32(PouchCategory::Weapon)])
                mListHeads[s32(PouchCategory::Weapon)] = &ptr;
            break;
        case PouchItemType::Bow:
        case PouchItemType::Arrow:
            if (!mListHeads[s32(PouchCategory::Bow)])
                mListHeads[s32(PouchCategory::Bow)] = &ptr;
            break;
        case PouchItemType::Shield:
            if (!mListHeads[s32(PouchCategory::Shield)])
                mListHeads[s32(PouchCategory::Shield)] = &ptr;
            break;
        case PouchItemType::ArmorHead:
        case PouchItemType::ArmorUpper:
        case PouchItemType::ArmorLower:
            if (!mListHeads[s32(PouchCategory::Armor)])
                mListHeads[s32(PouchCategory::Armor)] = &ptr;
            break;
        case PouchItemType::Material:
            if (!mListHeads[s32(PouchCategory::Material)])
                mListHeads[s32(PouchCategory::Material)] = &ptr;
            break;
        case PouchItemType::Food:
            if (!mListHeads[s32(PouchCategory::Food)])
                mListHeads[s32(PouchCategory::Food)] = &ptr;
            break;
        case PouchItemType::KeyItem:
            if (!mListHeads[s32(PouchCategory::KeyItem)])
                mListHeads[s32(PouchCategory::KeyItem)] = &ptr;
            break;
        case PouchItemType::Invalid:
            break;
        }
    }
}

void PauseMenuDataMgr::removeArrow(const sead::SafeString& arrow_name, int count) {
    if (!ksys::act::InfoData::instance()->hasTag(arrow_name.cstr(), ksys::act::tags::Arrow))
        return;

    const auto lock = sead::makeScopedLock(mCritSection);
    s32 idx = 0;
    for (auto& item : getItems()) {
        if (arrow_name == item.getName()) {
            if (count < item.getCount()) {
                item.mValue -= count;
                break;
            }

            count -= item.getCount();
            item.mValue = 0;
        }
        ++idx;
    }

    const auto num = getItemCount(arrow_name);
    if (!mIsPouchForQuest && idx >= 0)
        ksys::gdt::setFlag_PorchItem_Value1(num, idx);
}

void PauseMenuDataMgr::setWeaponItemValue(s32 value, PouchItemType type) {
    if (isPouchItemNotWeapon(type))
        return;

    const auto lock = sead::makeScopedLock(mCritSection);

    s32 idx = 0;
    for (auto& item : getItems()) {
        if (!item.isValid() || item.getType() != type) {
            ++idx;
            continue;
        }

        item.setValue(value);
        if (idx >= 0 && !mIsPouchForQuest)
            ksys::gdt::setFlag_PorchItem_Value1(value, idx);

        return;
    }
}

const sead::SafeString& PauseMenuDataMgr::getDefaultEquipment(EquipmentSlot idx) const {
    if (idx != EquipmentSlot::WeaponArrow &&
        u32(idx) < u32(sValues.default_equipment_names.size())) {
        return sValues.default_equipment_names(u32(idx));
    }
    return sead::SafeString::cEmptyString;
}

bool PauseMenuDataMgr::hasItem(const sead::SafeString& name) const {
    const auto lock = sead::makeScopedLock(mCritSection);
    bool ret = false;
    for (auto& item : getItems()) {
        if (name != item.getName())
            continue;

        if (ksys::act::InfoData::instance()->hasTag(name.cstr(), ksys::act::tags::CanStack))
            ret = item.getValue() > 0;
        else
            ret = true;
        break;
    }
    return ret;
}

PouchItem* PauseMenuDataMgr::getMasterSword() const {
    const auto lock = sead::makeScopedLock(mCritSection);

    for (auto* item = getItemHead(PouchCategory::Weapon); item; item = nextItem(item)) {
        if (item->getType() != PouchItemType::Weapon)
            return nullptr;
        if (item->_25 && item->getName() == "Weapon_Sword_070")
            return item;
    }

    return nullptr;
}

int PauseMenuDataMgr::getArrowCount(const sead::SafeString& name) const {
    const auto lock = sead::makeScopedLock(mCritSection);
    for (auto item = getItemHead(PouchCategory::Bow); item; item = nextItem(item)) {
        if (item->getType() > PouchItemType::Arrow)
            break;
        if (item->getType() == PouchItemType::Arrow && item->_25 && item->getName() == name)
            return item->getCount();
    }
    return 0;
}

int PauseMenuDataMgr::getRealArrowCount(const sead::SafeString& name) const {
    if (!mIsPouchForQuest)
        return getArrowCount(name);

    s32 count;
    s32 status = 2;
    for (u32 i = 0; i < u32(NumPouchItemsMax); ++i) {
        const char* item_name;
        ksys::gdt::getFlag_PorchItem(&item_name, i);
        if (sead::SafeString(item_name).isEmpty())
            break;
        if (sead::SafeString(item_name) == name) {
            count = ksys::gdt::getFlag_PorchItem_Value1(i);
            status = 1;
            break;
        }
    }
    return status == 2 ? 0 : count;
}

void PauseMenuDataMgr::breakMasterSword() {
    const auto lock = sead::makeScopedLock(mCritSection);
    s32 idx = 0;
    for (auto& item : getItems()) {
        if (item.getType() == PouchItemType::Weapon && isMasterSwordActorName(item.getName())) {
            item.mValue = 0;
            item.mValid = false;
            if (!mIsPouchForQuest && idx >= 0) {
                ksys::gdt::setFlag_PorchItem_Value1(0, idx);
                ksys::gdt::setFlag_PorchItem_EquipFlag(false, idx);
            }
            break;
        }
        ++idx;
    }
}

void PauseMenuDataMgr::restoreMasterSword(bool only_if_broken) {
    const auto lock = sead::makeScopedLock(mCritSection);
    s32 idx = 0;
    for (auto& item : getItems()) {
        if (item.getType() == PouchItemType::Weapon && isMasterSwordActorName(item.getName())) {
            if (only_if_broken && item.getValue() > 0)
                break;

            item.mValue = getWeaponInventoryLife(item.getName());
            if (!mIsPouchForQuest && idx >= 0)
                ksys::gdt::setFlag_PorchItem_Value1(item.mValue, idx);

            break;
        }
        ++idx;
    }
}

using SortPredicate = int (*)(const PouchItem* lhs, const PouchItem* rhs,
                              ksys::act::InfoData* data);

static PouchCategory getTypeForCategory(PouchItemType type) {
    static constexpr sead::SafeArray<PouchCategory, NumPouchItemTypes> sMap{{
        PouchCategory::Weapon,    // Weapon
        PouchCategory::Bow,       // Bow
        PouchCategory::Bow,       // Arrow
        PouchCategory::Shield,    // Shield
        PouchCategory::Armor,     // ArmorHead
        PouchCategory::Armor,     // ArmorUpper
        PouchCategory::Armor,     // ArmorLower
        PouchCategory::Material,  // Material
        PouchCategory::Food,      // Food
        PouchCategory::KeyItem,   // KeyItem
    }};
    return sMap[s32(type)];
}

static auto getSortPredicateTable() {
    sead::SafeArray<SortPredicate, 7> table{
        {sortWeapon, sortBow, sortShield, sortArmor, sortMaterial, sortFood, sortKeyItem}};
    return table;
}

int pouchItemSortPredicate(const PouchItem* lhs, const PouchItem* rhs) {
    if (!lhs || !rhs)
        return 0;

    auto* info_data = ksys::act::InfoData::instance();
    if (!info_data || !lhs->get25() || !rhs->get25())
        return 0;

    const auto cat1 = getTypeForCategory(lhs->getType());
    const auto cat2 = getTypeForCategory(rhs->getType());
    if (cat1 != cat2)
        return 0;

    const auto cat3 = PauseMenuDataMgr::instance()->get44800();
    if (cat3 != PouchCategory::Invalid && cat1 != cat3)
        return 0;

    auto predicate_table = getSortPredicateTable();
    const auto* fn = &predicate_table[0];
    if (u32(cat1) < u32(predicate_table.size()))
        fn = &predicate_table(u32(cat1));
    return (*fn)(lhs, rhs, info_data);
}

static s32 compareSortKeys(const PouchItem* lhs, const PouchItem* rhs, ksys::act::InfoData* data) {
    const auto a = data->getSortKey(lhs->getName().cstr());
    const auto b = data->getSortKey(rhs->getName().cstr());
    if (a < b)
        return -1;
    if (a > b)
        return 1;
    return 0;
}

int sortArmor(const PouchItem* lhs, const PouchItem* rhs, ksys::act::InfoData* data) {
    if (ksys::gdt::getFlag_SortTypeArmorPouch()) {
        if (auto cmp = compareSortKeys(lhs, rhs, data))
            return cmp;

        if (lhs->getType() < rhs->getType())
            return -1;
        if (lhs->getType() > rhs->getType())
            return 1;
    } else {
        if (lhs->getType() < rhs->getType())
            return -1;
        if (lhs->getType() > rhs->getType())
            return 1;

        if (auto cmp = compareSortKeys(lhs, rhs, data))
            return cmp;
    }

    if (lhs->getCount() < rhs->getCount())
        return -1;
    if (lhs->getCount() > rhs->getCount())
        return 1;

    return 0;
}

int sortKeyItem(const PouchItem* lhs, const PouchItem* rhs, ksys::act::InfoData* data) {
    if (auto cmp = compareSortKeys(lhs, rhs, data))
        return cmp;
    return 0;
}

int getCookItemOrder(const PouchItem* item, ksys::act::InfoData* data) {
    const auto& order = sValues.cook_item_order;
    al::ByamlIter iter;
    if (data->getActorIter(&iter, item->getName().cstr())) {
        for (auto it = order.begin(), end = order.end(); it != end; ++it) {
            if (it->is_tag == 0 && item->getName() == it->name.cstr())
                return it.getIndex();
        }

        for (auto it = order.begin(), end = order.end(); it != end; ++it) {
            if (it->is_tag == 1 && data->hasTag(iter, it->hash))
                return it.getIndex();
        }
    }
    return order.size();
}

int pouchItemSortPredicateForArrow(const PouchItem* lhs, const PouchItem* rhs) {
    if (!lhs || !rhs)
        return 0;

    static constexpr sead::SafeArray<int, NumPouchItemTypes> sMap{{0, 1, 2, 3, 4, 4, 4, 7, 8, 9}};
    const auto x1 = sMap[s32(lhs->getType())];
    const auto x2 = sMap[s32(rhs->getType())];
    if (x1 < x2)
        return -1;
    if (x1 > x2)
        return 1;

    if (lhs->getType() != PouchItemType::Arrow)
        return 0;

    auto* info_data = ksys::act::InfoData::instance();
    if (!info_data || !lhs->get25() || !rhs->get25())
        return 0;

    const auto cat1 = getTypeForCategory(lhs->getType());
    const auto cat2 = getTypeForCategory(rhs->getType());
    if (cat1 != cat2)
        return 0;

    const auto cat3 = PauseMenuDataMgr::instance()->get44800();
    if (cat3 != PouchCategory::Invalid && cat1 != cat3)
        return 0;

    const auto predicate_table = getSortPredicateTable();
    const auto* fn = &predicate_table[0];
    if (u32(cat1) < u32(predicate_table.size()))
        fn = &predicate_table(u32(cat1));
    return (*fn)(lhs, rhs, info_data);
}

bool PauseMenuDataMgr::isHeroSoulEnabled(const sead::SafeString& name) const {
    if (name == sValues.Obj_HeroSoul_Zora || name == sValues.Obj_DLC_HeroSoul_Zora) {
        if (mZoraSoulItem)
            return mZoraSoulItem->isValid();
    }

    if (name == sValues.Obj_HeroSoul_Rito || name == sValues.Obj_DLC_HeroSoul_Rito) {
        if (mRitoSoulItem)
            return mRitoSoulItem->isValid();
    }

    if (name == sValues.Obj_HeroSoul_Goron || name == sValues.Obj_DLC_HeroSoul_Goron) {
        if (mGoronSoulItem)
            return mGoronSoulItem->isValid();
    }

    if (name == sValues.Obj_HeroSoul_Gerudo || name == sValues.Obj_DLC_HeroSoul_Gerudo) {
        if (mGerudoSoulItem)
            return mGerudoSoulItem->isValid();
    }

    return false;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool PauseMenuDataMgr::hasRitoSoulPlus() const {
    return ksys::gdt::getFlag_IsGet_Obj_DLC_HeroSoul_Rito() && aoc::Manager::instance()->hasAoc3();
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool PauseMenuDataMgr::hasGoronSoulPlus() const {
    return ksys::gdt::getFlag_IsGet_Obj_DLC_HeroSoul_Goron() && aoc::Manager::instance()->hasAoc3();
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool PauseMenuDataMgr::hasGerudoSoulPlus() const {
    return ksys::gdt::getFlag_IsGet_Obj_DLC_HeroSoul_Gerudo() &&
           aoc::Manager::instance()->hasAoc3();
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool PauseMenuDataMgr::hasZoraSoulPlus() const {
    return ksys::gdt::getFlag_IsGet_Obj_DLC_HeroSoul_Zora() && aoc::Manager::instance()->hasAoc3();
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void PauseMenuDataMgr::updateDivineBeastClearFlags(int num_cleared_beasts) {
    switch (num_cleared_beasts) {
    case 1:
        ksys::gdt::setFlag_Find_4Relic_1stClear(true);
        break;
    case 2:
        ksys::gdt::setFlag_Find_4Relic_2ndClear(true);
        ksys::gdt::setFlag_BattleArena_Level1(false, true);
        ksys::gdt::setFlag_BattleArena_Level2(true, true);
        break;
    case 3:
        ksys::gdt::setFlag_Find_4Relic_3rdClear(true);
        ksys::gdt::setFlag_BattleArena_Level1(false, true);
        ksys::gdt::setFlag_BattleArena_Level2(false, true);
        ksys::gdt::setFlag_BattleArena_Level3(true, true);
        break;
    case 4:
        ksys::gdt::setFlag_Find_4Relic_4thClear(true);
        ksys::gdt::setFlag_BattleArena_Level1(false, true);
        ksys::gdt::setFlag_BattleArena_Level2(false, true);
        ksys::gdt::setFlag_BattleArena_Level3(false, true);
        ksys::gdt::setFlag_BattleArena_Level4(true, true);
        break;
    default:
        break;
    }
}

bool PauseMenuDataMgr::isOverCategoryLimit(PouchItemType type) const {
    const auto count = countItems(type);
    switch (type) {
    case PouchItemType::Weapon:
        return ksys::gdt::getFlag_WeaponPorchStockNum() <= count || count >= NumWeaponsMax;
    case PouchItemType::Bow:
        return ksys::gdt::getFlag_BowPorchStockNum() <= count || count >= NumBowsMax;
    case PouchItemType::Arrow:
        return count >= NumArrowItemsMax;
    case PouchItemType::Shield:
        return ksys::gdt::getFlag_ShieldPorchStockNum() <= count || count >= NumShieldsMax;
    case PouchItemType::ArmorHead:
    case PouchItemType::ArmorUpper:
    case PouchItemType::ArmorLower:
        return count >= NumArmorsMax;
    case PouchItemType::Material:
        return count >= NumMaterialsMax;
    case PouchItemType::Food:
        return count >= NumFoodMax;
    case PouchItemType::KeyItem:
        return count >= NumKeyItemsMax;
    case PouchItemType::Invalid:
        break;
    }
    return true;
}

void PauseMenuDataMgr::openItemCategoryIfNeeded() const {
    for (s32 i = 0; i < NumPouch50; ++i) {
        const auto type = mArray2[i];
        if (isPouchItemArmor(type)) {
            ksys::gdt::setFlag_IsOpenItemCategory(true, u32(PouchCategory::Armor));
        } else {
            switch (type) {
            case PouchItemType::Weapon:
                ksys::gdt::setFlag_IsOpenItemCategory(true, u32(PouchCategory::Weapon));
                break;
            case PouchItemType::Bow:
                ksys::gdt::setFlag_IsOpenItemCategory(true, u32(PouchCategory::Bow));
                break;
            case PouchItemType::Shield:
                ksys::gdt::setFlag_IsOpenItemCategory(true, u32(PouchCategory::Shield));
                break;
            case PouchItemType::Material:
                ksys::gdt::setFlag_IsOpenItemCategory(true, u32(PouchCategory::Material));
                break;
            case PouchItemType::Food:
                ksys::gdt::setFlag_IsOpenItemCategory(true, u32(PouchCategory::Food));
                break;
            case PouchItemType::KeyItem:
                ksys::gdt::setFlag_IsOpenItemCategory(true, u32(PouchCategory::KeyItem));
                break;
            default:
                break;
            }
        }
    }
}

}  // namespace uking::ui
