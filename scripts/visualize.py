#!/usr/bin/env python3
"""
ECS vs OOP Performance Benchmark Visualization Script

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
    oop_frame_path = os.path.join(results_dir, 'oop_frame_time.csv')
    ecs_frame_path = os.path.join(results_dir, 'ecs_frame_time.csv')

    if os.path.exists(oop_frame_path):
        results['oop_frame'] = pd.read_csv(oop_frame_path)
    if os.path.exists(ecs_frame_path):
        results['ecs_frame'] = pd.read_csv(ecs_frame_path)

    # Memory data
    oop_mem_path = os.path.join(results_dir, 'oop_memory.csv')
    ecs_mem_path = os.path.join(results_dir, 'ecs_memory.csv')

    if os.path.exists(oop_mem_path):
        results['oop_memory'] = pd.read_csv(oop_mem_path)
    if os.path.exists(ecs_mem_path):
        results['ecs_memory'] = pd.read_csv(ecs_mem_path)

    # Creation data
    oop_create_path = os.path.join(results_dir, 'oop_creation.csv')
    ecs_create_path = os.path.join(results_dir, 'ecs_creation.csv')

    if os.path.exists(oop_create_path):
        results['oop_creation'] = pd.read_csv(oop_create_path)
    if os.path.exists(ecs_create_path):
        results['ecs_creation'] = pd.read_csv(ecs_create_path)

    # Hardware counters
    oop_hw_path = os.path.join(results_dir, 'oop_hw_counters.csv')
    ecs_hw_path = os.path.join(results_dir, 'ecs_hw_counters.csv')

    if os.path.exists(oop_hw_path):
        results['oop_hw'] = pd.read_csv(oop_hw_path)
    if os.path.exists(ecs_hw_path):
        results['ecs_hw'] = pd.read_csv(ecs_hw_path)

    return results

def plot_frame_time_comparison(results, output_dir):
    """Plot frame time comparison between OOP and ECS."""
    if 'oop_frame' not in results or 'ecs_frame' not in results:
        print("Frame time data not available")
        return

    oop_data = results['oop_frame']
    ecs_data = results['ecs_frame']

    # Get unique scales
    scales = oop_data['scale'].unique()

    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Frame Time Comparison: OOP vs ECS', fontsize=16)

    colors = {'OOP': '#e74c3c', 'ECS': '#3498db'}

    for idx, scale in enumerate(sorted(scales)):
        ax = axes[idx // 2, idx % 2]

        oop_scale = oop_data[oop_data['scale'] == scale]
        ecs_scale = ecs_data[ecs_data['scale'] == scale]

        # Plot frame time over frames
        ax.plot(oop_scale['frame'], oop_scale['frame_time_ms'],
                label='OOP', color=colors['OOP'], alpha=0.7, linewidth=0.5)
        ax.plot(ecs_scale['frame'], ecs_scale['frame_time_ms'],
                label='ECS', color=colors['ECS'], alpha=0.7, linewidth=0.5)

        ax.set_xlabel('Frame')
        ax.set_ylabel('Frame Time (ms)')
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
    if 'oop_frame' not in results or 'ecs_frame' not in results:
        return

    oop_data = results['oop_frame']
    ecs_data = results['ecs_frame']

    # Calculate average frame time per scale
    oop_avg = oop_data.groupby('scale')['frame_time_ms'].mean()
    ecs_avg = ecs_data.groupby('scale')['frame_time_ms'].mean()

    scales = sorted(oop_avg.index)

    fig, ax = plt.subplots(figsize=(10, 6))

    x = np.arange(len(scales))
    width = 0.35

    bars1 = ax.bar(x - width/2, [oop_avg[s] for s in scales], width,
                   label='OOP', color='#e74c3c')
    bars2 = ax.bar(x + width/2, [ecs_avg[s] for s in scales], width,
                   label='ECS', color='#3498db')

    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Average Frame Time (ms)')
    ax.set_title('Average Frame Time Comparison')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')

    # Add value labels on bars
    for bars in [bars1, bars2]:
        for bar in bars:
            height = bar.get_height()
            ax.annotate(f'{height:.3f}',
                       xy=(bar.get_x() + bar.get_width() / 2, height),
                       xytext=(0, 3),
                       textcoords="offset points",
                       ha='center', va='bottom', fontsize=8)

    plt.tight_layout()
    output_path = os.path.join(output_dir, 'avg_frame_time.png')
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Saved: {output_path}")

def plot_performance_improvement(results, output_dir):
    """Plot performance improvement percentage."""
    if 'oop_frame' not in results or 'ecs_frame' not in results:
        return

    oop_data = results['oop_frame']
    ecs_data = results['ecs_frame']

    oop_avg = oop_data.groupby('scale')['frame_time_ms'].mean()
    ecs_avg = ecs_data.groupby('scale')['frame_time_ms'].mean()

    scales = sorted(oop_avg.index)
    improvements = []

    for s in scales:
        oop_time = oop_avg[s]
        ecs_time = ecs_avg[s]
        improvement = ((oop_time - ecs_time) / oop_time) * 100
        improvements.append(improvement)

    fig, ax = plt.subplots(figsize=(10, 6))

    colors = ['#2ecc71' if imp > 0 else '#e74c3c' for imp in improvements]
    bars = ax.bar(range(len(scales)), improvements, color=colors)

    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Performance Improvement (%)')
    ax.set_title('ECS Performance Improvement vs OOP\n(Positive = ECS Faster)')
    ax.set_xticks(range(len(scales)))
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.axhline(y=0, color='black', linestyle='-', linewidth=0.5)
    ax.grid(True, alpha=0.3, axis='y')

    # Add value labels
    for bar, imp in zip(bars, improvements):
        height = bar.get_height()
        ax.annotate(f'{imp:.1f}%',
                   xy=(bar.get_x() + bar.get_width() / 2, height),
                   xytext=(0, 3 if height >= 0 else -15),
                   textcoords="offset points",
                   ha='center', va='bottom' if height >= 0 else 'top',
                   fontsize=10, fontweight='bold')

    plt.tight_layout()
    output_path = os.path.join(output_dir, 'performance_improvement.png')
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Saved: {output_path}")

def plot_memory_comparison(results, output_dir):
    """Plot memory usage comparison."""
    if 'oop_memory' not in results or 'ecs_memory' not in results:
        return

    oop_mem = results['oop_memory']
    ecs_mem = results['ecs_memory']

    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    scales = oop_mem['scale'].values
    x = np.arange(len(scales))

    # Peak memory
    ax = axes[0]
    width = 0.35
    ax.bar(x - width/2, oop_mem['peak_usage'] / 1024 / 1024, width,
           label='OOP', color='#e74c3c')
    ax.bar(x + width/2, ecs_mem['peak_usage'] / 1024 / 1024, width,
           label='ECS', color='#3498db')
    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Peak Memory (MB)')
    ax.set_title('Peak Memory Usage')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')

    # Per-entity overhead
    ax = axes[1]
    ax.bar(x - width/2, oop_mem['per_entity_overhead'], width,
           label='OOP', color='#e74c3c')
    ax.bar(x + width/2, ecs_mem['per_entity_overhead'], width,
           label='ECS', color='#3498db')
    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Bytes per Entity')
    ax.set_title('Per-Entity Memory Overhead')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')

    # Memory comparison ratio
    ax = axes[2]
    ratio = ecs_mem['peak_usage'] / oop_mem['peak_usage']
    ax.bar(x, ratio.values, color='#9b59b6')
    ax.axhline(y=1, color='black', linestyle='--', linewidth=1)
    ax.set_xlabel('Entity Count')
    ax.set_ylabel('ECS / OOP Ratio')
    ax.set_title('Memory Ratio (ECS/OOP)')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.grid(True, alpha=0.3, axis='y')

    plt.tight_layout()
    output_path = os.path.join(output_dir, 'memory_comparison.png')
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Saved: {output_path}")

def plot_creation_time(results, output_dir):
    """Plot entity creation/destruction time comparison."""
    if 'oop_creation' not in results or 'ecs_creation' not in results:
        return

    oop_cre = results['oop_creation']
    ecs_cre = results['ecs_creation']

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    scales = oop_cre['scale'].values
    x = np.arange(len(scales))
    width = 0.35

    # Creation time
    ax = axes[0]
    ax.bar(x - width/2, oop_cre['create_time_ms'], width,
           label='OOP', color='#e74c3c')
    ax.bar(x + width/2, ecs_cre['create_time_ms'], width,
           label='ECS', color='#3498db')
    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Total Creation Time (ms)')
    ax.set_title('Entity Creation Time')
    ax.set_xticks(x)
    ax.set_xticklabels([f'{int(s):,}' for s in scales])
    ax.legend()
    ax.grid(True, alpha=0.3, axis='y')

    # Average time per entity
    ax = axes[1]
    ax.bar(x - width/2, oop_cre['avg_create_time_ms'] * 1000, width,
           label='OOP', color='#e74c3c')
    ax.bar(x + width/2, ecs_cre['avg_create_time_ms'] * 1000, width,
           label='ECS', color='#3498db')
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

def plot_frame_time_distribution(results, output_dir):
    """Plot frame time distribution (box plot)."""
    if 'oop_frame' not in results or 'ecs_frame' not in results:
        return

    oop_data = results['oop_frame']
    ecs_data = results['ecs_frame']

    scales = sorted(oop_data['scale'].unique())

    fig, axes = plt.subplots(1, 2, figsize=(14, 6))

    # OOP distribution
    ax = axes[0]
    oop_data_to_plot = [oop_data[oop_data['scale'] == s]['frame_time_ms'].values
                        for s in scales]
    bp1 = ax.boxplot(oop_data_to_plot, labels=[f'{int(s):,}' for s in scales],
                     patch_artist=True)
    for patch in bp1['boxes']:
        patch.set_facecolor('#e74c3c')
    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Frame Time (ms)')
    ax.set_title('OOP Frame Time Distribution')
    ax.grid(True, alpha=0.3)

    # ECS distribution
    ax = axes[1]
    ecs_data_to_plot = [ecs_data[ecs_data['scale'] == s]['frame_time_ms'].values
                        for s in scales]
    bp2 = ax.boxplot(ecs_data_to_plot, labels=[f'{int(s):,}' for s in scales],
                     patch_artist=True)
    for patch in bp2['boxes']:
        patch.set_facecolor('#3498db')
    ax.set_xlabel('Entity Count')
    ax.set_ylabel('Frame Time (ms)')
    ax.set_title('ECS Frame Time Distribution')
    ax.grid(True, alpha=0.3)

    plt.tight_layout()
    output_path = os.path.join(output_dir, 'frame_time_distribution.png')
    plt.savefig(output_path, dpi=150)
    plt.close()
    print(f"Saved: {output_path}")

def generate_summary_table(results, output_dir):
    """Generate summary table as text file."""
    summary_lines = []
    summary_lines.append("=" * 80)
    summary_lines.append("ECS vs OOP Performance Benchmark Summary")
    summary_lines.append("=" * 80)
    summary_lines.append("")

    if 'oop_frame' in results and 'ecs_frame' in results:
        oop_data = results['oop_frame']
        ecs_data = results['ecs_frame']

        oop_avg = oop_data.groupby('scale')['frame_time_ms'].mean()
        ecs_avg = ecs_data.groupby('scale')['frame_time_ms'].mean()
        oop_std = oop_data.groupby('scale')['frame_time_ms'].std()
        ecs_std = ecs_data.groupby('scale')['frame_time_ms'].std()

        summary_lines.append("Frame Time Results:")
        summary_lines.append("-" * 80)
        summary_lines.append(f"{'Scale':>12} | {'OOP Avg (ms)':>14} | {'OOP Std':>10} | "
                            f"{'ECS Avg (ms)':>14} | {'ECS Std':>10} | {'Improvement':>12}")
        summary_lines.append("-" * 80)

        for scale in sorted(oop_avg.index):
            oop_t = oop_avg[scale]
            ecs_t = ecs_avg[scale]
            oop_s = oop_std[scale]
            ecs_s = ecs_std[scale]
            improvement = ((oop_t - ecs_t) / oop_t) * 100
            summary_lines.append(f"{int(scale):>12,} | {oop_t:>14.4f} | {oop_s:>10.4f} | "
                                f"{ecs_t:>14.4f} | {ecs_s:>10.4f} | {improvement:>11.1f}%")

        summary_lines.append("")

    if 'oop_memory' in results and 'ecs_memory' in results:
        oop_mem = results['oop_memory']
        ecs_mem = results['ecs_memory']

        summary_lines.append("Memory Results:")
        summary_lines.append("-" * 60)
        summary_lines.append(f"{'Scale':>12} | {'OOP Peak (MB)':>14} | {'ECS Peak (MB)':>14} | "
                            f"{'Ratio':>10}")
        summary_lines.append("-" * 60)

        for idx, scale in enumerate(oop_mem['scale']):
            oop_peak = oop_mem.iloc[idx]['peak_usage'] / 1024 / 1024
            ecs_peak = ecs_mem.iloc[idx]['peak_usage'] / 1024 / 1024
            ratio = ecs_peak / oop_peak if oop_peak > 0 else 0
            summary_lines.append(f"{int(scale):>12,} | {oop_peak:>14.2f} | {ecs_peak:>14.2f} | "
                                f"{ratio:>10.2f}")

        summary_lines.append("")

    if 'oop_creation' in results and 'ecs_creation' in results:
        oop_cre = results['oop_creation']
        ecs_cre = results['ecs_creation']

        summary_lines.append("Creation Time Results:")
        summary_lines.append("-" * 70)
        summary_lines.append(f"{'Scale':>12} | {'OOP Total (ms)':>15} | {'ECS Total (ms)':>15} | "
                            f"{'OOP Avg (μs)':>12} | {'ECS Avg (μs)':>12}")
        summary_lines.append("-" * 70)

        for idx, scale in enumerate(oop_cre['scale']):
            oop_total = oop_cre.iloc[idx]['create_time_ms']
            ecs_total = ecs_cre.iloc[idx]['create_time_ms']
            oop_avg = oop_cre.iloc[idx]['avg_create_time_ms'] * 1000
            ecs_avg = ecs_cre.iloc[idx]['avg_create_time_ms'] * 1000
            summary_lines.append(f"{int(scale):>12,} | {oop_total:>15.2f} | {ecs_total:>15.2f} | "
                                f"{oop_avg:>12.2f} | {ecs_avg:>12.2f}")

        summary_lines.append("")

    summary_lines.append("=" * 80)

    # Write to file
    output_path = os.path.join(output_dir, 'benchmark_summary.txt')
    with open(output_path, 'w') as f:
        f.write('\n'.join(summary_lines))

    print(f"Saved: {output_path}")

    # Also print to console
    print('\n'.join(summary_lines))

def main():
    # Default results directory
    results_dir = './results'
    output_dir = './results'

    # Check for command line arguments
    import sys
    if len(sys.argv) > 1:
        results_dir = sys.argv[1]
    if len(sys.argv) > 2:
        output_dir = sys.argv[2]

    # Ensure output directory exists
    os.makedirs(output_dir, exist_ok=True)

    print(f"Loading results from: {results_dir}")
    results = load_csv_results(results_dir)

    if not results:
        print("No benchmark results found. Please run the benchmarks first.")
        return

    print(f"Found {len(results)} result files")

    # Generate all plots
    plot_frame_time_comparison(results, output_dir)
    plot_avg_frame_time(results, output_dir)
    plot_performance_improvement(results, output_dir)
    plot_memory_comparison(results, output_dir)
    plot_creation_time(results, output_dir)
    plot_frame_time_distribution(results, output_dir)

    # Generate summary table
    generate_summary_table(results, output_dir)

    print("\nVisualization complete!")

if __name__ == '__main__':
    main()
