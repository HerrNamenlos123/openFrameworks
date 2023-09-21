
set(OF_HEADERS          # Here all headers must be listed so they will be visible in Visual Studio
    3d/of3dPrimitives.h
    3d/of3dUtils.h
    3d/ofCamera.h
    3d/ofEasyCam.h
    3d/ofMesh.h
    3d/ofNode.h

    app/ofAppBaseWindow.h
    app/ofAppEGLWindow.h
    app/ofAppGLFWWindow.h
    app/ofAppGlutWindow.h
    app/ofAppNoWindow.h
    app/ofAppRunner.h
    app/ofBaseApp.h
    app/ofIcon.h
    app/ofMainLoop.h
    app/ofWindowSettings.h

    communication/ofArduino.h
    communication/ofSerial.h

    events/ofEvent.h
    events/ofEvents.h
    events/ofEventUtils.h

    gl/ofBufferObject.h
    gl/ofCubeMap.h
    gl/ofCubeMapShaders.h
    gl/ofFbo.h
    gl/ofGLBaseTypes.h
    gl/ofGLProgrammableRenderer.h
    gl/ofGLRenderer.h
    gl/ofGLUtils.h
    gl/ofLight.h
    gl/ofMaterial.h
    gl/ofMaterialBaseTypes.h
    gl/ofShader.h
    gl/ofShadow.h
    gl/ofTexture.h
    gl/ofVbo.h
    gl/ofVboMesh.h

    graphics/of3dGraphics.h
    graphics/ofBitmapFont.h
    graphics/ofCairoRenderer.h
    graphics/ofGraphics.h
    graphics/ofGraphicsBaseTypes.h
    graphics/ofGraphicsCairo.h
    graphics/ofGraphicsConstants.h
    graphics/ofImage.h
    graphics/ofPath.h
    graphics/ofPixels.h
    graphics/ofPolyline.h
    graphics/ofRendererCollection.h
    graphics/ofTessellator.h
    graphics/ofTrueTypeFont.h

    math/ofMath.h
    math/ofMathConstants.h
    math/ofMatrix3x3.h
    math/ofMatrix4x4.h
    math/ofQuaternion.h
    math/ofVec2f.h
    math/ofVec3f.h
    math/ofVec4f.h
    math/ofVectorMath.h

    sound/ofAVEngineSoundPlayer.h
    sound/ofFmodSoundPlayer.h
    sound/ofOpenALSoundPlayer.h
    sound/ofRtAudioSoundStream.h
    sound/ofSoundBaseTypes.h
    sound/ofSoundBuffer.h
    sound/ofSoundPlayer.h
    sound/ofSoundStream.h
    sound/ofSoundUtils.h

    types/ofBaseTypes.h
    types/ofColor.h
    types/ofParameter.h
    types/ofParameterGroup.h
    types/ofPoint.h
    types/ofRectangle.h
    types/ofTypes.h

    utils/ofConstants.h
    utils/ofFileUtils.h
    utils/ofFpsCounter.h
    utils/ofJson.h
    utils/ofLog.h
    utils/ofMatrixStack.h
    utils/ofNoise.h
    utils/ofSystemUtils.h
    utils/ofThread.h
    utils/ofThreadChannel.h
    utils/ofTimer.h
    utils/ofURLFileLoader.h
    utils/ofUtils.h
    utils/ofXml.h

    video/ofAVFoundationGrabber.h
    video/ofAVFoundationPlayer.h
    video/ofAVFoundationVideoPlayer.h
    video/ofDirectShowGrabber.h
    video/ofDirectShowPlayer.h
    video/ofGstUtils.h
    video/ofGstVideoGrabber.h
    video/ofGstVideoPlayer.h
    video/ofMediaFoundationPlayer.h
    video/ofVideoBaseTypes.h
    video/ofVideoGrabber.h
    video/ofVideoPlayer.h

    ofMain.h
)

set(OF_SOURCES
    3d/of3dPrimitives.cpp
    3d/of3dUtils.cpp
    3d/ofCamera.cpp
    3d/ofEasyCam.cpp
    3d/ofMesh.inl
    3d/ofNode.cpp

    app/ofAppEGLWindow.cpp
    app/ofAppGLFWWindow.cpp
    app/ofAppGlutWindow.cpp
    app/ofAppNoWindow.cpp
    app/ofAppRunner.cpp
    app/ofBaseApp.cpp
    app/ofMainLoop.cpp
    
    communication/ofArduino.cpp
    communication/ofSerial.cpp

    events/ofEvents.cpp

    gl/ofBufferObject.cpp
    gl/ofCubeMap.cpp
    gl/ofFbo.cpp
    gl/ofGLProgrammableRenderer.cpp
    gl/ofGLRenderer.cpp
    gl/ofGLUtils.cpp
    gl/ofLight.cpp
    gl/ofMaterial.cpp
    gl/ofShader.cpp
    gl/ofShadow.cpp
    gl/ofTexture.cpp
    gl/ofVbo.cpp
    gl/ofVboMesh.cpp

    graphics/of3dGraphics.cpp
    graphics/ofBitmapFont.cpp
    graphics/ofCairoRenderer.cpp
    graphics/ofGraphics.cpp
    graphics/ofGraphicsBaseTypes.cpp
    graphics/ofGraphicsCairo.cpp
    graphics/ofImage.cpp
    graphics/ofPath.cpp
    graphics/ofPixels.cpp
    graphics/ofPolyline.inl
    graphics/ofRendererCollection.cpp
    graphics/ofTessellator.cpp
    graphics/ofTrueTypeFont.cpp

    math/ofMath.cpp
    math/ofMatrix3x3.cpp
    math/ofMatrix4x4.cpp
    math/ofQuaternion.cpp
    math/ofVec2f.cpp
    math/ofVec4f.cpp

    sound/ofFmodSoundPlayer.cpp
    sound/ofOpenALSoundPlayer.cpp
    sound/ofRtAudioSoundStream.cpp
    sound/ofSoundBaseTypes.cpp
    sound/ofSoundBuffer.cpp
    sound/ofSoundPlayer.cpp
    sound/ofSoundStream.cpp

    types/ofBaseTypes.cpp
    types/ofColor.cpp
    types/ofParameter.cpp
    types/ofParameterGroup.cpp
    types/ofRectangle.cpp

    utils/ofFileUtils.cpp
    utils/ofFpsCounter.cpp
    utils/ofLog.cpp
    utils/ofMatrixStack.cpp
    utils/ofSystemUtils.cpp
    utils/ofThread.cpp
    utils/ofTimer.cpp
    utils/ofURLFileLoader.cpp
    utils/ofUtils.cpp
    utils/ofXml.cpp

    video/ofDirectShowGrabber.cpp
    video/ofDirectShowPlayer.cpp
    video/ofGstUtils.cpp
    video/ofGstVideoGrabber.cpp
    video/ofGstVideoPlayer.cpp
    video/ofMediaFoundationPlayer.cpp
    video/ofVideoGrabber.cpp
    video/ofVideoPlayer.cpp
)

if (APPLE)   # These Objective-C++ files are for Apple compilers only
    list(APPEND OF_SOURCES
        sound/ofAVEngineSoundPlayer.mm
        video/ofAVFoundationGrabber.mm
        video/ofAVFoundationPlayer.mm
        video/ofAVFoundationVideoPlayer.mm
    )
endif()

target_sources(openframeworks PRIVATE ${OF_SOURCES} ${OF_HEADERS})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/" PREFIX "include" FILES ${OF_HEADER})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}/" PREFIX "src" FILES ${OF_SOURCES})

target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/3d)
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/app)
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/communication)
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/events)
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/gl)
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/graphics)
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/math)
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/sound)
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/types)
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/utils)
target_include_directories(openframeworks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/video)