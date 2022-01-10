# SOGA
Single-object genetic solver.

| Header: | `#include<Genetic>` |
| ----: | :---- |
| Location: | [SOGA.hpp](../../GA/SOGA.hpp) |
| Inherits from: | [GABase](./GABase.md) |

<br>

## Defination
```cpp
template<typename Var_t,bool isGreaterBetter,bool Record,class ...Args> class OptimT::SOGA;
```
<br>

## Types
| Access | Name | Type | Defination |
| :----: | :----: | ----: | :---- |
|  | [`isGreaterBetter`](#isgreaterbetter) | `template bool` |  |

Some other types are inherited from [GABase](./GABase.md).

<br>

## Members
| Access | Type | Name | Default value |
| :----: | ----: | :---- | :----: |
| public | `GeneIt_t` | [`_eliteIdx`](#_eliteit) |  |

Some other members are inherited from [GABase](./GABase.md).

<br>

## Member functions
| Access | Return type | Defination |
| :----: | ----: | :---- |
| public |  | [`SOGA()`](#soga) |
| public | `virtual` | [`~SOGA()`](#\~soga) |
| public | `const Var_t &` | [`result() const`](#result-const) |
| public | `virtual double` | [`bestFitness() const`](#bestfitness-const) |
| protected | `virtual void` | [`select()`](#select) |
| protected | `virtual void` | [`mutate()`](#mutate) |

Some other functions are inherited from [GABase](./GABase.md).

<br>

## Detailed description
Class/Strcut description here.

<br>

## Type details
### `isGreaterBetter`
Boolean template parameter. This boolean denotes whether a greater fitness value is better. If yes, SOGA will try to find the maximum, otherwise SOGA will try to find the minimum.

Use `FITNESS_GREATER_BETTER`(true) or `FITNESS_LESS_BETTER`(false) here to make your code more readable.


<br>

## Member details
### `_eliteIt`
Iterator to elite gene.

<br>

## Function details
### `SOGA()`
Default constructor.

Initializated function ptrs in base class to empty lambda.

### `~SOGA()`
Default destructor.

### `result() const`
Get result (`Var_t`).

### `bestFitness() const`
Implemented this pure virtual function defined in [GABase](./GABase.md). It will return fitness of elite gene.

### `select()`
Simple select implementation. The best gene in population will be assigned to be elite.

### `mutate()`
Simple mutate implementation. Elite gene won't mutate.
