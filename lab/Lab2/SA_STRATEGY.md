# Simulated Annealing Strategy for Fixed-Outline Floorplanning

## Overview

This implementation uses **Simulated Annealing (SA)** combined with a **B*-Tree** representation to solve the fixed-outline floorplanning problem. The goal is to place rectangular blocks within a fixed outline while minimizing both the total wirelength (HPWL) and the bounding area.

## Problem Definition

Given:
- A set of rectangular blocks with specific dimensions (width and height)
- A fixed outline constraint (maximum width and height)
- A set of nets connecting blocks and terminals
- A weight parameter `α` to balance area and wirelength optimization

Objective:
- Minimize: `α × (BoundingArea) + (1 - α) × (TotalHPWL)`
- Subject to: The layout must fit within the fixed outline dimensions

## Algorithm Components

### 1. B*-Tree Representation

The B*-Tree (B-star Tree) is a binary tree-based representation for non-slicing floorplans:

- **Left child**: Represents a block placed horizontally adjacent (to the right) of its parent
- **Right child**: Represents a block placed vertically above its parent
- **Root**: The first block placed at the bottom-left corner
- **Contour list**: Maintains the current placement frontier to efficiently compute block positions

This representation allows O(n) packing time and provides a compact, non-redundant encoding of the floorplan.

### 2. Cost Function

The algorithm uses a two-phase cost function strategy:

#### Phase 1: Penalty-based Cost (During Search)
```
SA_cost = {
    -4 × outlineWidth × outlineHeight / objective,     if layout fits within outline
    10 × (x_error + y_error),                          otherwise
}
```
where:
- `x_error = max(0, BoundaryWidth - OutlineWidth)`
- `y_error = max(0, BoundaryHeight - OutlineHeight)`
- `objective = α × BoundingArea + (1 - α) × TotalHPWL`

This penalty-based approach:
- Heavily penalizes infeasible solutions (those exceeding outline bounds)
- Rewards feasible solutions proportional to their quality
- Guides the search toward the feasible region

#### Phase 2: Final Evaluation
```
finalCost = α × BoundingArea + (1 - α) × TotalHPWL
```

### 3. Perturbation Moves

Three types of moves are used to explore the solution space:

#### a) **SwapBlock** (Probability: 1/3)
- Randomly selects two blocks
- Swaps their positions in the B*-Tree by exchanging their parent-child relationships
- Handles special cases:
  - When one block is the root
  - When blocks are parent-child (adjacent in tree)
  - When blocks are independent

**Effect**: Changes relative positions of blocks significantly, enabling exploration of different topologies

#### b) **RotateBlock** (Probability: 1/3)
- Randomly selects one block
- Rotates it 90° by swapping its width and height
- Updates the contour and recalculates block positions

**Effect**: Changes aspect ratios to potentially reduce bounding box or improve wirelength

#### c) **MoveBlock** (Probability: 1/3)
- Randomly selects two blocks
- Attempts to make one a child of the other
- Determines parent-child relationship based on:
  - Which block has an available child slot
  - Random selection if both have available slots
- Falls back to SwapBlock if move is invalid (e.g., blocks are adjacent or both have two children)

**Effect**: Fine-tunes block placement by changing tree structure locally

### 4. Temperature Schedule

The algorithm uses a geometric cooling schedule:

- **Initial temperature (T)**: 10000
  - High enough to accept most moves initially
  - Enables thorough exploration of the solution space

- **Cooling rate (t_decay)**: 0.9
  - Temperature update: `T = T × 0.9` every 1,000 iterations
  - Geometric schedule gradually reduces randomness

- **Termination conditions**:
  1. Time limit: 285 seconds (95% of 300s limit)
  2. Debug mode: Maximum iteration count (if enabled)
  3. (Commented out) Rejection rate > 95%

### 5. Acceptance Criterion (Metropolis Criterion)

A new solution is accepted if:
```
ΔCost < 0  OR  exp(-ΔCost / T) > random[0,1]
```

where:
- `ΔCost = NewCost - CurrentCost`
- `T` is the current temperature

**Implications**:
- Always accepts improvements (ΔCost < 0)
- Probabilistically accepts worse solutions when ΔCost > 0
- Acceptance probability decreases as:
  - ΔCost increases (worse solutions less likely)
  - T decreases (late iterations more greedy)

### 6. Best Solution Tracking

The algorithm maintains three solutions:

1. **Current solution** (`bs_tree`): The most recently accepted configuration
2. **New solution** (`new_tree`): The candidate solution being evaluated
3. **Best solution** (`best_tree`): The best solution found so far

**Best solution update conditions**:
- New cost is better than best cost
- Current best doesn't fit outline, but new solution does (first feasible solution)

This ensures the algorithm returns the best feasible solution even if the search explores infeasible regions.

## Algorithm Flow

```
1. Initialize:
   - Parse input blocks and nets
   - Build initial B*-Tree by inserting blocks sequentially
   - Calculate initial cost
   - Set T = 10000, best_tree = current_tree

2. Main SA Loop (until termination):
   a. Randomly select perturbation move (Swap/Rotate/Move)
   b. Apply move to create new_tree
   c. Calculate new_cost using SA_cost function
   d. Calculate ΔCost = new_cost - current_cost
   
   e. Acceptance decision:
      - If ΔCost < 0 OR exp(-ΔCost/T) > rand():
          * Accept: current_tree = new_tree
          * If new_cost < best_cost: best_tree = new_tree
      - Else:
          * Reject: new_tree = current_tree
          * Increment rejection counter
   
   f. Every 1000 iterations:
      - Update temperature: T = T × 0.9
   
   g. Check termination conditions:
      - Time limit (285s)
      - Debug iteration limit

3. Finalize:
   - Return best_tree
   - Output final cost, HPWL, bounding area, and block positions
```

## Key Design Decisions

### 1. Penalty-Based Cost Function
The heavy penalty (10×) for constraint violations drives the search toward feasible solutions while still exploring the infeasible region when necessary. The inverse reward for feasible solutions creates a strong gradient toward better layouts.

### 2. Uniform Move Selection
Each move type (Swap/Rotate/Move) has equal probability (1/3), providing balanced exploration:
- Swap: Large-scale topology changes
- Rotate: Aspect ratio optimization
- Move: Local fine-tuning

### 3. Adaptive Fallback
When MoveBlock fails due to structural constraints, it automatically falls back to SwapBlock, ensuring every iteration makes a perturbation.

### 4. Best Solution Preservation
By tracking the best solution separately and preferring the first feasible solution, the algorithm guarantees returning a valid result even if later iterations explore infeasible regions for optimization.

### 5. Contour-Based Packing
The contour list enables O(n) packing time by maintaining only the active placement frontier, making cost evaluation efficient for each SA iteration.

## Performance Characteristics

- **Time Complexity per iteration**: O(n) for packing + O(m) for HPWL calculation
  - n = number of blocks
  - m = number of nets
  
- **Space Complexity**: O(n) for tree structure and contour list

- **Typical iteration count**: Thousands to tens of thousands within the time limit

- **Success factors**:
  - Initial temperature high enough for exploration
  - Cooling rate slow enough for convergence
  - Diverse move set for comprehensive search
  - Penalty function effectively guides toward feasible region

## References

This implementation is based on B*-Tree representation for floorplanning:
- Y.-C. Chang, Y.-W. Chang, G.-M. Wu, and S.-W. Wu, "B*-Trees: A New Representation for Non-Slicing Floorplans," in Proceedings of the 37th Annual Design Automation Conference (DAC), 2000, pp. 458-463, doi: 10.1145/337292.337541
- The algorithm combines classic simulated annealing metaheuristic with modern floorplan representation techniques

## Usage

```bash
./floorplan <alpha> <block_file> <net_file> <output_file>
```

Where:
- `alpha`: Weight factor (0 to 1) balancing area vs. wirelength
  - α = 1: Pure area optimization
  - α = 0: Pure wirelength optimization
  - α = 0.5: Balanced optimization
