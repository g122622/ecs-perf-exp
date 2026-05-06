# ECS vs OOP Performance Benchmark

A comprehensive performance comparison between **Object-Oriented Programming (OOP)** and **Entity-Component-System (ECS)** architectures for game development scenarios.

## Overview

This project implements the same game simulation logic using two different architectural approaches:

1. **OOP (Object-Oriented Programming)** - Traditional inheritance-based hierarchy
2. **ECS (EnTT)** - Modern archetype-based Entity-Component-System using the [EnTT](https://github.com/skypjack/entt) library

Both implementations feature identical:
- 22 Minecraft-inspired entity types (Monsters, Animals, Flying, Aquatic)
- AI state machines (Idle, Wander, Chase, Flee)
- Physics simulation
- Combat system with target acquisition
- Health regeneration

## Results Summary

| Entity Count | OOP (μs) | EnTT ECS (μs) | Memory (OOP) | Memory (EnTT) |
|-------------:|---------:|--------------:|-------------:|---------------:|
| 1,000 | 43.86 | 136.06 | 0.11 MB | 0.27 MB |
| 5,000 | 237.09 | 323.28 | 0.54 MB | 1.02 MB |
| 20,000 | 1,015.20 | 1,341.96 | 2.22 MB | 3.90 MB |
| 100,000 | 5,692.39 | 6,901.08 | 11.11 MB | 19.29 MB |

### Key Findings

- **OOP is faster at all tested scales** (up to 3.1x at 1,000 entities)
- **Gap narrows at larger scales** (only 1.2x difference at 100,000 entities)
- **ECS uses more memory** (~1.74-2.52x overhead)

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
├── ecs_entt/                   # EnTT ECS Implementation
│   ├── include/
│   │   ├── components.hpp      # Component definitions
│   │   └── world.hpp           # System implementations
│   └── src/
│       └── world.cpp
│
├── shared/                     # Shared configuration
│   ├── entity_types.hpp        # Entity type definitions
│   ├── constants.hpp           # World constants
│   └── benchmark_config.hpp    # Test configurations
│
├── third_party/
│   └── entt.hpp               # EnTT single-header library
│
├── main_oop.cpp               # OOP benchmark entry
├── main_ecs_entt.cpp          # EnTT benchmark entry
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
.\build\bin\Release\oop_benchmark.exe
.\build\bin\Release\ecs_entt_benchmark.exe

# Generate visualization
python scripts\visualize.py build\bin\Release\results build\bin\Release\results
```

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

Results are saved to `build/bin/Release/results/`:

| File | Description |
|------|-------------|
| `*_frame_time.csv` | Per-frame timing data |
| `*_memory.csv` | Memory usage statistics |
| `*_creation.csv` | Entity creation/destruction timing |
| `frame_time_comparison.png` | Frame time comparison chart |
| `avg_frame_time.png` | Average frame time bar chart |
| `performance_improvement.png` | Performance difference chart |
| `memory_comparison.png` | Memory usage comparison |
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

### Why OOP is Faster Here

1. **Small Scale Dominance**: At 1K-100K entities, virtual function overhead is negligible compared to instruction cache effects

2. **Object Locality**: Each entity is a contiguous memory block - all data needed per-frame is in one place

3. **Compiler Optimization**: MSVC can inline virtual calls when it sees the concrete types

4. **Interaction System**: O(n²) target-finding algorithm dominates in both implementations

### When ECS Would Excel

ECS architectures typically show advantages when:

- **Very large entity counts** (>500K entities)
- **Data-oriented processing** (same operation on millions of components)
- **Parallel execution** (systems can run in parallel on component pools)
- **Memory bandwidth bound** (ECS's cache-friendly iteration pattern)

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
