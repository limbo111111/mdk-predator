# Contributing to MDK-Predator

Thank you for your interest in contributing to MDK-Predator! This document provides guidelines for contributing to the project.

## Code of Conduct

- Be respectful and professional
- Focus on constructive feedback
- Remember this is a security research tool - use responsibly

## How to Contribute

### Reporting Bugs

1. Check existing issues to avoid duplicates
2. Provide detailed information:
   - Hardware setup (HackRF model, Mayhem-MDK version)
   - Firmware version
   - Steps to reproduce
   - Expected vs actual behavior
   - Logs or error messages

### Suggesting Features

1. Check if the feature aligns with project goals
2. Provide clear use cases
3. Explain how it enhances security research
4. Consider hardware limitations

### Submitting Code

#### Before You Start

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature-name`
3. Ensure your code follows project standards

#### Code Standards

**C Code Style:**
- Use K&R style bracing
- 4-space indentation (no tabs)
- Meaningful variable names
- Comment complex logic
- Maximum line length: 100 characters

**Example:**
```c
bool example_function(example_t *config) {
    if (!config) {
        return false;
    }
    
    // Perform operation
    for (int i = 0; i < count; i++) {
        // Process item
    }
    
    return true;
}
```

**Documentation:**
- Add header comments for all public functions
- Document parameters and return values
- Update API.md for new public APIs
- Update USER_GUIDE.md for user-facing features

#### Testing

- Test on actual hardware when possible
- Verify no regressions in existing features
- Include test cases for new features
- Document test procedures

#### Commit Messages

Use clear, descriptive commit messages:
```
Add SubGHz protocol decoder for Nice FLO

- Implement Nice FLO protocol decoding
- Add unit tests for decoder
- Update documentation
```

Format:
- First line: Brief summary (50 chars max)
- Blank line
- Detailed description with bullet points

#### Pull Request Process

1. **Update Documentation**:
   - Update relevant .md files
   - Add API documentation if needed
   - Update CHANGELOG.md

2. **Test Your Changes**:
   - Compile without warnings
   - Test on hardware
   - Verify no memory leaks

3. **Submit PR**:
   - Reference related issues
   - Describe changes clearly
   - List testing performed
   - Include before/after comparisons

4. **Review Process**:
   - Respond to feedback promptly
   - Make requested changes
   - Keep discussions professional

### Adding New Modules

When adding a new security module:

1. **Create Module Files**:
   ```
   src/category/module_name.c
   include/module_name.h
   ```

2. **Follow Existing Structure**:
   - Init function
   - Cleanup function
   - Core functionality
   - Consistent naming

3. **Update Build System**:
   - Add to Makefile
   - Update include paths

4. **Document Thoroughly**:
   - API reference
   - User guide
   - Architecture diagram

### Adding New Protocols

For SubGHz protocol decoders:

1. Implement decoder function
2. Add protocol constants
3. Register with decoder registry
4. Add tests with sample data
5. Document protocol details

### Security Considerations

**Important**: All contributions must:
- ✅ Be for legitimate security research
- ✅ Include appropriate warnings
- ✅ Not enable illegal activities
- ✅ Follow responsible disclosure practices

**Do NOT**:
- ❌ Add features that enable unauthorized access
- ❌ Include tools for malicious purposes
- ❌ Bypass security without authorization
- ❌ Omit safety warnings

### Project Structure

```
mdk-predator/
├── src/               # Implementation files
│   ├── automotive/    # Automotive security
│   ├── wireless/      # Wireless protocols
│   └── crypto/        # Cryptographic analysis
├── include/           # Public headers
├── docs/              # Documentation
├── tests/             # Test suite
└── examples/          # Usage examples
```

### Development Setup

1. **Install ARM Toolchain**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install gcc-arm-none-eabi
   
   # macOS
   brew install arm-none-eabi-gcc
   ```

2. **Clone Repository**:
   ```bash
   git clone https://github.com/limbo111111/mdk-predator.git
   cd mdk-predator
   ```

3. **Build**:
   ```bash
   make
   ```

4. **Test**:
   ```bash
   make test  # If test suite exists
   ```

### Getting Help

- Open an issue for questions
- Tag with appropriate labels
- Provide context and details
- Be patient and respectful

### License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Thank You!

Your contributions help make security research more accessible and effective. Thank you for helping improve MDK-Predator!
