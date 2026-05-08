# ECS vs OOP Performance Benchmark

A comprehensive performance comparison between **Object-Oriented Programming (OOP)**, **Entity-Component-System (ECS)**, and **Hybrid** architectures for game development scenarios.

## Overview

This project implements the same game simulation logic using three different architectural approaches:

1. **OOP (Object-Oriented Programming)** - Traditional inheritance-based hierarchy
2. **Hybrid** - Performance-critical components in ECS, behavior-rich logic in OOP
3. **ECS (EnTT)** - Pure archetype-based Entity-Component-System using the [EnTT](https://github.com/skypjack/entt) library

All implementations feature identical:
- 22 Minecraft-inspired entity types (Monsters, Animals, Flying, Aquatic)
- AI state machines (Idle, Wander, Chase, Flee)
- Physics simulation
- Combat system with target acquisition
- Health regeneration

## Project Structure

```
ecs-perf-exp/
├── CMakeLists.txt              # Root CMake configuration
├── run_benchmark.bat           # Windows batch script
├── run_benchmark.ps1           # PowerShell script
│
├── benchmark/                  # Benchmark framework
│   ├── include/
│   │   ├── timer.hpp           # High-precision timer
│   │   ├── memory_tracker.hpp  # Memory allocation tracking
│   │   ├── hw_counter.hpp      # Hardware counters
│   │   └── benchmark_runner.hpp
│   └── src/
│
├── oop/                        # OOP Implementation
│   ├── include/
│   │   ├── entity.hpp          # Base Entity class
│   │   ├── living_entity.hpp   # Health component
│   │   ├── creature.hpp        # AI state machine
│   │   ├── monster.hpp         # Hostile entities
│   │   ├── animal.hpp          # Passive entities
│   │   ├── flying_creature.hpp
│   │   ├── aquatic_creature.hpp
│   │   ├── world.hpp           # World management
│   │   └── entities/           # 22 concrete entity types
│   └── src/
│
├── ecs_entt/                   # Pure ECS Implementation
│   ├── include/
│   │   ├── components.hpp      # Component definitions
│   │   └── world.hpp           # System implementations
│   └── src/
│       └── world.cpp
│
├── hybrid/                     # Hybrid Implementation
│   ├── include/
│   │   ├── components.hpp      # High-frequency ECS components
│   │   ├── hybrid_entity.hpp   # OOP entity with special behaviors
│   │   └── world.hpp           # World + system management
│   └── src/
│       ├── world.cpp
│       ├── hybrid_entity.cpp
│       └── systems/
│           ├── ai_system.cpp
│           ├── movement_system.cpp
│           └── interaction_system.cpp
│
├── shared/                     # Shared configuration
│   ├── entity_types.hpp        # Entity type definitions
│   ├── behavior_rules.hpp      # Shared AI behavior logic
│   ├── spatial_grid.hpp        # Spatial partitioning
│   ├── constants.hpp           # World constants
│   └── benchmark_config.hpp    # Test configurations
│
├── third_party/
│   └── entt.hpp               # EnTT single-header library
│
├── main_oop.cpp               # OOP benchmark entry
├── main_ecs_entt.cpp          # Pure ECS benchmark entry
├── main_hybrid.cpp            # Hybrid benchmark entry
│
└── scripts/
    └── visualize.py           # Results visualization
```

## Requirements

- **Compiler**: MSVC 2022 (C++20 support)
- **CMake**: 3.20+
- **Python**: 3.8+ (for visualization)
- **Python packages**: pandas, matplotlib, numpy

## Build & Run

### Quick Start

```powershell
# Using PowerShell (recommended)
.\run_benchmark.ps1

# Or using batch file
.\run_benchmark.bat
```

### Manual Build

```powershell
# Configure
cmake -B build -G "Visual Studio 17 2022" -A x64

# Build
cmake --build build --config Release

# Run benchmarks
.\build\bin\Release\oop_benchmark.exe .\results
.\build\bin\Release\ecs_entt_benchmark.exe .\results
.\build\bin\Release\hybrid_benchmark.exe .\results

# Generate visualization
python scripts\visualize.py .\results .\results
```

## Key Findings

Based on benchmark results across 5,000 - 50,000 entities:

| Metric | OOP | Hybrid | Pure ECS |
|--------|-----|--------|----------|
| **Frame Time** | Baseline | **20-42% faster** | 8-21% slower |
| **Memory** | Baseline | 52-62% more | 23-34% more |
| **Creation Time** | Baseline | 27% slower | 96% slower |

**Hybrid architecture delivers the best performance** by combining:
- ECS-style iteration over high-frequency data (Transform, AI, Velocity)
- OOP objects for low-frequency data (Health, BehaviorState) and complex behaviors

The hybrid approach avoids both the pointer-chasing overhead of pure OOP and the indirection costs of pure ECS when accessing low-frequency data.

## Architecture Details

### OOP Architecture

```
Entity (base)
├── LivingEntity (health, regeneration)
│   └── Creature (AI state, movement)
│       ├── Monster (attack behavior)
│       │   ├── Zombie, Skeleton, Spider, Creeper, Enderman, Blaze, Ghast, Phantom
│       ├── Animal (passive behavior)
│       │   ├── Pig, Cow, Sheep, Chicken, Rabbit, Wolf, Cat, Fox
│       ├── FlyingCreature (flight)
│       │   └── Bat, Bee
│       └── AquaticCreature (swimming)
│           └── Fish, Squid, Dolphin, Turtle
```

Each entity type overrides virtual `update()` method with custom behavior.

### ECS Architecture (EnTT)

```
Components:
├── Transform    (position: x, y, z)
├── Physics      (velocity, acceleration, drag)
├── Health       (current, maximum, regenRate)
├── AI           (state, target, perceptionRange)
├── Attributes   (speed, damage, range, type, behaviorFlags)
└── TypeTag      (entityType, isMonster, isAnimal, isFlying, isAquatic)

Systems:
├── AISystem          - State machine updates
├── MovementSystem    - Position updates based on AI
├── PhysicsSystem     - Velocity integration
├── DecaySystem       - Health regeneration
└── InteractionSystem - Combat and targeting
```

Systems iterate over component views using EnTT's archetype-based storage.

### Hybrid Architecture

The hybrid approach combines the best of both worlds:

```
ECS Components (high-frequency, updated every frame):
├── TransformComponent    (position: x, y, z)
├── AIComponent           (state, target, perceptionRange, wanderDir)
├── VelocityComponent     (dx, dz, speed)
├── TypeTagComponent      (entityType, isMonster, isAnimal, etc.)
├── AttributesComponent   (attackDamage, attackRange, moveSpeed)
└── EntityLink            (index to OOP entity for special behaviors)

OOP Classes (low-frequency, behavior-rich):
├── HybridEntity (base class)
│   ├── Health          (current, maximum, isDead)
│   ├── BehaviorState   (stamina, alertness, timers, home)
│   └── virtual updateSpecialBehavior()
│
├── CreeperEntity       (explosion fuse logic)
├── EndermanEntity      (teleportation)
├── BatEntity           (height adjustment)
├── BeeEntity           (anger state)
├── FishEntity          (depth + direction)
├── DolphinEntity       (jumping behavior)
├── PhantomEntity       (diving at night)
├── SquidEntity         (depth maintenance)
└── TurtleEntity        (shallow water behavior)

Systems (ECS-style iteration):
├── AISystem          - Iterate AIComponent + TypeTagComponent
├── MovementSystem    - Iterate TransformComponent + VelocityComponent
└── InteractionSystem - Spatial queries + combat (access Health via EntityLink)
```

**Key Design Decisions:**
- High-frequency data (position, AI state, velocity) stored in ECS components for cache-friendly iteration
- Low-frequency data (health, behavior state) kept in OOP objects for code clarity
- Special entity behaviors (Creeper explosion, Enderman teleport) use virtual dispatch only when needed
- Systems iterate components directly without virtual calls in the hot path

## Configuration

Edit `shared/benchmark_config.hpp` to modify test parameters:

```cpp
inline std::vector<BenchmarkConfig> getAllBenchmarkConfigs() {
    return {
        {1000, 10000, 12345},    // entityCount, frameCount, randomSeed
        {5000, 10000, 12345},
        {20000, 10000, 12345},
        {100000, 10000, 12345},
    };
}
```

## Output Files

Results are saved to `results/`:

| File | Description |
|------|-------------|
| `*_frame_time.csv` | Per-frame timing data |
| `*_memory.csv` | Memory usage statistics |
| `*_creation.csv` | Entity creation/destruction timing |
| `frame_time_comparison.png` | Frame time comparison chart |
| `avg_frame_time.png` | Average frame time bar chart |
| `performance_improvement.png` | Performance difference chart |
| `memory_comparison.png` | Memory usage comparison |
| `creation_time.png` | Entity creation timing chart |
| `benchmark_summary.txt` | Text summary of results |

## Entity Types

### Monsters (30% spawn rate)
Zombie, Skeleton, Spider, Creeper, Enderman, Blaze, Ghast, Phantom

### Animals (40% spawn rate)
Pig, Cow, Sheep, Chicken, Rabbit, Wolf, Cat, Fox

### Flying (15% spawn rate)
Bat, Bee

### Aquatic (15% spawn rate)
Fish, Squid, Dolphin, Turtle

## Analysis

### Why Hybrid Outperforms Both

The hybrid architecture achieves the best performance because:

1. **Cache-Friendly Hot Path**: High-frequency data (Transform, AI, Velocity) is stored contiguously in ECS components, enabling efficient iteration

2. **Minimal Indirection**: Unlike pure ECS, low-frequency data (Health, BehaviorState) doesn't require component lookups during movement/AI updates

3. **Selective Virtual Dispatch**: Special behaviors (Creeper explosion, Enderman teleport) only dispatch through virtual calls when needed, not every frame

4. **Spatial Locality**: Component pools are cache-friendly, while OOP objects for health/behavior are rarely accessed during the hot loop

### Why Pure OOP Can Be Slower

1. **Pointer Chasing**: Virtual function calls and object pointers scattered in memory
2. **Cache Misses**: Each entity's data may be in different cache lines
3. **vtable Overhead**: Virtual dispatch for every entity update

### Why Pure ECS Has Overhead

1. **Component Indirection**: Accessing multiple components requires separate lookups
2. **Memory Fragmentation**: Multiple component pools spread across memory
3. **Archetype Churn**: Entity composition changes can cause data movement

### When Each Architecture Shines

| Architecture | Best For |
|-------------|----------|
| **OOP** | Small entity counts (<10K), rapid prototyping, clear code flow |
| **Hybrid** | Medium-large entity counts (10K-100K), balanced performance + maintainability |
| **Pure ECS** | Very large counts (>100K), parallel systems, data-oriented design |

### Optimization Opportunities

1. **Spatial Partitioning**: Replace O(n²) target-finding with grid/quadtree
2. **Component Pooling**: Pre-allocate component storage to avoid runtime allocation
3. **SIMD Processing**: Batch process component arrays with vectorization
4. **Parallel Systems**: Run independent systems on separate threads

## License

This project is for educational and benchmarking purposes.

## References

- [EnTT Documentation](https://github.com/skypjack/entt)
- [Data-Oriented Design](https://blog.therocode.net/2018/08/data-oriented-design)
- [ECS FAQ](https://github.com/SanderMertens/ecs-faq)
