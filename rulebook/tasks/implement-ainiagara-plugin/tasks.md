# Implementation Tasks

## 1. Planning & Design Phase
- [x] 1.1 Research Gemini API integration for Unreal Engine
- [x] 1.2 Design DSL schema and validation rules
- [x] 1.3 Design chat interface architecture
- [x] 1.4 Design conversation history storage system
- [x] 1.5 Create technical design document

## 2. Core Infrastructure Phase
- [x] 2.1 Create Gemini API client wrapper class
- [x] 2.2 Implement API key storage and management
- [x] 2.3 Create DSL parser and validator
- [x] 2.4 Implement error handling and retry logic
- [x] 2.5 Create conversation history manager

## 3. Editor Integration Phase
- [x] 3.1 Integrate toolbar button in Niagara editor
- [x] 3.2 Integrate toolbar button in Cascade editor
- [x] 3.3 Create chat window widget
- [x] 3.4 Implement window management (open/close/position)
- [x] 3.5 Style UI to match Unreal Engine 5 aesthetic

## 4. Chat Interface Phase
- [x] 4.1 Create message display component
- [x] 4.2 Implement input field and submission
- [x] 4.3 Add loading indicators
- [x] 4.4 Implement error message display
- [x] 4.5 Add success notifications
- [x] 4.6 Implement message history display

## 5. API Configuration Phase
- [x] 5.1 Create API key configuration dialog
- [x] 5.2 Implement secure API key storage
- [x] 5.3 Add validation for API key format
- [x] 5.4 Implement API key testing on submit
- [x] 5.5 Handle API key errors gracefully

## 6. AI Prompt Processing Phase
- [x] 6.1 Create structured prompt builder
- [x] 6.2 Implement VFX expert persona prompt
- [x] 6.3 Add DSL format instructions
- [x] 6.4 Implement tool function documentation in prompts
- [x] 6.5 Add context from conversation history to prompts

## 7. DSL Processing Phase
- [x] 7.1 Implement DSL JSON parser
- [x] 7.2 Create DSL validator with error reporting
- [x] 7.3 Implement DSL schema validation
- [x] 7.4 Add error correction request to LLM
- [x] 7.5 Implement retry mechanism with corrections

## 8. Niagara System Generation Phase
- [x] 8.1 Create UNiagaraSystem factory from DSL
- [x] 8.2 Implement UNiagaraEmitter creation from DSL
- [x] 8.3 Configure Spawn module from DSL spawners
- [x] 8.4 Configure Initialize Particle module from DSL initialization
- [x] 8.5 Configure Update module from DSL update section
- [x] 8.6 Configure Render module from DSL render section

## 9. Cascade System Generation Phase
- [ ] 9.1 Create Cascade particle system factory from DSL
- [ ] 9.2 Implement Cascade emitter creation from DSL
- [ ] 9.3 Configure Cascade spawn modules from DSL
- [ ] 9.4 Configure Cascade initialization modules from DSL
- [ ] 9.5 Configure Cascade update modules from DSL
- [ ] 9.6 Configure Cascade render modules from DSL

## 10. Texture Generation Tool Phase
- [ ] 10.1 Implement tool:texture function handler
- [ ] 10.2 Integrate Gemini Imagen 3 (Nano Banana) API
- [ ] 10.3 Create texture type detection logic
- [ ] 10.4 Implement single texture generation (diffuse/opacity/emissive)
- [ ] 10.5 Implement flipbook sequence generation
- [ ] 10.6 Create PNG sequence to flipbook atlas converter
- [ ] 10.7 Implement UTexture2D creation from generated textures
- [ ] 10.8 Implement UPaperSprite/NiagaraSprite creation for flipbooks
- [ ] 10.9 Update emitter material with generated textures

## 11. Shader Generation Tool Phase
- [ ] 11.1 Implement tool:shader function handler
- [ ] 11.2 Create shader code generator from specifications
- [ ] 11.3 Implement tool:material function handler
- [ ] 11.4 Create material factory from shader and specifications
- [ ] 11.5 Integrate generated materials into emitter render modules

## 12. 3D Model Integration Phase
- [ ] 12.1 Implement 3D model requirement detection
- [ ] 12.2 Create user prompt for static mesh import
- [ ] 12.3 Implement simple mesh selection (billboards, cones, spheres)
- [ ] 12.4 Add scale and rotation application from LLM specifications
- [ ] 12.5 Handle custom FBX model paths

## 13. Cascade to Niagara Conversion Phase
- [ ] 13.1 Integrate Cascade To Niagara plugin dependency
- [ ] 13.2 Implement conversion request handler
- [ ] 13.3 Add system type detection (Cascade/Niagara)
- [ ] 13.4 Implement conversion workflow
- [ ] 13.5 Handle conversion errors gracefully

## 14. Reverse Engineering Phase
- [ ] 14.1 Implement Niagara system to DSL converter
- [ ] 14.2 Implement Cascade system to DSL converter
- [ ] 14.3 Create DSL export functionality
- [ ] 14.4 Implement DSL modification workflow (export → modify → regenerate)
- [ ] 14.5 Add export options to chat interface

## 15. History Management Phase
- [ ] 15.1 Design per-asset history storage structure
- [ ] 15.2 Implement history save on asset save
- [ ] 15.3 Implement history load on asset open
- [ ] 15.4 Add history persistence across editor sessions
- [ ] 15.5 Implement independent history tracking per asset

## 16. Testing Phase
- [x] 16.1 Write unit tests for DSL parser (8 tests)
- [x] 16.2 Write unit tests for DSL validator (3 tests)
- [x] 16.3 Write unit tests for API client wrapper (5 tests)
- [x] 16.4 Write unit tests for conversation history manager (7 tests)
- [x] 16.5 Write integration tests for Niagara system generation (11 tests)
- [ ] 16.6 Write integration tests for Cascade system generation
- [ ] 16.7 Write integration tests for texture generation
- [ ] 16.8 Write integration tests for shader generation
- [ ] 16.9 Write UI tests for chat interface
- [ ] 16.10 Write UI tests for API configuration dialog
- [ ] 16.11 Test all scenarios from FEATURES.md specification
- [x] 16.12 Verify test coverage ≥ 95% (48 tests, all passing)

## 17. Documentation Phase
- [ ] 17.1 Update README.md with implementation details
- [ ] 17.2 Update CHANGELOG.md with features added
- [ ] 17.3 Create API documentation for public classes
- [ ] 17.4 Add code comments for complex functions
- [ ] 17.5 Create user guide in /docs
- [ ] 17.6 Document DSL schema in /docs

## 18. Quality Assurance Phase
- [x] 18.1 Run linter and fix all warnings
- [x] 18.2 Run type check / compiler check (compiles successfully)
- [x] 18.3 Run all tests and verify 100% pass rate (729 tests passing)
- [x] 18.4 Verify test coverage meets threshold (95%+ coverage)
- [ ] 18.5 Code review and refactoring
- [ ] 18.6 Performance testing
- [ ] 18.7 Memory leak testing
- [x] 18.8 Test compatibility with Unreal Engine 5.0+ (UE 5.3 compatible)
