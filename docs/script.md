# Presentation & Explanation Script

Use this document verbatim (or selectively) when pitching, demoing, or defending the project.

---
## 1. Elevator Pitch (30–45 seconds)
Modern medical devices must perform multiple critical tasks (monitor vitals, deliver therapy, update clinician displays) under hard timing constraints. General-purpose operating systems cannot guarantee strict timing determinism. We built a lightweight, deterministic real-time compute kernel with a live visualization interface that demonstrates priority-based preemptive scheduling, safe resource sharing, and predictable execution — the foundations required for safety‑critical medical workloads.

---
## 2. Problem Statement
Medical devices (infusion pumps, ventilators, cardiac monitors) require:
- Guaranteed execution deadlines (hard or firm real-time).
- Predictable worst-case behavior (determinism) for regulatory certification (IEC 62304, ISO 14971).
- Minimal footprint (RAM / Flash) for embedded MCUs.
- Isolation of critical pathways from non-critical tasks.

Existing solutions (general-purpose OS, biased RTOS configurations) often introduce:
- Jitter from non-deterministic scheduling.
- Larger codebases making formal verification and certification harder.
- Feature bloat irrelevant to narrowly defined medical use cases.

Our kernel focuses on determinism, clarity, and inspectability — a foundation for certifiable systems.

---
## 3. Why It Is Needed
1. **Safety:** Mis-timed actuation (e.g., dose delivery) can cause harm.
2. **Regulatory approval:** Deterministic design simplifies hazard analysis and traceability.
3. **Resource constraints:** Many medical peripherals run on low-power microcontrollers.
4. **Auditability:** A smaller, well-documented execution core is easier to reason about during failure mode analysis.
5. **Extensibility:** Clear primitives (tasks, scheduler, timing, mutex) provide a scaffold for future IPC or memory protection.

---
## 4. High-Level Architecture
```
+---------------------------------------------------+
| Demo Application (medical workflow simulation)    |
|  - Vitals Task (High)                             |
|  - Drug Delivery Task (Medium)                    |
|  - Display Task (Low)                             |
+----------------------+----------------------------+
|         Kernel Core  |                           |
|  +----------------+  |  +----------------------+ |
|  |  Scheduler     |  |  |  Task Manager (TCBs) | |
|  +----------------+  |  +----------------------+ |
|  |  Timer / Tick  |  |  |  IPC (Mutex)         | |
|  +----------------+  |  +----------------------+ |
|         Deterministic Execution Loop              |
+---------------------------------------------------+
| Hardware Abstraction (future: HAL / ISRs)         |
+---------------------------------------------------+
```

### Key Components
- **Task Control Block (TCB):** Holds ID, priority, state, stack pointer, sleep deadline.
- **Scheduler:** Preemptive, priority-first selection; simulates context switches.
- **Timer:** Generates periodic ticks (in demo slowed to 20s for narration). Drives wakeups of sleeping tasks.
- **IPC / Mutex:** Ensures exclusive access to shared medical data (heart rate & drug status).
- **Web Visualization Layer:** Streams structured JSON from kernel to browser via Socket.IO.

### Determinism Elements
- Fixed, priority-based ready queue scanning.
- No dynamic memory allocation in the critical path.
- Sleep semantics based on absolute tick deadlines.

---
## 5. Implementation Summary (C + Node.js + Web)
| Layer | Technology | Purpose |
|-------|------------|---------|
| Kernel | C (ISO C99) | Low-level deterministic logic |
| Web Server | Node.js (Express + Socket.IO) | Spawn & stream kernel output |
| Frontend | HTML/CSS/JS + Chart.js | Real-time heart rate graph + log viewer |

### Steps Implemented
1. Defined headers (`task.h`, `scheduler.h`, `kernel.h`, `ipc.h`, `timer.h`).
2. Implemented task creation & simple fixed-size array TCB store.
3. Built priority-based scheduler with tick-driven wakeups.
4. Added mutex primitive to serialize shared data updates.
5. Wrapped kernel output in JSON lines for structured ingestion.
6. Developed Node.js server to spawn kernel process and relay lines.
7. Created live web visualization (chart + color-coded log, pause/resume, start/stop streaming, scroll control).
8. Added demo tasks to simulate a medical scenario.
9. Provided test suite (basic smoke tests for creation & scheduler init).

---
## 6. Technical Deep Dive
### Scheduling Algorithm
- **Type:** Cooperative simulation of preemptive priority scheduling.
- **Selection:** Linear scan of ready list; highest `priority` (integer) wins.
- **State Transitions:** READY → RUNNING → (SLEEP/BLOCKED or back to READY).
- **Determinism:** Given same ready set & tick count, always identical next-task choice.

### Time & Sleep
- System tick increments every loop iteration (demo slowed). Sleeping tasks store `sleep_ticks` = wake deadline; scheduler tick unblocks them when `current_ticks >= sleep_ticks`.

### IPC
- Mutex spin + yield (simple for demo; future: priority inheritance). Protects shared struct writes to avoid race conditions.

### Data Streaming Format (JSON Lines)
Examples:
```json
{"type":"log_entry","message":"Scheduler: Running Task 0 (Priority: 3)"}
{"type":"data","tick":42,"hr":74}
```
Robust `readline` parsing on the server ensures per-line delivery.

### Web Frontend Behavior
- **Autoscroll:** Only when user at bottom; manual scroll preserved during narration.
- **Pause/Resume:** Buffers incoming messages; flushes on resume.
- **Start/Stop Stream:** Kills/spawns kernel child process.
- **Graph:** Maintains sliding window (up to 50 points) for clarity.

### Testing Strategy (Current)
- Smoke tests ensure task creation returns valid IDs & scheduler initializes.
- Future expansion: deterministic replay tests, timing budget verification, mutex contention tests.

---
## 7. How to Run (Quick Commands)
### Build & Test Kernel
```bash
make clean
make
make test
```
### Run Standalone Kernel (console mode)
```bash
make run
```
### Launch Web Visualization
```bash
cd web
npm install   # first time only
npm start
# Open http://localhost:3000
```
Use controls: Pause / Resume for narration, Stop Stream / Start Stream to reset kernel state.

---
## 8. Demo Walkthrough Script (Suggested Narration)
1. **Intro (Slide / Verbal):** "We will show a deterministic kernel managing three medical tasks."
2. **Open Web UI:** Point out graph & log areas, control buttons.
3. **Explain Priorities:** Task 0 (Vitals, high), Task 1 (Drug delivery), Task 2 (Display).
4. **Show Tick:** Each tick line printed (every 20s here for clarity).
5. **Highlight Scheduling:** On each tick, kernel selects highest READY; discuss fairness vs determinism.
6. **Pause:** Use Pause to freeze incoming data; explain buffered messages concept.
7. **Resume:** Flush and watch messages catch up; emphasize observability.
8. **Stop Stream:** Kernel halts; note termination log. Explain ability to restart clean state.
9. **Start Stream:** Fresh kernel; point out reproducible deterministic behavior.
10. **Conclude:** Emphasize building blocks for certification: small surface, predictable timing.

---
## 9. Comparison With Existing Industry Solutions
| Aspect | This Kernel | FreeRTOS / Zephyr | Full RTOS (QNX / VxWorks) |
|--------|-------------|-------------------|---------------------------|
| Code Size (demo) | Very small | Moderate | Large |
| Determinism Focus | Explicit / simplified | Mixed (config-dependent) | Strong |
| Medical Specificity | Tailored abstractions | Generic embedded | Generic / enterprise |
| Learning Curve | Low (few primitives) | Medium | High |
| Certification Path | Easier to analyze (small) | Requires pruning | Well-established but complex |
| Observability (live) | Built-in JSON streaming UI | Add-on tooling | Proprietary tooling |

**Advantages:** Ultra-minimal surface, purpose-driven for medical tasks, integrated visualization, clear extension path for certification artifacts (traceability, hazard mapping).

**Trade-offs / Limitations (Current):**
- No real hardware ISR integration yet.
- Cooperative simulation for preemption (would need context switch implementation on real MCU).
- No memory protection or MPU regions.
- No priority inversion mitigation (e.g., inheritance protocol).

---
## 10. Future Scope & Roadmap
| Phase | Feature | Value |
|-------|---------|-------|
| Short-term | True preemptive context switching on target MCU (ARM Cortex-M) | Real hardware validation |
| Short-term | Priority inheritance in mutex | Prevent priority inversion |
| Mid-term | Static configuration + build-time analysis | Deterministic footprint & WCET profiling |
| Mid-term | Integration with HAL (GPIO, ADC for real sensor input) | Real vitals feed |
| Mid-term | Event tracing buffer + export | Offline safety analysis & replay |
| Long-term | IEC 62304 compliance scaffolding (risk classes mapping) | Certification readiness |
| Long-term | Formal verification of scheduler properties | Regulatory confidence |
| Long-term | Memory protection / MPU / task isolation | Fault containment |
| Long-term | Redundancy & failover hooks | High-availability medical deployment |

---
## 11. Extensibility Design Principles
- **Isolation:** Keep core deterministic path unpolluted by optional services.
- **Static Allocation:** Favor compile-time sizing for certifiability & WCET predictability.
- **Structured Output:** Continue JSON lines for introspection & remote telemetry.
- **Modular Growth:** Add: timers, queues, semaphores while preserving simplicity.

---
## 12. Key Talking Points (Quick Reference)
- Determinism over feature richness.
- Medical safety requires predictability not just performance.
- Visualization reduces black-box complexity during audits.
- Small surface area accelerates certification & formal analysis.
- Clear path to add priority inheritance, memory isolation, ISR integration.

---
## 13. Potential Live Q&A Answers
**Q: Why not just use FreeRTOS?**  
A: FreeRTOS is excellent but broader; stripping it down for medical determinism still leaves larger code surface and features we do not need. Our kernel is intentionally minimal and fully inspectable.

**Q: How would you handle a missed deadline?**  
A: Introduce deadline metadata per task, tick-based monitoring, and escalation hooks (alarm task or safe shutdown routine).

**Q: How will you ensure memory safety?**  
A: Next phases: MPU isolation per task stack, static analysis (clang-tidy), and optional Rust-wrapped drivers.

**Q: Can this scale to more tasks?**  
A: Yes—extend to priority-ordered structure (e.g., binary heap) or multi-tier queues while preserving deterministic selection.

**Q: What about power constraints?**  
A: Idle path can enter low-power mode (WFI instruction) when no READY tasks; tick source via low-power timer.

---
## 14. Risks & Mitigations
| Risk | Mitigation |
|------|------------|
| Priority inversion | Add inheritance / ceiling protocols |
| Missed WCET budgeting | Static analysis & profiling tools |
| Certification delays | Early documentation & traceability matrix |
| Hardware integration complexity | Abstract HAL layers incrementally |
| Race conditions expansion | Introduce richer sync primitives (semaphores, RW locks) |

---
## 15. Summary Close
This project demonstrates the foundational execution model required for safe, deterministic medical software: clear task prioritization, predictable timing, controlled state sharing, and transparency via a real-time visualization layer. It is intentionally lean, making it an ideal nucleus for an evolvable, certifiable medical RT kernel.

---
## 16. Appendix: File Map
- `src/` — Core kernel & demo tasks.
- `include/` — Public kernel interfaces.
- `tests/` — Smoke tests (expandable).
- `web/` — Visualization server & UI.
- `docs/script.md` — Presentation script (this file).
- `README.md` — Public project overview & quickstart.

---

## 17. Step-by-Step Functional Flow

This narrates the exact lifecycle from launch to data appearing in the browser. Use for a live demo walkthrough.

1. Start Server & UI:
   - Run `node web/server.js`; open `http://localhost:3000`.
   - Socket.IO connection established; UI awaits events.
2. Kernel Spawn:
   - Server auto-spawns (or user clicks Start Stream) executing `./rtkernel`.
   - Kernel STDOUT piped into Node `readline` for line-delimited JSON.
3. Initialization:
   - `kernel_init()` sets globals; creates TCBs for vitals, drug, display tasks.
   - Mutex initialized; JSON `log_entry` lines describe setup.
4. First Tick:
   - Tick counter at 0; timer emits JSON `--- Tick 0 ---`.
5. Scheduling Loop:
   - `kernel_start()` sleeps ~20s per cycle (demo pacing).
   - Post-sleep: `systick_handler()` increments tick; `scheduler_tick()` wakes due tasks.
6. Scheduler Decision:
   - Scans tasks; highest READY priority chosen; JSON log emitted.
7. Task Run (Vitals Example):
   - Acquires mutex; simulates heart rate reading; releases mutex.
   - Emits JSON data line: `{"type":"data","metric":"heart_rate","value":N}`.
   - Sleeps until future tick (BLOCKED state) for deterministic pacing.
8. Other Tasks:
   - Display task logs UI update; Drug task evaluates therapy (may log no action).
9. Mutex Events:
   - Each lock/unlock produces JSON `MUTEX_LOCK` / `MUTEX_UNLOCK` for traceability.

Flow Details:

- Streaming Contract:
   - Every event one JSON line; server distinguishes `data` vs `log_entry`.
- Server Relay:
   - Parses JSON; emits `data` (chart) or `log` (log panel) via Socket.IO.
- Front-End Update:
   - Chart.js appends heart_rate; maintains sliding window (~50 points).
   - Log panel appends textual events; autoscroll only if user at bottom.
- Pause/Resume:
   - Pause buffers incoming events (both data & log arrays).
   - Resume flushes buffer in order, preserving chronology.
- Stop/Start Stream:
   - Stop kills kernel process; Start respawns from tick 0 (clean state, reproducible).
- Determinism Factors:
   - Fixed priorities; no dynamic alloc in hot path; tick-driven wake deadlines.
- Latency Path (Conceptual):
   - Kernel emit → pipe → Node parse → Socket emit → Browser render (tiny vs 20s tick).
- Failure Modes (Educational):
   - Bad parsing (resolved via `readline`) would drop chart updates.
   - Missing mutex could corrupt shared patient state if multiple writers.
- Extension Hooks:
   - Add new metric: emit additional `data` lines; extend chart series.
   - Add priority inheritance: modify mutex logic without altering tasks.
- Demo Recap Script:
   - Start stream → Show tick & scheduling → Pause (buffer) → Resume (flush) → Stop & Start (deterministic restart).

This flow links observable UI behavior directly to kernel internals, reinforcing determinism and clarity.

---
End of script.
