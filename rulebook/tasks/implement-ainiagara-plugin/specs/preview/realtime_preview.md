# Real-time Preview System Specification

## Overview

The real-time preview system allows users to see live updates of Niagara/Cascade particle systems in the editor viewport as the DSL is being modified through the chat interface. This provides immediate visual feedback during the AI-assisted creation process.

## Requirements

### Functional Requirements

1. **DSL Change Detection**
   - Monitor DSL changes in the chat interface
   - Detect when a new valid DSL is parsed from AI responses
   - Track DSL modifications during conversation

2. **Preview System Generation**
   - Create temporary preview systems (non-persistent)
   - Generate preview systems from DSL without saving to disk
   - Support both Niagara and Cascade preview systems

3. **Viewport Integration**
   - Display preview system in Niagara editor viewport
   - Display preview system in Cascade editor viewport
   - Update preview automatically when DSL changes

4. **User Controls**
   - Toggle preview on/off in chat interface
   - Clear preview manually
   - Option to save preview as permanent system

5. **Performance**
   - Throttle/debounce rapid DSL updates
   - Optimize preview generation to avoid editor lag
   - Clean up preview systems when chat closes

### Technical Requirements

1. **Architecture**
   - Separate preview system from permanent system generation
   - Use temporary asset paths for preview systems
   - Implement preview system manager/controller

2. **Integration Points**
   - Hook into DSL parsing in `SAINiagaraChatWidget`
   - Integrate with Niagara/Cascade editor viewports
   - Use editor asset preview APIs

3. **Error Handling**
   - Handle preview generation errors gracefully
   - Show errors in chat interface, don't break preview
   - Fallback to previous preview on error

4. **Memory Management**
   - Clean up preview systems when not needed
   - Avoid memory leaks from temporary systems
   - Proper garbage collection

## Implementation Plan

### Phase 19.1: Design Architecture

**Tasks:**
- Design preview system manager class
- Design DSL change detection mechanism
- Design preview system lifecycle
- Design viewport integration approach

**Deliverables:**
- Architecture diagram
- Class structure design
- Integration points documentation

### Phase 19.2: DSL Change Detection

**Tasks:**
- Implement DSL change detection in chat widget
- Add DSL version tracking
- Create DSL change event system

**Implementation:**
- Add DSL change delegate/event to `SAINiagaraChatWidget`
- Track current DSL state
- Emit events when DSL changes

### Phase 19.3: Preview System Generator

**Tasks:**
- Create `FPreviewSystemGenerator` utility class
- Implement temporary system creation (non-persistent)
- Support both Niagara and Cascade previews

**Implementation:**
- Create preview-specific generator methods
- Use temporary package paths
- Mark systems as preview-only (don't save to disk)

### Phase 19.4: Preview Update System

**Tasks:**
- Implement preview system update on DSL changes
- Handle preview system replacement
- Manage preview system lifecycle

**Implementation:**
- Update preview when DSL changes
- Replace old preview with new one
- Clean up old preview systems

### Phase 19.5-19.6: Viewport Integration

**Tasks:**
- Integrate preview with Niagara editor viewport
- Integrate preview with Cascade editor viewport
- Update viewport when preview changes

**Implementation:**
- Access Niagara editor viewport
- Access Cascade editor viewport
- Set preview system as active in viewport
- Refresh viewport display

### Phase 19.7: UI Controls

**Tasks:**
- Add preview toggle button in chat interface
- Add preview status indicator
- Add "Save Preview" button

**Implementation:**
- Add UI controls to `SAINiagaraChatWidget`
- Show preview status (on/off, updating, error)
- Implement save preview functionality

### Phase 19.8: Cleanup System

**Tasks:**
- Implement preview cleanup on chat close
- Clean up preview systems when switching assets
- Handle editor shutdown gracefully

**Implementation:**
- Cleanup in widget destructor
- Cleanup on asset change
- Register cleanup callbacks

### Phase 19.9: Performance Optimization

**Tasks:**
- Implement throttling for rapid DSL updates
- Add debouncing for DSL change events
- Optimize preview generation performance

**Implementation:**
- Throttle preview updates (max 1 per second)
- Debounce DSL change events (wait for stable DSL)
- Cache preview systems when DSL unchanged

### Phase 19.10: Error Handling

**Tasks:**
- Handle preview generation errors
- Show errors in chat interface
- Maintain previous preview on error

**Implementation:**
- Catch preview generation errors
- Display error messages in chat
- Keep previous preview visible on error

### Phase 19.11: Preview Comparison (Optional)

**Tasks:**
- Show diff between preview versions
- Highlight changes in DSL
- Visual comparison in viewport

**Implementation:**
- Compare DSL versions
- Highlight changed properties
- Side-by-side preview (optional)

### Phase 19.12: Testing

**Tasks:**
- Test preview with rapid DSL updates
- Test preview performance under load
- Test preview cleanup
- Test error handling

**Test Cases:**
- Rapid DSL updates (10+ per second)
- Large DSL with many emitters
- Invalid DSL handling
- Preview cleanup on close
- Preview persistence across chat sessions

## Technical Details

### Preview System Manager

```cpp
class FPreviewSystemManager
{
public:
    // Create/update preview system from DSL
    void UpdatePreview(const FVFXDSL& DSL);
    
    // Clear current preview
    void ClearPreview();
    
    // Get current preview system
    UNiagaraSystem* GetNiagaraPreview() const;
    UParticleSystem* GetCascadePreview() const;
    
    // Check if preview is active
    bool IsPreviewActive() const;
    
private:
    // Temporary preview systems
    UNiagaraSystem* NiagaraPreview;
    UParticleSystem* CascadePreview;
    
    // Preview state
    bool bPreviewEnabled;
    FVFXDSL CurrentPreviewDSL;
};
```

### DSL Change Detection

```cpp
// In SAINiagaraChatWidget
DECLARE_DELEGATE_OneParam(FOnDSLChanged, const FVFXDSL&);

// When DSL is parsed from AI response
void OnDSLParsed(const FVFXDSL& DSL)
{
    if (PreviewManager && bPreviewEnabled)
    {
        PreviewManager->UpdatePreview(DSL);
    }
}
```

### Viewport Integration

```cpp
// Access Niagara editor viewport
void UpdateNiagaraViewport(UNiagaraSystem* PreviewSystem)
{
    // Get Niagara editor toolkit
    // Set preview system in viewport
    // Refresh viewport display
}

// Access Cascade editor viewport
void UpdateCascadeViewport(UParticleSystem* PreviewSystem)
{
    // Get Cascade editor toolkit
    // Set preview system in viewport
    // Refresh viewport display
}
```

## User Experience

### Workflow

1. User opens chat interface
2. User enables preview toggle (optional, can be auto-enabled)
3. User sends prompt to AI
4. AI generates DSL
5. Preview automatically updates in viewport
6. User sees changes in real-time
7. User can refine prompt based on preview
8. User saves final system when satisfied

### UI Elements

- **Preview Toggle**: Enable/disable preview mode
- **Preview Status**: Show "Preview Active", "Updating...", "Error"
- **Save Preview**: Convert preview to permanent system
- **Clear Preview**: Remove preview from viewport

## Performance Considerations

- **Throttling**: Limit preview updates to 1 per second max
- **Debouncing**: Wait 500ms after DSL change before updating
- **Caching**: Don't regenerate if DSL hasn't changed
- **Lazy Loading**: Only generate preview when enabled
- **Cleanup**: Immediately clean up old previews

## Error Handling

- **Invalid DSL**: Show error in chat, keep previous preview
- **Generation Failure**: Show error, don't break preview
- **Viewport Error**: Log error, continue with chat functionality
- **Memory Error**: Clean up and disable preview

## Future Enhancements

- Side-by-side preview comparison
- Preview animation playback controls
- Preview parameter tweaking
- Preview export/import
- Preview history (undo/redo)

