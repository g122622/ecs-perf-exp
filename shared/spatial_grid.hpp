#pragma once

#include "constants.hpp"
#include "entity_types.hpp"
#include <cmath>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace shared {

// 空间网格 - 用于加速邻近查询
// 将 O(n²) 的目标搜索降为 O(n)
class SpatialGrid {
public:
    // 网格单元大小，应该 >= 最大感知范围 (DEAGGRO_RANGE = 32.0f)
    static constexpr float CELL_SIZE = 32.0f;

    // 网格维度
    static constexpr int GRID_SIZE_X = static_cast<int>(WorldConstants::WORLD_SIZE_X / CELL_SIZE) + 1;
    static constexpr int GRID_SIZE_Z = static_cast<int>(WorldConstants::WORLD_SIZE_Z / CELL_SIZE) + 1;

    // 网格键
    struct CellKey {
        int16_t cx;
        int16_t cz;

        bool operator==(const CellKey& other) const {
            return cx == other.cx && cz == other.cz;
        }
    };

    struct CellKeyHash {
        size_t operator()(const CellKey& k) const {
            return static_cast<size_t>(k.cx) * 131 + static_cast<size_t>(k.cz);
        }
    };

    // 实体信息
    struct EntityEntry {
        uint32_t id;           // 实体ID
        float x, y, z;         // 位置
        bool isMonster;        // 是否是怪物
        bool isAnimal;         // 是否是动物
        bool isFlying;         // 是否飞行
        bool isAquatic;        // 是否水生
        bool isDead;           // 是否死亡

        // 用于快速访问的组件指针（ECS）或标记
        void* userData;        // ECS: 指向组件数组，OOP: 指向实体指针
    };

private:
    std::unordered_map<CellKey, std::vector<EntityEntry>, CellKeyHash> cells_;
    std::vector<EntityEntry> empty_;  // 空结果占位

public:
    void clear() {
        cells_.clear();
    }

    // 计算网格坐标
    static CellKey toCellKey(float x, float z) {
        return {
            static_cast<int16_t>(std::floor(x / CELL_SIZE)),
            static_cast<int16_t>(std::floor(z / CELL_SIZE))
        };
    }

    // 添加实体到网格
    void insert(const EntityEntry& entry) {
        CellKey key = toCellKey(entry.x, entry.z);
        cells_[key].push_back(entry);
    }

    // 查询指定位置和范围内的所有实体
    // 返回包含实体的单元格列表（最多9个单元格：当前+周围8个）
    std::vector<EntityEntry> query(float x, float z, float range) const {
        std::vector<EntityEntry> result;

        int16_t minCx = static_cast<int16_t>(std::floor((x - range) / CELL_SIZE));
        int16_t maxCx = static_cast<int16_t>(std::floor((x + range) / CELL_SIZE));
        int16_t minCz = static_cast<int16_t>(std::floor((z - range) / CELL_SIZE));
        int16_t maxCz = static_cast<int16_t>(std::floor((z + range) / CELL_SIZE));

        // 限制在世界范围内
        minCx = std::max(minCx, static_cast<int16_t>(0));
        maxCx = std::min(maxCx, static_cast<int16_t>(GRID_SIZE_X - 1));
        minCz = std::max(minCz, static_cast<int16_t>(0));
        maxCz = std::min(maxCz, static_cast<int16_t>(GRID_SIZE_Z - 1));

        for (int16_t cx = minCx; cx <= maxCx; ++cx) {
            for (int16_t cz = minCz; cz <= maxCz; ++cz) {
                CellKey key{cx, cz};
                auto it = cells_.find(key);
                if (it != cells_.end()) {
                    result.insert(result.end(), it->second.begin(), it->second.end());
                }
            }
        }

        return result;
    }

    // 查询单个单元格（用于优化已知在同一单元格的情况）
    const std::vector<EntityEntry>& getCell(float x, float z) const {
        CellKey key = toCellKey(x, z);
        auto it = cells_.find(key);
        if (it != cells_.end()) {
            return it->second;
        }
        return empty_;
    }

    // 预估实体数量
    size_t size() const {
        size_t count = 0;
        for (const auto& [key, entries] : cells_) {
            count += entries.size();
        }
        return count;
    }
};

} // namespace shared
