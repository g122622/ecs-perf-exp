#!/usr/bin/env python3
"""
ECS vs OOP vs EnTT ECS Performance Benchmark Visualization Script

Generates comparison charts from benchmark CSV results.
"""

import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

def load_csv_results(results_dir):
    """Load all benchmark CSV files."""
    results = {}

    # Frame time data
    for prefix in ['oop', 'ecs', 'ecs_entt']:
        frame_path = os.path.join(results_dir, f'{prefix}_frame_time.csv')
        if os.path.exists(frame_path):
            results[f'{prefix}_frame'] = pd.read_csv(frame_path)

        mem_path = os.path.join(results_dir, f'{prefix}_memory.csv')
        if os.path.exists(mem_path):
            results[f'{prefix}_memory'] = pd.read_csv(mem_path)

        create_path = os.path.join(results_dir, f'{prefix}_creation.csv')
        if os.path.exists(create_path):
            results[f'{prefix}_creation'] = pd.read_csv(create_path)

        hw_path = os.path.join(results_dir, f'{prefix}_hw_counters.csv')
        if os.path.exists(hw_path):
            results[f'{prefix}_hw'] = pd.read_csv(hw_path)

    return results

def plot_frame_time_comparison(results, output_dir):
    """Plot frame time comparison between OOP, ECS, and EnTT."""
    prefixes = []
    if 'oop_frame' in results:
        prefixes.append(('OOP', 'oop', '#e74c3c'))
    if 'ecs_frame' in results:
        prefixes.append(('ECS (naive)', 'ecs', '#3498db'))
    if 'ecs_entt_frame' in results:
        prefixes.append(('ECS (EnTT)', 'ecs_entt', '#2ecc71'))

    if len(prefixes) == 0:
        print("No frame time data available")
        return

    scales = results[prefixes[0][1] + '_frame']['scale'].unique()

    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Frame Time Comparison: OOP vs ECS (naive) vs ECS (EnTT)', fontsize=16)

    for idx, scale in enumerate(sorted(scales)):
        if idx >= 4:
            break
        ax = axes[idx // 2, idx % 2]

        for label, prefix, color in prefixes:
            data = results[f'{prefix}_frame']
            scale_data = data[data['scale'] == scale]
            ax.plot(scale_data['frame'], scale_data['frame_time_ms'] * 1000,
                    label=label, color=color, alpha=0.7, linewidth=0.5)

        ax.set_xlabel('Frame')
        ax.set_ylabel('Frame Time (μs)')
        ax.set_title(f'{int(scale)} Entities')
        ax.legend()
        ax.grid(True, alpha=0.3)

    plt.tight_layout()
    output_path = os.path.join(output_dir, 'frame_time_comparison.png')
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Saved: {output_path}")

def plot_avg_frame_time(results, output_dir):
    """Plot average frame time comparison."""
    prefixes = []
    if 'oop_frame' in results:
        prefixes.append(('OOP', 'oop', '#e74c3c'))
    if 'ecs_frame' in results:
        prefixes.append(('ECS (naive)', 'ecs', '#3498db'))
    if 'ecs_entt_frame' in results:
        prefixes.append(('ECS (EnTT)', 'ecs_entt', '#2ecc71'))

    if len(prefixes) == 0:
        return

    scales = sorted(results[prefixes[0][1] + '_frame']['scale'].unique())
    x = np.arange(len(scales))
    width = 0.8 / len(prefixes)

    fig, ax = plt.subplots(figsize=(12, 6))

    for i, (label, prefix, color) in enumerate(prefixes):
        data = results[f'{prefix}_frame']
        avg_data = data.groupby('scale')['frame_time_ms'].mean() * 1000  # Convert to microseconds
        values = [avg_data.get(s, 0) for s in scales]
        bars = ax.bar(x + i * width - (len(prefixes) - 1) * width / 2, values, width,
                      label=label, color=color)

        for bar, val in zip(bars, values):
            height = bar.get_height()
            ax.annotate(f'{val:.2f}',
                       xy=(bar.get_x() + bar.get_width() / 2, height),
                       xytext=(0, 3),
                       textcoords="offset points",
                       ha='center', va='bottom', fontsize=7)

    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Average Frame Time (μs)')
    ax.set_title('Average Frame Time Comparison')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')

    plt.tight_layout()
    output_path = os.path.join(output_dir, 'avg_frame_time.png')
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Saved: {output_path}")

def plot_performance_improvement(results, output_dir):
    """Plot performance improvement percentage."""
    if 'oop_frame' not in results:
        return

    oop_data = results['oop_frame']
    oop_avg = oop_data.groupby('scale')['frame_time_ms'].mean()

    improvements = {}
    for prefix, label in [('ecs', 'ECS (naive)'), ('ecs_entt', 'ECS (EnTT)')]:
        if f'{prefix}_frame' in results:
            data = results[f'{prefix}_frame']
            avg_data = data.groupby('scale')['frame_time_ms'].mean()
            improvements[label] = []
            for s in sorted(oop_avg.index):
                if s in avg_data:
                    oop_time = oop_avg[s]
                    other_time = avg_data[s]
                    imp = ((oop_time - other_time) / oop_time) * 100
                    improvements[label].append((s, imp))

    if not improvements:
        return

    fig, ax = plt.subplots(figsize=(10, 6))

    colors = {'ECS (naive)': '#3498db', 'ECS (EnTT)': '#2ecc71'}
    x = np.arange(len(sorted(oop_avg.index)))

    for i, (label, data) in enumerate(improvements.items()):
        scales, imps = zip(*data)
        bars = ax.bar(x + i * 0.35, imps, 0.35, label=label, color=colors.get(label, 'gray'))

        for bar, imp in zip(bars, imps):
            height = bar.get_height()
            va = 'bottom' if height >= 0 else 'top'
            offset = 3 if height >= 0 else -3
            ax.annotate(f'{imp:.1f}%',
                       xy=(bar.get_x() + bar.get_width() / 2, height),
                       xytext=(0, offset),
                       textcoords="offset points",
                       ha='center', va=va, fontsize=9, fontweight='bold')

    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Performance Improvement (%)')
    ax.set_title('ECS Performance vs OOP\n(Positive = ECS Faster)')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in sorted(oop_avg.index)])
    ax.axhline(y=0, color='black', linestyle='-', linewidth=0.5)
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')

    plt.tight_layout()
    output_path = os.path.join(output_dir, 'performance_improvement.png')
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Saved: {output_path}")

def plot_memory_comparison(results, output_dir):
    """Plot memory usage comparison."""
    prefixes = []
    if 'oop_memory' in results:
        prefixes.append(('OOP', 'oop', '#e74c3c'))
    if 'ecs_memory' in results:
        prefixes.append(('ECS (naive)', 'ecs', '#3498db'))
    if 'ecs_entt_memory' in results:
        prefixes.append(('ECS (EnTT)', 'ecs_entt', '#2ecc71'))

    if len(prefixes) == 0:
        return

    scales = results[prefixes[0][1] + '_memory']['scale'].values
    x = np.arange(len(scales))

    fig, axes = plt.subplots(1, 2, figsize=(14, 5))

    # Peak Memory
    ax = axes[0]
    width = 0.8 / len(prefixes)
    for i, (label, prefix, color) in enumerate(prefixes):
        mem = results[f'{prefix}_memory']
        ax.bar(x + i * width - (len(prefixes) - 1) * width / 2,
               mem['peak_usage'] / 1024 / 1024, width, label=label, color=color)
    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Peak Memory (MB)')
    ax.set_title('Peak Memory Usage')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')

    # Per-Entity Overhead
    ax = axes[1]
    for i, (label, prefix, color) in enumerate(prefixes):
        mem = results[f'{prefix}_memory']
        ax.bar(x + i * width - (len(prefixes) - 1) * width / 2,
               mem['per_entity_overhead'], width, label=label, color=color)
    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Bytes per Entity')
    ax.set_title('Per-Entity Memory Overhead')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')

    plt.tight_layout()
    output_path = os.path.join(output_dir, 'memory_comparison.png')
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Saved: {output_path}")

def plot_creation_time(results, output_dir):
    """Plot entity creation time comparison."""
    prefixes = []
    if 'oop_creation' in results:
        prefixes.append(('OOP', 'oop', '#e74c3c'))
    if 'ecs_creation' in results:
        prefixes.append(('ECS (naive)', 'ecs', '#3498db'))
    if 'ecs_entt_creation' in results:
        prefixes.append(('ECS (EnTT)', 'ecs_entt', '#2ecc71'))

    if len(prefixes) == 0:
        return

    scales = results[prefixes[0][1] + '_creation']['scale'].values
    x = np.arange(len(scales))

    fig, axes = plt.subplots(1, 2, figsize=(14, 5))

    # Total Creation Time
    ax = axes[0]
    width = 0.8 / len(prefixes)
    for i, (label, prefix, color) in enumerate(prefixes):
        cre = results[f'{prefix}_creation']
        ax.bar(x + i * width - (len(prefixes) - 1) * width / 2,
               cre['create_time_ms'], width, label=label, color=color)
    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Total Creation Time (ms)')
    ax.set_title('Entity Creation Time')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')

    # Average Time per Entity
    ax = axes[1]
    for i, (label, prefix, color) in enumerate(prefixes):
        cre = results[f'{prefix}_creation']
        ax.bar(x + i * width - (len(prefixes) - 1) * width / 2,
               cre['avg_create_time_ms'] * 1000, width, label=label, color=color)
    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Avg Creation Time (μs)')
    ax.set_title('Average Creation Time per Entity')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')

    plt.tight_layout()
    output_path = os.path.join(output_dir, 'creation_time.png')
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Saved: {output_path}")

def generate_summary_table(results, output_dir):
    """Generate summary table as text file."""
    lines = []
    lines.append("=" * 100)
    lines.append("ECS vs OOP vs EnTT Performance Benchmark Summary")
    lines.append("=" * 100)
    lines.append("")

    # Frame Time Results
    if 'oop_frame' in results:
        prefixes = [('OOP', 'oop')]
        if 'ecs_frame' in results:
            prefixes.append(('ECS (naive)', 'ecs'))
        if 'ecs_entt_frame' in results:
            prefixes.append(('ECS (EnTT)', 'ecs_entt'))

        oop_data = results['oop_frame']
        oop_avg = oop_data.groupby('scale')['frame_time_ms'].mean() * 1000  # Convert to microseconds
        oop_std = oop_data.groupby('scale')['frame_time_ms'].std() * 1000

        lines.append("Frame Time Results (microseconds):")
        lines.append("-" * 100)
        header = f"{'Scale':>10}"
        for label, _ in prefixes:
            header += f" | {label + ' Avg (μs)':>15} | {label + ' Std':>10}"
        header += f" | {'vs OOP':>12}"
        lines.append(header)
        lines.append("-" * 100)

        for scale in sorted(oop_avg.index):
            row = f"{int(scale):>10,}"
            for label, prefix in prefixes:
                data = results[f'{prefix}_frame']
                avg = data[data['scale'] == scale]['frame_time_ms'].mean() * 1000
                std = data[data['scale'] == scale]['frame_time_ms'].std() * 1000
                row += f" | {avg:>15.2f} | {std:>10.2f}"

            # Improvement vs OOP
            ecs_improvement = ""
            if 'ecs_frame' in results:
                ecs_avg = results['ecs_frame'][results['ecs_frame']['scale'] == scale]['frame_time_ms'].mean()
                imp = ((oop_avg[scale] / 1000 - ecs_avg) / (oop_avg[scale] / 1000)) * 100
                ecs_improvement = f" ECS: {imp:>+6.1f}%"
            if 'ecs_entt_frame' in results:
                entt_avg = results['ecs_entt_frame'][results['ecs_entt_frame']['scale'] == scale]['frame_time_ms'].mean()
                imp = ((oop_avg[scale] / 1000 - entt_avg) / (oop_avg[scale] / 1000)) * 100
                ecs_improvement += f" EnTT: {imp:>+6.1f}%"
            row += f" | {ecs_improvement:>12}"
            lines.append(row)

        lines.append("")

    # Memory Results
    if 'oop_memory' in results:
        lines.append("Memory Results:")
        lines.append("-" * 80)
        header = f"{'Scale':>10}"
        for label, prefix in prefixes:
            header += f" | {label + ' Peak (MB)':>15}"
        header += f" | {'Ratios':>20}"
        lines.append(header)
        lines.append("-" * 80)

        oop_mem = results['oop_memory']
        for idx, scale in enumerate(oop_mem['scale']):
            row = f"{int(scale):>10,}"
            ratios = []
            for label, prefix in prefixes:
                mem = results[f'{prefix}_memory']
                peak = mem.iloc[idx]['peak_usage'] / 1024 / 1024
                row += f" | {peak:>15.2f}"
                if prefix != 'oop':
                    oop_peak = oop_mem.iloc[idx]['peak_usage'] / 1024 / 1024
                    ratios.append(f"{label}: {peak/oop_peak:.2f}x")

            row += f" | {' '.join(ratios):>20}"
            lines.append(row)

        lines.append("")

    # Creation Time Results
    if 'oop_creation' in results:
        lines.append("Creation Time Results:")
        lines.append("-" * 90)
        header = f"{'Scale':>10}"
        for label, prefix in prefixes:
            header += f" | {label + ' Total (ms)':>15}"
        header += f" | {'Per-Entity (μs)':>30}"
        lines.append(header)
        lines.append("-" * 90)

        oop_cre = results['oop_creation']
        for idx, scale in enumerate(oop_cre['scale']):
            row = f"{int(scale):>10,}"
            per_entity = []
            for label, prefix in prefixes:
                cre = results[f'{prefix}_creation']
                total = cre.iloc[idx]['create_time_ms']
                avg = cre.iloc[idx]['avg_create_time_ms'] * 1000
                row += f" | {total:>15.2f}"
                per_entity.append(f"{label}: {avg:.2f}μs")

            row += f" | {' | '.join(per_entity):>30}"
            lines.append(row)

        lines.append("")

    lines.append("=" * 100)

    output_path = os.path.join(output_dir, 'benchmark_summary.txt')
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines))

    print(f"Saved: {output_path}")
    print('\n'.join(lines))

def main():
    results_dir = './results'
    output_dir = './results'

    import sys
    if len(sys.argv) > 1:
        results_dir = sys.argv[1]
    if len(sys.argv) > 2:
        output_dir = sys.argv[2]

    os.makedirs(output_dir, exist_ok=True)

    print(f"Loading results from: {results_dir}")
    results = load_csv_results(results_dir)

    if not results:
        print("No benchmark results found. Please run the benchmarks first.")
        return

    print(f"Found {len(results)} result files")

    plot_frame_time_comparison(results, output_dir)
    plot_avg_frame_time(results, output_dir)
    plot_performance_improvement(results, output_dir)
    plot_memory_comparison(results, output_dir)
    plot_creation_time(results, output_dir)
    generate_summary_table(results, output_dir)

    print("\nVisualization complete!")

if __name__ == '__main__':
    main()
