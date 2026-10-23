# Concurrency Engine

A C concurrency simulation in which developer threads compete for shared dongles, with FIFO or earliest-deadline-first scheduling.

The executable is named **codexion**. This is a learning project about synchronization and resource contention, not a general-purpose job scheduler.

## Model and architecture

Each developer needs two neighboring dongles to compile, then moves through debugging and refactoring. Dongles have a configurable cooldown after release. A monitor ends the simulation when all developers meet their compilation target or a developer exceeds the burnout interval.

| Component | Responsibility |
| --- | --- |
| `parser.c` | Numeric validation and scheduling mode |
| `init.c`, `cleanup.c` | Shared state, synchronization primitives, and cleanup |
| `threads.c` | Worker and monitor lifecycle |
| `routine.c` | Resource acquisition and work phases |
| `scheduler.c` | FIFO/EDF priorities and per-dongle wait queues |
| `dongle.c` | Release and cooldown timestamps |
| `monitor.c` | Completion and burnout detection |
| `logger.c`, `utils.c` | Serialized output and timing helpers |

Mutexes protect dongle state, global state, and logging. Condition variables wake resource waiters; timed waits account for cooldowns. Each dongle queue has two slots, matching the neighboring-worker model.

## Build and run

Requires a C compiler, Make, and POSIX threads on a Unix-like system.

```bash
git clone https://github.com/ahmedaitelaaoud/Concurrency-Engine.git
cd Concurrency-Engine
make
./codexion 4 2000 20 20 20 2 0 fifo
./codexion 4 2000 20 20 20 2 0 edf
```

Arguments, in order:

| Argument | Meaning |
| --- | --- |
| `devs` | Number of developer threads, from 1 to 1000 |
| `burnout` | Maximum interval since the last compilation start, in milliseconds |
| `do_compile` | Compilation duration, in milliseconds |
| `debug` | Debugging duration, in milliseconds |
| `refactor` | Refactoring duration, in milliseconds |
| `required_compiles` | Positive compilation target per developer |
| `cooldown` | Nonnegative dongle cooldown, in milliseconds |
| `fifo\|edf` | Resource scheduling policy |

Timing values other than cooldown must be positive. Output lines contain elapsed milliseconds, a developer ID, and a state such as `is compiling` or `burned out`.

## Scheduling decisions

FIFO uses the resource request timestamp as its priority. EDF uses the developer’s last compilation start plus the burnout interval. Queues belong to individual dongles; this is not a global queue or an atomic reservation of both resources.

## Validation and limitations

Both example commands have been smoke-tested: four workers completed two compilations each without burnout. This is evidence for those runs, not a proof of race freedom or starvation freedom.

- Timing uses wall-clock time and polling in some paths; there are no hard real-time guarantees.
- Small startup staggering does not prove deadlock avoidance.
- Thread-creation failure and shutdown paths need dedicated stress tests.
- The fixed two-slot queue is specific to this topology.
- There is no committed automated concurrency test suite yet.

Next steps: add bounded termination tests, exercise single-worker and short-deadline cases, and run race-detection tooling.

```bash
make clean
make fclean
```

## License

No repository-level license is currently included.

