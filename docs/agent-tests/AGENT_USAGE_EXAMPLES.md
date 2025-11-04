# Agent Usage Examples for MDK-Predator

This document demonstrates how to use the adapted agents for the MDK-Predator embedded C codebase.

## Available Agents

All agents have been adapted from web development to embedded C/ARM Cortex-M4 development:

1. **code-architecture-reviewer** - Review C code for best practices
2. **code-refactor-master** - Comprehensive code refactoring
3. **documentation-architect** - Create embedded documentation
4. **auto-error-resolver** - Fix GCC compilation errors
5. **plan-reviewer** - Review implementation plans
6. **refactor-planner** - Create refactoring strategies
7. **web-research-specialist** - Research technical solutions
8. **hardware-interface-tester** - Test embedded modules

## Example Usage

### Code Architecture Review

**User Request:**
> "I've just implemented the SubGHz analyzer module. Can you review it for best practices?"

**How to use:**
```
Use the code-architecture-reviewer agent to review the SubGHz analyzer implementation in src/wireless/subghz_analyzer.c
```

**What it checks:**
- C11 standard compliance
- Memory management (malloc/free, buffer overflows)
- Volatile usage for hardware registers
- Interrupt safety
- ARM Cortex-M4 optimization opportunities
- Real-time constraints

---

### Auto Error Resolution

**User Request:**
> "The build is failing with several GCC errors"

**How to use:**
```
Use the auto-error-resolver agent to fix the compilation errors
```

**What it does:**
- Runs `make clean && make`
- Identifies error types (missing includes, type mismatches)
- Fixes errors systematically
- Verifies fixes by rebuilding

---

### Documentation Creation

**User Request:**
> "Document the new rolling code tester module"

**How to use:**
```
Use the documentation-architect agent to create documentation for the rolling code tester in src/automotive/rolling_code_tester.c
```

**What it creates:**
- API documentation with function signatures
- Usage examples
- Hardware requirements
- Memory usage notes
- Signal flow diagrams

---

### Plan Review

**User Request:**
> "I'm planning to add Bluetooth Classic support. Can you review my plan?"

**How to use:**
```
Use the plan-reviewer agent to review my Bluetooth Classic implementation plan
```

**What it checks:**
- Resource constraints (RAM/ROM)
- Hardware compatibility
- Real-time performance impact
- Missing considerations
- Alternative approaches
- Risk assessment

---

### Hardware Interface Testing

**User Request:**
> "Test the WiFi analyzer module"

**How to use:**
```
Use the hardware-interface-tester agent to test the WiFi analyzer module
```

**What it does:**
- Builds the project
- Runs existing tests
- Checks for compilation warnings
- Reviews code for common pitfalls
- Verifies hardware interface patterns

---

### Refactoring Strategy

**User Request:**
> "The crypto analyzer file is too large. Help me refactor it."

**How to use:**
```
Use the refactor-planner agent to create a refactoring plan for src/crypto/crypto_analyzer.c
```

**What it provides:**
- Current state analysis
- Identified issues
- Step-by-step refactoring plan
- Dependency mapping
- Risk assessment
- Memory impact analysis

---

## Agent Capabilities

### Embedded-Specific Understanding

All agents understand:
- **Language**: C11 standard, embedded C patterns
- **Platform**: ARM Cortex-M4 with hardware FPU
- **Hardware**: HackRF Portapack H4M interface
- **Constraints**: Limited RAM, real-time requirements
- **Build System**: Make-based with arm-none-eabi-gcc
- **Focus**: Security research (automotive, wireless, crypto)

### Common Checks

Agents automatically check for:
- Buffer overflows and bounds checking
- Memory leaks and allocation patterns
- Integer overflow in calculations
- Null pointer dereference
- Uninitialized variables
- Race conditions in interrupts
- Stack overflow risks
- Volatile usage for hardware
- Interrupt safety
- Real-time constraint violations

## Testing the Agents

To verify agents work correctly:

1. **Build the project:**
   ```bash
   make clean && make
   ```

2. **Run tests (if available):**
   ```bash
   make test
   ```

3. **Use an agent:**
   Ask Claude to use any agent for a specific task

4. **Verify output:**
   Check that the agent produces relevant embedded C advice, not web development advice

## Notes

- Agents have been fully adapted from web technologies (React/TypeScript/Node.js) to embedded C
- Web-specific agents (auth-route-tester, auth-route-debugger, frontend-error-fixer) have been removed
- New embedded-specific agent (hardware-interface-tester) has been added
- All agent descriptions and instructions reference C, ARM, embedded patterns, not web technologies

## Success Criteria

✅ Agents understand C11 and embedded development
✅ Agents reference ARM Cortex-M4 architecture  
✅ Agents consider memory and real-time constraints
✅ Agents check for embedded-specific issues
✅ No references to TypeScript, React, Node.js, or web APIs
✅ Agents work with MDK-Predator's build system (Make)
