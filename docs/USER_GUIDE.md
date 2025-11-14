# User Guide

This guide will help you get started with the AINiagara plugin and create VFX effects using natural language.

## Table of Contents

- [Installation](#installation)
- [Getting Started](#getting-started)
- [Basic Usage](#basic-usage)
- [Advanced Features](#advanced-features)
- [Tips and Best Practices](#tips-and-best-practices)
- [Troubleshooting](#troubleshooting)

---

## Installation

### Prerequisites

- Unreal Engine 5.3 or later
- Google Gemini API key ([Get one here](https://makersuite.google.com/app/apikey))
- Windows, Mac, or Linux (Editor only)

### Installing the Plugin

1. Copy the `AINiagara` folder to your project's `Plugins` directory:
   ```
   YourProject/
   └── Plugins/
       └── AINiagara/
   ```

2. Open your project in Unreal Editor

3. The plugin should be automatically detected and enabled

4. If prompted, restart the editor to complete installation

---

## Getting Started

### Step 1: Configure API Key

1. Open a Niagara or Cascade particle system in the editor

2. Click the **AINiagara** button in the editor toolbar

3. If this is your first time, you'll see an API key configuration dialog

4. Enter your Google Gemini API key

5. Click **Test** to verify the key works

6. Click **Save** to store the key securely

**Note:** Your API key is stored in project settings and will persist across sessions.

### Step 2: Open the Chat Interface

1. With a Niagara or Cascade system open in the editor

2. Click the **AINiagara** toolbar button

3. The chat window will open, styled to match Unreal Engine 5

---

## Basic Usage

### Creating Your First Effect

1. **Open the chat interface** (see Step 2 above)

2. **Type a natural language description** of the effect you want:
   ```
   Create a fire effect with orange and yellow particles
   ```

3. **Click Send** or press Enter

4. **Wait for processing** - you'll see loading indicators:
   - "Preparing request..."
   - "Sending request to AI..."
   - "Validating DSL..."
   - "Generating Niagara system..."

5. **Review the result** - the system will be created automatically

6. **Find your system** - it will be saved in `/Game/VFX/` by default

### Example Prompts

Here are some example prompts to get you started:

**Simple Effects:**
- "Create a spark effect"
- "Make a smoke cloud"
- "Generate a rain effect"

**Detailed Effects:**
- "Create a fire effect with orange and yellow particles that spawn at 50 per second"
- "Make a magical sparkle effect with blue and white particles that burst every 2 seconds"
- "Generate a smoke trail that follows a path with gray particles"

**Complex Effects:**
- "Create a fire effect with multiple emitters - one for flames and one for sparks"
- "Make a waterfall effect with blue particles that fall downward with gravity"
- "Generate an explosion effect with particles that burst outward in all directions"

### Understanding the Response

The AI will:
1. **Parse your request** and understand what you want
2. **Generate a DSL specification** (Domain-Specific Language) describing the effect
3. **Validate the DSL** to ensure it's correct
4. **Create the particle system** automatically
5. **Show a success message** with the system location

If there's an error, you'll see an error message explaining what went wrong.

---

## Advanced Features

### Conversation History

The chat interface maintains conversation history **per asset**. This means:

- Each particle system has its own conversation history
- You can refine effects by continuing the conversation
- Previous context is automatically included in new requests

**Example:**
```
You: Create a fire effect
AI: [Creates fire effect]

You: Make it more intense
AI: [Modifies the existing fire effect to be more intense]
```

### Exporting DSL

You can export the DSL specification for any system:

1. Click the **Export DSL** button in the chat interface
2. Choose a location to save the JSON file
3. The DSL will be saved as a JSON file

**Use cases:**
- Share effect specifications
- Version control your effects
- Modify DSL manually and regenerate

### Importing DSL

You can import a previously exported DSL:

1. Click the **Import DSL** button
2. Select a JSON file containing DSL
3. The DSL will be loaded and displayed in the chat

### Regenerating from DSL

After importing a DSL, you can regenerate the system:

1. Import a DSL (see above)
2. Click the **Regenerate** button
3. The system will be recreated from the DSL

**Use cases:**
- Modify DSL in a text editor and regenerate
- Share effects with others
- Version control effects

### Reverse Engineering

You can convert existing systems to DSL:

1. Open an existing Niagara or Cascade system
2. Click **Export DSL** in the chat interface
3. The system will be analyzed and converted to DSL format

**Use cases:**
- Understand how an effect is structured
- Modify existing effects
- Learn DSL format by example

---

## Tips and Best Practices

### Writing Effective Prompts

**Be Specific:**
- ✅ "Create a fire effect with orange particles spawning at 100 per second"
- ❌ "Make fire"

**Describe Visual Properties:**
- ✅ "Create blue particles that fade from opaque to transparent"
- ❌ "Make particles"

**Specify Behavior:**
- ✅ "Generate particles that burst outward in a circle every 2 seconds"
- ❌ "Make an explosion"

**Use Natural Language:**
- ✅ "Create a magical sparkle effect with twinkling particles"
- ❌ "Generate emitter with spawn rate 50, color blue, lifetime 3"

### Iterative Refinement

Don't try to get everything perfect in one prompt. Instead:

1. Start with a basic description
2. Review the result
3. Refine with follow-up prompts:
   - "Make it more intense"
   - "Change the color to blue"
   - "Increase the spawn rate"
   - "Add more particles"

### Understanding DSL

The DSL (Domain-Specific Language) is a JSON format that describes VFX effects. Key concepts:

- **Effect**: Overall effect properties (type, duration, looping)
- **Emitters**: Individual particle emitters
- **Spawners**: How particles are spawned (burst, rate)
- **Initialization**: Initial particle properties (position, velocity, color)
- **Update**: How particles change over time (forces, color over lifetime)
- **Render**: How particles are rendered (material, blend mode)

See [DSL_SCHEMA.md](./DSL_SCHEMA.md) for complete documentation.

### Performance Considerations

- **Start simple**: Begin with one emitter, then add more
- **Use appropriate spawn rates**: Higher rates = more particles = more performance cost
- **Limit particle lifetime**: Shorter lifetimes = fewer particles = better performance
- **Test in-game**: Editor preview may differ from runtime performance

---

## Troubleshooting

### API Key Issues

**Problem:** "API key is not set" error

**Solution:**
1. Make sure you've configured the API key (see Getting Started)
2. Check that the API key is valid by clicking Test
3. Verify you have internet connectivity

**Problem:** "API key test failed" error

**Solution:**
1. Verify your API key is correct
2. Check your API quota/billing status
3. Ensure you have internet connectivity

### Generation Issues

**Problem:** "Failed to parse DSL" error

**Solution:**
1. The AI may have generated invalid DSL
2. Try rephrasing your prompt
3. Check the chat history for error details
4. The system will automatically request correction from the AI

**Problem:** "Failed to generate system" error

**Solution:**
1. Check that the package path exists (`/Game/VFX/` by default)
2. Ensure you have write permissions
3. Verify the DSL is valid (check Export DSL)
4. Check the Output Log for detailed error messages

### UI Issues

**Problem:** Chat window doesn't open

**Solution:**
1. Make sure you have a Niagara or Cascade system open
2. Try closing and reopening the editor
3. Check that the plugin is enabled in Plugin Settings

**Problem:** Buttons are disabled

**Solution:**
1. Make sure you have a valid API key configured
2. Wait for any ongoing operations to complete
3. Check the status messages in the chat interface

### Performance Issues

**Problem:** System generation is slow

**Solution:**
1. This is normal - AI processing takes time
2. Complex effects take longer than simple ones
3. Check your internet connection speed
4. API response times vary

**Problem:** Editor becomes unresponsive

**Solution:**
1. Large systems with many emitters may take time
2. Wait for the operation to complete
3. Check the Output Log for progress
4. If stuck, restart the editor

---

## FAQ

**Q: Do I need an internet connection?**
A: Yes, the plugin requires internet connectivity to communicate with the Gemini API.

**Q: Is my API key secure?**
A: Yes, your API key is stored securely in project settings and is masked when displayed.

**Q: Can I use this in a packaged game?**
A: No, this plugin is Editor-only and requires the Unreal Editor.

**Q: What's the cost of using the Gemini API?**
A: Check Google's pricing for the Gemini API. The plugin makes standard API calls.

**Q: Can I modify generated systems manually?**
A: Yes! Generated systems are standard Unreal Engine assets and can be edited normally.

**Q: Does this work with Cascade?**
A: Yes, the plugin supports both Niagara and Cascade particle systems.

**Q: Can I share my API key with my team?**
A: API keys are stored per-project. Each team member should use their own API key for security.

---

## Next Steps

- Read the [API Reference](./API_REFERENCE.md) for programmatic access
- Check the [Features Specification](./FEATURES.md) for complete feature list
- Review the [DSL Schema](./DSL_SCHEMA.md) for DSL format details
- See [Testing Guide](./TESTING.md) for running automated tests

---

## Support

For issues, questions, or contributions:
- Check the [README.md](../README.md) for project information
- Review the [CHANGELOG.md](../CHANGELOG.md) for recent updates
- File issues on the project repository

