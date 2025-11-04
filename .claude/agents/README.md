# Agents

Specialized agents for complex, multi-step tasks in embedded C development.

---

## What Are Agents?

Agents are autonomous Claude instances that handle specific complex tasks for the MDK-Predator embedded security research platform. Unlike skills (which provide inline guidance), agents:
- Run as separate sub-tasks
- Work autonomously with minimal supervision
- Have specialized tool access for C/embedded development
- Return comprehensive reports when complete

**Key advantage:** Agents are **standalone** - just copy the `.md` file and use immediately!

---

## Available Agents (7)

### code-architecture-reviewer
**Purpose:** Review C code for architectural consistency and embedded best practices

**When to use:**
- After implementing a new hardware interface module
- Before merging significant changes to signal processing code
- When refactoring embedded modules
- To validate architectural decisions for real-time systems

**Integration:** ✅ Adapted for C/embedded

---

### code-refactor-master
**Purpose:** Plan and execute comprehensive refactoring of C embedded code

**When to use:**
- Reorganizing module structures
- Breaking down large C files
- Updating include paths after moves
- Improving embedded code maintainability and memory efficiency

**Integration:** ✅ Adapted for C/embedded

---

### documentation-architect
**Purpose:** Create comprehensive documentation for embedded systems

**When to use:**
- Documenting new hardware interface modules
- Creating API documentation for C functions
- Writing developer guides for security research features
- Generating architectural overviews and signal flow diagrams

**Integration:** ✅ Adapted for C/embedded

---

### auto-error-resolver
**Purpose:** Automatically fix C compilation errors from ARM GCC toolchain

**When to use:**
- Build failures with GCC errors
- After refactoring that breaks compilation
- Systematic error resolution needed
- Type mismatches or missing includes

**Integration:** ✅ Adapted for C/embedded

---

### plan-reviewer
**Purpose:** Review development plans before implementation

**When to use:**
- Before starting complex embedded features
- Validating hardware interface plans
- Identifying potential issues early
- Getting second opinion on signal processing approach

**Integration:** ✅ Adapted for C/embedded

---

### refactor-planner
**Purpose:** Create comprehensive refactoring strategies for C code

**When to use:**
- Planning code reorganization
- Modernizing legacy embedded code
- Breaking down large modules
- Improving code structure and memory efficiency

**Integration:** ✅ Adapted for C/embedded

---

### web-research-specialist
**Purpose:** Research technical issues online

**When to use:**
- Debugging obscure ARM Cortex-M4 errors
- Finding solutions to embedded problems
- Researching signal processing best practices
- Comparing hardware interface implementation approaches

**Integration:** ✅ Copy as-is (universally applicable)

---

### hardware-interface-tester
**Purpose:** Test hardware interface modules and embedded functionality

**When to use:**
- Testing new signal processing modules
- Validating hardware interface implementations
- Debugging embedded module integration issues
- Verifying real-time constraints

**Integration:** ✅ Created for embedded systems

---

## How to Integrate an Agent

### Standard Integration (Most Agents)

**Step 1: Copy the file**
```bash
cp <source-project>/.claude/agents/agent-name.md \
   mdk-predator/.claude/agents/
```

**Step 2: Use it**
Ask Claude: "Use the [agent-name] agent to [task]"

That's it! Agents work immediately.

---

## When to Use Agents vs Skills

| Use Agents When... | Use Skills When... |
|-------------------|-------------------|
| Task requires multiple steps | Need inline guidance |
| Complex analysis needed | Checking best practices |
| Autonomous work preferred | Want to maintain control |
| Task has clear end goal | Ongoing development work |
| Example: "Review all wireless modules" | Example: "Creating a new protocol decoder" |

**Both can work together:**
- Skill provides patterns during development
- Agent reviews the result when complete

---

## Agent Quick Reference

| Agent | Complexity | Focus Area | Hardware Required |
|-------|-----------|------------|-------------------|
| code-architecture-reviewer | Medium | Code review, best practices | No |
| code-refactor-master | High | Code reorganization | No |
| documentation-architect | Medium | Documentation | No |
| auto-error-resolver | Low | Build errors | No |
| plan-reviewer | Low | Plan validation | No |
| refactor-planner | Medium | Refactoring strategy | No |
| web-research-specialist | Low | Research | No |
| hardware-interface-tester | Medium | Module testing | Recommended |

---

## For Claude Code

**When integrating agents for MDK-Predator:**

1. **Just copy the .md file** - agents are standalone
2. **These agents are adapted for embedded C** - they understand:
   - ARM Cortex-M4 architecture
   - Real-time constraints
   - Memory limitations (RAM/ROM)
   - Hardware interface patterns
   - Signal processing requirements
   - Security research context

---

## Creating Your Own Agents

Agents are markdown files with optional YAML frontmatter:

```markdown
# Agent Name

## Purpose
What this agent does for embedded development

## Instructions
Step-by-step instructions for autonomous execution

## Tools Available
List of tools this agent can use (make, gcc, etc.)

## Expected Output
What format to return results in
```

**Tips for Embedded Agents:**
- Be very specific about build commands
- Include memory and performance considerations
- Specify hardware constraints
- Include examples of good embedded patterns
- List available tools explicitly (ARM toolchain, make, etc.)
- Consider real-time and safety requirements

---

## Troubleshooting

### Agent not found

**Check:**
```bash
# Is agent file present?
ls -la .claude/agents/[agent-name].md
```

### Agent gives web-specific advice

**Solution:**
These agents have been adapted for MDK-Predator's C/embedded context. If you see references to TypeScript, React, or web technologies, the agent file may not have been properly updated. Check that you're using the MDK-Predator version of the agent.

---

## Embedded Development Context

These agents understand:
- **Language**: C11 standard
- **Platform**: ARM Cortex-M4 with FPU
- **Hardware**: HackRF Portapack H4M with Mayhem firmware
- **Constraints**: Limited RAM, real-time requirements, power efficiency
- **Purpose**: Security research (automotive, wireless, crypto analysis)
- **Build**: Make-based with arm-none-eabi-gcc toolchain

---

## Next Steps

1. **Browse agents above** - Find ones useful for your embedded work
2. **Ask Claude to use them** - "Use [agent] to [task]"
3. **Create your own** - Follow the pattern for your specific needs

**Questions?** See project documentation in `/docs/`
